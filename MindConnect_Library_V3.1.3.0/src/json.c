/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     json.c
 * @date     Jul 14, 2016
 * @brief    Json module implementation file.
 *
 ************************************************************************/

#include "json.h"
#include "json_util.h"
#include "memory.h"
#include "log_util.h"
#include "definitions.h"
#include "event.h"
#include "mcl/mcl_event.h"

// Private Function Prototypes:
static E_MCL_ERROR_CODE _add_string_field_to_object(json_t *parent_object,  char *field_name, const string_t *optional_field_to_be_added, mcl_bool_t is_mandatory);
static E_MCL_ERROR_CODE _add_item_meta_details(item_meta_t *item_meta, json_t *root);
static E_MCL_ERROR_CODE _add_item_meta_payload_details(item_meta_t *item_meta, json_t *payload);
static E_MCL_ERROR_CODE _add_item_meta_payload(item_meta_t *item_meta, json_t *root);
static E_MCL_ERROR_CODE _add_time_series_value_sets(list_t *value_sets, json_t *value_set_array);
static E_MCL_ERROR_CODE _add_time_series_value_set(time_series_value_set_t *value_set, json_t *value_set_array);
static E_MCL_ERROR_CODE _add_time_series_payload_values_array(list_t *values, json_t *value_set);
static E_MCL_ERROR_CODE _add_time_series_payload_values_objects(time_series_value_t *value, json_t *values_array);
static E_MCL_ERROR_CODE _add_data_source_configuration_data_sources(list_t *data_source_list, json_t *payload_object);
static E_MCL_ERROR_CODE _add_data_source_configuration_data_points(list_t *data_points, json_t *data_sources_object);

static E_MCL_ERROR_CODE _add_event_list(list_t *event_list, json_t *event_list_array);
static E_MCL_ERROR_CODE _add_event(event_t *event, json_t *event_list_array);

