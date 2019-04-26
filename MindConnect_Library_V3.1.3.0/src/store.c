/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     store.c
 * @date     Jul 18, 2016
 * @brief    This file implements MCL Store Interface.
 *
 ************************************************************************/

#include "store.h"
#include "time_series.h"
#include "custom_data.h"
#include "stream_data.h"
#include "data_source_configuration.h"
#include "event_list.h"
#include "file.h"
#include "log_util.h"
#include "memory.h"
#include "definitions.h"
#include "mcl/mcl_store.h"
#include "time_util.h"

typedef enum E_STORE_DATA_PRIORITY
{
    PRIORITY_HIGH, //!< Represents the high priority data to be sent to MindSphere.
    PRIORITY_LOW   //!< Represents the low priority data to be sent to MindSphere.
} E_STORE_DATA_PRIORITY;

typedef struct
{
    char *type;    //!< Type of payload.
    char *version; //!< Version of payload.
} item_meta_payload_local_t;

typedef enum E_EVENT_VERSION
{
    EVENT_VERSION_1_0 = 0,
    EVENT_VERSION_2_0,
    EVENT_VERSION_END
} E_EVENT_VERSION;

static const string_t _event_versions[EVENT_VERSION_END] =
{
    {"1.0", 3, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"2.0", 3, MCL_STRING_NOT_COPY_NOT_DESTROY}
};

static mcl_int32_t _event_severity_values[EVENT_VERSION_END][MCL_EVENT_SEVERITY_END] =
{
    {1, 2, 3},
    {20, 30, 40}
};

// custom list destroyer ( destroys based on the type of the item ) for destroying high and low priority lists.
void _store_list_destroy_callback(void **item);

static E_MCL_ERROR_CODE _store_add_data(mcl_store_t *store, void *data, E_STORE_DATA_TYPE data_type, E_STORE_DATA_PRIORITY priority);
static E_MCL_ERROR_CODE _compare_item_meta_of_event(void *data, const item_meta_payload_local_t *item_meta_payload);

// Checks version format.
static mcl_bool_t _is_valid_version(const char *version);
static mcl_bool_t _is_positive_integer(const char *version, mcl_size_t start_index, mcl_size_t end_index);

