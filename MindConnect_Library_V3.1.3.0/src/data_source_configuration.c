/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     data_source_configuration.c
* @date     Nov 13, 2017
* @brief    Data source configuration module implementation file.
*
************************************************************************/

#include "mcl/mcl_data_source_configuration.h"
#include "data_source_configuration.h"
#include "log_util.h"
#include "memory.h"
#include "random.h"
#include "json_util.h"
#include "definitions.h"

// Private Function Prototypes:
static E_MCL_ERROR_CODE _initialize_meta(const char *version, data_source_configuration_t *data_source_configuration);
static E_MCL_ERROR_CODE _initialize_payload(data_source_configuration_t *data_source_configuration);
static void _destroy_data_source(data_source_t **data_source);
static void _destroy_data_point(data_point_t **data_point);

E_MCL_ERROR_CODE data_source_configuration_initialize(const char *version, data_source_configuration_t **data_source_configuration)
{
	DEBUG_ENTRY("const char *version = <%p>, data_source_configuration_t **data_source_configuration = <%p>", version, data_source_configuration)

	E_MCL_ERROR_CODE code;

    // New data_source_configuration:
    MCL_NEW(*data_source_configuration);
    ASSERT_CODE_MESSAGE(MCL_NULL != *data_source_configuration, MCL_OUT_OF_MEMORY, "Not enough memory left to create the data_source_configuration handle!");

    (*data_source_configuration)->meta.content_id = MCL_NULL;
    (*data_source_configuration)->meta.type = MCL_NULL;
    (*data_source_configuration)->meta.version = MCL_NULL;
    (*data_source_configuration)->meta.details.routing = MCL_NULL;
    (*data_source_configuration)->meta.payload.type = MCL_NULL;
    (*data_source_configuration)->meta.payload.version = MCL_NULL;
    (*data_source_configuration)->payload.configuration_id = MCL_NULL;
    (*data_source_configuration)->payload.data_sources = MCL_NULL;

    code = _initialize_meta(version, *data_source_configuration);

    (MCL_OK == code) && (code = _initialize_payload(*data_source_configuration));

    if (MCL_OK != code)
    {
        data_source_configuration_destroy(data_source_configuration);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_data_source_configuration_add_data_source(mcl_data_source_configuration_t *data_source_configuration, const char *name, const char *description,
        mcl_json_t *custom_data, mcl_data_source_t **data_source)
{
    DEBUG_ENTRY("mcl_data_source_configuration_t *data_source_configuration = <%p>, const char *name = <%p>, const char *description = <%p>, mcl_json_t *custom_data = <%p>, mcl_data_source_t **data_source = <%p>",
                data_source_configuration, name, description, custom_data, data_source)
	
	E_MCL_ERROR_CODE code;
	mcl_bool_t ok;

    // Description and custom_data parameters are optional fields. Thats why there are no null checks for them.
    ASSERT_NOT_NULL(data_source_configuration);
    ASSERT_NOT_NULL(name);
    ASSERT_NOT_NULL(data_source);

    ASSERT_CODE_MESSAGE(MCL_NULL != MCL_NEW(*data_source), MCL_OUT_OF_MEMORY, "Not enough memory to allocate new data_source.");

    (*data_source)->name = MCL_NULL;
    (*data_source)->description = MCL_NULL;
    (*data_source)->data_points = MCL_NULL;
    (*data_source)->custom_data = MCL_NULL;

    // Initialize name.
    ok = MCL_OK == (code = string_initialize_new(name, 0, &(*data_source)->name));

    // Initialize description.
    if (MCL_NULL != description)
    {
        ok = ok && (MCL_OK == (code = string_initialize_new(description, 0, &(*data_source)->description)));
    }

    // Initialize custom_data.
    if (MCL_NULL != custom_data)
    {
        ok = ok && (MCL_OK == (code = json_util_duplicate(custom_data, MCL_TRUE, &(*data_source)->custom_data)));
    }

    // Initialize data_points.
    ok = ok && (MCL_OK == (code = list_initialize(&(*data_source)->data_points)));

    // Add data_source to the list.
    ok = ok && (MCL_OK == (code = list_add(data_source_configuration->payload.data_sources, *data_source)));

    // error check:
    if (MCL_FALSE == ok)
    {
        MCL_DEBUG("String or list initialize operation(s) has been failed! Clearing strings.");
        _destroy_data_source(data_source);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_data_source_configuration_add_data_point(mcl_data_source_t *data_source, const char *id, const char *name, const char *description, const char *type,
        const char *unit, mcl_json_t *custom_data)
{
    DEBUG_ENTRY("mcl_data_source_t *data_source = <%p>, const char *id = <%p>, const char *name = <%p>, const char *description = <%p>, const char *type = <%p>, const char *unit = <%p>, mcl_json_t *custom_data = <%p>",
                data_source, id, name, description, type, unit, custom_data)

	E_MCL_ERROR_CODE code;
	data_point_t *data_point = MCL_NULL;
	mcl_bool_t ok;

    // Description and custom_data parameters are optional fields. Thats why there are no null checks for them.
    ASSERT_NOT_NULL(data_source);
    ASSERT_NOT_NULL(id);
    ASSERT_NOT_NULL(name);
    ASSERT_NOT_NULL(type);
    ASSERT_NOT_NULL(unit);

    ASSERT_CODE_MESSAGE(MCL_NULL != MCL_NEW(data_point), MCL_OUT_OF_MEMORY, "Not enough memory to allocate new data_point.");

    data_point->id = MCL_NULL;
    data_point->name = MCL_NULL;
    data_point->description = MCL_NULL;
    data_point->type = MCL_NULL;
    data_point->unit = MCL_NULL;
    data_point->custom_data = MCL_NULL;

    // Initialize id.
    ok = MCL_OK == (code = string_initialize_new(id, 0, &data_point->id));

    // Initialize name.
    ok = ok && (MCL_OK == (code = string_initialize_new(name, 0, &data_point->name)));

    // Initialize description.
    if (MCL_NULL != description)
    {
        ok = ok && (MCL_OK == (code = string_initialize_new(description, 0, &data_point->description)));
    }

    // Initialize type.
    ok = ok && (MCL_OK == (code = string_initialize_new(type, 0, &data_point->type)));

    // Initialize unit.
    ok = ok && (MCL_OK == (code = string_initialize_new(unit, 0, &data_point->unit)));

    // Initialize custom_data.
    if (MCL_NULL != custom_data)
    {
        ok = ok && (MCL_OK == (code = json_util_duplicate(custom_data, MCL_TRUE, &data_point->custom_data)));
    }

    // Add data_point to the list.
    ok = ok && (MCL_OK == (code = list_add(data_source->data_points, data_point)));

    // error check:
    if (MCL_FALSE == ok)
    {
        MCL_DEBUG("String or list initialize operation(s) has been failed! Clearing strings.");
        _destroy_data_point(&data_point);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_data_source_configuration_get_id(mcl_data_source_configuration_t *data_source_configuration, char **id)
{
	DEBUG_ENTRY("mcl_data_source_configuration_t *data_source_configuration = <%p>, char **id = <%p>", data_source_configuration, id)

	E_MCL_ERROR_CODE code;
	string_t *configuration_id_local = MCL_NULL;

    ASSERT_NOT_NULL(data_source_configuration);
    ASSERT_NOT_NULL(id);

    code = string_initialize(data_source_configuration->payload.configuration_id, &configuration_id_local);
    ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_OUT_OF_MEMORY, "Not enough memory left to create configuration_id!");

    *id = configuration_id_local->buffer;

    // Free string and remain buffer
    MCL_FREE(configuration_id_local);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void data_source_configuration_destroy(data_source_configuration_t **data_source_configuration)
{
    DEBUG_ENTRY("data_source_configuration_t **data_source_configuration = <%p>", data_source_configuration)

    if (MCL_NULL != *data_source_configuration)
    {
        string_destroy(&((*data_source_configuration)->meta.content_id));
        string_destroy(&((*data_source_configuration)->meta.type));
        string_destroy(&((*data_source_configuration)->meta.version));
        string_destroy(&((*data_source_configuration)->meta.payload.type));
        string_destroy(&((*data_source_configuration)->meta.payload.version));
        string_destroy(&((*data_source_configuration)->payload.configuration_id));
        list_destroy_with_content(&(*data_source_configuration)->payload.data_sources, (list_item_destroy_callback)_destroy_data_source);
        MCL_FREE(*data_source_configuration);
    }

    DEBUG_LEAVE("retVal = void");
}

static E_MCL_ERROR_CODE _initialize_meta(const char *version, data_source_configuration_t *data_source_configuration)
{
    DEBUG_ENTRY("const char *version = <%p>, data_source_configuration_t *data_source_configuration = <%p>", version, data_source_configuration)

    // Set meta.type.
    E_MCL_ERROR_CODE code = string_initialize_static(meta_field_values[META_FIELD_TYPE_ITEM].buffer, meta_field_values[META_FIELD_TYPE_ITEM].length, &data_source_configuration->meta.type);

    // Set meta.version.
    (MCL_OK == code) && (code = string_initialize_static(meta_field_values[META_FIELD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_VERSION_CURRENT].length, &data_source_configuration->meta.version));

    // Set meta.payload.type.
    (MCL_OK == code) && (code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_TYPE_DATA_SOURCE_CONFIGURATION].buffer, meta_field_values[META_FIELD_PAYLOAD_TYPE_DATA_SOURCE_CONFIGURATION].length,
                                    &data_source_configuration->meta.payload.type));

    // Set meta.payload.version.
    (MCL_OK == code) && (code = string_initialize_new(version, 0, &data_source_configuration->meta.payload.version));

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _initialize_payload(data_source_configuration_t *data_source_configuration)
{
    DEBUG_ENTRY("data_source_configuration_t *data_source_configuration = <%p>", data_source_configuration)

    // Set configuration_id.
    E_MCL_ERROR_CODE code = random_generate_guid(&data_source_configuration->payload.configuration_id);

    // Initialize data sources.
    (MCL_OK == code) && (code = list_initialize(&data_source_configuration->payload.data_sources));

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static void _destroy_data_source(data_source_t **data_source)
{
    DEBUG_ENTRY("data_source_t **data_source = <%p>", data_source)

    string_destroy(&(*data_source)->name);
    string_destroy(&(*data_source)->description);
    list_destroy_with_content(&(*data_source)->data_points, (list_item_destroy_callback)_destroy_data_point);
    json_util_destroy(&(*data_source)->custom_data);
    MCL_FREE((*data_source));

    DEBUG_LEAVE("retVal = void");
}

static void _destroy_data_point(data_point_t **data_point)
{
    DEBUG_ENTRY("data_point_t **data_point = <%p>", data_point)

    string_destroy(&(*data_point)->id);
    string_destroy(&(*data_point)->name);
    string_destroy(&(*data_point)->description);
    string_destroy(&(*data_point)->type);
    string_destroy(&(*data_point)->unit);
    json_util_destroy(&(*data_point)->custom_data);
    MCL_FREE(*data_point);

    DEBUG_LEAVE("retVal = void");
}
