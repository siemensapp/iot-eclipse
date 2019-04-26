/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_http_request.c
* @date     Sep 5, 2016
* @brief    This file contains test case functions to test http_request module.
*
************************************************************************/

#include "data_types.h"
#include "string_type.h"
#include "unity.h"
#include "http_request.h"
#include "http_definitions.h"
#include "log_util.h"
#include "memory.h"
#include "random.h"
#include "string_array.h"
#include "string_util.h"
#include "definitions.h"
#include "security.h"
#include "security_libcrypto.h"

#define USER_AGENT "MCL/0.9.0.0"

http_request_t *http_request = MCL_NULL;
string_t *host = MCL_NULL;
string_t *uri = MCL_NULL;
string_t *user_agent = MCL_NULL;
E_MCL_HTTP_METHOD method;
mcl_size_t header_size;
mcl_size_t payload_size;
mcl_size_t max_http_payload_size = 16384;

void setUp(void)
{
    method = MCL_HTTP_GET;
    string_initialize_new("host", 0, &host);
    string_initialize_new("host/uri", 0, &uri);
    string_initialize_new(USER_AGENT, 0, &user_agent);
    header_size = 5;
    payload_size = 0;
}

void tearDown(void)
{
    http_request_destroy(&http_request);
    string_destroy(&host);
    string_destroy(&uri);
    string_destroy(&user_agent);
}

mcl_size_t _get_payload_from_buffer(void *destination, void *source, mcl_size_t size, void *user_context)
{
    string_util_memcpy(destination, source, size);

    return size;
}

// Private Function Prototypes:
static void _replace_all_random_generated_boundaries_with_known_string(http_request_t *http_request);

/**
 * GIVEN : Host, uri and **http_request are not null and payload_size is zero.
 * WHEN  : http_request_initialize() is called.
 * THEN  : MCL_OK is returned and http_request is created with null payload.
 */
void test_initialize_001(void)
{
    E_MCL_ERROR_CODE result = http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size,
                                  &http_request);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_initialize() function returns error.");
    TEST_ASSERT_NOT_NULL_MESSAGE(http_request, "Http request object is null after initialization.");
    TEST_ASSERT_EQUAL_MESSAGE(method, http_request->method, "Http request method is not set correctly.");
    TEST_ASSERT_NOT_NULL_MESSAGE(http_request->header, "Http request header array is null after initialization.");
    TEST_ASSERT_EQUAL_MESSAGE(header_size + 1, http_request->header->count, "Http request header array size is not set correctly.");
    TEST_ASSERT_NULL_MESSAGE(http_request->payload, "Payload of the http request is not null although it should be.");
    TEST_ASSERT_EQUAL_MESSAGE(payload_size, http_request->payload_size, "Payload size of the http request is not set correctly.");
    TEST_ASSERT_EQUAL_MESSAGE(max_http_payload_size, http_request->max_http_payload_size, "max_http_payload_size of the http request is not set correctly.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("host/uri", http_request->uri->buffer, "Http request URI is not set correctly.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("User-Agent: " USER_AGENT, string_array_get(http_request->header, 0)->buffer, "Http request User-Agent is not set correctly.");
}

/**
 * GIVEN : Host, request_uri and **http_request are not null and payload_size is non-zero.
 * WHEN  : http_request_initialize() is called.
 * THEN  : MCL_OK is returned and http_request is created with not-null payload.
 */
