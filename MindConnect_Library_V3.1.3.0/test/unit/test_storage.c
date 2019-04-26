/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_storage.c
* @date     Oct 11, 2016
* @brief    This file contains test case functions to test storage module.
*
************************************************************************/

#include "unity.h"
#include "storage.h"
#include "memory.h"
#include "string_type.h"
#include "string_util.h"
#include "file_util.h"
#include "definitions.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Registration information for shared secret security profile.
 * WHEN  : Registration information is saved to given file path and loaded back.
 * THEN  : MCL_OK is returned and the loaded information matches the saved information.
 */
void test_save_shared_secret_registration_information_001(void)
{
    string_t *path;
    string_t *client_id;
    string_t *client_id_loaded;
    string_t *client_secret;
    string_t *client_secret_loaded;
    string_t *registration_access_token;
    string_t *registration_access_token_loaded;
    string_t *registration_uri;
    string_t *registration_uri_loaded;

    // Given registration information.
    string_initialize_new("registrationInformation.txt", 0, &path);
    string_initialize_new("12345-67890", 0, &client_id);
    string_initialize_new("yLoG05o+5pCvlaP1zlhdmWIkOX5+rb6F+9Tr7I0fl4U=", 0, &client_secret);
    string_initialize_new("eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=", 0, &registration_access_token);
    string_initialize_new("https://dev8-mindconnectcom.cfapps.industrycloud-staging.siemens.com/api/register/", 0, &registration_uri);

    // Save registration information to storage.
    E_MCL_ERROR_CODE code = storage_save_shared_secret_registration_information(path, client_id, client_secret, registration_access_token, registration_uri);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Registration information can not be saved!");

    // Load registration information from storage.
    code = storage_load_shared_secret_registration_information(path, &client_id_loaded, &client_secret_loaded, &registration_access_token_loaded, &registration_uri_loaded);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Registration information can not be loaded!");

    // Check saved and reloaded information equality.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(client_id->buffer, client_id_loaded->buffer, "Saved and loaded client id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(client_secret->buffer, client_secret_loaded->buffer, "Saved and loaded client secret mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(registration_access_token->buffer, registration_access_token_loaded->buffer, "Saved and loaded registration access token mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(registration_uri->buffer, registration_uri_loaded->buffer, "Saved and loaded registration uri mismatch.");

    // Clean up.
    remove(path->buffer);
    string_destroy(&path);
    string_destroy(&client_id);
    string_destroy(&client_secret);
    string_destroy(&registration_access_token);
    string_destroy(&registration_uri);
    string_destroy(&client_id_loaded);
    string_destroy(&client_secret_loaded);
    string_destroy(&registration_access_token_loaded);
    string_destroy(&registration_uri_loaded);
}

/**
 * GIVEN : Registration information for RSA security profile.
 * WHEN  : Registration information is saved to given file path and loaded back.
 * THEN  : MCL_OK is returned and the loaded information matches the saved information.
 */
