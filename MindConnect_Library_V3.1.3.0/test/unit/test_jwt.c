/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_jwt.c
* @date     Aug 12, 2016
* @brief    Unit test cases for testing jwt module.
*
************************************************************************/

#include "unity.h"
#include "memory.h"
#include "definitions.h"
#include "mcl/mcl_common.h"
#include "jwt.h"
#include "string_array.h"
#include "string_util.h"
#include "string_type.h"
#include "mock_security_handler.h"
#include "json_util.h"
#include "time_util.h"

jwt_t *jwt = MCL_NULL;
security_handler_t *security_handler = MCL_NULL;
E_MCL_ERROR_CODE result;
string_t *jti = MCL_NULL;
string_t *tenant = MCL_NULL;
char *jti_value = "2b1a7b1a56384e018fee50f02c38366a";
char *expected_schema_value = "urn:siemens:mindsphere:v1";

void setUp(void)
{
    result = -1;

    MCL_NEW(security_handler);
    string_initialize_new("s6BhdRkqt3", 0, &security_handler->client_id);
    string_initialize_new("br-smk1", 0, &tenant);

    string_initialize_new(jti_value, 0, &jti);
    security_handler_generate_jti_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_generate_jti_ReturnThruPtr_jti(&jti);
}

void tearDown(void)
{
    string_destroy(&tenant);
    string_destroy(&security_handler->client_id);
    MCL_FREE(security_handler);
}

void _check_json_child_string(json_t *root, const char *child_name, const char *child_value)
{
    json_t *name = MCL_NULL;
    string_t *value = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_get_object_item(root, child_name, &name);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "Child does not exist in root.");

    json_util_get_string(name, &value);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(child_value, value->buffer, "Value of child mismatch.");

    MCL_FREE(name);
    string_destroy(&value);
}

/**
 * GIVEN : MCL_SECURITY_SHARED_SECRET security profile, JTI value and a security handler with client id.
 * WHEN  : Initialize JWT.
 * THEN  : JWT is created, its header and payload are correctly set and MCL_OK is returned.
 */
void test_initialize_001(void)
{
    E_MCL_ERROR_CODE code;
    jwt_t *jwt = MCL_NULL;
    json_t *exp = MCL_NULL;
    json_t *nbf = MCL_NULL;
    json_t *iat = MCL_NULL;
    json_t *schema = MCL_NULL;
    json_t *schema_array = MCL_NULL;
    double exp_value = 0;
    double nbf_value = 0;
    double iat_value = 0;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, result, "jwt_initialize returned error.");
    TEST_ASSERT_NOT_NULL(jwt);

    TEST_ASSERT_EQUAL(security_handler, jwt->security_handler);
    TEST_ASSERT_EQUAL(MCL_SECURITY_SHARED_SECRET, jwt->security_profile);
    TEST_ASSERT_NOT_NULL(jwt->header);
    TEST_ASSERT_NOT_NULL(jwt->payload);

    _check_json_child_string(jwt->header, "typ", "JWT");
    _check_json_child_string(jwt->header, "alg", "HS256");
    _check_json_child_string(jwt->payload, "iss", security_handler->client_id->buffer);
    _check_json_child_string(jwt->payload, "sub", security_handler->client_id->buffer);
    _check_json_child_string(jwt->payload, "aud", "southgate");
    _check_json_child_string(jwt->payload, "ten", tenant->buffer);
    _check_json_child_string(jwt->payload, "jti", jti_value);

    code = json_util_get_object_item(jwt->payload, "iat", &iat);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "<iat> field does not exist in payload.");

    code = json_util_get_object_item(jwt->payload, "nbf", &nbf);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "<nbf> field does not exist in payload.");

    code = json_util_get_object_item(jwt->payload, "exp", &exp);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "<exp> field does not exist in payload.");

    json_util_get_double_value(iat, &iat_value);
    json_util_get_double_value(nbf, &nbf_value);
    json_util_get_double_value(exp, &exp_value);

    TEST_ASSERT_EQUAL_INT_MESSAGE(iat_value, nbf_value, "Values of <iat> and <nbf> fields are not equal.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(iat_value + 86400, exp_value, "Value of <exp> is incorrect.");

    code = json_util_get_object_item(jwt->payload, "schemas", &schema_array);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "<schemas> array does not exist in payload.");

    code = json_util_get_array_item(schema_array, 0, &schema);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "No schema in <schemas> array.");

    string_t *schema_value = MCL_NULL;
    json_util_get_string(schema, &schema_value);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_schema_value, schema_value->buffer, "Value of schema mismatch.");
    string_destroy(&schema_value);

    MCL_FREE(iat);
    MCL_FREE(nbf);
    MCL_FREE(exp);
    json_util_destroy(&schema);
    MCL_FREE(schema_array);
    jwt_destroy(&jwt);
}

