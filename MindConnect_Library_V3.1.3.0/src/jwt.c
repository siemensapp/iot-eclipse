/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     jwt.c
 * @date     Jun 28, 2016
 * @brief    JWT module implementation file.
 *
 ************************************************************************/

#include "jwt.h"
#include "log_util.h"
#include "definitions.h"
#include "memory.h"
#include "string_array.h"
#include "time_util.h"
#include "json_util.h"

// JWT Specific constants:
#define JWT_HEADER_NAME_ALG "alg"
#define JWT_HEADER_NAME_TYPE "typ"
#define JWT_HEADER_NAME_KID "kid"

#define JWT_PAYLOAD_NAME_ISS "iss"
#define JWT_PAYLOAD_NAME_SUB "sub"
#define JWT_PAYLOAD_NAME_AUD "aud"
#define JWT_PAYLOAD_NAME_IAT "iat"
#define JWT_PAYLOAD_NAME_NBF "nbf"
#define JWT_PAYLOAD_NAME_EXP "exp"
#define JWT_PAYLOAD_NAME_SCHEMAS "schemas"
#define JWT_PAYLOAD_NAME_TEN "ten"
#define JWT_PAYLOAD_NAME_JTI "jti"
#define JWT_PAYLOAD_NAME_PUBLIC_KEY "public_key"
#define JWT_PAYLOAD_NAME_CONTENT_MD5 "content-md5"

#define JWT_HEADER_VALUE_ALG_SHARED_SECRET "HS256"
#define JWT_HEADER_VALUE_ALG_RSA_3072 "RS256"
#define JWT_HEADER_VALUE_TYP "JWT"

#define JWT_PAYLOAD_VALUE_SCHEMAS "urn:siemens:mindsphere:v1"
#define JWT_PAYLOAD_VALUE_AUD "southgate"

// Creates header JSON object of JWT.
static E_MCL_ERROR_CODE _create_self_issued_jwt_header(E_MCL_SECURITY_PROFILE security_profile, json_t **header);

// Creates payload JSON object of JWT.
static E_MCL_ERROR_CODE _create_self_issued_jwt_payload(security_handler_t *security_handler, string_t *tenant, jwt_t *jwt);

// to add schema array to payload of JWT.
static E_MCL_ERROR_CODE _add_schema_to_jwt(json_t *payload);

// encodes header and payload with base64. Clears the input strings if fails.
static E_MCL_ERROR_CODE _get_header_and_payload_encoded_base64_url(string_t *header, string_t *payload, string_t **header_encoded, string_t **payload_encoded);

// join two string with dot (.) between and generate a new string while destroys the two string :
static E_MCL_ERROR_CODE _join_with_dot(string_t *string_a, string_t *string_b, string_t **joined);

// calculates the signature from the header and payload string by calling the HMAC_SHA256 and encode_base64_url functions. NOT destroys the header and payload string!:
static E_MCL_ERROR_CODE _calculate_signature(jwt_t *jwt, string_t *header_and_payload, string_t **signature);

// prepares the jwt, then calculates the necessary headers and signature and produces the token :
static E_MCL_ERROR_CODE _generate_token(jwt_t *jwt, string_t *header, string_t *payload, string_t **token);

// this dot will be used every time a jwt is created. so instead of creating a new one in each call, define here once:
static string_t dot = { ".", 1, MCL_STRING_NOT_COPY_NOT_DESTROY };