E_MCL_ERROR_CODE mcl_store_initialize(mcl_bool_t streamable, mcl_store_t **store)
{
	DEBUG_ENTRY("mcl_bool_t streamable = <%u>, mcl_store_t **store = <%p>", streamable, store)
	
	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(store);

#if !MCL_STREAM_ENABLED
	ASSERT_CODE_MESSAGE(MCL_FALSE == streamable, MCL_OPERATION_IS_NOT_SUPPORTED, "Streamable store is not supported currently.");
#endif

    // Allocate store
    MCL_NEW(*store);
    ASSERT_CODE_MESSAGE(MCL_NULL != *store, MCL_OUT_OF_MEMORY, "Memory for store could not be allocated!");

    // Initialize lists containing mcl data types
    code = list_initialize(&((*store)->high_priority_list));
    code = (MCL_OK == code) ? list_initialize(&((*store)->low_priority_list)) : MCL_FAIL;
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, mcl_store_destroy(store), MCL_FAIL, "Initialization of store lists failed!");

    // set streamable property :
    (*store)->streamable = streamable;

    MCL_DEBUG("High and Low priority lists has been successfully initialized.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_store_new_time_series(mcl_store_t *store, const char *version, const char *configuration_id, const char *routing, mcl_time_series_t **time_series)
{
	DEBUG_ENTRY("mcl_store_t *store = <%p>, const char *version = <%p>, const char *configuration_id = <%p>, const char *routing = <%p>, mcl_time_series_t **time_series = <%p>",
		store, version, configuration_id, routing, time_series)
		
	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(store);
    ASSERT_NOT_NULL(version);
    ASSERT_NOT_NULL(configuration_id);
    ASSERT_NOT_NULL(time_series);

    // Check version format.
    ASSERT_CODE_MESSAGE(MCL_TRUE == _is_valid_version(version), MCL_INVALID_PARAMETER, "Version format is not correct.");

    // Initialize a new time series.
     code = time_series_initialize(version, configuration_id, routing, time_series);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Creation of new time_series store failed!");

    // Add new time_series to list, or if failed destroy it
    code = _store_add_data(store, (void *)*time_series, STORE_DATA_TIME_SERIES, PRIORITY_HIGH);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, time_series_destroy(time_series), code, "Adding time_series to list failed!");

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_store_new_event(mcl_store_t *store, const char *version, const char *type, const char *type_version, E_MCL_EVENT_SEVERITY severity, const char *timestamp, mcl_event_t **event)
{
    DEBUG_ENTRY("mcl_store_t *store = <%p>, const char *version = <%p>, const char *type = <%p>, const char *type_version = <%p>, E_MCL_EVENT_SEVERITY severity = <%d>, const char *timestamp = <%p>, mcl_event_t **event = <%p>", store, version, type, type_version, severity, timestamp, event)

	E_MCL_ERROR_CODE code;
	E_MCL_ERROR_CODE event_list_exists;
    item_meta_payload_local_t item_meta_payload_local;
	event_list_t *event_list = MCL_NULL;
    mcl_size_t index;
    store_data_t *store_data;

	ASSERT_NOT_NULL(store);
    ASSERT_NOT_NULL(version);
    ASSERT_NOT_NULL(type);
    ASSERT_NOT_NULL(type_version);
    ASSERT_NOT_NULL(timestamp);
    ASSERT_NOT_NULL(event);
    ASSERT_CODE_MESSAGE(severity >= MCL_EVENT_SEVERITY_ERROR && severity < MCL_EVENT_SEVERITY_END, MCL_INVALID_PARAMETER, "Invalid severity.");

    // Check version.
    for (index = 0; index < (mcl_size_t)EVENT_VERSION_END; index++)
    {
        if (MCL_OK == string_compare_with_cstr(&_event_versions[(E_EVENT_VERSION)index], version))
        {
            break;
        }
    }

    ASSERT_CODE_MESSAGE(EVENT_VERSION_END != index, MCL_INVALID_PARAMETER, "Version is not correct.");

    // Validate timestamp.
    ASSERT_CODE_MESSAGE(MCL_TRUE == time_util_validate_timestamp(timestamp), MCL_INVALID_PARAMETER, "Timestamp format is not correct.");

    // Set event type.
    item_meta_payload_local.type = meta_field_values[META_FIELD_PAYLOAD_TYPE_BUSINESS_EVENT].buffer;

    // Set event version.
    item_meta_payload_local.version = (char *)version;

    // Check if the necessary event set exists.
    event_list_exists = list_exist(store->high_priority_list, (const void *)(&item_meta_payload_local), (list_compare_callback)_compare_item_meta_of_event,
                                             (void **)(&store_data));

    // If the event set does not exist, initialize the event and add to store.
    if (MCL_FAIL == event_list_exists)
    {
        // event set initialize
        code = event_list_initialize(version, &event_list);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Creation of new event set failed!");

        // Add a new event set to store list.
        code = _store_add_data(store, (void *)event_list, STORE_DATA_EVENT_LIST, PRIORITY_HIGH);
        ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, event_list_destroy(&event_list), code, "Adding event set to store list failed!");
    }
    else
    {
        event_list = store_data->data;
    }

    // Initialize a new event.
    code = event_initialize(event_list->meta, type, type_version, _event_severity_values[index][severity], timestamp, event);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Creation of new event store failed!");

    // add event to event set
    code = event_list_add_event(*event, event_list);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, event_destroy(event), code, "Creation of new event store failed!");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_store_new_file(mcl_store_t *store, const char *version, const char *file_path, const char *file_name, const char *file_type, const char *routing, mcl_file_t **file)
{
	DEBUG_ENTRY("mcl_store_t *store = <%p>, const char *version = <%p>, const char *file_path = <%p>, const char *file_name = <%p>, const char *file_type = <%p>, const char *routing = <%p>, mcl_file_t **file = <%p>", store, version, file_path, file_name, file_type, routing, file)

#if MCL_FILE_EXCHANGE_ENABLED
	
	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(store);
    ASSERT_NOT_NULL(version);
    ASSERT_NOT_NULL(file_path);
    ASSERT_NOT_NULL(file);
    ASSERT_NOT_NULL(file_name);

    // Check version format.
    ASSERT_CODE_MESSAGE(MCL_TRUE == _is_valid_version(version), MCL_INVALID_PARAMETER, "Version format is not correct.");

    // Initialize a file item.
    code = file_initialize(version, file_path, file_name, file_type, routing, file);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Creation of a new file item failed!");

    // Add new file item to store.
    code = _store_add_data(store, (void *)*file, STORE_DATA_FILE, PRIORITY_HIGH);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, file_destroy(file), code, "Adding file item to store failed!");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