void test_initialize_002(void)
{
    payload_size = 10;

    E_MCL_ERROR_CODE result = http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size,
                                  &http_request);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_initialize() function returns error.");
    TEST_ASSERT_NOT_NULL_MESSAGE(http_request, "Http request object is null after initialization.");
    TEST_ASSERT_EQUAL_MESSAGE(method, http_request->method, "Http request method is not set correctly.");
    TEST_ASSERT_NOT_NULL_MESSAGE(http_request->header, "Http request header array is null after initialization.");
    TEST_ASSERT_EQUAL_MESSAGE(header_size + 1, http_request->header->count, "Http request header array size is not set correctly.");
    TEST_ASSERT_NOT_NULL_MESSAGE(http_request->payload, "Payload of the http request is null although it should be.");
    TEST_ASSERT_EQUAL_MESSAGE(payload_size, http_request->payload_size, "Payload size of the http request is not set correctly.");
    TEST_ASSERT_EQUAL_MESSAGE(max_http_payload_size, http_request->max_http_payload_size, "max_http_payload_size of the http request is not set correctly.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("host/uri", http_request->uri->buffer, "Http request URI is not set correctly.");
    TEST_ASSERT_MESSAGE(MCL_NULL != http_request->boundary->buffer, "Boundary fail.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("User-Agent: " USER_AGENT, string_array_get(http_request->header, 0)->buffer, "Http request User-Agent is not set correctly.");

    // Boundary length is defined as macro like BOUNDARY_LENGTH 22 in http_request.c
    TEST_ASSERT_EQUAL_MESSAGE(22, http_request->boundary->length, "Boundary length fail.");
}

/**
 * GIVEN : An http request object is successfully initialized.
 * WHEN  : http_request_add_header() is called two times with a two different header name and a header value pairs.
 * THEN  : MCL_OK is returned and the new header line is added to string array of headers of the given http request object.
 */
void test_add_header_001(void)
{
    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    // Initialize header name.
    string_t *header_name = MCL_NULL;
    string_initialize_new("Content Type", 0, &header_name);

    // Initialize header value.
    string_t *header_value = MCL_NULL;
    string_initialize_new("multipart/mixed;boundary=gc0p4Jq0M2Yt08jU534c0p", 0, &header_value);

    // Add header_name and header_value pair to http_request.
    E_MCL_ERROR_CODE result = http_request_add_header(http_request, header_name, header_value);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_add_header() function returns error.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Content Type: multipart/mixed;boundary=gc0p4Jq0M2Yt08jU534c0p", string_array_get(http_request->header, 1)->buffer,
                                     "Header line can not be added to string array of header lines.");
    TEST_ASSERT_EQUAL_MESSAGE(2, http_request->header->index, "Number of header lines added to http request mismatch.");

    // Initialize a second header name.
    string_t *header_name_2 = MCL_NULL;
    string_initialize_new("Content Length", 0, &header_name_2);

    // Initialize a second header value.
    string_t *header_value_2 = MCL_NULL;
    string_initialize_new("345", 0, &header_value_2);

    // Add second header_name and header_value pair to http_request.
    result = http_request_add_header(http_request, header_name_2, header_value_2);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_add_header() function returns error.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Content Length: 345", string_array_get(http_request->header, 2)->buffer,
                                     "Header line can not be added to string array of header lines.");
    TEST_ASSERT_EQUAL_MESSAGE(3, http_request->header->index, "Number of header lines added to http request mismatch.");

    // Clean up.
    string_destroy(&header_name);
    string_destroy(&header_value);
    string_destroy(&header_name_2);
    string_destroy(&header_value_2);
}

/**
 * GIVEN : Initialized @p http_request.
 * WHEN  : http_request_add_single() is called with valid parameters.
 * THEN  : MCL_OK is returned.
 */