E_MCL_ERROR_CODE jwt_initialize(security_handler_t *security_handler, E_MCL_SECURITY_PROFILE security_profile, string_t *tenant, jwt_t **jwt)
{
    E_MCL_ERROR_CODE code;

    DEBUG_ENTRY("security_handler_t *security_handler = <%p>, E_MCL_SECURITY_PROFILE security_profile = <%d>, string_t *tenant = <%p>, jwt_t **jwt = <%p>", security_handler,
                security_profile, tenant, jwt)

    MCL_NEW(*jwt);
    ASSERT_CODE_MESSAGE(MCL_NULL != *jwt, MCL_OUT_OF_MEMORY, "Not enough memory to allocate jwt!");

    (*jwt)->header = MCL_NULL;
    (*jwt)->payload = MCL_NULL;
    (*jwt)->security_handler = security_handler;
    (*jwt)->security_profile = security_profile;

    // Create JWT header.
    code = _create_self_issued_jwt_header(security_profile, &((*jwt)->header));
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, jwt_destroy(jwt), code, "Header of JWT can not be created.");

    // Create JWT payload.
    code = _create_self_issued_jwt_payload(security_handler, tenant, *jwt);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, jwt_destroy(jwt), code, "Payload of JWT can not be created.");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

string_t *jwt_get_token(jwt_t *jwt)
{
    DEBUG_ENTRY("jwt_t *jwt = <%p>", jwt)

    E_MCL_ERROR_CODE code;
    string_t *token = MCL_NULL;
    string_t *header = MCL_NULL;
    string_t *payload = MCL_NULL;

    char *header_json = MCL_NULL;
    char *payload_json = MCL_NULL;

    code = json_util_to_string(jwt->header, &header_json);
    (MCL_OK == code) && (code = string_initialize_dynamic(header_json, 0, &header));

    (MCL_OK == code) && (code = json_util_to_string(jwt->payload, &payload_json));
    (MCL_OK == code) && (code = string_initialize_dynamic(payload_json, 0, &payload));

    (MCL_OK == code) && _generate_token(jwt, header, payload, &token);

    // call reset() before go back to make it ready for the next call :
    // TODO : jwt_reset(jwt);

    // clean up
    string_destroy(&header);
    string_destroy(&payload);

    DEBUG_LEAVE("retVal = <%p>", token);
    return token;
}

void jwt_destroy(jwt_t **jwt)
{
    DEBUG_ENTRY("jwt_t **jwt = <%p>", jwt)

    if (MCL_NULL != *jwt)
    {
        if (MCL_NULL != (*jwt)->header)
        {
            json_util_destroy(&(*jwt)->header);
        }

        if (MCL_NULL != (*jwt)->payload)
        {
            json_util_destroy(&(*jwt)->payload);
        }

        MCL_FREE(*jwt);

        MCL_DEBUG("JWT handle is destroyed.");
    }
    else
    {
        MCL_DEBUG("JWT handle is already NULL.");
    }

    DEBUG_LEAVE("retVal = void");
}

// ------ Private Functions ---------------

