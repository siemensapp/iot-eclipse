/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_security_handler.c
* @date     Aug 17, 2016
* @brief    This file contains test case functions to test security_handler module.
*
************************************************************************/

#include "unity.h"
#include "security_handler.h"
#include "memory.h"
#include "random.h"
#include "string_type.h"
#include "string_util.h"
#include "definitions.h"
#include "mock_base64.h"
#include "mock_security.h"

security_handler_t *security_handler = MCL_NULL;

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Input argument to security_handler_initialize() function is not null.
 * WHEN  : Security handler is initialized.
 * THEN  : MCL_OK is returned, security_handler is not null but its pointer members are null.
 */
void test_initialize_001(void)
{
	E_MCL_ERROR_CODE result = security_handler_initialize(&security_handler);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "security_handler_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(security_handler, "Security handler is null after initialization.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->authentication_key, "Authentication key is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->hmac_key, "HMAC key is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->onboarding_key, "Onboarding key is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->rsa.private_key, "RSA private key is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->rsa.public_key, "RSA public key is not null after initialization although it should be.");
    TEST_ASSERT_NULL_MESSAGE(security_handler->rsa.session_key, "Session key is not null after initialization although it should be.");
    TEST_ASSERT_EQUAL_MESSAGE(0, security_handler->authentication_key_size, "Authentication key size is not set to 0 initially.");

    security_handler_destroy(&security_handler);
}

/**
 * GIVEN : Security handler is null.
 * WHEN  : Security handler is destroyed.
 * THEN  : MCL_OK is returned.
 */
void test_destroy_001(void)
{
    security_handler = MCL_NULL;
	
    security_handler_destroy(&security_handler);

    TEST_ASSERT_NULL_MESSAGE(security_handler, "Security handler is not null after it is destroyed.");
}

/**
 * GIVEN : Security handler is initialized successfully.
 * WHEN  : Security handler is destroyed.
 * THEN  : MCL_OK is returned and security handler is null.
 */
void test_destroy_002(void)
{
    security_handler_initialize(&security_handler);

    security_handler_destroy(&security_handler);

    TEST_ASSERT_NULL_MESSAGE(security_handler, "Security handler is not null after it is destroyed.");
}

/**
 * GIVEN : Test data for mocked base64 URL encoding given.
 * WHEN  : Base64 URL encoding in security handler module called.
 * THEN  : MCL_OK is configured to be returned by mock. This return value must be also returned by test function.
 */
void test_encode_base64_url_001()
{
    const mcl_uint8_t *data = "tmp";
    const mcl_size_t data_size = 0;
    string_t *encoded_data;
	string_initialize_new("tmp", 3, &encoded_data);

    base64_url_encode_ExpectAndReturn(data, data_size, &encoded_data, MCL_OK);

    E_MCL_ERROR_CODE code = security_handler_base64_url_encode(data, data_size, &encoded_data);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK  expected as return code!");

	string_destroy(&encoded_data);
}

/**
 * GIVEN : Test data for mocked base64 URL encoding given.
 * WHEN  : Base64 URL encoding in security handler module called.
 * THEN  : MCL_TRIGGERED_WITH_NULL is configured to be returned by mock. This return value must be also returned by test function.
 */
void test_encode_base64_url_002()
{
    const mcl_uint8_t *data = "tmp";
    const mcl_size_t data_size = 0;
	string_t *encoded_data;
	string_initialize_new("tmp", 3, &encoded_data);

    base64_url_encode_ExpectAndReturn(data, data_size, &encoded_data, MCL_TRIGGERED_WITH_NULL);

    E_MCL_ERROR_CODE code = security_handler_base64_url_encode(data, data_size, &encoded_data);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_TRIGGERED_WITH_NULL, code, "MCL_TRIGGERED_WITH_NULL  expected as return code!");

	string_destroy(&encoded_data);
}

/**
 * GIVEN : Test data for mocked SHA256 calculation given.
 * WHEN  : SHA256 calculation in security handler module called.
 * THEN  : MCL_OK is configured to be returned by mock. This return value must be also returned by test function.
 */
void test_hash_sha256_001()
{
    const mcl_uint8_t *data = "tmp";
    const mcl_size_t data_size = 0;
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size;

    security_hash_sha256_ExpectAndReturn(data, data_size, &hash, &hash_size, MCL_OK);

    E_MCL_ERROR_CODE code = security_handler_hash_sha256(data, data_size, &hash, &hash_size);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "MCL_OK  expected as return code!");
}

/**
* GIVEN : Test data for mocked base64 encoding given.
* WHEN  : Generate jti in security handler module called.
* THEN  : MCL_OK is configured to be returned by mock. This return value must be also returned by test function.
*/
void test_generate_jti_001(void)
{
	string_t *jti;
    security_generate_random_bytes_IgnoreAndReturn(MCL_OK);
	E_MCL_ERROR_CODE result = security_handler_generate_jti(&jti);

	TEST_ASSERT_EQUAL(MCL_OK, result);
	TEST_ASSERT_NOT_NULL(jti->buffer);

	string_destroy(&jti);
}

/**
* GIVEN : Initialized security handler.
* WHEN  : security_handler_generate_rsa_key is called.
* THEN  : MCL_OK is returned. Public and private keys are both not null.
*/
void test_generate_rsa_key_001(void)
{
    E_MCL_ERROR_CODE result = security_handler_initialize(&security_handler);
    TEST_ASSERT_EQUAL(MCL_OK, result);
    TEST_ASSERT_NOT_NULL(security_handler);

    security_generate_rsa_key_IgnoreAndReturn(MCL_OK);

    result = security_handler_generate_rsa_key(security_handler);
    TEST_ASSERT_EQUAL(MCL_OK, result);

    security_handler_destroy(&security_handler);
}