#else
    MCL_ERROR("File exchange feature is not enabled.");
    DEBUG_LEAVE("retVal = <%d>", MCL_FAIL);
    return MCL_FAIL;
#endif
}

E_MCL_ERROR_CODE mcl_store_new_custom_data(mcl_store_t *store, const char *version, const char *type, const char *routing, mcl_custom_data_t **custom_data)
{
	DEBUG_ENTRY("mcl_store_t *store = <%p>, const char *version = <%p>, const char *type = <%p>, const char *routing = <%p>, mcl_custom_data_t **custom_data = <%p>", store,
		version, type, routing, custom_data)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(store);
    ASSERT_NOT_NULL(custom_data);
    ASSERT_NOT_NULL(version);
    ASSERT_NOT_NULL(type);

    // Initialize a new custom data
    code = custom_data_initialize(version, type, routing, custom_data);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Creation of new custom data store failed!");

    // Add new custom data to list, or if failed destroy it
    // TODO : Currently adding only to the high priority list
    code = _store_add_data(store, (void *)*custom_data, STORE_DATA_CUSTOM, PRIORITY_HIGH);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, custom_data_destroy(custom_data), code, "Adding custom data to list failed!");

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_store_new_stream_data(mcl_store_t *store, const char *version, const char *type, const char *routing,
        mcl_stream_data_read_callback_t stream_data_read_callback, void *user_context, mcl_stream_data_t **stream_data)
{
	DEBUG_ENTRY("mcl_store_t *store = <%p>, const char *version = <%p>, const char *type = <%p>, const char *routing = <%p>, stream_data_read_callback_t stream_data_read_callback = <%p>, void *user_context = <%p>, mcl_stream_data_t **stream_data = <%p>",
		store, version, type, routing, stream_data_read_callback, user_context, stream_data)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(store);
    ASSERT_CODE_MESSAGE(MCL_TRUE == store->streamable, MCL_INVALID_PARAMETER, "Store must be streamable to add this type of item!");
    ASSERT_NOT_NULL(stream_data);
    ASSERT_NOT_NULL(version);
    ASSERT_NOT_NULL(type);
    ASSERT_NOT_NULL(stream_data_read_callback);

    // Check version format.
    ASSERT_CODE_MESSAGE(MCL_TRUE == _is_valid_version(version), MCL_INVALID_PARAMETER, "Version format is not correct.");

    // Initialize a new stream data
    code = stream_data_initialize(version, type, routing, stream_data_read_callback, user_context, stream_data);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Creation of new stream data store failed!");

    // Add new stream data to list, or if failed destroy it
    // TODO : Currently adding only to the high priority list
    code = _store_add_data(store, (void *)*stream_data, STORE_DATA_STREAM, PRIORITY_HIGH);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, stream_data_destroy(stream_data), code, "Adding stream data to list failed!");

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_store_new_data_source_configuration(mcl_store_t *store, const char *version, mcl_data_source_configuration_t **data_source_configuration)
{
	DEBUG_ENTRY("mcl_store_t *store = <%p>, const char *version = <%p>, mcl_data_source_configuration_t **data_source_configuration = <%p>", store, version, data_source_configuration)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(store);
    ASSERT_NOT_NULL(version);
    ASSERT_NOT_NULL(data_source_configuration);

    // Check version format.
    ASSERT_CODE_MESSAGE(MCL_TRUE == _is_valid_version(version), MCL_INVALID_PARAMETER, "Version format is not correct.");

    // Initialize a new data_source_configuration
    code = data_source_configuration_initialize(version, data_source_configuration);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Creation of new data source configuration failed!");

    // Add new data_source_configuration to list, or if failed destroy it
    // TODO : Currently adding only to the high priority list
    code = _store_add_data(store, (void *)*data_source_configuration, STORE_DATA_DATA_SOURCE_CONFIGURATION, PRIORITY_HIGH);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, data_source_configuration_destroy(data_source_configuration), code, "Adding data source configuration to list failed!");

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_store_destroy(mcl_store_t **store)
{
    DEBUG_ENTRY("mcl_store_t **store = <%p>", store)

    // Make sure input argument is not NULL.
    ASSERT_NOT_NULL(store);

    if (MCL_NULL != *store)
    {
        list_destroy_with_content(&(*store)->high_priority_list, _store_list_destroy_callback);
        list_destroy_with_content(&(*store)->low_priority_list, _store_list_destroy_callback);
        MCL_FREE(*store);
    }
    else
    {
        MCL_DEBUG("Store is already NULL. Nothing will be destroyed.");
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void store_data_set_state(store_data_t *store_data, E_STORE_DATA_STATE state)
{
    DEBUG_ENTRY("store_data_t *store_data = <%p>, E_STORE_DATA_STATE state = <%d>", store_data, state)

    MCL_DEBUG("Updating state : %d --> %d", store_data->state, state);
    store_data->state = state;

    DEBUG_LEAVE("retVal = <void>");
    return;
}

E_STORE_DATA_STATE store_data_get_state(store_data_t *store_data)
{
    DEBUG_ENTRY("store_data_t *store_data = <%p>", store_data)

    MCL_DEBUG("Current state is <%d>", store_data->state);

    DEBUG_LEAVE("retVal = <%d>", store_data->state);
    return store_data->state;
}

E_MCL_ERROR_CODE store_data_remove(list_t *store_list, list_node_t *store_data_node)
{
    DEBUG_ENTRY("list_t *store_list = <%p>, list_node_t *store_data_node = <%p>", store_list, store_data_node)

    // first remove the item from the list :
    E_MCL_ERROR_CODE result = list_remove_with_content(store_list, store_data_node, _store_list_destroy_callback);

    DEBUG_LEAVE("retVal = <%d>", result);
    return result;
}

mcl_size_t store_get_data_count(store_t *store)
{
    DEBUG_ENTRY("store_t *store = <%p>", store)

    DEBUG_LEAVE("retVal = <%u>", store->high_priority_list->count + store->low_priority_list->count);
    return store->high_priority_list->count + store->low_priority_list->count;
}

// Private Functions:
static E_MCL_ERROR_CODE _store_add_data(mcl_store_t *store, void *data, E_STORE_DATA_TYPE data_type, E_STORE_DATA_PRIORITY priority)
{
    DEBUG_ENTRY("mcl_store_t *store = <%p>, void *data = <%p>, E_STORE_DATA_TYPE data_type = <%d>, E_STORE_DATA_PRIORITY priority = <%d>", store, data, data_type, priority)

	E_MCL_ERROR_CODE code;
    store_data_t *store_data;
	list_t *list_to_add;
	
    MCL_NEW(store_data);
    ASSERT_CODE_MESSAGE(MCL_NULL != store_data, MCL_OUT_OF_MEMORY, "Not enough memory to create store_data!");

    store_data->data = data;
    store_data->type = data_type;
    store_data->meta = MCL_NULL;
    store_data->payload_buffer = MCL_NULL;
    store_data->payload_size = 0;
    store_data->stream_info = MCL_NULL;
    store_data->state = DATA_STATE_INITIAL;

    list_to_add = (PRIORITY_HIGH == priority) ? store->high_priority_list : store->low_priority_list;

    code = list_add(list_to_add, store_data);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, MCL_FREE(store_data), code, "Add to list failed!");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void _store_list_destroy_callback(void **item)
{
    DEBUG_ENTRY("void **item = <%p>", item)

    // common destroy :
    store_data_t *store_data = (store_data_t *)*item;

    // destroy meta string :
    string_destroy(&store_data->meta);

    // destroy payload string ( only if it is not file or custom or stream. payload for those is just a pointer to their payload and it will be freed by them ):
    if ((MCL_NULL != store_data->payload_buffer)
            && ((STORE_DATA_FILE != store_data->type) && (STORE_DATA_CUSTOM != store_data->type) && (STORE_DATA_STREAM != store_data->type)))
    {
        MCL_FREE(store_data->payload_buffer);
    }

    // type based data destroy :
    if (STORE_DATA_TIME_SERIES == store_data->type)
    {
        MCL_DEBUG("Item type is time_series, calling its destroy function.");
        time_series_destroy((time_series_t **)&(store_data->data));
    }
    else if (STORE_DATA_EVENT_LIST == store_data->type)
    {
        MCL_DEBUG("Item type is event, calling its destroy function.");
        event_list_destroy((event_list_t **)&(store_data->data));
    }
    else if (STORE_DATA_FILE == store_data->type)
    {
        MCL_DEBUG("Item type is file, calling its destroy function.");
        file_destroy((file_t **)&(store_data->data));
    }
    else if (STORE_DATA_CUSTOM == store_data->type)
    {
        MCL_DEBUG("Item type is custom data, calling its destroy function.");
        custom_data_destroy((custom_data_t **)&(store_data->data));
    }
    else if (STORE_DATA_STREAM == store_data->type)
    {
        MCL_DEBUG("Item type is stream data, calling its destroy function.");
        stream_data_destroy((stream_data_t **)&(store_data->data));
    }
    else if (STORE_DATA_DATA_SOURCE_CONFIGURATION == store_data->type)
    {
        MCL_DEBUG("Item type is data source configuration, calling its destroy function.");
        data_source_configuration_destroy((data_source_configuration_t **)&(store_data->data));
    }
    else
    {
        MCL_FATAL("Received type of store list item is unknown!");
    }

    // clean up stream info :
    if (MCL_NULL != store_data->stream_info)
    {
        string_destroy(&(store_data->stream_info->tuple_subboundary));
        MCL_FREE(store_data->stream_info);
    }

    MCL_FREE(store_data);

    DEBUG_LEAVE("retVal = void");
}

static E_MCL_ERROR_CODE _compare_item_meta_of_event(void *data, const item_meta_payload_local_t *item_meta_payload)
{
    DEBUG_ENTRY("void *data = <%p>, const item_meta_payload_local_t *item_meta_payload = <%p>", data, item_meta_payload)

    store_data_t *store_data = data;

    if (STORE_DATA_EVENT_LIST == store_data->type)
    {
        event_list_t *event_list = store_data->data;

        if ((MCL_OK == string_compare_with_cstr(event_list->meta->payload.type, item_meta_payload->type))
                && (MCL_OK == string_compare_with_cstr(event_list->meta->payload.version, item_meta_payload->version)))
        {
            MCL_DEBUG("There is an event set already created with the same type and version in the list.");
            DEBUG_LEAVE("retVal = <%d>", MCL_OK);
            return MCL_OK;
        }
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_FAIL);
    return MCL_FAIL;
}

static mcl_bool_t _is_valid_version(const char *version)
{
    DEBUG_ENTRY("const char *version = <%p>", version)

    mcl_bool_t ok;
    mcl_size_t dot_index = 0;
    mcl_size_t version_length = string_util_strlen(version);

    // Search dot.
    ok = string_util_find(version, ".", &dot_index);

    // Make sure there is major version and minor version strings.
    ok = ok && (dot_index > 0 && dot_index < version_length - 1);

    // Make sure major version is a positive integer.
    ok = ok && _is_positive_integer(version, 0, dot_index);

    // Make sure minor version is a positive integer.
    ok = ok && _is_positive_integer(version, dot_index + 1, version_length);

    DEBUG_LEAVE("retVal = <%d>", ok);
    return ok;
}

static mcl_bool_t _is_positive_integer(const char *version, mcl_size_t start_index, mcl_size_t end_index)
{
    DEBUG_ENTRY("const char *version = <%p>, mcl_size_t start_index = <%u>, mcl_size_t end_index = <%u>", version, start_index, end_index)

    mcl_size_t index;
    mcl_bool_t status = MCL_TRUE;

    for (index = start_index; index < end_index; index++)
    {
        if (version[index] < '0'  || version[index] > '9')
        {
            status = MCL_FALSE;
        }
    }

    DEBUG_LEAVE("retVal = <%d>", status);
    return status;
}
