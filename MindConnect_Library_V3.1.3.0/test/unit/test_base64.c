/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_base64.c
 * @date     Aug 9, 2016
 * @brief    Test module for testing base64 encoding and decoding.
 *
 ************************************************************************/

#include "base64.h"
#include "log_util.h"
#include "unity.h"
#include "memory.h"
#include "string_type.h"
#include "string_util.h"
#include "definitions.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Data to be encoded is given. Size of data is given as 0.
 * WHEN  : Base64 encoding is called.
 * THEN  : Expected encoding is asserted. Size of data is calculated internally.
 */
void test_encode_001(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_encode(data, data_size, &encoded_data);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_MESSAGE((encoded_data->length > 0) && (encoded_data->length % 4 == 0), "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJtZXNzYWdlIiA6ICI8e0hlbGxvIFdvcmxkIX0+In0=", encoded_data->buffer, "Expected base64 encoding not returned!");

    string_destroy(&encoded_data);
}

/**
 * GIVEN : Data to be encoded is given. The data has different size which causes different padding (count of '=' at the end).
 * WHEN  : Base64 encoding is called.
 * THEN  : Expected encoding is asserted.
 */
void test_encode_002(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_encode(data, data_size, &encoded_data);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_MESSAGE((encoded_data->length > 0) && (encoded_data->length % 4 == 0), "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJtZXNzYWdlIiA6ICI8e0hlbGxvIFdvcmxkISF9PiJ9", encoded_data->buffer, "Expected base64 encoding not returned!");

    string_destroy(&encoded_data);
}

/**
 * GIVEN : Data to be encoded is given. The data has different size which causes different padding (count of '=' at the end).
 * WHEN  : Base64 encoding is called.
 * THEN  : Expected encoding is asserted.
 */
void test_encode_003(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!!!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_encode(data, data_size, &encoded_data);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_MESSAGE((encoded_data->length > 0) && (encoded_data->length % 4 == 0), "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJtZXNzYWdlIiA6ICI8e0hlbGxvIFdvcmxkISEhfT4ifQ==", encoded_data->buffer, "Expected base64 encoding not returned!");

    string_destroy(&encoded_data);
}

/**
 * GIVEN : Data to be encoded is given. Size of data is less than original.
 * WHEN  : Base64 encoding is called.
 * THEN  : Expected encoding is asserted. Only selected length of data is encoded.
 */
void test_encode_004(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    const mcl_size_t data_size = 5;
    string_t *encoded_data;

    E_MCL_ERROR_CODE code = base64_encode(data, data_size, &encoded_data);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_MESSAGE((encoded_data->length > 0) && (encoded_data->length % 4 == 0), "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("eyJtZXM=", encoded_data->buffer, "Expected base64 encoding not returned!");

    string_destroy(&encoded_data);
}

/**
 * GIVEN : Test data is given and encoded first.
 * WHEN  : Base64 URL decoding is called.
 * THEN  : Result of decoding is compared to given test data.
 */
void test_decode_001(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_url_encode(data, data_size, &encoded_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_MESSAGE(data_size, decoded_data_size, "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data, decoded_data, "Expected base64 decoded data not returned!");

    string_destroy(&encoded_data);
    MCL_FREE(decoded_data);
}

/**
 * GIVEN : Test data is given and encoded first. The data has different size which causes different padding (count of '=' at the end).
 * WHEN  : Base64 URL decoding is called.
 * THEN  : Result of decoding is compared to given test data.
 */
void test_decode_002(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_url_encode(data, data_size, &encoded_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_MESSAGE(data_size, decoded_data_size, "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data, decoded_data, "Expected base64 decoded data not returned!");

    string_destroy(&encoded_data);
    MCL_FREE(decoded_data);
}

/**
 * GIVEN : Test data is given and encoded first. The data has different size which causes different padding (count of '=' at the end).
 * WHEN  : Base64 URL decoding is called.
 * THEN  : Result of decoding is compared to given test data.
 */
void test_decode_003(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!!!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_url_encode(data, data_size, &encoded_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_MESSAGE(data_size, decoded_data_size, "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data, decoded_data, "Expected base64 decoded data not returned!");

    string_destroy(&encoded_data);
    MCL_FREE(decoded_data);
}

/**
 * GIVEN : Test data is given and encoded first.
 * WHEN  : Base64 decoding is called.
 * THEN  : Result of decoding is compared to given test data.
 */
void test_decode_004(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_encode(data, data_size, &encoded_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    code = base64_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_MESSAGE(data_size, decoded_data_size, "Base64 calculation returned wrong size!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(data, decoded_data, "Expected base64 decoded data not returned!");

    string_destroy(&encoded_data);
    MCL_FREE(decoded_data);
}

/**
 * GIVEN : Test data is given and encoded first.
 * WHEN  : Base64 URL decoding is called. Encoding and decoding methods are different.
 * THEN  : Expected error code is asserted.
 */
void test_decode_005(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_encode(data, data_size, &encoded_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");

    string_destroy(&encoded_data);
}

/**
 * GIVEN : Empty test data is given. Size of data is set to 0.
 * WHEN  : Base64 URL decoding is called.
 * THEN  : Expected error code is asserted.
 */
void test_decode_006(void)
{
    string_t *encoded_data;
    string_initialize_new(MCL_NULL, 0, &encoded_data);

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    E_MCL_ERROR_CODE code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");

    string_destroy(&encoded_data);
}

/**
 * GIVEN : Empty test data is given. Size of data is set to a number different than multiples of 4.
 * WHEN  : Base64 URL decoding is called.
 * THEN  : Expected error code is asserted.
 */
void test_decode_007(void)
{
    string_t *encoded_data;
    string_initialize_new("abc", 3, &encoded_data);

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    E_MCL_ERROR_CODE code = base64_url_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");

    string_destroy(&encoded_data);
}

/**
 * GIVEN : Test data is given. Position of padding is at wrong place.
 * WHEN  : Base64 decoding is called.
 * THEN  : Expected error code is asserted. Incomplete encoded data results in failure of decoding.
 */
void test_decode_008(void)
{
    const mcl_uint8_t *data = "{\"message\" : \"<{Hello World!}>\"}";
    mcl_size_t data_size;
    string_t *encoded_data;

    data_size = string_util_strlen(data);

    E_MCL_ERROR_CODE code = base64_encode(data, data_size, &encoded_data);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "MCL_OK expected as return code!");

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    encoded_data->buffer[encoded_data->length - 4] = '=';
    code = base64_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");

    string_destroy(&encoded_data);
}

/**
 * GIVEN : Test data is given. Data contains invalid count of padding char.
 * WHEN  : Base64 decoding is called.
 * THEN  : Expected error code is asserted.
 */
void test_decode_009(void)
{
    string_t *encoded_data;
    string_initialize_new("eyJtZ===", 0, &encoded_data);

    mcl_uint8_t *decoded_data;
    mcl_size_t decoded_data_size;

    E_MCL_ERROR_CODE code = base64_decode(encoded_data, &decoded_data, &decoded_data_size);

    TEST_ASSERT_MESSAGE(MCL_BAD_CONTENT_ENCODING == code, "MCL_BAD_CONTENT_ENCODING expected as return code!");
    string_destroy(&encoded_data);
}