void test_save_rsa_registration_information_001(void)
{
    string_t *path;
    string_t *client_id;
    string_t *client_id_loaded;

    string_t *registration_access_token;
    string_t *registration_access_token_loaded;
    string_t *registration_uri;
    string_t *registration_uri_loaded;
    char *private_key_loaded = MCL_NULL;
    char *public_key_loaded = MCL_NULL;

    char *private_key = "-----BEGIN RSA PRIVATE KEY-----\n"\
    "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\n"\
    "-----END RSA PRIVATE KEY-----\n";

    char *public_key = "-----BEGIN PUBLIC KEY-----\n"\
    "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC7vbqajDw4o6gJy8UtmIbkcpnk\n"\
    "O3Kwc4qsEnSZp/TR+fQi62F79RHWmwKOtFmwteURgLbj7D/WGuNLGOfa/2vse3G2\n"\
    "eHnHl5CB8ruRX9fBl/KgwCVr2JaEuUm66bBQeP5XeBotdR4cvX38uPYivCDdPjJ1\n"\
    "QWPdspTBKcxeFbccDwIDAQAB\n"\
    "-----END PUBLIC KEY-----\n";

    // Given registration information.
    string_initialize_new("registrationInformation.txt", 0, &path);
    string_initialize_new("12345-67890", 0, &client_id);
    string_initialize_new("eyJ0eXAiOiJKV1QiLCJraWQiOiJrZXkxIiwiYWxnIjoiUlMyNTYifQ==.eyJpc3MiOiJTQ0kiLCJzdWIiOiJjMDY0MWMzYi03ODdmLTQ2NmEtOTJjNy05YWY4Yjc2Yjc1ZDkiLCJhdWQiOiJNQ0FTIiwiaWF0IjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwibmJmIjoiMjAxNy0wMy0yN1QxMTowODozMS43ODhaIiwiZXhwIjoiMjAxNy0wNC0yNFQxMTowODozMS43ODhaIiwianRpIjoiYzA2NDFjM2ItNzg3Zi00NjZhLTkyYzctOWFmOGI3NmI3NWQ5Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjoiW1widXJuOnNpZW1lbnM6c2NpOnRva2VuOklBVDp2MVwiXSIsInRlbiI6Im1zLXQxNSIsImNsaWVudF9jcmVkZW50aWFsc19wcm9maWxlIjoiU0hBUkVEX1NFQ1JFVCJ9.pNkVTP6IhBtUbFy0o4fdy5TCdYeoODFdbt298gawvt0=", 0, &registration_access_token);
    string_initialize_new("https://dev8-mindconnectcom.cfapps.industrycloud-staging.siemens.com/api/register/", 0, &registration_uri);

    // Save registration information to storage.
    E_MCL_ERROR_CODE code = storage_save_rsa_registration_information(path, client_id, public_key, private_key, registration_access_token, registration_uri);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Registration information can not be saved!");

    // Load registration information from storage.
    code = storage_load_rsa_registration_information(path, &client_id_loaded, &public_key_loaded, &private_key_loaded, &registration_access_token_loaded, &registration_uri_loaded);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Registration information can not be loaded!");

    // Check saved and reloaded information equality.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(client_id->buffer, client_id_loaded->buffer, "Saved and loaded client id mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(public_key, public_key_loaded, "Saved and loaded public key mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(private_key, private_key_loaded, "Saved and loaded private key mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(registration_access_token->buffer, registration_access_token_loaded->buffer, "Saved and loaded registration access token mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(registration_uri->buffer, registration_uri_loaded->buffer, "Saved and loaded registration uri mismatch.");

    // Clean up.
    remove(path->buffer);
    string_destroy(&path);
    string_destroy(&client_id);
    MCL_FREE(private_key_loaded);
    MCL_FREE(public_key_loaded);
    string_destroy(&registration_access_token);
    string_destroy(&registration_uri);
    string_destroy(&client_id_loaded);
    string_destroy(&registration_access_token_loaded);
    string_destroy(&registration_uri_loaded);
}

/**
 * GIVEN : Non-existing registration file path.
 * WHEN  : Registration information is to be loaded.
 * THEN  : MCL_REGISTRATION_INFO_IS_NOT_LOADED is returned with no registration information.
 */
void test_load_shared_secret_registration_information_001(void)
{
    string_t *path;
    string_t *client_id = MCL_NULL;
    string_t *client_secret = MCL_NULL;
    string_t *registration_access_token = MCL_NULL;
    string_t *registration_uri = MCL_NULL;

    // Given registration information.
    string_initialize_new("registrationInformation.txt", 0, &path);
    remove(path->buffer);

    // Load registration information from storage.
    E_MCL_ERROR_CODE code = storage_load_shared_secret_registration_information(path, &client_id, &client_secret, &registration_access_token, &registration_uri);
    TEST_ASSERT_EQUAL(MCL_REGISTRATION_INFO_IS_NOT_LOADED, code);

    // Check saved and reloaded information equality.
    TEST_ASSERT_NULL_MESSAGE(client_id, "Client id is not null.");
    TEST_ASSERT_NULL_MESSAGE(client_secret, "Client secret is not null.");
    TEST_ASSERT_NULL_MESSAGE(registration_access_token, "Registration access token is not null.");
    TEST_ASSERT_NULL_MESSAGE(registration_uri, "Registration uri is not null.");

    // Clean up.
    string_destroy(&path);
}

/**
 * GIVEN : Non-existing registration file path.
 * WHEN  : Registration information is to be loaded.
 * THEN  : MCL_REGISTRATION_INFO_IS_NOT_LOADED is returned with no registration information.
 */
void test_load_rsa_registration_information_001(void)
{
    string_t *path;
    string_t *client_id = MCL_NULL;
    char *public_key = MCL_NULL;
    char *private_key = MCL_NULL;
    string_t *registration_access_token = MCL_NULL;
    string_t *registration_uri = MCL_NULL;

    // Given registration information.
    string_initialize_new("registrationInformation.txt", 0, &path);
    remove(path->buffer);

    // Load registration information from storage.
    E_MCL_ERROR_CODE code = storage_load_rsa_registration_information(path, &client_id, &public_key, &private_key, &registration_access_token, &registration_uri);
    TEST_ASSERT_EQUAL(MCL_REGISTRATION_INFO_IS_NOT_LOADED, code);

    // Check saved and reloaded information equality.
    TEST_ASSERT_NULL_MESSAGE(client_id, "Client id is not null.");
    TEST_ASSERT_NULL_MESSAGE(public_key, "Public key is not null.");
    TEST_ASSERT_NULL_MESSAGE(private_key, "Private key is not null.");
    TEST_ASSERT_NULL_MESSAGE(registration_access_token, "Registration access token is not null.");
    TEST_ASSERT_NULL_MESSAGE(registration_uri, "Registration uri is not null.");

    // Clean up.
    string_destroy(&path);
}
