/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_http_response.c
 * @date     Apr 17, 2017
 * @brief    Unit test cases for http_response module.
 *
 ************************************************************************/

#include "unity.h"
#include "string_util.h"
#include "http_response.h"
#include "definitions.h"
#include "string_type.h"
#include "memory.h"
#include "string_array.h"
#include "list.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
* GIVEN : Header, payload and code of an HTTP response.
* WHEN  : #http_response_t structure is initialized.
* THEN  : MCL_OK is returned and fields of #http_response_t are correctly set.
*/
void test_initialize_001()
{
    string_array_t *header = MCL_NULL;
    string_array_initialize(3, &header);
    mcl_size_t payload_size = 5;
    mcl_uint8_t *payload = MCL_MALLOC(payload_size);
    mcl_int32_t result_code = MCL_HTTP_RESULT_CODE_SUCCESS;
    http_response_t *http_response = MCL_NULL;

    E_MCL_ERROR_CODE code = http_response_initialize(header, payload, payload_size, result_code, &http_response);

    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_EQUAL(header, http_response->header);
    TEST_ASSERT_EQUAL(payload_size, http_response->payload_size);
    TEST_ASSERT_EQUAL_MEMORY(payload, http_response->payload, payload_size);
    TEST_ASSERT_EQUAL(result_code, http_response->result_code);

    http_response_destroy(&http_response);
    TEST_ASSERT_EQUAL(MCL_NULL, http_response);
}

/**
* GIVEN : User provides valid parameters such as buffer, uri etc.
* WHEN  : #http_response_get_header() is called.
* THEN  : Correct header value is returned.
*/
void test_get_header_001()
{
    http_response_t response;

    string_array_t *headers = MCL_NULL;
    string_array_initialize(2, &headers);

    string_t *content_type_header = MCL_NULL;
    string_t *accept_header = MCL_NULL;
	string_t *content_disposition_haeder = MCL_NULL;

    string_initialize_static("Content-Type: application/json", 0, &content_type_header);
	string_initialize_static("Accept: */*", 0, &accept_header);
	string_initialize_static("Content-Disposition: attachment; filename\"dummy_file_range\"", 0, &content_disposition_haeder);

    string_array_add(headers, content_type_header, MCL_TRUE);
    string_array_add(headers, accept_header, MCL_TRUE);
	string_array_add(headers, content_disposition_haeder, MCL_TRUE);

    response.header = headers;

	string_t *content_type_header_value = MCL_NULL;
    http_response_get_header(&response, "Content-Type", &content_type_header_value);

    E_MCL_ERROR_CODE comparison_result;
    comparison_result = string_util_strncmp(" application/json", content_type_header_value->buffer, 32);
    TEST_ASSERT_EQUAL(MCL_OK, comparison_result);

	string_t *content_disposition_header_value = MCL_NULL;
	http_response_get_header(&response, "Content-Disposition", &content_disposition_header_value);

    comparison_result = string_util_strncmp(" attachment; filename\"dummy_file_range\"", content_disposition_header_value->buffer, 32);
	TEST_ASSERT_EQUAL(MCL_OK, comparison_result);

	string_destroy(&content_disposition_header_value);
	string_destroy(&content_type_header_value);
	string_array_destroy(&headers);
}

/**
* GIVEN : Correctly initialized #http_response_t structure.
* WHEN  : Payload of the response is requested.
* THEN  : Memory address of the payload is returned.
*/
void test_get_payload_001()
{
    string_array_t *header = MCL_NULL;
    string_array_initialize(3, &header);
    mcl_size_t payload_size = 5;
    mcl_uint8_t *payload = MCL_MALLOC(payload_size);
    mcl_int32_t result_code = MCL_HTTP_RESULT_CODE_SUCCESS;
    http_response_t *http_response = MCL_NULL;

    E_MCL_ERROR_CODE code = http_response_initialize(header, payload, payload_size, result_code, &http_response);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    mcl_uint8_t *returned_payload = http_response_get_payload(http_response);
    TEST_ASSERT_EQUAL_MEMORY(payload, returned_payload, payload_size);

    http_response_destroy(&http_response);
    TEST_ASSERT_EQUAL(MCL_NULL, http_response);
}

/**
* GIVEN : Correctly initialized #http_response_t structure.
* WHEN  : Result code of the response is requested.
* THEN  : Result code is returned.
*/
void test_get_result_code_001()
{
    string_array_t *header = MCL_NULL;
    string_array_initialize(3, &header);
    mcl_size_t payload_size = 5;
    mcl_uint8_t *payload = MCL_MALLOC(payload_size);
    mcl_int32_t result_code = MCL_HTTP_RESULT_CODE_SUCCESS;
    http_response_t *http_response = MCL_NULL;

    E_MCL_ERROR_CODE code = http_response_initialize(header, payload, payload_size, result_code, &http_response);
    TEST_ASSERT_EQUAL(MCL_OK, code);

    E_MCL_HTTP_RESULT_CODE returned_result_code = http_response_get_result_code(http_response);
    TEST_ASSERT_EQUAL(result_code, returned_result_code);

    http_response_destroy(&http_response);
    TEST_ASSERT_EQUAL(MCL_NULL, http_response);
}
