/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     time_series.c
 * @date     Jul 11, 2016
 * @brief    Time series module implementation file.
 *
 ************************************************************************/

#include "time_series.h"
#include "definitions.h"
#include "memory.h"
#include "log_util.h"
#include "mcl/mcl_time_series.h"
#include "time_util.h"

// Private Function Prototypes:
static E_MCL_ERROR_CODE _initialize_meta(const char *version, const char *configuration_id, const char *routing, time_series_t *time_series);
static void _destroy_value_set_value(time_series_value_t **value);
static void _destroy_value_set(time_series_value_set_t **value_set);

E_MCL_ERROR_CODE time_series_initialize(const char *version, const char *configuration_id, const char *routing, time_series_t **time_series)
{
	DEBUG_ENTRY("const char *version = <%s>, const char *configuration_id = <%s>, const char *routing = <%p>, time_series_t **time_series = <%p>", version, configuration_id,
		routing, time_series)

	E_MCL_ERROR_CODE code;
    MCL_NEW(*time_series);
    ASSERT_CODE_MESSAGE(MCL_NULL != *time_series, MCL_OUT_OF_MEMORY, "Memory couldn't be allocated for time_series.");

    (*time_series)->meta.content_id = MCL_NULL;
    (*time_series)->meta.type = MCL_NULL;
    (*time_series)->meta.version = MCL_NULL;
    (*time_series)->meta.details.routing = MCL_NULL;
    (*time_series)->meta.payload.type = MCL_NULL;
    (*time_series)->meta.payload.version = MCL_NULL;
    (*time_series)->meta.payload.details.time_series_details.configuration_id = MCL_NULL;
    (*time_series)->payload.value_sets = MCL_NULL;

    code = _initialize_meta(version, configuration_id, routing, *time_series);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, time_series_destroy(time_series), code, "Initializing time series meta fields fails.");

    // Initialize value_sets.
    code = list_initialize(&(*time_series)->payload.value_sets);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, time_series_destroy(time_series), code, "Initialization failed for time_series->time_series_payload.value_sets.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_time_series_new_value_set(mcl_time_series_t *time_series, const char *timestamp, mcl_time_series_value_set_t **value_set)
{
	DEBUG_ENTRY("mcl_time_series_t *time_series = <%p>, const char *timestamp = <%p>, mcl_time_series_value_set_t **value_set = <%p>", time_series, timestamp, value_set)
	
	E_MCL_ERROR_CODE code;
    ASSERT_NOT_NULL(time_series);
    ASSERT_NOT_NULL(timestamp);
    ASSERT_NOT_NULL(value_set);

    // Validate timestamp.
    ASSERT_CODE_MESSAGE(time_util_validate_timestamp(timestamp), MCL_INVALID_PARAMETER, "Timestamp validation failed.");

    MCL_NEW(*value_set);
    ASSERT_CODE_MESSAGE(MCL_NULL != *value_set, MCL_OUT_OF_MEMORY, "Memory couldn't be allocated for value_set.");

    // Initialize timestamp.
    code = string_initialize_new(timestamp, 0, &(*value_set)->timestamp);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, _destroy_value_set(value_set), code, "String initialize fail for timestamp.");

    // Initialize values.
    code = list_initialize(&(*value_set)->values);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, _destroy_value_set(value_set), code, "List initialize fail for values.");

    // Link value_set to a parent time_series
    (*value_set)->parent = time_series;

    // Add new value_set to value_sets.
    MCL_DEBUG("New value_set will be added to value_sets.");
    code = list_add(time_series->payload.value_sets, *value_set);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, _destroy_value_set(value_set), code, "Adding value_set to the list failed.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_time_series_add_value(mcl_time_series_value_set_t *value_set, const char *data_point_id, const char *value, const char *quality_code)
{
    DEBUG_ENTRY("mcl_time_series_value_set_t *value_set = <%p>, const char *data_point_id = <%p>, const char *value = <%p>, const char *quality_code = <%p>", value_set,
                data_point_id, value, quality_code)

	E_MCL_ERROR_CODE code;
	time_series_value_t *value_local;

    ASSERT_NOT_NULL(value_set);
    ASSERT_NOT_NULL(data_point_id);
    ASSERT_NOT_NULL(value);
    ASSERT_NOT_NULL(quality_code);

    MCL_NEW(value_local);
    ASSERT_CODE_MESSAGE(MCL_NULL != value_local, MCL_OUT_OF_MEMORY, "Not enough memory to allocate new value_local.");

    code = string_initialize_new(data_point_id, 0, &value_local->data_point_id);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, _destroy_value_set_value(&value_local), code, "String initialize fail for data_point_id");

    code = string_initialize_new(value, 0, &value_local->value);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, _destroy_value_set_value(&value_local), code, "String initialize fail for value");

    code = string_initialize_new(quality_code, 0, &value_local->quality_code);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, _destroy_value_set_value(&value_local), code, "String initialize fail for quality_code");

    code = list_add(value_set->values, value_local);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, _destroy_value_set_value(&value_local), code, "Adding value to the list failed.");

    MCL_DEBUG("New value added into values array. Current index = <%d>", value_set->values->count);
    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void time_series_destroy(time_series_t **time_series)
{
    DEBUG_ENTRY("time_series_t **time_series = <%p>", time_series)

    if (MCL_NULL != *time_series)
    {
        string_destroy(&((*time_series)->meta.content_id));
        string_destroy(&((*time_series)->meta.type));
        string_destroy(&((*time_series)->meta.version));
        string_destroy(&((*time_series)->meta.details.routing));
        string_destroy(&((*time_series)->meta.payload.type));
        string_destroy(&((*time_series)->meta.payload.version));
        string_destroy(&((*time_series)->meta.payload.details.time_series_details.configuration_id));
        list_destroy_with_content(&(*time_series)->payload.value_sets, (list_item_destroy_callback)_destroy_value_set);
        MCL_FREE(*time_series);
    }

    DEBUG_LEAVE("retVal = void");
}

