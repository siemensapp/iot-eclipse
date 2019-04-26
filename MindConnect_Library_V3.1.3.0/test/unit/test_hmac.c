/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_hmac.c
 * @date     Aug 16, 2016
 * @brief    Test module for testing functions in HMAC module.
 *
 ************************************************************************/

#include "hmac.h"
#include "security_libcrypto.h"
#include "memory.h"
#include "unity.h"
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
 * GIVEN : Test data to calculate HMAC SHA256 is given.
 * WHEN  : HMAC SHA256 calculation called.
 * THEN  : Expected HMAC SHA256 with correct size must be returned.
 */
void test_sha256_001(void)
{
    string_t *data;
    string_initialize_static("ABC", 0, &data);

	string_t *key;
	string_initialize_static("123", 0, &key);

    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

    E_MCL_ERROR_CODE code = hmac_sha256((mcl_uint8_t *)(data->buffer), data->length, (mcl_uint8_t *)(key->buffer), key->length, &hash, &hash_size);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected return code not received");
    TEST_ASSERT_NOT_NULL_MESSAGE(hash, "HMAC SHA256 calculation failed!");
    TEST_ASSERT_MESSAGE(32 == hash_size, "Hash size must be 32 bytes long!");

    // convert for hex comparison
    string_t *hex_hash;
    code = string_convert_binary_to_hex(hash, hash_size, &hex_hash);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected return code not received");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("C05510B67A06D73B01EEB7D663AB79210F704A45DEDC013452F7E0563038ED53", hex_hash->buffer, "Expected MD5 not achieved!");

    MCL_FREE(hash);
    string_destroy(&data);
	string_destroy(&key);
    string_destroy(&hex_hash);
}

/**
 * GIVEN : Test data to calculate HMAC SHA256 is given. This time a long secret key is given. SHA256 of the key should be used.
 * WHEN  : HMAC SHA256 calculation called.
 * THEN  : Expected HMAC SHA256 with correct size must be returned.
 */
void test_sha256_002(void)
{
    string_t *data;
    string_initialize_static("ABC", 0, &data);

	string_t *key;
	string_initialize_static("0123456789012345678901234567890123456789012345678901234567890123456789", 0, &key);

    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

	E_MCL_ERROR_CODE code = hmac_sha256((mcl_uint8_t *)(data->buffer), data->length, (mcl_uint8_t *)(key->buffer), key->length, &hash, &hash_size);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected return code not received");
    TEST_ASSERT_NOT_NULL_MESSAGE(hash, "HMAC SHA256 calculation failed!");
    TEST_ASSERT_MESSAGE(32 == hash_size, "Hash size must be 32 bytes long!");

    // convert for hex comparison
    string_t *hex_hash;
    code = string_convert_binary_to_hex(hash, hash_size, &hex_hash);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected return code not received");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("0CF72FDDBD2EE9864877B2E46E5E2D9CFD1BF461C29B7288497E07100F32874E", hex_hash->buffer, "Expected MD5 not achieved!");

    MCL_FREE(hash);
	string_destroy(&data);
	string_destroy(&key);
    string_destroy(&hex_hash);
}

/**
 * GIVEN : Key with size 0 is given.
 * WHEN  : HMAC SHA256 calculation called.
 * THEN  : Expected error code must be returned.
 */
void test_sha256_003(void)
{
	string_t *data;
	string_initialize_static("ABC", 0, &data);

	string_t *key;
	string_initialize_static("123", 0, &key);
	key->length = 0;

    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

	E_MCL_ERROR_CODE code = hmac_sha256((mcl_uint8_t *)(data->buffer), data->length, (mcl_uint8_t *)(key->buffer), key->length, &hash, &hash_size);

    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "Expected return code not received");

	string_destroy(&data);
	string_destroy(&key);
}
