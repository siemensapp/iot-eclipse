/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_custom_data.c
* @date     Aug 19, 2016
* @brief    This file implements all test cases of mcl_custom_data module.
*
************************************************************************/

#include "string_type.h"
#include "string_util.h"
#include "log_util.h"
#include "data_types.h"
#include "mcl/mcl_custom_data.h"
#include "custom_data.h"
#include "mcl/mcl_common.h"
#include "memory.h"
#include "list.h"
#include "unity.h"
#include "definitions.h"
#include "mcl/mcl_json_util.h"
#include "json_util.h"

mcl_custom_data_t *custom_data;
char *payload_type = "customType";
char *payload_version = "1.0";
char *routing = "vnd.kuka.FingerprintAnalizer";

void setUp(void)
{
}

void tearDown(void)
{
    custom_data_destroy(&custom_data);
}

/**
 * GIVEN : custom_data is provided without allocated memory.
 * WHEN  : custom_data is initialized.
 * THEN  : It gives the initialized custom_data struct to the user.
 */
void test_initialize_001(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    TEST_ASSERT_NOT_NULL_RETURN(custom_data);

    // Test content_id
    TEST_ASSERT_NULL_MESSAGE(custom_data->meta.content_id, "content_id is not NULL.");

    // Test meta.type
    TEST_ASSERT_EQUAL_STRING_MESSAGE("item", custom_data->meta.type->buffer, "meta.type fail.");

    // Test meta.version
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_version, custom_data->meta.version->buffer, "meta.version fail.");

    // Test routing
    TEST_ASSERT_EQUAL_STRING_MESSAGE(routing, custom_data->meta.details.routing->buffer, "routing fail.");

    // Test meta.payload.type
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_type, custom_data->meta.payload.type->buffer, "meta.payload.type fail.");

    // Test meta.payload.version
    TEST_ASSERT_EQUAL_STRING_MESSAGE(payload_version, custom_data->meta.payload.version->buffer, "meta.payload.version fail.");

    // Test details_json
    TEST_ASSERT_NULL_MESSAGE(custom_data->meta.payload.details.custom_details.details_json, "details_json is not NULL.");

    // Test content
    TEST_ASSERT_NULL_MESSAGE(custom_data->payload.buffer, "content is not NULL.");
}

/**
 * GIVEN : custom_data is provided without allocated memory.
 * WHEN  : custom_data is initialized without routing information.
 * THEN  : It gives the initialized custom_data struct to the user.
 */