/**
 * GIVEN : MCL_SECURITY_RSA_3072 security profile, JTI value and a security handler with client id.
 * WHEN  : Initialize JWT.
 * THEN  : JWT is created, its header and payload are correctly set and MCL_OK is returned.
 */
void test_initialize_002(void)
{
    E_MCL_ERROR_CODE code;
    jwt_t *jwt = MCL_NULL;
    json_t *exp = MCL_NULL;
    json_t *nbf = MCL_NULL;
    json_t *iat = MCL_NULL;
    json_t *schema = MCL_NULL;
    json_t *schema_array = MCL_NULL;
    double exp_value = 0;
    double nbf_value = 0;
    double iat_value = 0;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);

    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, result, "jwt_initialize returned error.");
    TEST_ASSERT_NOT_NULL(jwt);

    TEST_ASSERT_EQUAL(security_handler, jwt->security_handler);
    TEST_ASSERT_EQUAL(MCL_SECURITY_RSA_3072, jwt->security_profile);
    TEST_ASSERT_NOT_NULL(jwt->header);
    TEST_ASSERT_NOT_NULL(jwt->payload);

    _check_json_child_string(jwt->header, "typ", "JWT");
    _check_json_child_string(jwt->header, "alg", "RS256");
    _check_json_child_string(jwt->payload, "iss", security_handler->client_id->buffer);
    _check_json_child_string(jwt->payload, "sub", security_handler->client_id->buffer);
    _check_json_child_string(jwt->payload, "aud", "southgate");
    _check_json_child_string(jwt->payload, "ten", tenant->buffer);
    _check_json_child_string(jwt->payload, "jti", jti_value);

    code = json_util_get_object_item(jwt->payload, "iat", &iat);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "<iat> field does not exist in payload.");

    code = json_util_get_object_item(jwt->payload, "nbf", &nbf);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "<nbf> field does not exist in payload.");

    code = json_util_get_object_item(jwt->payload, "exp", &exp);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "<exp> field does not exist in payload.");

    json_util_get_double_value(iat, &iat_value);
    json_util_get_double_value(nbf, &nbf_value);
    json_util_get_double_value(exp, &exp_value);

    TEST_ASSERT_EQUAL_INT_MESSAGE(iat_value, nbf_value, "Values of <iat> and <nbf> fields are not equal.");
    TEST_ASSERT_EQUAL_INT_MESSAGE(iat_value + 86400, exp_value, "Value of <exp> is incorrect.");

    code = json_util_get_object_item(jwt->payload, "schemas", &schema_array);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "<schemas> array does not exist in payload.");

    code = json_util_get_array_item(schema_array, 0, &schema);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "No schema in <schemas> array.");

    string_t *schema_value = MCL_NULL;
    json_util_get_string(schema, &schema_value);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_schema_value, schema_value->buffer, "Value of schema mismatch.");
    string_destroy(&schema_value);

    MCL_FREE(iat);
    MCL_FREE(nbf);
    MCL_FREE(exp);
    json_util_destroy(&schema);
    MCL_FREE(schema_array);
    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initializes jwt.
 * WHEN  : User destroys the jwt
 * THEN  : Expects it to be NULL
 */
