/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_event.c
* @date     Apr 11, 2017
* @brief    This file implements all test cases of mcl_event module.
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
#include "random.h"
#include "mcl_json_util.h"
#include "json_util.h"
#include "event_list.h"
#include "security.h"
#include "security_libcrypto.h"
#include "time_util.h"

E_MCL_ERROR_CODE code;

const char *meta_type = "item";
const char *meta_version = "1.0";
const char *payload_version = "0.1";
const char *correlation_id = "6ba7b810-9dad-11d1-80b4-00c04fd430c8";

const char *timestamp = "2016-04-26T08:06:25.317Z";
E_MCL_EVENT_SEVERITY severity = MCL_EVENT_SEVERITY_ERROR;
const char *description = "nice event";
const char *event_payload_type = "thresholdViolation";
const char *event_payload_version = "1.0.1";

const char *details_name = "message";
const char *details_value = "Critical oscillation level";

const char *business_event_type = "businessEvent";

event_t *event = MCL_NULL;
event_list_t *event_list = MCL_NULL;

void setUp(void)
{
    code = event_list_initialize(payload_version, &event_list);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event list.");
}

void tearDown(void)
{
    event_list_destroy(&event_list);
}

/**
 * GIVEN : Created *event.
 * WHEN  : event_initialize() is called.
 * THEN  : MCL_OK is returned and it gives the initialized event data struct with mandatory fields.
 */
void test_initialize_001(void)
{
    // Initialize event.
    code = event_initialize(event_list->meta, event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    TEST_ASSERT_NOT_NULL_RETURN(event);
    TEST_ASSERT_NOT_NULL_RETURN(event->meta);

    // Check meta part.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("item", event->meta->type->buffer, "event meta type fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("1.0", event->meta->version->buffer, "event meta version fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(business_event_type, event->meta->payload.type->buffer, "event meta payload type fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_version, event->meta->payload.version->buffer, "event meta payload version fail.");

    // Check payload part.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(timestamp, event->payload->timestamp->buffer, "event timestamp fail.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_EVENT_SEVERITY_ERROR, event->payload->severity, "event severity fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_payload_type, event->payload->type->buffer, "event payload type fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(event_payload_version, event->payload->version->buffer, "event payload version fail.");

    event_destroy(&event);
}

/**
* GIVEN : Initialized event.
* WHEN  : mcl_event_set_option() is called with null correlation id.
* THEN  : MCL_TRIGGERED_WITH_NULL is returned.
*/
void test_set_option_001(void)
{
    event_t event;

    code = mcl_event_set_option(&event, MCL_EVENT_OPTION_CORRELATION_ID, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_event_set_option() unexpected return error!");
}

/**
* GIVEN : Initialized event.
* WHEN  : mcl_event_set_option() is called with null description.
* THEN  : MCL_TRIGGERED_WITH_NULL is returned.
*/
void test_set_option_002(void)
{
    event_t event;

    code = mcl_event_set_option(&event, MCL_EVENT_OPTION_DESCRIPTION, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_event_set_option() unexpected return error!");
}

/**
* GIVEN : Initialized event.
* WHEN  : mcl_event_set_option() is called with null details.
* THEN  : MCL_TRIGGERED_WITH_NULL is returned.
*/
void test_set_option_003(void)
{
    event_t event;

    code = mcl_event_set_option(&event, MCL_EVENT_OPTION_DETAILS, MCL_NULL);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "mcl_event_set_option() unexpected return error!");
}

/**
* GIVEN : Initialized event.
* WHEN  : mcl_event_set_option() is called.
* THEN  : MCL_OK is returned and optional fields are set.
*/
void test_set_option_004(void)
{
    // Initialize event.
    code = event_initialize(event_list->meta, event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    // Check correlation id.
    code = mcl_event_set_option(event, MCL_EVENT_OPTION_CORRELATION_ID, correlation_id);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_option() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(correlation_id, event->payload->correlation_id->buffer, "Correlation id could not be set.");

    // Check description.
    code = mcl_event_set_option(event, MCL_EVENT_OPTION_DESCRIPTION, description);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_option() unexpected return error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(description, event->payload->description->buffer, "Description could not be set.");

    // Check details.
    mcl_json_t *details = MCL_NULL;
    code = mcl_json_util_initialize(MCL_JSON_OBJECT, &details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for json.");

    code = mcl_json_util_add_string(details, details_name, details_value);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "String could not be added to the json object.");

    code = mcl_event_set_option(event, MCL_EVENT_OPTION_DETAILS, details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "mcl_event_set_option() unexpected return error!");

    char *actual_details = MCL_NULL;
    code = json_util_to_string(event->payload->details, &actual_details);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Json could not be converted to string.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE("{\"message\":\"Critical oscillation level\"}", actual_details, "Details could not be set.");

    event_destroy(&event);
    json_util_destroy(&details);
    MCL_FREE(actual_details);
}

/**
* GIVEN : Initialized event.
* WHEN  : mcl_event_set_option() is called with an undefined option.
* THEN  : MCL_INVALID_PARAMETER is returned.
*/
void test_set_option_005(void)
{
    event_t event;

    const char *dummy_string = "dummy";
    code = mcl_event_set_option(&event, (E_MCL_EVENT_OPTION)5, dummy_string);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_INVALID_PARAMETER, code, "mcl_event_set_option() unexpected return error!");
}

/**
 * GIVEN : Initialized event without optional fields.
 * WHEN  : User requests to destroy event.
 * THEN  : User expects the event and everything in it have been freed and pointer is NULL
 */
void test_destroy_001(void)
{
    // Initialize event.
    code = event_initialize(event_list->meta, event_payload_type, event_payload_version, severity, timestamp, &event);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Initialization failed for event.");

    event_destroy(&event);

    TEST_ASSERT_NULL_MESSAGE(event, "After destroy operation, event is not NULL!");
}

/**
 * GIVEN : Initialized event with optional fields.
 * WHEN  : User requests to destroy event.
 * THEN  : User expects the event and everything in it have been freed and pointer is NULL
 */
void test_destroy_002(void)
{
    // Initialize event.
    code = event_initialize(event_list->meta, event_payload_type, event_payload_version, severity, timestamp, &event);
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

    event_destroy(&event);

    TEST_ASSERT_NULL_MESSAGE(event, "After destroy operation, event is not NULL!");

    mcl_json_util_destroy(&details);
}