void test_add_single_001(void)
{
    // Allocated memory size for payload.
    mcl_size_t payload_size = 5000;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    // Initialize content_id.
    string_t *content_id = MCL_NULL;
    string_initialize_new("113fe5ec-1035-11e6-a148-3e1d05defe78", 0, &content_id);

    // Initialize meta_string.
    string_t *meta_string = MCL_NULL;
    string_initialize_new("{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\",\"totalItems\":3},\"payload\":{\"type\":\"fingerprint\",\"version\":\"1.2.4\",\"details\":{\"timestamp\":\"2016-04-12 22:34.3453Z\",\"duration\":42}}}",
                          0, &meta_string);

    // Add a single to http_request with Null http_request parameter.
    E_MCL_ERROR_CODE result = http_request_add_single(http_request, &content_type_values[CONTENT_TYPE_META_JSON], content_id, meta_string);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_add_single() function didn't return MCL_OK.");

    // Replace random generated boundary with known string to be able to compare the rest of the payload successfully.
    string_util_memcpy(&http_request->payload[BOUNDARY_SIGN_LENGTH], "xxxxxxxxxxxxxxxxxxxxxx", BOUNDARY_LENGTH);

    char *expected =
        "--xxxxxxxxxxxxxxxxxxxxxx\r\nContent-Type: application/vnd.siemens.mindsphere.meta+json\r\nContent-ID: 113fe5ec-1035-11e6-a148-3e1d05defe78\r\n\r\n{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\",\"totalItems\":3},\"payload\":{\"type\":\"fingerprint\",\"version\":\"1.2.4\",\"details\":{\"timestamp\":\"2016-04-12 22:34.3453Z\",\"duration\":42}}}\r\n";

    // Payload must consist of boundary, Content-Type, Content-ID and meta string. But we ignore random boundary and compare the rest with fixed string.
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((mcl_uint8_t *)expected, http_request->payload, http_request->payload_offset, "Payload content is wrong.");

    // Payload buffer has enough space for add single. So we don't expect any resize in payload size.
    TEST_ASSERT_EQUAL_INT_MESSAGE(payload_size, http_request->payload_size, "Payload size changed unexpectedly.");

    // Clean up.
    string_destroy(&content_id);
    string_destroy(&meta_string);
}

/**
 * GIVEN : Initialized @p http_request.
 * WHEN  : Empty space in payload is lower than the required memory.(empty_space_in_payload = 100, required_empty_size = 354, max_http_payload_size = 16384)
 * THEN  : Payload buffer is resized and addition is performed.
 */
void test_add_single_002(void)
{
    // Allocated memory size for payload.
    mcl_size_t payload_size = 100;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    // Initialize content_id.
    string_t *content_id = MCL_NULL;
    string_initialize_new("113fe5ec-1035-11e6-a148-3e1d05defe78", 0, &content_id);

    // Initialize meta_string.
    string_t *meta_string = MCL_NULL;
    string_initialize_new("{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\",\"totalItems\":3},\"payload\":{\"type\":\"fingerprint\",\"version\":\"1.2.4\",\"details\":{\"timestamp\":\"2016-04-12 22:34.3453Z\",\"duration\":42}}}",
                          0, &meta_string);

    // Add a single to http_request with Null http_request parameter..
    E_MCL_ERROR_CODE result = http_request_add_single(http_request, &content_type_values[CONTENT_TYPE_META_JSON], content_id, meta_string);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_add_single() function didn't return MCL_OK.");

    // Replace random generated boundary with known string to be able to compare the rest of the payload successfully.
    string_util_memcpy(&http_request->payload[BOUNDARY_SIGN_LENGTH], "xxxxxxxxxxxxxxxxxxxxxx", BOUNDARY_LENGTH);

    char *expected =
        "--xxxxxxxxxxxxxxxxxxxxxx\r\nContent-Type: application/vnd.siemens.mindsphere.meta+json\r\nContent-ID: 113fe5ec-1035-11e6-a148-3e1d05defe78\r\n\r\n{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\",\"totalItems\":3},\"payload\":{\"type\":\"fingerprint\",\"version\":\"1.2.4\",\"details\":{\"timestamp\":\"2016-04-12 22:34.3453Z\",\"duration\":42}}}\r\n";

    // Payload must consist of boundary, Content-Type, Content-ID and meta string. But we ignore random boundary and compare the rest with fixed string.
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((mcl_uint8_t *)expected, http_request->payload, http_request->payload_offset, "Payload content is wrong.");

    // Payload buffer does not have enough space for add single. So we expect resize operation in payload size.
    TEST_ASSERT_NOT_EQUAL_MESSAGE(payload_size, http_request->payload_size, "Payload size should have been resized.");

    // Clean up.
    string_destroy(&content_id);
    string_destroy(&meta_string);
}

