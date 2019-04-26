/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     custom_data.c
* @date     Jul 27, 2016
* @brief    Custom data module implementation file.
*
************************************************************************/

#include "custom_data.h"
#include "memory.h"
#include "log_util.h"
#include "definitions.h"
#include "mcl/mcl_custom_data.h"
#include "json_util.h"

// Private Function Prototypes:
static E_MCL_ERROR_CODE _initialize_custom_data_meta_fields(custom_data_t *custom_data, const char *type, const char *version, const char *routing);

E_MCL_ERROR_CODE custom_data_initialize(const char *version, const char *type, const char *routing, custom_data_t **custom_data)
{
    DEBUG_ENTRY("const char *version = <%s>, const char *type = <%s>, const char *routing = <%p>, custom_data_t **custom_data = <%p>", version, type, routing, custom_data)

    E_MCL_ERROR_CODE code;

    // New custom_data:
    MCL_NEW(*custom_data);
    ASSERT_CODE_MESSAGE(MCL_NULL != *custom_data, MCL_OUT_OF_MEMORY, "Memory can not be allocated for custom data.");

    // Initialize custom data struct members.
    code = _initialize_custom_data_meta_fields(*custom_data, type, version, routing);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, custom_data_destroy(custom_data), code, "custom_data initialization fail.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_custom_data_set_meta_details(mcl_custom_data_t *custom_data, const mcl_json_t *details)
{
    DEBUG_ENTRY("mcl_custom_data_t *custom_data = <%p>, const mcl_json_t *details = <%p>", custom_data, details)

    E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(custom_data);
    ASSERT_NOT_NULL(details);

    code = json_util_duplicate(details, MCL_TRUE, &custom_data->meta.payload.details.custom_details.details_json);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_custom_data_set_payload(mcl_custom_data_t *custom_data, const mcl_uint8_t *content, mcl_size_t content_size)
{
    DEBUG_ENTRY("mcl_custom_data_t *custom_data = <%p>, const mcl_uint8_t *content = <%p>, mcl_size_t content_size = <%u>", custom_data, content, content_size)

    ASSERT_NOT_NULL(custom_data);
    ASSERT_NOT_NULL(content);
    ASSERT_CODE_MESSAGE(0 < content_size, MCL_INVALID_PARAMETER, "content_size is not bigger than zero.");

    custom_data->payload.size = content_size;
    custom_data->payload.buffer = (mcl_uint8_t *)content;

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void custom_data_destroy(custom_data_t **custom_data)
{
    DEBUG_ENTRY("custom_data_t **custom_data = <%p>", custom_data)

    if (MCL_NULL != *custom_data)
    {
        string_destroy(&((*custom_data)->meta.content_id));
        string_destroy(&((*custom_data)->meta.type));
        string_destroy(&((*custom_data)->meta.version));
        string_destroy(&((*custom_data)->meta.details.routing));
        string_destroy(&((*custom_data)->meta.payload.type));
        string_destroy(&((*custom_data)->meta.payload.version));
        json_util_destroy(&((*custom_data)->meta.payload.details.custom_details.details_json));
        MCL_FREE(*custom_data);
    }

    DEBUG_LEAVE("retVal = void");
}

// Private Functions:
static E_MCL_ERROR_CODE _initialize_custom_data_meta_fields(custom_data_t *custom_data, const char *type, const char *version, const char *routing)
{
    DEBUG_ENTRY("custom_data_t *custom_data = <%p>, const char *type = <%s>, const char *version = <%s>, const char *routing = <%p>", custom_data, type, version, routing)

    E_MCL_ERROR_CODE code;

    custom_data->meta.content_id = MCL_NULL;
    custom_data->meta.type = MCL_NULL;
    custom_data->meta.version = MCL_NULL;
    custom_data->meta.details.routing = MCL_NULL;
    custom_data->meta.payload.type = MCL_NULL;
    custom_data->meta.payload.version = MCL_NULL;
    custom_data->meta.payload.details.custom_details.details_json = MCL_NULL;
    custom_data->payload.size = 0;
    custom_data->payload.buffer = MCL_NULL;

    // Set meta.type.
    code = string_initialize_static(meta_field_values[META_FIELD_TYPE_ITEM].buffer, meta_field_values[META_FIELD_TYPE_ITEM].length, &custom_data->meta.type);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta_type.");

    // Set meta.version.
    code = string_initialize_static(meta_field_values[META_FIELD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_VERSION_CURRENT].length, &custom_data->meta.version);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta_version.");

    // Set routing which is optional.
    if (MCL_NULL != routing)
    {
        code = string_initialize_new(routing, 0, &custom_data->meta.details.routing);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for routing.");
    }

    // Set Fill meta_payload_type.
    code = string_initialize_new(type, 0, &(custom_data->meta.payload.type));
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta_payload_type.");

    // Set meta.payload.version.
    code = string_initialize_new(version, 0, &(custom_data->meta.payload.version));
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta_payload_version.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}
