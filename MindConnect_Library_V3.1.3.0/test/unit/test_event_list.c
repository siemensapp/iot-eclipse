/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_event_list.c
* @date     May 17, 2017
* @brief    This file implements all test cases of event_list module.
*
************************************************************************/

#include "string_type.h"
#include "string_util.h"
#include "log_util.h"
#include "data_types.h"
#include "mcl/mcl_common.h"
#include "memory.h"
#include "list.h"
#include "unity.h"
#include "definitions.h"
#include "mcl/mcl_event.h"
#include "event.h"
#include "event_list.h"
#include "random.h"
#include "mcl_json_util.h"
#include "json_util.h"
#include "security.h"
#include "security_libcrypto.h"
#include "time_util.h"

E_MCL_ERROR_CODE code;

const char *meta_type = "item";
const char *meta_version = "1.0";
const char *business_event_type = "businessEvent";

const char *payload_version = "0.1";
const char *correlation_id = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";

const char *timestamp = "2016-04-26T08:06:25.317Z";
E_MCL_EVENT_SEVERITY severity = MCL_EVENT_SEVERITY_ERROR;
const char *description = "nice event";
const char *event_payload_type = "thresholdViolation";
const char *event_payload_version = "1.1";

const char *details_name = "sample_name";
const char *details_value = "sample_value";

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Created *event_list.
 * WHEN  : event_list is initialized with businessEvent type.
 * THEN  : It gives the initialized event_list data struct.
 */
void test_initialize_001(void)
{
    event_list_t *event_list = MCL_NULL;
    code = event_list_initialize(payload_version, &event_list);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for event_list.");

    TEST_ASSERT_NOT_NULL_RETURN(event_list);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(meta_type, event_list->meta->type->buffer, "meta_type fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(meta_version, event_list->meta->version->buffer, "meta_version fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(business_event_type, event_list->meta->payload.type->buffer, "meta_payload_type fail");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_version, event_list->meta->payload.version->buffer, "meta_payload_version fail");

    event_list_destroy(&event_list);
}

/**
 * GIVEN : Initialized event and event_list.
 * WHEN  : User requests to add event to event_list.
 * THEN  : event is added to event_list.
 */
void test_add_event_001(void)
{
    event_list_t *event_list = MCL_NULL;
    event_list_initialize(payload_version, &event_list);

    // Initialize event.
    event_t *event = MCL_NULL;
    code = event_initialize(event_list->meta, event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Set optional correlation id.
    code = mcl_event_set_option(event, MCL_EVENT_OPTION_CORRELATION_ID, correlation_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Correlation id could not be set.");

    code = event_list_add_event(event, event_list);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "event_list_add_event() failed.");

    // Get the added payload of event from event_list.
    event_t *event_of_event_list = (event_t *)(event_list->events->head->data);

    // only one event payload added.
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, event_list->events->count, "added event payload count is not 1.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(event->payload->correlation_id->buffer, event_of_event_list->payload->correlation_id->buffer, "correlation_id fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event->payload->timestamp->buffer, event_of_event_list->payload->timestamp->buffer, "timestamp fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event->payload->type->buffer, event_of_event_list->payload->type->buffer, "type fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event->payload->version->buffer, event_of_event_list->payload->version->buffer, "version fail.");

    event_list_destroy(&event_list);
}

/**
 * GIVEN : Two initialized event and event_list.
 * WHEN  : User requests to add events to event_list.
 * THEN  : events are added to event_list.
 */
void test_add_event_002(void)
{
    // Initialize event_list.
    event_list_t *event_list = MCL_NULL;
    event_list_initialize(payload_version, &event_list);

    // Initialize event_1.
    event_t *event_1 = MCL_NULL;
    code = event_initialize(event_list->meta, event_payload_type, event_payload_version, severity, timestamp, &event_1);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Set optional description for event_1.
    code = mcl_event_set_option(event_1, MCL_EVENT_OPTION_DESCRIPTION, description);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Description could not be set.");

    // Initialize event_2.
    event_t *event_2 = MCL_NULL;
    code = event_initialize(event_list->meta, event_payload_type, "9.9", severity, timestamp, &event_2);

    // Add event 1 to event list.
    code = event_list_add_event(event_1, event_list);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "event_list_add_event() failed for event_1.");

    // Add event 2 to event list.
    code = event_list_add_event(event_2, event_list);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "event_list_add_event() failed for event_2.");

    list_reset(event_list->events);

    // Get the first added payload of event from event_list.
    event_t *event_of_event_list = (event_t *)(list_next(event_list->events)->data);

    // Two event payloads added.
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, event_list->events->count, "added event payload count is not 2.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_1->payload->timestamp->buffer, event_of_event_list->payload->timestamp->buffer, "timestamp fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_1->payload->description->buffer, event_of_event_list->payload->description->buffer, "description fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_1->payload->type->buffer, event_of_event_list->payload->type->buffer, "type fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_1->payload->version->buffer, event_of_event_list->payload->version->buffer, "version fail.");

    // Get the second added payload of event from event_list.
    event_of_event_list = (event_t *)(list_next(event_list->events)->data);

    // Make sure the second event is added successfully.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_2->payload->version->buffer, event_of_event_list->payload->version->buffer, "version fail.");

    event_list_destroy(&event_list);
}

/**
 * GIVEN : Initialized event_list.
 * WHEN  : User requests to destroy event_list.
 * THEN  : User expects the event_list and everything in it have been freed and pointer is NULL
 */
void test_destroy_001(void)
{
    // Prepare event to add to event_list.
    // Initialize event_list.
    event_list_t *event_list = MCL_NULL;
    event_list_initialize(payload_version, &event_list);

    // Initialize event.
    event_t *event = MCL_NULL;
    E_MCL_ERROR_CODE code = event_initialize(event_list->meta, event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Set optional correlation id.
    code = mcl_event_set_option(event, MCL_EVENT_OPTION_CORRELATION_ID, correlation_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Correlation id could not be set.");

    // Set optional description.
    code = mcl_event_set_option(event, MCL_EVENT_OPTION_DESCRIPTION, description);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Description could not be set.");

    // Create details json object.
    mcl_json_t *details = MCL_NULL;
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for json.");

    code = mcl_json_util_add_string(details, details_name, details_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");

    // Set optional details.
    code = mcl_event_set_option(event, MCL_EVENT_OPTION_DETAILS, details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Details could not be set.");

    code = event_list_add_event(event, event_list);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "event_list_add_event() failed.");

    event_list_destroy(&event_list);

    TEST_ASSERT_NULL_MESSAGE(event_list, "After destroy operation, event_list is not NULL!");

    json_util_destroy(&details);
}