/**
* GIVEN : Initialized @p http_request.
* WHEN  : http_request_add_single() is called with data larger than 10MB.
* THEN  : MCL_HTTP_REQUEST_NO_MORE_SPACE is returned.
*/
void test_add_single_003(void)
{
	// Allocated memory size for payload.
	mcl_size_t payload_size = 5000000;
	mcl_size_t very_large_http_payload_size = 10000000;

	// Initialize header request.
	http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, very_large_http_payload_size, &http_request);

	// Initialize content_id.
	string_t *content_id = MCL_NULL;
	string_initialize_new("113fe5ec-1035-11e6-a148-3e1d05defe78", 0, &content_id);

	// Initialize meta_string.
	string_t *meta_string = MCL_NULL;
	
	string_initialize_new("{\"type\":\"item\",\"version\":\"1.0\",\"details\":{\"routing\":\"vnd.kuka.FingerprintAnalizer\",\"totalItems\":3},\"payload\":{\"type\":\"fingerprint\",\"version\":\"1.2.4\",\"details\":{\"timestamp\":\"2016-04-12 22:34.3453Z\",\"duration\":42}}}",
		0, &meta_string);

	meta_string->length = 20000000;

	// Add a single to http_request with Null http_request parameter.
	E_MCL_ERROR_CODE result = http_request_add_single(http_request, &content_type_values[CONTENT_TYPE_META_JSON], content_id, meta_string);
	TEST_ASSERT_MESSAGE(MCL_HTTP_REQUEST_NO_MORE_SPACE == result, "http_request_add_single() function didn't return MCL_HTTP_REQUEST_NO_MORE_SPACE.");

	// Clean up.
	string_destroy(&content_id);
	string_destroy(&meta_string);
}

/**
 * GIVEN : Initialized #http_request.
 * WHEN  : http_request_add_tuple() is called with valid parameters.
 * THEN  : MCL_OK is returned.
 */
void test_add_tuple_001(void)
{
    mcl_size_t payload_size_local = 1000;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size_local, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    // Initialize meta_string.
    string_t *meta_string = MCL_NULL;
    string_initialize_new("{\"type\":\"chunk\",\"version\":\"1.0\",\"details\":{\"chunkSetId\":\"98fcf48a-102f-11e6-a148-3e1d05defe78\",\"chunkNo\":0}}", 0, &meta_string);

    // Random numbers. 0x41 = 'A'.
    mcl_uint8_t payload[] = { 0x41, 0x41 };
    mcl_size_t payload_size = 2;

    // Add a tuple to http_request with Null http_request parameter.
    E_MCL_ERROR_CODE result = http_request_add_tuple(http_request, meta_string, &content_type_values[CONTENT_TYPE_META_JSON], _get_payload_from_buffer, MCL_NULL, payload,
                                  payload_size, &content_type_values[CONTENT_TYPE_TEXT_PLAIN]);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_add_tuple() function failed.");

    // Replace random generated boundary with known string to be able to compare the rest of the payload successfully.
    _replace_all_random_generated_boundaries_with_known_string(http_request);

    char *expected =
        "--xxxxxxxxxxxxxxxxxxxxxx\r\nContent-Type: multipart/related;boundary=xxxxxxxxxxxxxxxxxxxxxx\r\n\r\n--xxxxxxxxxxxxxxxxxxxxxx\r\nContent-Type: application/vnd.siemens.mindsphere.meta+json\r\n\r\n{\"type\":\"chunk\",\"version\":\"1.0\",\"details\":{\"chunkSetId\":\"98fcf48a-102f-11e6-a148-3e1d05defe78\",\"chunkNo\":0}}\r\n--xxxxxxxxxxxxxxxxxxxxxx\r\nContent-Type: text/plain\r\n\r\nAA\r\n--xxxxxxxxxxxxxxxxxxxxxx--\r\n";

    // Payload must consist of boundary, Content-Type, Content-ID and meta string. But we ignore random boundary and compare the rest with fixed string.
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((mcl_uint8_t *)expected, http_request->payload, http_request->payload_offset, "Payload content is wrong.");

    // Payload buffer has enough space for add tuple. So we don't expect any resize in payload size.
    TEST_ASSERT_EQUAL_INT_MESSAGE(payload_size_local, http_request->payload_size, "Payload size changed unexpectedly.");

    // Clean up.
    string_destroy(&meta_string);
}