void test_destroy_001(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_NOT_NULL_MESSAGE(jwt, "JWT is NULL after it is initialized.");

    jwt_destroy(&jwt);

    TEST_ASSERT_NULL_MESSAGE(jwt, "JWT is not NULL after it is destroyed.");
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_SHARED_SECRET
 *         Mock settings :
 *             - jti               : 12345678901234567890
 *             - nonce             : 12345678901234567890
 *             - encoded header    : 12345678901234567890
 *             - encoded payload   : 12345678901234567890
 *             - encoded signature : 12345678901234567890
 *             - raw signature     : 12345678901234567890
 * THEN  : Expecting token will be 12345678901234567890.12345678901234567890.12345678901234567890
 */
void test_get_token_onboarding_basic_001(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("12345678901234567890", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("12345678901234567890", 0, &encoded_payload);

    string_t *encoded_signature;
    string_initialize_new("12345678901234567890", 0, &encoded_signature);

    // no need to fill raw string since base64 mock will return indepentendly the expected result. Just allocate a memory in order to release it later.
    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    char *expected = "12345678901234567890.12345678901234567890.12345678901234567890";

    // mock function settings :

    // this will be called to get the raw signature
    security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
    security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

    // security_handler_encode_base64_url will be called 3 times in jwt_get_token() func. So, we are mocking it two times:
    // 1. call will be to get encoded jwt header :
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    // 2. call will be to get encoded jwt payload
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    // 3. call will be to get encoded jwt signature
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

    // so, expected jwt token will be 12345678901234567890.12345678901234567890.12345678901234567890
    token = jwt_get_token(jwt);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

    string_destroy(&token);
    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_SHARED_SECRET
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - nonce             : 22222222222222222222
 *             - encoded header    : 33333333333333333333
 *             - encoded payload   : 44444444444444444444
 *             - encoded signature : 55555555555555555555
 *             - raw signature     : 66666666666666666666
 * THEN  : Expecting token will be 33333333333333333333.44444444444444444444.55555555555555555555
 */
void test_get_token_onboarding_basic_002(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("33333333333333333333", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("44444444444444444444", 0, &encoded_payload);

    string_t *encoded_signature;
    string_initialize_new("55555555555555555555", 0, &encoded_signature);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    char *expected = "33333333333333333333.44444444444444444444.55555555555555555555";

    // mock function settings :

    // this will be called to get the raw signature
    security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
    security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

    // security_handler_encode_base64_url will be called 3 times in jwt_get_token() func. So, we are mocking it two times:
    // 1. call will be to get encoded jwt header :
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    // 2. call will be to get encoded jwt payload
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    // 3. call will be to get encoded jwt signature
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

    // so, expected jwt token will be 33333333333333333333.44444444444444444444.55555555555555555555
    token = jwt_get_token(jwt);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

    string_destroy(&token);
    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_SHARED_SECRET
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - nonce             : 22222222222222222222
 *             - encoded header    : WILL RETURN NULL
 * THEN  : Expecting generate token will fail.
 */
void test_get_token_onboarding_basic_003(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    // will be called to get encoded header
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NULL(token);

    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_SHARED_SECRET
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - nonce             : 22222222222222222222
 *             - encoded header    : 33333333333333333333
 *             - encoded payload   : WILL RETURN FAIL
 * THEN  : Expecting generate token will fail.
 */
void test_get_token_onboarding_basic_004(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("33333333333333333333", 0, &encoded_header);

    // security_handler_encode_base64_url will be called 3 times in jwt_get_token() func. So, we are mocking it two times:
    // 1. call will be to get encoded jwt header :
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    // 2. call will be to get encoded jwt payload
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NULL(token);

    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_SHARED_SECRET
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - nonce             : 22222222222222222222
 *             - encoded header    : 33333333333333333333
 *             - encoded payload   : 44444444444444444444
 *             - raw signature     : WILL RETURN FAIL
 * THEN  : Expecting generate token will fail.
 */
void test_get_token_onboarding_basic_005(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("33333333333333333333", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("44444444444444444444", 0, &encoded_payload);

    // mock function settings :

    // security_handler_encode_base64_url will be called 3 times in jwt_get_token() func. So, we are mocking it 3 times:
    // 1. call will be to get encoded jwt header :
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    // 2. call will be to get encoded jwt payload
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    // this will be called to get the raw signature
    security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_FAIL);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NULL(token);

    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_SHARED_SECRET
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - nonce             : 22222222222222222222
 *             - encoded header    : 33333333333333333333
 *             - encoded payload   : 44444444444444444444
 *             - encoded signature : WILL RETURN FAIL
 *             - raw signature     : 66666666666666666666
 * THEN  : Expecting token will be 33333333333333333333.44444444444444444444.55555555555555555555
 */
void test_get_token_onboarding_basic_006(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("33333333333333333333", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("44444444444444444444", 0, &encoded_payload);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    // mock function settings :

    // this will be called to get the raw signature
    security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
    security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

    // security_handler_encode_base64_url will be called 3 times in jwt_get_token() func. So, we are mocking it two times:
    // 1. call will be to get encoded jwt header :
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    // 2. call will be to get encoded jwt payload
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    // 3. call will be to get encoded jwt signature
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NULL(token);

    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_SHARED_SECRET
 *         Mock settings :
 *             - jti               : 1
 *             - nonce             : 1
 *             - encoded header    : 1
 *             - encoded payload   : 1
 *             - encoded signature : 1
 *             - raw signature     : 1
 * THEN  : Expecting token will be 1.1.1
 */
void test_get_token_onboarding_basic_007(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("1", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("1", 0, &encoded_payload);

    string_t *encoded_signature;
    string_initialize_new("1", 0, &encoded_signature);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    char *expected = "1.1.1";

    // mock function settings :

    // this will be called to get the raw signature
    security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
    security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

    // security_handler_encode_base64_url will be called 3 times in jwt_get_token() func. So, we are mocking it two times:
    // 1. call will be to get encoded jwt header :
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    // 2. call will be to get encoded jwt payload
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    // 3. call will be to get encoded jwt signature
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

    // so, expected jwt token will be 1.1.1
    token = jwt_get_token(jwt);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

    jwt_destroy(&jwt);
    string_destroy(&token);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_SHARED_SECRET
 *         Mock settings :
 *             - jti               : EMTPY
 *             - nonce             : EMTPY
 *             - encoded header    : EMTPY
 *             - encoded payload   : EMTPY
 *             - encoded signature : EMTPY
 *             - raw signature     : EMTPY
 * THEN  : Expecting token will be ..
 */
void test_get_token_onboarding_basic_008(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("", 0, &encoded_payload);

    string_t *encoded_signature;
    string_initialize_new("", 0, &encoded_signature);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    char *expected = "..";

    // mock function settings :

    // this will be called to get the raw signature
    security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
    security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

    // security_handler_encode_base64_url will be called 3 times in jwt_get_token() func. So, we are mocking it two times:
    // 1. call will be to get encoded jwt header :
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    // 2. call will be to get encoded jwt payload
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    // 3. call will be to get encoded jwt signature
    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

    // so, expected jwt token will be ..
    token = jwt_get_token(jwt);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

    jwt_destroy(&jwt);
    string_destroy(&token);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_RSA_3072
 *         Mock settings :
 *             - jti               : 12345678901234567890
 *             - public_key        : 12345678901234567890
 *             - encoded header    : 12345678901234567890
 *             - encoded payload   : 12345678901234567890
 *             - encoded signature : 12345678901234567890
 *             - raw signature     : 12345678901234567890
 * THEN  : Expecting token will be 12345678901234567890.12345678901234567890.12345678901234567890
 */
void test_get_token_onboarding_smart_001(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("12345678901234567890", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("12345678901234567890", 0, &encoded_payload);

    string_t *encoded_signature;
    string_initialize_new("12345678901234567890", 0, &encoded_signature);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    char *expected = "12345678901234567890.12345678901234567890.12345678901234567890";

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    security_handler_rsa_sign_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_rsa_sign_ReturnThruPtr_signature(&raw_signature);
    security_handler_rsa_sign_ReturnThruPtr_signature_size(&raw_signature_size);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

    jwt_destroy(&jwt);
    string_destroy(&token);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_RSA_3072
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - public_key        : 22222222222222222222
 *             - encoded header    : 33333333333333333333
 *             - encoded payload   : 44444444444444444444
 *             - encoded signature : 55555555555555555555
 *             - raw signature     : 66666666666666666666
 * THEN  : Expecting token will be 33333333333333333333.44444444444444444444.55555555555555555555
 */
void test_get_token_onboarding_smart_002(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("33333333333333333333", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("44444444444444444444", 0, &encoded_payload);

    string_t *encoded_signature;
    string_initialize_new("55555555555555555555", 0, &encoded_signature);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    char *expected = "33333333333333333333.44444444444444444444.55555555555555555555";

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    security_handler_rsa_sign_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_rsa_sign_ReturnThruPtr_signature(&raw_signature);
    security_handler_rsa_sign_ReturnThruPtr_signature_size(&raw_signature_size);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

    jwt_destroy(&jwt);
    string_destroy(&token);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_RSA_3072
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - public_key        : 22222222222222222222
 *             - encoded header    : WILL RETURN NULL
 * THEN  : Expecting generate token will fail.
 */
void test_get_token_onboarding_smart_003(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NULL(token);

    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_RSA_3072
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - public_key        : 22222222222222222222
 *             - encoded header    : 33333333333333333333
 *             - encoded payload   : WILL RETURN FAIL
 * THEN  : Expecting generate token will fail.
 */
void test_get_token_onboarding_smart_004(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("33333333333333333333", 0, &encoded_header);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NULL(token);

    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_RSA_3072
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - public_key        : 22222222222222222222
 *             - encoded header    : 33333333333333333333
 *             - encoded payload   : 44444444444444444444
 *             - raw signature     : WILL RETURN FAIL
 * THEN  : Expecting generate token will fail.
 */
void test_get_token_onboarding_smart_005(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("33333333333333333333", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("44444444444444444444", 0, &encoded_payload);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    security_handler_rsa_sign_ExpectAnyArgsAndReturn(MCL_FAIL);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NULL(token);

    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_RSA_3072
 *         Mock settings :
 *             - jti               : 11111111111111111111
 *             - public_key        : 22222222222222222222
 *             - encoded header    : 33333333333333333333
 *             - encoded payload   : 44444444444444444444
 *             - encoded signature : WILL RETURN FAIL
 *             - raw signature     : 66666666666666666666
 * THEN  : Expecting token will be 33333333333333333333.44444444444444444444.55555555555555555555
 */
void test_get_token_onboarding_smart_006(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("33333333333333333333", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("44444444444444444444", 0, &encoded_payload);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    security_handler_rsa_sign_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_rsa_sign_ReturnThruPtr_signature(&raw_signature);
    security_handler_rsa_sign_ReturnThruPtr_signature_size(&raw_signature_size);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NULL(token);

    jwt_destroy(&jwt);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_RSA_3072
 *         Mock settings :
 *             - jti               : 1
 *             - public_key        : 1
 *             - encoded header    : 1
 *             - encoded payload   : 1
 *             - encoded signature : 1
 *             - raw signature     : 1
 * THEN  : Expecting token will be 1.1.1
 */
void test_get_token_onboarding_smart_007(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("1", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("1", 0, &encoded_payload);

    string_t *encoded_signature;
    string_initialize_new("1", 0, &encoded_signature);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    char *expected = "1.1.1";

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    security_handler_rsa_sign_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_rsa_sign_ReturnThruPtr_signature(&raw_signature);
    security_handler_rsa_sign_ReturnThruPtr_signature_size(&raw_signature_size);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

    jwt_destroy(&jwt);
    string_destroy(&token);
}

/**
 * GIVEN : User initialized jwt. Sets security_handler mock functions.
 * WHEN  : User requests jwt to generate onboarding token. Security profile is MCL_SECURITY_RSA_3072
 *         Mock settings :
 *             - jti               : EMTPY
 *             - public_key        : EMTPY
 *             - encoded header    : EMTPY
 *             - encoded payload   : EMTPY
 *             - encoded signature : EMTPY
 *             - raw signature     : EMTPY
 * THEN  : Expecting token will be 12345678901234567890.12345678901234567890.12345678901234567890
 */
void test_get_token_onboarding_smart_008(void)
{
    jwt_t *jwt = MCL_NULL;

    result = jwt_initialize(security_handler, MCL_SECURITY_RSA_3072, tenant, &jwt);
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *token;

    string_t *encoded_header;
    string_initialize_new("", 0, &encoded_header);

    string_t *encoded_payload;
    string_initialize_new("", 0, &encoded_payload);

    string_t *encoded_signature;
    string_initialize_new("", 0, &encoded_signature);

    mcl_size_t raw_signature_size = 20;
    mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

    char *expected = "..";

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

    security_handler_rsa_sign_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_rsa_sign_ReturnThruPtr_signature(&raw_signature);
    security_handler_rsa_sign_ReturnThruPtr_signature_size(&raw_signature_size);

    security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

    token = jwt_get_token(jwt);

    TEST_ASSERT_NOT_NULL(token);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

    jwt_destroy(&jwt);
    string_destroy(&token);
}

/**
* GIVEN : User initialized jwt. Sets security_handler mock functions.
* WHEN  : User requests jwt to generate authentication token. Security profile is MCL_SECURITY_SHARED_SECRET.
*         Mock settings :
*             - jti               : 12345678901234567890
*             - content_md5       : 12345678901234567890
*             - encoded header    : 12345678901234567890
*             - encoded payload   : 12345678901234567890
*             - encoded signature : 12345678901234567890
*             - encoded content   : HTTP
*             - raw signature     : 12345678901234567890
* THEN  : Expecting token will be 12345678901234567890.12345678901234567890.12345678901234567890
*/
void test_get_token_authentication_basic_001(void)
{
	jwt_t *jwt = MCL_NULL;

	result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
	TEST_ASSERT_EQUAL_INT(0, result);

	string_t *token;

	string_t *encoded_header;
	string_initialize_new("12345678901234567890", 0, &encoded_header);

	string_t *encoded_payload;
	string_initialize_new("12345678901234567890", 0, &encoded_payload);

	string_t *encoded_signature;
	string_initialize_new("12345678901234567890", 0, &encoded_signature);

	mcl_size_t raw_signature_size = 20;
	mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

	char *expected = "12345678901234567890.12345678901234567890.12345678901234567890";

	// mock function settings :

	// this will be called to get the raw signature
	security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
	security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

	// 1. call will be to get encoded jwt header :
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

	// 2. call will be to get encoded jwt payload
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

	// 3. call will be to get encoded jwt signature
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

	// so, expected jwt token will be 12345678901234567890.12345678901234567890.12345678901234567890
	token = jwt_get_token(jwt);

	TEST_ASSERT_NOT_NULL(token);
	TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

	jwt_destroy(&jwt);
	string_destroy(&token);
}

/**
* GIVEN : User initialized jwt. Sets security_handler mock functions.
* WHEN  : User requests jwt to generate authentication token.  Security profile is MCL_SECURITY_SHARED_SECRET.
*         Mock settings :
*             - jti               : 11111111111111111111
*             - content_md5       : 22222222222222222222
*             - encoded header    : 33333333333333333333
*             - encoded payload   : 44444444444444444444
*             - encoded signature : 55555555555555555555
*             - raw signature     : 66666666666666666666
*             - encoded content   : HTTP
* THEN  : Expecting token will be 33333333333333333333.44444444444444444444.55555555555555555555
*/
void test_get_token_authentication_basic_002(void)
{
	jwt_t *jwt = MCL_NULL;

	result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
	TEST_ASSERT_EQUAL_INT(0, result);

	string_t *token;

	string_t *encoded_header;
	string_initialize_new("33333333333333333333", 0, &encoded_header);

	string_t *encoded_payload;
	string_initialize_new("44444444444444444444", 0, &encoded_payload);

	string_t *encoded_signature;
	string_initialize_new("55555555555555555555", 0, &encoded_signature);

	mcl_size_t raw_signature_size = 20;
	mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

	char *expected = "33333333333333333333.44444444444444444444.55555555555555555555";

	// mock function settings :

	// this will be called to get the raw signature
	security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
	security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

	// 1. call will be to get encoded jwt header :
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

	// 2. call will be to get encoded jwt payload
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

	// 3. call will be to get encoded jwt signature
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

	// so, expected jwt token will be 12345678901234567890.12345678901234567890.12345678901234567890
	token = jwt_get_token(jwt);

	TEST_ASSERT_NOT_NULL(token);
	TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

	jwt_destroy(&jwt);
	string_destroy(&token);
}

/**
* GIVEN : User initialized jwt. Sets security_handler mock functions.
* WHEN  : User requests jwt to generate authentication token.  Security profile is MCL_SECURITY_SHARED_SECRET.
*         Mock settings :
*             - jti               : 11111111111111111111
*             - content_md5       : 22222222222222222222
*             - encoded header    : WILL RETURN NULL
* THEN  : Expecting generate token will fail.
*/
void test_get_token_authentication_basic_003(void)
{
	jwt_t *jwt = MCL_NULL;

	result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
	TEST_ASSERT_EQUAL_INT(0, result);

	string_t *token;

	// will be called to get encoded header
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

	// end mocking

	token = jwt_get_token(jwt);

	TEST_ASSERT_NULL(token);

	jwt_destroy(&jwt);
}

/**
* GIVEN : User initialized jwt. Sets security_handler mock functions.
* WHEN  : User requests jwt to generate authentication token.  Security profile is MCL_SECURITY_SHARED_SECRET.
*         Mock settings :
*             - jti               : 11111111111111111111
*             - content_md5       : 22222222222222222222
*             - encoded header    : 33333333333333333333
*             - encoded payload   : WILL RETURN FAIL
* THEN  : Expecting generate token will fail.
*/
void test_get_token_authentication_basic_004(void)
{
	jwt_t *jwt = MCL_NULL;

	result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
	TEST_ASSERT_EQUAL_INT(0, result);

	string_t *token;

	string_t *encoded_header;
	string_initialize_new("33333333333333333333", 0, &encoded_header);

	// 1. call will be to get encoded jwt header :
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

	// 2. call will be to get encoded jwt payload
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

	token = jwt_get_token(jwt);

	TEST_ASSERT_NULL(token);

	jwt_destroy(&jwt);
}

/**
* GIVEN : User initialized jwt. Sets security_handler mock functions.
* WHEN  : User requests jwt to generate authentication token.  Security profile is MCL_SECURITY_SHARED_SECRET.
*         Mock settings :
*             - jti               : 11111111111111111111
*             - content_md5       : 22222222222222222222
*             - encoded header    : 33333333333333333333
*             - encoded content   : HTTP
*             - encoded payload   : 44444444444444444444
*             - raw signature     : WILL RETURN FAIL
* THEN  : Expecting generate token will fail.
*/
void test_get_token_authentication_basic_005(void)
{
	jwt_t *jwt = MCL_NULL;

	result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
	TEST_ASSERT_EQUAL_INT(0, result);

	string_t *token;

	string_t *encoded_header;
	string_initialize_new("33333333333333333333", 0, &encoded_header);

	string_t *encoded_payload;
	string_initialize_new("44444444444444444444", 0, &encoded_payload);

	// 1. call will be to get encoded jwt header :
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

	// 2. call will be to get encoded jwt payload
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

	// this will be called to get the raw signature
	security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_FAIL);

	token = jwt_get_token(jwt);

	TEST_ASSERT_NULL(token);

	jwt_destroy(&jwt);
}

/**
* GIVEN : User initialized jwt. Sets security_handler mock functions.
* WHEN  : User requests jwt to generate authentication token.  Security profile is MCL_SECURITY_SHARED_SECRET.
*         Mock settings :
*             - jti               : 11111111111111111111
*             - content_md5       : 22222222222222222222
*             - encoded header    : 33333333333333333333
*             - encoded content   : HTTP
*             - encoded payload   : 44444444444444444444
*             - encoded signature : WILL RETURN FAIL
*             - raw signature     : 66666666666666666666
* THEN  : Expecting generate token will fail.
*/
void test_get_token_authentication_basic_006(void)
{
	jwt_t *jwt = MCL_NULL;

	result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
	TEST_ASSERT_EQUAL_INT(0, result);

	string_t *token;

	string_t *encoded_header;
	string_initialize_new("33333333333333333333", 0, &encoded_header);

	string_t *encoded_payload;
	string_initialize_new("44444444444444444444", 0, &encoded_payload);

	mcl_size_t raw_signature_size = 20;
	mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

	// this will be called to get the raw signature
	security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
	security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

	// 1. call will be to get encoded jwt header :
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

	// 2. call will be to get encoded jwt payload
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

	// 3. call will be to get encoded jwt signature
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_FAIL);

	token = jwt_get_token(jwt);

	TEST_ASSERT_NULL(token);

	jwt_destroy(&jwt);
}

/**
* GIVEN : User initialized jwt. Sets security_handler mock functions.
* WHEN  : User requests jwt to generate authentication token.  Security profile is MCL_SECURITY_SHARED_SECRET.
*         Mock settings :
*             - jti               : 1
*             - content_md5       : 1
*             - encoded header    : 1
*             - encoded content   : 1
*             - encoded payload   : 1
*             - encoded signature : 1
*             - raw signature     : 1
* THEN  : Expecting token will be 1.1.1
*/
void test_get_token_authentication_basic_007(void)
{
	jwt_t *jwt = MCL_NULL;

	result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
	TEST_ASSERT_EQUAL_INT(0, result);

	string_t *token;

	string_t *encoded_header;
	string_initialize_new("1", 0, &encoded_header);

	string_t *encoded_payload;
	string_initialize_new("1", 0, &encoded_payload);

	string_t *encoded_signature;
	string_initialize_new("1", 0, &encoded_signature);

	mcl_size_t raw_signature_size = 20;
	mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

	char *expected = "1.1.1";

	// this will be called to get the raw signature
	security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
	security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

	// 1. call will be to get encoded jwt header :
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

	// 2. call will be to get encoded jwt payload
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

	// 3. call will be to get encoded jwt signature
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

	// so, expected jwt token will be 1.1.1
	token = jwt_get_token(jwt);

	TEST_ASSERT_NOT_NULL(token);
	TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

	jwt_destroy(&jwt);
	string_destroy(&token);
}

/**
* GIVEN : User initialized jwt. Sets security_handler mock functions.
* WHEN  : User requests jwt to generate authentication token.  Security profile is MCL_SECURITY_SHARED_SECRET.
*         Mock settings :
*             - jti               : EMTPY
*             - content_md5       : EMTPY
*             - encoded content   : EMPTY
*             - encoded header    : EMTPY
*             - encoded payload   : EMTPY
*             - encoded signature : EMTPY
*             - raw signature     : EMTPY
* THEN  : Expecting token will be ".."
*/
void test_get_token_authentication_basic_008(void)
{
	jwt_t *jwt = MCL_NULL;

	result = jwt_initialize(security_handler, MCL_SECURITY_SHARED_SECRET, tenant, &jwt);
	TEST_ASSERT_EQUAL_INT(0, result);

	string_t *token;

	string_t *encoded_header;
	string_initialize_new("", 0, &encoded_header);

	string_t *encoded_payload;
	string_initialize_new("", 0, &encoded_payload);

	string_t *encoded_signature;
	string_initialize_new("", 0, &encoded_signature);

	mcl_size_t raw_signature_size = 20;
	mcl_uint8_t *raw_signature = MCL_MALLOC(raw_signature_size);

	char *expected = "..";

	// this will be called to get the raw signature
	security_handler_hmac_sha256_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_hmac_sha256_ReturnThruPtr_hash(&raw_signature);
	security_handler_hmac_sha256_ReturnThruPtr_hash_size(&raw_signature_size);

	// 1. call will be to get encoded jwt header :
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_header);

	// 2. call will be to get encoded jwt payload
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_payload);

	// 3. call will be to get encoded jwt signature
	security_handler_base64_url_encode_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_base64_url_encode_ReturnThruPtr_encoded_data(&encoded_signature);

	// so, expected jwt token will be ..
	token = jwt_get_token(jwt);

	TEST_ASSERT_NOT_NULL(token);
	TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, token->buffer, "Token is not prepared as expected!");

	jwt_destroy(&jwt);
	string_destroy(&token);
}