// Private Functions:
static E_MCL_ERROR_CODE _initialize_meta(const char *version, const char *configuration_id, const char *routing, time_series_t *time_series)
{
    DEBUG_ENTRY("const char *version = <%s>, const char *configuration_id = <%s>, const char *routing = <%p>, time_series_t *time_series = <%p>", version, configuration_id,
                routing, time_series)

    // Note that null check for version, configuration_id and time_series is already done by the caller of this function.

    // Set meta.type.
    E_MCL_ERROR_CODE code = string_initialize_static(meta_field_values[META_FIELD_TYPE_ITEM].buffer, meta_field_values[META_FIELD_TYPE_ITEM].length, &time_series->meta.type);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta type.");

    // Set meta.version.
    code = string_initialize_static(meta_field_values[META_FIELD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_VERSION_CURRENT].length, &time_series->meta.version);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta version.");

    // Set meta.details.routing.
    if (MCL_NULL != routing)
    {
        code = string_initialize_new(routing, 0, &time_series->meta.details.routing);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for routing");
    }

    // Set meta.payload.type.
    code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_TYPE_TIME_SERIES].buffer, meta_field_values[META_FIELD_PAYLOAD_TYPE_TIME_SERIES].length,
                                    &(time_series->meta.payload.type));
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta.payload.type.");

    // Set meta.payload.version.
    code = string_initialize_new(version, 0, &time_series->meta.payload.version);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta.payload.version.");

    // Set configuration_id.
    code = string_initialize_new(configuration_id, 0, &time_series->meta.payload.details.time_series_details.configuration_id);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for configuration id.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static void _destroy_value_set_value(time_series_value_t **value)
{
    DEBUG_ENTRY("time_series_value_t **value = <%p>", value)

    string_destroy(&(*value)->data_point_id);
    string_destroy(&(*value)->value);
    string_destroy(&(*value)->quality_code);
    MCL_FREE(*value);

    DEBUG_LEAVE("retVal = void");
}

static void _destroy_value_set(time_series_value_set_t **value_set)
{
    DEBUG_ENTRY("time_series_value_set_t **value_set = <%p>", value_set)

    string_destroy(&(*value_set)->timestamp);
    list_destroy_with_content(&(*value_set)->values, (list_item_destroy_callback)_destroy_value_set_value);
    MCL_FREE(*value_set);

    DEBUG_LEAVE("retVal = void");
}