/**
 * GIVEN : Initialized #http_request.
 * WHEN  : Empty space in payload is lower than the required memory.(empty_space_in_payload = 100, required_empty_size = 380, max_http_payload_size = 16384)
 * THEN  : Payload buffer is resized and addition is performed.
 */
void test_add_tuple_002(void)
{
    mcl_size_t payload_size_local = 100;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size_local, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    // Initialize meta_string.
    string_t *meta_string = MCL_NULL;
    string_initialize_new("{\"type\":\"chunk\",\"version\":\"1.0\",\"details\":{\"chunkSetId\":\"98fcf48a-102f-11e6-a148-3e1d05defe78\",\"chunkNo\":0}}", 0, &meta_string);

    // Random numbers. 0x41 = 'A'.
    mcl_uint8_t payload[] = { 0x41, 0x41 };
    mcl_size_t payload_size = 2;

    // Add a tuple to http_request with Null http_request parameter.
    E_MCL_ERROR_CODE result = http_request_add_tuple(http_request, meta_string, &content_type_values[CONTENT_TYPE_META_JSON], _get_payload_from_buffer, MCL_NULL, payload,
                                  payload_size, &content_type_values[CONTENT_TYPE_TEXT_PLAIN]);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_add_tuple() function failed.");

    // Replace random generated boundary with known string to be able to compare the rest of the payload successfully.
    _replace_all_random_generated_boundaries_with_known_string(http_request);

    char *expected =
        "--xxxxxxxxxxxxxxxxxxxxxx\r\nContent-Type: multipart/related;boundary=xxxxxxxxxxxxxxxxxxxxxx\r\n\r\n--xxxxxxxxxxxxxxxxxxxxxx\r\nContent-Type: application/vnd.siemens.mindsphere.meta+json\r\n\r\n{\"type\":\"chunk\",\"version\":\"1.0\",\"details\":{\"chunkSetId\":\"98fcf48a-102f-11e6-a148-3e1d05defe78\",\"chunkNo\":0}}\r\n--xxxxxxxxxxxxxxxxxxxxxx\r\nContent-Type: text/plain\r\n\r\nAA\r\n--xxxxxxxxxxxxxxxxxxxxxx--\r\n";

    // Payload must consist of boundary, Content-Type, Content-ID and meta string. But we ignore random boundary and compare the rest with fixed string.
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((mcl_uint8_t *)expected, http_request->payload, http_request->payload_offset, "Payload content is wrong.");

    // Payload buffer does not have enough space for add tuple. So we expect resize operation in payload size.
    TEST_ASSERT_NOT_EQUAL_MESSAGE(payload_size_local, http_request->payload_size, "Payload size should have been resized.");

    // Clean up.
    string_destroy(&meta_string);
}

/**
 * GIVEN : Initialized #http_request.
 * WHEN  : new_size is higher than max_http_payload_size.
 * THEN  : MCL_HTTP_REQUEST_NO_MORE_SPACE is returned.
 */