E_MCL_ERROR_CODE json_from_item_meta(item_meta_t *item_meta, string_t **json_string)
{
	DEBUG_ENTRY("item_meta_t *item_meta = <%p>, string_t **json_string = <%p>", item_meta, json_string)
	
	E_MCL_ERROR_CODE code;
	char *json_string_local = MCL_NULL;
	json_t *root = MCL_NULL;

    code = json_util_initialize(JSON_OBJECT, &root);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Root object of item meta couldn't be allocated in memory for json_string.");

    // Add meta type.
    code = _add_string_field_to_object(root, meta_field_names[META_FIELD_TYPE].buffer, item_meta->type, MCL_TRUE);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&root), code, "Type couldn't be added to meta!");

    // Add meta version.
    code = _add_string_field_to_object(root, meta_field_names[META_FIELD_VERSION].buffer, item_meta->version, MCL_TRUE);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&root), code, "Version couldn't be added to meta!");

    // Add item meta details.
    code = _add_item_meta_details(item_meta, root);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&root), code, "Details couldn't be added to meta!");

    // Add item meta payload.
    code = _add_item_meta_payload(item_meta, root);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&root), code, "Payload couldn't be added to meta!");

    MCL_DEBUG("Json string will be set.");

    code = json_util_to_string(root, &json_string_local);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&root), code, "Json to string conversion failed!");

    code = string_initialize_dynamic(json_string_local, 0, json_string);

    json_util_destroy(&root);
    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_from_time_series_payload(time_series_payload_t *payload, string_t **json_string)
{
	DEBUG_ENTRY("time_series_payload_t *payload = <%p>, string_t **json_string = <%p>", payload, json_string)

	E_MCL_ERROR_CODE code;
	char *json_string_local = MCL_NULL;
	json_t *value_set_array = MCL_NULL;

    MCL_DEBUG("Create payload array.");

    code = json_util_initialize(JSON_ARRAY, &value_set_array);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "value_set_array couldn't be allocated in memory for json_string.");

    code = _add_time_series_value_sets(payload->value_sets, value_set_array);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&value_set_array), code, "Time series payload couldn't be added.");

    
    code = json_util_to_string(value_set_array, &json_string_local);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&value_set_array), code, "Json to string conversion failed!");

    code = string_initialize_dynamic(json_string_local, 0, json_string);

    MCL_DEBUG("Generated json_string = <%s>", (*json_string)->buffer);
    json_util_destroy(&value_set_array);
    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_from_data_source_configuration_payload(data_source_configuration_payload_t *payload, string_t **json_string)
{
	DEBUG_ENTRY("data_source_configuration_payload_t *payload = <%p>, string_t **json_string = <%p>", payload, json_string)

	E_MCL_ERROR_CODE code;
	json_t *payload_object = MCL_NULL;
	char *json_string_local = MCL_NULL;

    MCL_DEBUG("Create payload array.");

	code = json_util_initialize(JSON_OBJECT, &payload_object);

    // Add configuration_id to payload_object.
    (MCL_OK == code) && (code = json_util_add_string(payload_object, payload_field_names[PAYLOAD_FIELD_CONFIGURATION_ID].buffer, payload->configuration_id->buffer));

    (MCL_OK == code) && (code =_add_data_source_configuration_data_sources(payload->data_sources, payload_object));

    (MCL_OK == code) && (code = json_util_to_string(payload_object, &json_string_local));
    (MCL_OK == code) && (code = string_initialize_dynamic(json_string_local, 0, json_string));

    json_util_destroy(&payload_object);
    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_from_event_payload(list_t *event_list_payload, string_t **json_string)
{
    DEBUG_ENTRY("list_t *event_list_payload = <%p>, string_t **json_string = <%p>", event_list_payload, json_string)

    // Create root_array object which includes other objects as child.
    json_t *event_list_payload_array = MCL_NULL;
	char *json_string_local = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_initialize(JSON_ARRAY, &event_list_payload_array);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "event_list_array couldn't be allocated in memory for json_string.");

    code = _add_event_list(event_list_payload, event_list_payload_array);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&event_list_payload_array), code, "Event set payload couldn't be added.");

    code = json_util_to_string(event_list_payload_array, &json_string_local);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&event_list_payload_array), code, "Json to string conversion failed!");

    code = string_initialize_dynamic(json_string_local, 0, json_string);

    MCL_DEBUG("Generated json_string = <%s>", (*json_string)->buffer);
    json_util_destroy(&event_list_payload_array);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _add_event_list(list_t *event_list, json_t *event_list_array)
{
    DEBUG_ENTRY("list_t *event_list = <%p>, json_t *event_list_array = <%p>", event_list, event_list_array)

    list_node_t *current_event_node;
    list_reset(event_list);
    while (MCL_NULL != (current_event_node = list_next(event_list)))
    {
        E_MCL_ERROR_CODE code = _add_event((event_t *)(current_event_node->data), event_list_array);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Event objects couldn't be added to event set array!");
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _add_event(event_t *event, json_t *event_list_array)
{
    DEBUG_ENTRY("event_t *event = <%p>, json_t *event_list_array = <%p>", event, event_list_array)

    json_t *details_json = MCL_NULL;

    // Create event_local.
    json_t *event_local = MCL_NULL;

    E_MCL_ERROR_CODE code = json_util_initialize(JSON_OBJECT, &event_local);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "event_local couldn't be allocated in memory for json_string.");

    // Add id.
    code = json_util_add_string(event_local, payload_field_names[PAYLOAD_FIELD_ID].buffer, event->payload->id->buffer);

    // Add correlationId (optional field).
    if (MCL_OK == code)
    {
        code = _add_string_field_to_object(event_local, payload_field_names[PAYLOAD_FIELD_CORRELATION_ID].buffer, event->payload->correlation_id, MCL_FALSE);
    }

    // Add timestamp.
    if (MCL_OK == code)
    {
        code = json_util_add_string(event_local, payload_field_names[PAYLOAD_FIELD_TIMESTAMP].buffer, event->payload->timestamp->buffer);
    }

    // Add severity.
    if (MCL_OK == code)
    {
        code = json_util_add_uint(event_local, payload_field_names[PAYLOAD_FIELD_SEVERITY].buffer, event->payload->severity);
    }   

    // Add description (optional field).
    if (MCL_OK == code)
    {
        code = _add_string_field_to_object(event_local, payload_field_names[PAYLOAD_FIELD_DESCRIPTION].buffer, event->payload->description, MCL_FALSE);
    } 

    // Add type.
    if (MCL_OK == code)
    {
        code = json_util_add_string(event_local, payload_field_names[PAYLOAD_FIELD_TYPE].buffer, event->payload->type->buffer);
    }
    
    // Add version.
    if (MCL_OK == code)
    {
        code = json_util_add_string(event_local, payload_field_names[PAYLOAD_FIELD_VERSION].buffer, event->payload->version->buffer);
    }

    // Create details.
    if (MCL_OK == code)
    {
        if (MCL_NULL != event->payload->details)
        {
            code = json_util_duplicate(event->payload->details, MCL_TRUE, &details_json);
        }
        else
        {
            code = json_util_initialize(JSON_OBJECT, &details_json);
        }
    }

    // Add details even it is empty.
    if (MCL_OK == code)
    {
        code = json_util_add_object(event_local, payload_field_names[PAYLOAD_FIELD_DETAILS].buffer, details_json);

        if (MCL_OK != code)
        {
            // Destroy all the content.
            json_util_destroy(&details_json);
        }
    }

    json_util_finish_object(&details_json);

    // Add event to event list array.
    if (MCL_OK == code)
    {
        json_util_add_item_to_array(event_list_array, event_local);
        json_util_finish_object(&event_local);
    }
    else
    {
        json_util_destroy(&event_local);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _add_time_series_value_sets(list_t *value_sets, json_t *value_set_array)
{
    DEBUG_ENTRY("list_t *value_sets = <%p>, json_t *value_set_array = <%p>", value_sets, value_set_array)

    list_node_t *current_payloads_node;

    list_reset(value_sets);
    while (MCL_NULL != (current_payloads_node = list_next(value_sets)))
    {
        E_MCL_ERROR_CODE code = _add_time_series_value_set((time_series_value_set_t *)(current_payloads_node->data), value_set_array);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Values objects couldn't be added to values array!");
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _add_time_series_value_set(time_series_value_set_t *value_set, json_t *value_set_array)
{
	DEBUG_ENTRY("time_series_value_set_t *value_set = <%p>, json_t *value_set_array = <%p>", value_set, value_set_array)

	E_MCL_ERROR_CODE code;
    json_t *value_set_local = MCL_NULL;
    
	code = json_util_initialize(JSON_OBJECT, &value_set_local);

    (MCL_OK == code) && (code = json_util_add_string(value_set_local, payload_field_names[PAYLOAD_FIELD_TIMESTAMP].buffer, value_set->timestamp->buffer));

    (MCL_OK == code) && (code = _add_time_series_payload_values_array(value_set->values, value_set_local));

    ASSERT_STATEMENT_CODE(MCL_OK == code, json_util_destroy(&value_set_local), code);

    // Add value_set_local to value_set_array.
    json_util_add_item_to_array(value_set_array, value_set_local);

    // value_set_local is added to value_set_array. We can destroy value_set_local struct w/o destroying root_handle in it.
    json_util_finish_object(&value_set_local);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _add_time_series_payload_values_array(list_t *values, json_t *value_set)
{
	DEBUG_ENTRY("list_t *values = <%p>, json_t *value_set = <%p>", values, value_set)

	E_MCL_ERROR_CODE code;
    json_t *values_array = MCL_NULL;
	list_node_t *current_value_node;
    
	code = json_util_start_array(value_set, payload_field_names[PAYLOAD_FIELD_VALUES].buffer, &values_array);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "values_array object couldn't be allocated in memory for Json string.");

    list_reset(values);
    while (MCL_NULL != (current_value_node = list_next(values)))
    {
        code = _add_time_series_payload_values_objects((time_series_value_t *)current_value_node->data, values_array);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_finish_array(&values_array), code, "Values objects couldn't be added to values array!");
    }

    json_util_finish_array(&values_array);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _add_time_series_payload_values_objects(time_series_value_t *value, json_t *values_array)
{
	DEBUG_ENTRY("time_series_value_t *value = <%p>, json_t *values_array = <%p>", value, values_array)

	E_MCL_ERROR_CODE code;
    json_t *values_object = MCL_NULL;

    code = json_util_initialize(JSON_OBJECT, &values_object);

    // Add fields of values array in time series payload.
    (MCL_OK == code) && (code = json_util_add_string(values_object, payload_field_names[PAYLOAD_FIELD_VALUES_DATA_POINT_ID].buffer, value->data_point_id->buffer));
    (MCL_OK == code) && (code = json_util_add_string(values_object, payload_field_names[PAYLOAD_FIELD_VALUES_VALUE].buffer, value->value->buffer));
    (MCL_OK == code) && (code = json_util_add_string(values_object, payload_field_names[PAYLOAD_FIELD_VALUES_QUALITY_CODE].buffer, value->quality_code->buffer));

    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&values_object), code, "Values objects couldn't be added to values array!");

    // Add values_object to values_array.
    json_util_add_item_to_array(values_array, values_object);

    json_util_finish_object(&values_object);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _add_item_meta_details(item_meta_t *item_meta, json_t *root)
{
	DEBUG_ENTRY("item_meta_t *item_meta = <%p>, json_t *root = <%p>", item_meta, root)

	E_MCL_ERROR_CODE code;
    json_t *details = MCL_NULL;
    
	code = json_util_initialize(JSON_OBJECT, &details);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Details object of item meta couldn't be allocated in memory for json_string.");

    // Check whether the following fields are relevant. if so, add to object.
    code = _add_string_field_to_object(details, meta_field_names[META_FIELD_DETAILS_ROUTING].buffer, item_meta->details.routing, MCL_FALSE);

    // If details has at least one field, then we add it into root_object. Otherwise it is destroyed without adding to the root.
    if (MCL_OK == code && MCL_TRUE == json_util_has_child(details))
    {
        code = json_util_add_object(root, meta_field_names[META_FIELD_DETAILS].buffer, details);

        if (MCL_OK == code)
        {
            // details is added to root parent. We can destroy details struct w/o destroying root_handle in it.
            json_util_finish_object(&details);
        }
        else
        {
            json_util_destroy(&details);
        }
    }
    else
    {
        json_util_destroy(&details);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _add_item_meta_payload_details(item_meta_t *item_meta, json_t *payload)
{
    DEBUG_ENTRY("item_meta_t *item_meta = <%p>, json_t *payload = <%p>", item_meta, payload)

    E_MCL_ERROR_CODE code;
    json_t *payload_details = MCL_NULL;

    MCL_DEBUG("Item type specific fields will be added.");

    if (MCL_OK == string_compare(&meta_field_values[META_FIELD_PAYLOAD_TYPE_TIME_SERIES], item_meta->payload.type))
    {
        MCL_DEBUG("Payload type is time series.");

        code = json_util_initialize(JSON_OBJECT, &payload_details);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "payload_details object couldn't be allocated in memory for Json string.");

        code = _add_string_field_to_object(payload_details, meta_field_names[META_FIELD_PAYLOAD_DETAILS_CONFIGURATION_ID].buffer, item_meta->payload.details.time_series_details.configuration_id, MCL_TRUE);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&payload_details), code, "configuration_id couldn't be added to item meta of time series!");
    }
    else if (MCL_OK == string_compare(&meta_field_values[META_FIELD_PAYLOAD_TYPE_FILE], item_meta->payload.type))
    {
        MCL_DEBUG("Payload type is file");

        code = json_util_initialize(JSON_OBJECT, &payload_details);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "payload_details object couldn't be allocated in memory for Json string.");

        code = _add_string_field_to_object(payload_details, meta_field_names[META_FIELD_PAYLOAD_DETAILS_FILE_NAME].buffer, item_meta->payload.details.file_details.file_name, MCL_TRUE);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&payload_details), code, "file_name couldn't be added to item meta of file!");

        // Add file creation date (optional field).
        code = _add_string_field_to_object(payload_details, meta_field_names[META_FIELD_PAYLOAD_DETAILS_CREATION_DATE].buffer, item_meta->payload.details.file_details.creation_date, MCL_FALSE);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&payload_details), code, "creation_date couldn't be added to item meta of file!");

        // Add file type (optional field).
        code = _add_string_field_to_object(payload_details, meta_field_names[META_FIELD_PAYLOAD_DETAILS_FILE_TYPE].buffer, item_meta->payload.details.file_details.file_type, MCL_FALSE);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, json_util_destroy(&payload_details), code, "file_type couldn't be added to item meta of file!");

    }
    else if (MCL_OK == string_compare(&meta_field_values[META_FIELD_PAYLOAD_TYPE_DATA_SOURCE_CONFIGURATION], item_meta->payload.type))
    {
        MCL_DEBUG("Payload type is data source configuration.");
    }
    else if (MCL_OK == string_compare(&meta_field_values[META_FIELD_PAYLOAD_TYPE_BUSINESS_EVENT], item_meta->payload.type))
    {
        MCL_DEBUG("Payload type is event.");
    }
    else
    {
        MCL_DEBUG("Payload type is custom type.");

        if (MCL_NULL != item_meta->payload.details.custom_details.details_json)
        {
            // Add details.
            code = json_util_duplicate(item_meta->payload.details.custom_details.details_json, MCL_TRUE, &payload_details);
            ASSERT_CODE_MESSAGE(MCL_OK == code, code, "details json object couldn't be added to item meta of custom data!");
        }
    }

    // If payload_details has at least one field, then we add it into payload. Otherwise it is destroyed without adding to the root.
    if ((MCL_NULL != payload_details) && (MCL_TRUE == json_util_has_child(payload_details)))
    {
        code = json_util_add_object(payload, meta_field_names[META_FIELD_DETAILS].buffer, payload_details);

        if (MCL_OK == code)
        {
            // payload_details is added to payload parent. We can destroy payload_details struct w/o destroying root_handle in it.
            json_util_finish_object(&payload_details);
        }
        else
        {
            json_util_destroy(&payload_details);
        }
    }
    else
    {
        json_util_destroy(&payload_details);
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _add_item_meta_payload(item_meta_t *item_meta, json_t *root)
{
	DEBUG_ENTRY("item_meta_t *item_meta = <%p>, json_t *root = <%p>", item_meta, root)

	E_MCL_ERROR_CODE code;
    json_t *payload = MCL_NULL;
    
	code = json_util_start_object(root, meta_field_names[META_FIELD_PAYLOAD].buffer, &payload);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Payload object of item meta couldn't be allocated in memory for json_string.");

    // Add item meta payload type.
    code = json_util_add_string(payload, meta_field_names[META_FIELD_PAYLOAD_TYPE].buffer, item_meta->payload.type->buffer);
    
    // Add item meta payload version.
    if (MCL_OK == code)
    {
        code = json_util_add_string(payload, meta_field_names[META_FIELD_PAYLOAD_VERSION].buffer, item_meta->payload.version->buffer);
    }
    
    // Add item meta payload details.
    if (MCL_OK == code)
    {
        code = _add_item_meta_payload_details(item_meta, payload);
    }

    // Destroy payload struct w/o destroying root_handle in it.
    json_util_finish_object(&payload);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _add_data_source_configuration_data_sources(list_t *data_source_list, json_t *payload_object)
{
	DEBUG_ENTRY("list_t *data_source_list = <%p>, json_t *payload_object = <%p>", data_source_list, payload_object)

	E_MCL_ERROR_CODE code;
	list_node_t *current_data_sources_node;
    json_t *data_sources_array = MCL_NULL;

    code = json_util_start_array(payload_object, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES].buffer, &data_sources_array);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "data_sources_array couldn't be allocated in memory for Json string.");

    list_reset(data_source_list);
    while (MCL_NULL != (current_data_sources_node = list_next(data_source_list)))
    {
        // Create data_sources_object.
        json_t *data_sources_object = MCL_NULL;
		data_source_t *data_source;

        code = json_util_initialize(JSON_OBJECT, &data_sources_object);

        data_source = (data_source_t *)current_data_sources_node->data;

        // Add name to data source.
        (MCL_OK == code) && (code = json_util_add_string(data_sources_object, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_NAME].buffer, data_source->name->buffer));

        // Add description to data source (optional field).
        (MCL_OK == code) && (code = _add_string_field_to_object(data_sources_object, payload_field_names[PAYLOAD_FIELD_DESCRIPTION].buffer, data_source->description, MCL_FALSE));      

        // Add data points to data source.
        (MCL_OK == code) && (code = _add_data_source_configuration_data_points(data_source->data_points, data_sources_object));

        // Add custom data to data source.
        if (MCL_NULL != data_source->custom_data)
        {
            json_t *duplicated_custom_data_json = MCL_NULL;

            (MCL_OK == code) && (code = json_util_duplicate(data_source->custom_data, MCL_TRUE, &duplicated_custom_data_json));
            (MCL_OK == code) && (code = json_util_add_object(data_sources_object, payload_field_names[PAYLOAD_FIELD_CUSTOM_DATA].buffer, duplicated_custom_data_json));

            if (MCL_OK == code)
            {
                json_util_finish_object(&duplicated_custom_data_json);
            }
            else
            {
                json_util_destroy(&duplicated_custom_data_json);
            }
        }

        // Add data_sources_object to data_sources_array.
        json_util_add_item_to_array(data_sources_array, data_sources_object);

        json_util_finish_object(&data_sources_object);

        if (MCL_OK != code)
        {
            break;
        }
    }

    json_util_finish_array(&data_sources_array);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _add_data_source_configuration_data_points(list_t *data_points, json_t *data_sources_object)
{
	E_MCL_ERROR_CODE code;
    json_t *data_points_array = MCL_NULL;
	list_node_t *current_data_points_node;

    code = json_util_start_array(data_sources_object, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS].buffer, &data_points_array);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "data_points_array couldn't be allocated in memory for Json string.");

    list_reset(data_points);
    while (MCL_NULL != (current_data_points_node = list_next(data_points)))
    {
        // Create data_points_object.
        json_t *data_points_object = MCL_NULL;
		data_point_t *data_point;

        code = json_util_initialize(JSON_OBJECT, &data_points_object);

        data_point = (data_point_t *)current_data_points_node->data;

        (MCL_OK == code) && (code = json_util_add_string(data_points_object, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_ID].buffer, data_point->id->buffer));
        (MCL_OK == code) && (code = json_util_add_string(data_points_object, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_NAME].buffer, data_point->name->buffer));
        (MCL_OK == code) && (code = _add_string_field_to_object(data_points_object, payload_field_names[PAYLOAD_FIELD_DESCRIPTION].buffer, data_point->description, MCL_FALSE));
        (MCL_OK == code) && (code = json_util_add_string(data_points_object, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_TYPE].buffer, data_point->type->buffer));
        (MCL_OK == code) && (code = json_util_add_string(data_points_object, payload_field_names[PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_UNIT].buffer, data_point->unit->buffer));

        // Add custom_data to data_point.
        if (MCL_NULL != data_point->custom_data)
        {
            json_t *duplicated_custom_data_json = MCL_NULL;

            (MCL_OK == code) && (code = json_util_duplicate(data_point->custom_data, MCL_TRUE, &duplicated_custom_data_json));
            (MCL_OK == code) && (code = json_util_add_object(data_points_object, payload_field_names[PAYLOAD_FIELD_CUSTOM_DATA].buffer, duplicated_custom_data_json));

            if (MCL_OK == code)
            {
                json_util_finish_object(&duplicated_custom_data_json);
            }
            else
            {
                json_util_destroy(&duplicated_custom_data_json);
            }
        }

        // Add data_points_object to data_points_array.
        json_util_add_item_to_array(data_points_array, data_points_object);

        json_util_finish_object(&data_points_object);
    }

    json_util_finish_array(&data_points_array);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _add_string_field_to_object(json_t *parent_object, char *field_name, const string_t *field_to_be_added, mcl_bool_t is_mandatory)
{
    DEBUG_ENTRY("json_t *parent_object = <%p>, char *field_name = <%s>, const string_t *field_to_be_added = <%p>, mcl_bool_t is_mandatory = <%u>", parent_object, field_name, field_to_be_added, is_mandatory)

    E_MCL_ERROR_CODE code = is_mandatory ? MCL_FAIL : MCL_OK;

    // Check whether  string field is NULL or not and it is used or not before adding to object.
    if ((MCL_NULL != field_to_be_added) && (MCL_NULL_CHAR != field_to_be_added->buffer[0]))
    {
        MCL_DEBUG("<%s> field will be added.", field_name);
        code = json_util_add_string(parent_object, field_name, field_to_be_added->buffer);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
