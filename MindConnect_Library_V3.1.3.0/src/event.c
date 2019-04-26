/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     event.c
* @date     Jul 27, 2016
* @brief    Event module implementation file.
*
************************************************************************/

#include "definitions.h"
#include "event.h"
#include "log_util.h"
#include "memory.h"
#include "random.h"
#include "json_util.h"
#include "time_util.h"
#include "mcl/mcl_event.h"

E_MCL_ERROR_CODE event_initialize(item_meta_t *meta, const char* type, const char *version, mcl_int32_t severity, const char* timestamp, event_t **event)
{
    DEBUG_ENTRY("item_meta_t *meta = <%p>, const char *type = <%p>, const char *version = <%p>, mcl_int32_t severity = <%d>, const char *timestamp = <%p>, event_t **event = <%p>", meta, type, version, severity, timestamp, event)

    E_MCL_ERROR_CODE code;

	if (MCL_NULL == MCL_NEW(*event))
	{
		MCL_ERROR_RETURN(MCL_OUT_OF_MEMORY, "Memory couldn't be allocated for event.");
	}

	(*event)->meta = meta;

	if (MCL_NULL == MCL_NEW((*event)->payload))
	{
		MCL_FREE(*event);
		MCL_ERROR_RETURN(MCL_OUT_OF_MEMORY, "Memory couldn't be allocated for event payload.");
	}

    // Event payload.
    (*event)->payload->id = MCL_NULL;
    (*event)->payload->correlation_id = MCL_NULL;
    (*event)->payload->timestamp = MCL_NULL;
    (*event)->payload->description = MCL_NULL;
    (*event)->payload->type = MCL_NULL;
    (*event)->payload->version = MCL_NULL;
    (*event)->payload->details = MCL_NULL;

    // Set id (Unique identifier of the event).
    code = random_generate_guid(&(*event)->payload->id);

    // Set type.
    (MCL_OK == code) && (code = string_initialize_new(type, 0, &(*event)->payload->type));

    // Set version.
    (MCL_OK == code) && (code = string_initialize_new(version, 0, &(*event)->payload->version));

    // Set severity.
    (*event)->payload->severity = severity;

    // Set timestamp.
    (MCL_OK == code) && (code = string_initialize_new(timestamp, 0, &(*event)->payload->timestamp));

    // error check:
    if (MCL_OK != code)
    {
        MCL_DEBUG("payload initialize operation(s) has been failed!");
        event_destroy(event);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_event_set_option(mcl_event_t *event, E_MCL_EVENT_OPTION option, const void *value)
{
    DEBUG_ENTRY("mcl_event_t *event = <%p>, E_MCL_EVENT_OPTION option = <%d>, const void *value = <%p>", event, option, value)

    E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(event);
    ASSERT_NOT_NULL(value);

    switch (option)
    {
        case MCL_EVENT_OPTION_CORRELATION_ID:
            string_destroy(&event->payload->correlation_id);
            code = string_initialize_new((const char *)value, 0, &event->payload->correlation_id);
            break;
        case MCL_EVENT_OPTION_DESCRIPTION:
            string_destroy(&event->payload->description);
            code = string_initialize_new((const char *)value, 0, &event->payload->description);
            break;
        case MCL_EVENT_OPTION_DETAILS:
            json_util_destroy(&event->payload->details);
            code = json_util_duplicate((const json_t *)value, MCL_TRUE, &event->payload->details);
            break;
        default:
            code = MCL_INVALID_PARAMETER;
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

void event_destroy(event_t **event)
{
    DEBUG_ENTRY("event_t **event = <%p>", event)

    if (MCL_NULL != *event)
    {
        // payload
        string_destroy(&((*event)->payload->id));
        string_destroy(&((*event)->payload->correlation_id));
        string_destroy(&((*event)->payload->timestamp));
        string_destroy(&((*event)->payload->description));
        string_destroy(&((*event)->payload->type));
        string_destroy(&((*event)->payload->version));
        json_util_destroy(&((*event)->payload->details));
        MCL_FREE((*event)->payload);
        MCL_FREE(*event);
    }

    DEBUG_LEAVE("retVal = void");
}