void test_add_tuple_003(void)
{
    // To test no more space case, set payload size to a big value.
    mcl_size_t payload_size_local = max_http_payload_size - 10;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size_local, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    // Initialize meta_string.
    string_t *meta_string = MCL_NULL;
    string_initialize_new("{\"type\":\"chunk\",\"version\":\"1.0\",\"details\":{\"chunkSetId\":\"98fcf48a-102f-11e6-a148-3e1d05defe78\",\"chunkNo\":0}}", 0, &meta_string);

    // Random numbers. 0x41 = 'A'.
    mcl_uint8_t payload[] = { 0x41, 0x41 };
    mcl_size_t payload_size = 2;

    // Simulate payload is filled, and offset is set to a big value.
    http_request->payload_offset = max_http_payload_size - 15;

    // Add a tuple to http_request with Null http_request parameter.
    E_MCL_ERROR_CODE result = http_request_add_tuple(http_request, meta_string, &content_type_values[CONTENT_TYPE_META_JSON], _get_payload_from_buffer, MCL_NULL, payload,
                                  payload_size, &content_type_values[CONTENT_TYPE_TEXT_PLAIN]);
    TEST_ASSERT_MESSAGE(MCL_HTTP_REQUEST_NO_MORE_SPACE == result, "http_request_add_tuple() function didn't return MCL_HTTP_REQUEST_NO_MORE_SPACE.");

    // Payload buffer can't be resized by the required memory, since that exceeds max_http_payload_size.
    TEST_ASSERT_EQUAL_MESSAGE(payload_size_local, http_request->payload_size, "Payload size shouldn't have been resized.");

    // Clean up.
    string_destroy(&meta_string);
}

/**
 * GIVEN : Initialized http_request.
 * WHEN  : Payload size is more than required memory and http_request_finalize() is called, unused memory space is released.
 * THEN  : MCL_OK is returned.
 */
void test_finalize_001(void)
{
    // Allocated memory size for payload.
    mcl_size_t payload_size = 500;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    E_MCL_ERROR_CODE result = http_request_finalize(http_request);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_finalize() function failed.");

    // Replace random generated boundary with known string to be able to compare the rest of the payload successfully.
    string_util_memcpy(&http_request->payload[BOUNDARY_SIGN_LENGTH], "xxxxxxxxxxxxxxxxxxxxxx", BOUNDARY_LENGTH);

    // closing boundary format.
    char *expected = "--xxxxxxxxxxxxxxxxxxxxxx--";

    mcl_size_t expected_length;
    expected_length = string_util_strlen(expected);

    // Payload must consist of closing boundary. But we ignore random boundary and compare the rest with fixed string.
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((mcl_uint8_t *)expected, http_request->payload, http_request->payload_offset, "Payload content is wrong.");

    // Payload buffer needs empty space only for expected_length+1. So unused memory space will be released.
    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_length, http_request->payload_size, "Payload size changed unexpectedly.");
}

/**
 * GIVEN : Initialized http_request.
 * WHEN  : http_request_finalize() is called with not enough space in payload buffer.
 * THEN  : Payload buffer is extended and addition is performed.
 */
void test_finalize_002(void)
{
    // Allocated memory size for payload.
    mcl_size_t payload_size = 20;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    E_MCL_ERROR_CODE result = http_request_finalize(http_request);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_finalize() function failed.");

    // Replace random generated boundary with known string to be able to compare the rest of the payload successfully.
    string_util_memcpy(&http_request->payload[BOUNDARY_SIGN_LENGTH], "xxxxxxxxxxxxxxxxxxxxxx", BOUNDARY_LENGTH);

    // closing boundary format.
    char *expected = "--xxxxxxxxxxxxxxxxxxxxxx--\r\n";

    // Payload must consist of closing boundary. But we ignore random boundary and compare the rest with fixed string.
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE((mcl_uint8_t *)expected, http_request->payload, http_request->payload_offset, "Payload content is wrong.");

    // Payload buffer does not have enough space for add finalize. So we expect resize operation in payload size.
    TEST_ASSERT_NOT_EQUAL_MESSAGE(payload_size, http_request->payload_size, "Payload size should have been resized.");
}

/**
 * GIVEN : Initialized http_request.
 * WHEN  : new_size is higher than max_http_payload_size.
 * THEN  : MCL_HTTP_REQUEST_FINALIZE_FAILED is returned.
 */