static E_MCL_ERROR_CODE _create_self_issued_jwt_header(E_MCL_SECURITY_PROFILE security_profile, json_t **header)
{
    DEBUG_ENTRY("E_MCL_SECURITY_PROFILE security_profile = <%d>, json_t **header = <%p>", security_profile, header)

    E_MCL_ERROR_CODE code = json_util_initialize(JSON_OBJECT, header);

    if (MCL_SECURITY_SHARED_SECRET == security_profile)
    {
        (MCL_OK == code) && (code = json_util_add_string(*header, JWT_HEADER_NAME_TYPE, JWT_HEADER_VALUE_TYP));
        (MCL_OK == code) && (code = json_util_add_string(*header, JWT_HEADER_NAME_ALG, JWT_HEADER_VALUE_ALG_SHARED_SECRET));
    }
    else
    {
        // it has to be MCL_SECURITY_RSA_3072
        (MCL_OK == code) && (code = json_util_add_string(*header, JWT_HEADER_NAME_TYPE, JWT_HEADER_VALUE_TYP));
        (MCL_OK == code) && (code = json_util_add_string(*header, JWT_HEADER_NAME_ALG, JWT_HEADER_VALUE_ALG_RSA_3072));

        // TODO: Add JWT_HEADER_NAME_KID to header
    }

    if (MCL_OK != code)
    {
        json_util_destroy(header);
    }

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _create_self_issued_jwt_payload(security_handler_t *security_handler, string_t *tenant, jwt_t *jwt)
{
	DEBUG_ENTRY("security_handler_t *security_handler = <%p>, string_t *tenant = <%p>, jwt_t *jwt = <%p>", security_handler, tenant, jwt)

	E_MCL_ERROR_CODE code;
    mcl_time_t current_time;
    string_t *jti;
	json_t *payload = MCL_NULL;

    // Create JTI.
    code = security_handler_generate_jti(&jti);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Generate jti from security_handler failed!");

    // Initialize the payload json object.
    code = json_util_initialize(JSON_OBJECT, &payload);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_OK == code, string_destroy(&jti), code, "JSON payload can not be initialized.");

    // Get current time in seconds.
    time_util_get_time(&current_time);

    // Fill payload json object.
    code = json_util_add_string(payload, JWT_PAYLOAD_NAME_ISS, security_handler->client_id->buffer);
    (MCL_OK == code) && (code = json_util_add_string(payload, JWT_PAYLOAD_NAME_SUB, security_handler->client_id->buffer));
    (MCL_OK == code) && (code = json_util_add_string(payload, JWT_PAYLOAD_NAME_AUD, JWT_PAYLOAD_VALUE_AUD));
    (MCL_OK == code) && (code = json_util_add_double(payload, JWT_PAYLOAD_NAME_IAT, (double)current_time));
    (MCL_OK == code) && (code = json_util_add_double(payload, JWT_PAYLOAD_NAME_NBF, (double)current_time));
    (MCL_OK == code) && (code = json_util_add_double(payload, JWT_PAYLOAD_NAME_EXP, (double)(current_time + JWT_EXPIRATION_TIME)));
    (MCL_OK == code) && (code = _add_schema_to_jwt(payload));
    (MCL_OK == code) && (code = json_util_add_string(payload, JWT_PAYLOAD_NAME_TEN, tenant->buffer));
    (MCL_OK == code) && (code = json_util_add_string(payload, JWT_PAYLOAD_NAME_JTI, jti->buffer));

    if (MCL_OK == code)
    {
        jwt->payload = payload;
        jwt->issued_at = current_time;
    }
    else
    {
        json_util_destroy(&payload);
    }
    string_destroy(&jti);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

static E_MCL_ERROR_CODE _generate_token(jwt_t *jwt, string_t *header, string_t *payload, string_t **token)
{
    DEBUG_ENTRY("jwt_t *jwt = <%p>, string_t *header = <%p>, string_t *payload = <%p>, string_t **token = <%p>", jwt, header, payload, token)

    string_t *header_encoded;
    string_t *payload_encoded;
    string_t *header_and_payload = MCL_NULL;
    string_t *signature = MCL_NULL;

    // 1- Encode Header and Payload to Base64 URL :
    ASSERT_CODE_MESSAGE(MCL_OK == _get_header_and_payload_encoded_base64_url(header, payload, &header_encoded, &payload_encoded), MCL_FAIL, "Encode header - payload failed!");

    // 2- Join Header and Payload. Don't lose this joined header and payload. We will use it again :
    ASSERT_CODE_MESSAGE(MCL_OK == _join_with_dot(header_encoded, payload_encoded, &header_and_payload), MCL_FAIL, "Join header and payload failed!");

    // 3- Calculate the Base64 URL encoded signature from header and payload joined together :
    if (MCL_OK != _calculate_signature(jwt, header_and_payload, &signature))
    {
        string_destroy(&header_and_payload);
        MCL_ERROR_RETURN(MCL_FAIL, "Generate signature failed!");
    }

    // 4- Join signature with header and payload to get the final token :
    ASSERT_CODE_MESSAGE(MCL_OK == _join_with_dot(header_and_payload, signature, token), MCL_FAIL, "Joining header and payload with signature failed!");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _get_header_and_payload_encoded_base64_url(string_t *header, string_t *payload, string_t **header_encoded, string_t **payload_encoded)
{
    DEBUG_ENTRY("string_t *header = <%p>, string_t *payload = <%p>, string_t **header_encoded = <%p>, string_t **payload_encoded = <%p>", header, payload, header_encoded,
                payload_encoded)

    E_MCL_ERROR_CODE result = security_handler_base64_url_encode((mcl_uint8_t *)header->buffer, header->length, header_encoded);

    // header ok ?
    ASSERT_CODE_MESSAGE(MCL_OK == result, MCL_FAIL, "Encode header failed!");

    result = security_handler_base64_url_encode((mcl_uint8_t *)payload->buffer, payload->length, payload_encoded);

    // payload ok ?
    if (MCL_OK != result)
    {
        // header was ok. destroy it before go back
        string_destroy(header_encoded);
        MCL_ERROR_RETURN(MCL_FAIL, "Encode payload failed!");
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _join_with_dot(string_t *string_a, string_t *string_b, string_t **joined)
{
    DEBUG_ENTRY("string_t *string_a = <%p>, string_t *string_b = <%p>, string_t **joined = <%p>", string_a, string_b, joined)

    string_array_t *string_array;

    // 3 is for header + . + token + . + sign :
    E_MCL_ERROR_CODE code = string_array_initialize(3, &string_array);
    if (MCL_OK != code)
    {
        string_destroy(&string_a);
        string_destroy(&string_b);
        MCL_ERROR_RETURN(code, "Initializing string array is failed!");
    }

    string_array_add(string_array, string_a, MCL_FALSE);
    string_array_add(string_array, &dot, MCL_FALSE);
    string_array_add(string_array, string_b, MCL_FALSE);

    // to_string if index correct ( if add operations succeeded ) :
    if (string_array->index == 3)
    {
        *joined = string_array_to_string(string_array);
    }

    string_array_destroy(&string_array);
    string_destroy(&string_a);
    string_destroy(&string_b);

    // if *joined is NULL; either string_array_add failed or string_array_to_string :
    ASSERT_CODE_MESSAGE(MCL_NULL != *joined, MCL_FAIL, "Strings couldn't be joined!");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _calculate_signature(jwt_t *jwt, string_t *header_and_payload, string_t **signature)
{
    DEBUG_ENTRY("jwt_t *jwt = <%p>, string_t *header_and_payload = <%p>, string_t **signature = <%p>", jwt, header_and_payload, signature)

	E_MCL_ERROR_CODE code;
    mcl_uint8_t *hash = MCL_NULL;
    mcl_size_t hash_size = 0;

    // calculate HMAC SHA256 of header and payload. security_handler will use the appropriate key to sign:
    if (MCL_SECURITY_SHARED_SECRET == jwt->security_profile)
    {
        code = security_handler_hmac_sha256(jwt->security_handler, (mcl_uint8_t *)header_and_payload->buffer, header_and_payload->length, &hash, &hash_size);
    }
    else
    {
        code = security_handler_rsa_sign(jwt->security_handler->rsa.private_key, header_and_payload->buffer, header_and_payload->length, &hash, &hash_size);
    }
    ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_FAIL, "Can not sign JWT!");

    // encode calculated signature :
    code = security_handler_base64_url_encode(hash, hash_size, signature);

    // done with this :
    MCL_FREE(hash);

    // now can be checked if encode ok ( don't do it before MCL_FREE above ! ) :
    ASSERT_CODE_MESSAGE(MCL_OK == code, MCL_FAIL, "Encode calculated signature failed!");

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

static E_MCL_ERROR_CODE _add_schema_to_jwt(json_t *payload)
{
    DEBUG_ENTRY("json_t *payload = <%p>", payload)

    json_t *schemas_array;
    E_MCL_ERROR_CODE code = json_util_start_array(payload, JWT_PAYLOAD_NAME_SCHEMAS, &schemas_array);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Array for schema can not be created.");

    code = json_util_add_string(schemas_array, MCL_NULL, JWT_PAYLOAD_VALUE_SCHEMAS);
    json_util_finish_array(&schemas_array);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}