void test_initialize_002(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, MCL_NULL, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    TEST_ASSERT_NOT_NULL_RETURN(custom_data);

    // Test routing
    TEST_ASSERT_NULL_MESSAGE(custom_data->meta.details.routing, "routing is not NULL.");
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : User requests to set custom json object with mcl_custom_data_set_meta_details().
 * THEN  : User expects to see the custom json object in custom_data.
 */
void test_set_meta_details_001(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    mcl_json_t *details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &details);

    mcl_json_util_add_string(details, "test_name_1", "test_value_1");
    mcl_json_util_add_string(details, "test_name_2", "test_value_2");

    // Set details.
    code = mcl_custom_data_set_meta_details(custom_data, details);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding meta details failed for custom_data.");

    // Convert details_json of custom_data to string.
    char *meta_details = MCL_NULL;
    mcl_json_util_to_string(custom_data->meta.payload.details.custom_details.details_json, &meta_details);

    // Check whether the details is set correctly.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("{\"test_name_1\":\"test_value_1\",\"test_name_2\":\"test_value_2\"}", meta_details, "meta_details fail.");

    MCL_FREE(meta_details);
    mcl_json_util_destroy(&details);
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : User requests to set custom json object as details = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_set_meta_details_002(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    code = mcl_custom_data_set_meta_details(custom_data, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_custom_data_set_meta_details() failed when details is NULL.");
}

/**
 * GIVEN : Uninitialized custom_data.
 * WHEN  : User requests to set custom_data as MCL_NULL.
 * THEN  : Add meta details function must return "MCL_TRIGGERED_WITH_NULL" error code.
 */
void test_set_meta_details_003(void)
{
    mcl_json_t *details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &details);

    custom_data = MCL_NULL;

    E_MCL_ERROR_CODE code = mcl_custom_data_set_meta_details(custom_data, details);

    TEST_ASSERT(MCL_TRIGGERED_WITH_NULL == code);

    // Since details object couldn't be added to custom_data, destroying custom_data does not destroy details.
    mcl_json_util_destroy(&details);
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : User requests to set details of custom data with name duplication in json root .
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_set_meta_details_004(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    mcl_json_t *details = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &details);

    char *object_name = "test_name_1";
    char *object_value = "test_value_1";

    code = mcl_json_util_add_string(details, object_name, object_value);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    code = mcl_json_util_add_string(details, object_name, object_value);
    TEST_ASSERT_MESSAGE(MCL_JSON_NAME_DUPLICATION == code, "mcl_json_util_add_string() failed for name duplication.");

    // Set details.
    code = mcl_custom_data_set_meta_details(custom_data, details);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding meta details failed for custom_data.");

    // Convert details_json of custom_data to string.
    char *meta_details = MCL_NULL;
    mcl_json_util_to_string(custom_data->meta.payload.details.custom_details.details_json, &meta_details);

    // Since we couldn't add the same json object name in the details object, we expect to see only one
    // as {"test_name_1":"test_value_1"} instead of {"test_name_1":"test_value_1", "test_name_1":"test_value_1"}.
    // Check whether the details is set correctly.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("{\"test_name_1\":\"test_value_1\"}", meta_details, "meta_details fail.");

    MCL_FREE(meta_details);
    mcl_json_util_destroy(&details);
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : User requests to set payload content.
 * THEN  : User expects to see the same content as the original in custom_data->payload.buffer.
 */
void test_set_payload_001(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    mcl_uint8_t content[] = { 0x00, 0x01, 0xAB };

    // Get the content size.
    mcl_size_t content_length = sizeof(content);
    TEST_ASSERT_EQUAL_INT32_MESSAGE(MCL_OK, code, "String length fail for content.");

    code = mcl_custom_data_set_payload(custom_data, content, content_length);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Set payload function failed.");

    // Test the payload content.
    TEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(content, custom_data->payload.buffer, content_length, "Payload content is wrong.");
    TEST_ASSERT_MESSAGE(content_length == custom_data->payload.size, "Payload content size is wrong.");
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : User requests to set payload content as empty.
 * THEN  : User expects to see the same empty content in custom_data->payload.buffer.
 */
void test_set_payload_002(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    // The content is empty.
    mcl_uint8_t content[] = { 0x00, 0x01, 0xAB };

    code = mcl_custom_data_set_payload(custom_data, content, 0);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "content_length is not bigger than zero.");
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : User requests to set payload content as NULL.
 * THEN  : User expects "MCL_TRIGGERED_WITH_NULL" as return error code.
 */
void test_set_payload_003(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    mcl_uint8_t *content = MCL_NULL;
    mcl_size_t content_length = 0;
    code = mcl_custom_data_set_payload(custom_data, content, content_length);

    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "Received content argument is NULL.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(content, custom_data->payload.buffer, "Payload content is wrong.");
    TEST_ASSERT_MESSAGE(content_length == custom_data->payload.size, "Payload content_length is wrong.");
}

/**
 * GIVEN : Initialized custom_data.
 * WHEN  : User requests to destroy custom_data.
 * THEN  : MCL_OK is returned and custom_data is null.
 */
void test_destroy_001(void)
{
    E_MCL_ERROR_CODE code = custom_data_initialize(payload_version, payload_type, routing, &custom_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Initialization failed for custom_data.");

    custom_data_destroy(&custom_data);
    TEST_ASSERT_MESSAGE(MCL_NULL == custom_data, "custom_data is not NULL.");
}