void test_finalize_003(void)
{
    // To test no more space case, set payload size to a big value.
    mcl_size_t payload_size = max_http_payload_size - 10;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    // Simulate payload is filled, and offset is set to a big value.
    http_request->payload_offset = max_http_payload_size - 15;

    E_MCL_ERROR_CODE result = http_request_finalize(http_request);
    TEST_ASSERT_MESSAGE(MCL_HTTP_REQUEST_FINALIZE_FAILED == result, "http_request_finalize() function failed.");

    // Payload buffer can't be resized by the required memory, since that exceeds max_http_payload_size.
    TEST_ASSERT_EQUAL_MESSAGE(payload_size, http_request->payload_size, "Payload size shouldn't have been resized.");
}

/**
 * GIVEN : Initialized http_request.
 * WHEN  : Required memory is equal to the available memory in the payload buffer.
 * THEN  : MCL_OK is returned.
 */
void test_finalize_004(void)
{
    // 26 -> required_empty_size = BOUNDARY_LINE_LENGTH;
    mcl_size_t payload_size = 26;

    // Initialize header request.
    http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    E_MCL_ERROR_CODE result = http_request_finalize(http_request);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_request_finalize() function failed.");

    // Payload buffer shouldn't be resized, since available memory is enough.
    TEST_ASSERT_EQUAL_MESSAGE(payload_size, http_request->payload_size, "Payload size shouldn't have been resized.");
}

/**
 * GIVEN : An http request is initialized successfully.
 * WHEN  : http_request_destroy() is called.
 * THEN  : http_request is NULL.
 */
void test_destroy_001(void)
{
    http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);

    http_request_destroy(&http_request);

    TEST_ASSERT_NULL_MESSAGE(http_request, "Http request is not null after destroy.");
}

/**
 * GIVEN : An http request is initialized successfully.
 * WHEN  : http_request_start_tuple() is called.
 * THEN  : MCL_OK is returned.
 */
void test_start_tuple(void)
{
    E_MCL_ERROR_CODE result = http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size,
                                  &http_request);
    TEST_ASSERT(MCL_OK == result);

    result = http_request_start_tuple(http_request);
    TEST_ASSERT(MCL_OK == result);

    http_request_destroy(&http_request);
    TEST_ASSERT_NULL_MESSAGE(http_request, "Http request is not null after destroy.");
}

/**
 * GIVEN : An http request is initialized successfully.
 * WHEN  : http_request_start_tuple_sub_section() is called.
 * THEN  : MCL_OK is returned.
 */
void test_start_tuple_sub_section(void)
{
    E_MCL_ERROR_CODE result = http_request_initialize(method, uri, header_size, payload_size, HTTP_REQUEST_RESIZE_ENABLED, user_agent, max_http_payload_size, &http_request);
    TEST_ASSERT(MCL_OK == result);

	string_t content_type = STRING_CONSTANT("application / vnd.siemens.mindsphere.meta + json");
	string_t content_id = STRING_CONSTANT("content_id");

    string_t *sub_boundary = MCL_NULL;
    result = http_request_start_tuple_sub_section(http_request, &content_type, &content_id, &sub_boundary);
    TEST_ASSERT(MCL_OK == result);

    result = http_request_end_tuple_sub_section(http_request, sub_boundary);
    TEST_ASSERT(MCL_OK == result);

    string_destroy(&sub_boundary);
    http_request_destroy(&http_request);
    TEST_ASSERT_NULL_MESSAGE(http_request, "Http request is not null after destroy.");
}

// converts this "--GB5D4Gnnow0eaj7yss8ZCB\r\n to "--xxxxxxxxxxxxxxxxxxxxxx\r\n"
static void _replace_all_random_generated_boundaries_with_known_string(http_request_t *http_request)
{
    // Boundary start addresses in the payload of http request.
    mcl_size_t boundary_start_addresses[5] = {2, 67, 95, 293, 351};

    for (int i = 0; i < 5; ++i)
    {
        string_util_memcpy(&http_request->payload[boundary_start_addresses[i]], "xxxxxxxxxxxxxxxxxxxxxx", BOUNDARY_LENGTH);
    }
}
