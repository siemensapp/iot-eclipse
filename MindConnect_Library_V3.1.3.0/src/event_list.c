/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     event_list.c
* @date     Apr 13, 2017
* @brief    Event list module implementation file.
*
************************************************************************/

#include "event_list.h"
#include "definitions.h"
#include "log_util.h"
#include "memory.h"
#include "mcl/mcl_event.h"

// Private Function Prototypes:
static E_MCL_ERROR_CODE _initialize_meta(event_list_t *event_list, const char *version);

E_MCL_ERROR_CODE event_list_initialize(const char *version, event_list_t **event_list)
{
    DEBUG_ENTRY("const char *version = <%p>, event_list_t **event_list = <%p>", version, event_list)

    E_MCL_ERROR_CODE code;

    // New event_list:
    if ((MCL_NULL == MCL_NEW(*event_list)) || (MCL_NULL == MCL_NEW((*event_list)->meta)))
    {
        if (MCL_NULL != *event_list)
        {
            MCL_FREE((*event_list)->meta);
            MCL_FREE(*event_list);
        }

        MCL_ERROR_RETURN(MCL_OUT_OF_MEMORY, "Memory couldn't be allocated for one of event set or event set meta or event set payload.");
    }

    (*event_list)->meta->content_id = MCL_NULL;
    (*event_list)->meta->type = MCL_NULL;
    (*event_list)->meta->version = MCL_NULL;
    (*event_list)->meta->details.routing = MCL_NULL;
    (*event_list)->meta->payload.type = MCL_NULL;
    (*event_list)->meta->payload.version = MCL_NULL;
    (*event_list)->events= MCL_NULL;

    code = _initialize_meta(*event_list, version);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, event_list_destroy(event_list), code, "Initializing event_list meta fields fails.");

    // Initialize event_t list.
    code = list_initialize(&(*event_list)->events);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, event_list_destroy(event_list), code, "List initialize fail for event_t list.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE event_list_add_event(event_t *event, event_list_t *event_list)
{
    DEBUG_ENTRY("event_t *event = <%p>, event_list_t *event_list = <%p>", event, event_list)

    E_MCL_ERROR_CODE code = list_add(event_list->events, event);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _initialize_meta(event_list_t *event_list, const char *version)
{
    DEBUG_ENTRY("event_list_t *event_list = <%p>, const char *version = <%p>", event_list, version)

    // Note that null check for version and event is already done by the caller of this function.

    // Set meta.type.
    E_MCL_ERROR_CODE code = string_initialize_static(meta_field_values[META_FIELD_TYPE_ITEM].buffer, meta_field_values[META_FIELD_TYPE_ITEM].length, &event_list->meta->type);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta type.");

    // Set meta.version.
    code = string_initialize_static(meta_field_values[META_FIELD_VERSION_CURRENT].buffer, meta_field_values[META_FIELD_VERSION_CURRENT].length, &event_list->meta->version);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta version.");

    // Set meta.payload.type.
    code = string_initialize_static(meta_field_values[META_FIELD_PAYLOAD_TYPE_BUSINESS_EVENT].buffer, meta_field_values[META_FIELD_PAYLOAD_TYPE_BUSINESS_EVENT].length,
                                        &(event_list->meta->payload.type));

    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta payload type.");

    // Set meta.payload.version.
    code = string_initialize_new(version, 0, &event_list->meta->payload.version);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "String initialize fail for meta payload version.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void event_list_destroy(event_list_t **event_list)
{
    DEBUG_ENTRY("event_list_t **event_list = <%p>", event_list)

    if (MCL_NULL != *event_list)
    {
        // meta
        if (MCL_NULL != (*event_list)->meta)
        {
            string_destroy(&((*event_list)->meta->content_id));
            string_destroy(&((*event_list)->meta->type));
            string_destroy(&((*event_list)->meta->version));
            string_destroy(&((*event_list)->meta->details.routing));
            string_destroy(&((*event_list)->meta->payload.type));
            string_destroy(&((*event_list)->meta->payload.version));
            MCL_FREE((*event_list)->meta);
        }

        // payload
        list_destroy_with_content(&(*event_list)->events, (list_item_destroy_callback)event_destroy);
        MCL_FREE(*event_list);
    }

    DEBUG_LEAVE("retVal = void");
}
