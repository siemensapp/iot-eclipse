/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_http_processor.c
* @date     Aug 12, 2016
* @brief    This file contains test case functions to test http_processor module.
*
************************************************************************/

#include "string_type.h"
#include "string_array.h"
#include "unity.h"
#include "http_processor.h"
#include "memory.h"
#include "string_util.h"
#include "mock_http_client.h"
#include "mock_security_handler.h"
#include "mock_security.h"
#include "mock_jwt.h"
#include "mock_http_request.h"
#include "mock_http_response.h"
#include "mock_json.h"
#include "mock_event.h"
#include "mock_time_util.h"
#include "mock_event_list.h"
#include "http_definitions.h"
#include "mock_storage.h"
#include "data_types.h"
#include "definitions.h"
#include "file.h"
#include "json_util.h"
#include "file_util.h"
#include "list.h"
#include "random.h"
#include "mock_store.h"
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "mcl/mcl_list.h"
#include "list.h"

configuration_t *configuration = MCL_NULL;
http_processor_t *http_processor = MCL_NULL;
event_list_t *event_list = MCL_NULL;
jwt_t *jwt = MCL_NULL;
security_handler_t *security_handler_with_access_token = MCL_NULL;
mcl_time_t current_time = 0;
char *registration_file_name = "registrationFile.txt";
FILE *registration_file = NULL;
mcl_uint8_t update_security_001_state = 0;

static E_MCL_ERROR_CODE _custom_load_reg_info_func(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);
static E_MCL_ERROR_CODE _custom_save_reg_info_func(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);
static E_MCL_ERROR_CODE load_for_update_security_test(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);
static E_MCL_ERROR_CODE save_for_update_security_test(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);

void setUp(void)
{
	MCL_NEW(configuration);
	configuration->load_function.rsa = MCL_NULL;
	configuration->save_function.rsa = MCL_NULL;
	configuration->access_token_endpoint = MCL_NULL;
	configuration->registration_endpoint = MCL_NULL;
	configuration->exchange_endpoint = MCL_NULL;
	string_initialize_new("https://brsm-MindConnectCom.cfapps.industrycloud-staging.siemens.com", 0, &configuration->mindsphere_hostname);
	MCL_NEW(jwt);
	jwt->header = MCL_NULL;
	jwt->payload = MCL_NULL;
	jwt->security_handler = MCL_NULL;
	jwt->security_profile = MCL_SECURITY_SHARED_SECRET;
	jwt->issued_at = 1496907009;

	MCL_NEW(security_handler_with_access_token);
	security_handler_with_access_token->authentication_key = MCL_NULL;
	security_handler_with_access_token->onboarding_key = MCL_NULL;
	security_handler_with_access_token->hmac_key = MCL_NULL;
	security_handler_with_access_token->rsa.private_key = MCL_NULL;
	security_handler_with_access_token->rsa.public_key = MCL_NULL;
	security_handler_with_access_token->rsa.session_key = MCL_NULL;
	security_handler_with_access_token->authentication_key_size = 0;
	security_handler_with_access_token->access_token = MCL_NULL;
	security_handler_with_access_token->registration_access_token = MCL_NULL;
	security_handler_with_access_token->client_secret = MCL_NULL;
	security_handler_with_access_token->registration_client_uri = MCL_NULL;
	security_handler_with_access_token->client_id = MCL_NULL;
	string_initialize_new("dummy_access_token", 0, &(security_handler_with_access_token->access_token));

	current_time = jwt->issued_at + 100;

	remove(registration_file_name);
	registration_file = fopen(registration_file_name, "w");
	fputs("3c7e43b1-b09d-4c4c-b110-7b0d53699154\n", registration_file);
	fputs("RSVSzufos24VU3kSdDrLl2vFqLCHEbXijsuPnuhUDfM\n", registration_file);
	fputs("eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJTQ0kiLCJzdWIiOiIzYzdlNDNiMS1iMDlkLTRjNGMtYjExMC03YjBkNTM2OTkxNTQiLCJhdWQiOiJNQ0FTIiwiaWF0IjoxNDk2NDA3ODY0LCJuYmYiOjE0OTY0MDc4NjQsImV4cCI6MTQ5NzAxMjY2NCwianRpIjoiZTRiY2VlNGMtOTM2OC00ZjgyLTlkNGYtZjRkNjQ5M2U1OWVlIiwic2NvcGUiOiJSQVQiLCJzY2hlbWFzIjpbInVybjpzaWVtZW5zOm1pbmRzcGhlcmU6djEiXSwidGVuIjoic2llbWVucyIsImNsaWVudF9jcmVkZW50aWFsX3Byb2ZpbGUiOiJTSEFSRURfU0VDUkVUIn0.PSaJKRo5GrhTk63QJ2xcMKL2eOSsmKJr7dlNi7hEx3NwLbU3jrZUV-G7-6TlBWJdWBXM8pVfHVhfoiA09zIK2IJjUMpL1E6NBtSf217WaK78AWXLPo2kwrbKFzsobsIvaOWgVpDvAAL82YKNlAnScea4G-_Zc6eDxfWjpmIuoQffQJ6pyScSA3GLglHzXZDPBqBz7DaJpxYpLj-OAKr3CY-iPuL_ibMOofjqTnSpxEQgqzX8NGPMFpIhPu4zn-8izXSPVVkk3Lfn_i1phZtwr3fA3cq25ycFCIZj692l8NHbb-on_ZpaYKIWTUFzfRFyC5ivLMUfH6OFzlVqg-wEhUwlKcXEdrFXCcjDq0HIy5bF082_V9DdYHzXt4kuGpmjRrhkvmS4AgQvNRXsKlzWUTWsSeLK9uWyFtqIhXIR9trpZvRalL4DPLvO1Na02V86vwT90bQ3ovrBDBX1T2n4sjGYIofD0S7ZPtIXQhJguZVuLPKOBH_2d5F_k-hfZ4SE\n", registration_file);
	fputs("https://brsm-MindConnectCom.cfapps.industrycloud-staging.siemens.com/api/register/3c7e43b1-b09d-4c4c-b110-7b0d53699154\n", registration_file);
	fputs("eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJTQ0kiLCJzdWIiOiIwYWNiNTY2YS01NmNjLTRmMDYtMTIwMy04ODMzYmVjNjI0YzAiLCJhdWQiOiJNQ0FTIiwiaWF0IjoxNDk5NDAzMTY4LCJuYmYiOjE0OTk0MDMxNjgsImV4cCI6MTUwMDAwNzk2OCwianRpIjoiZGM2ZWRkOTYtZjg2MC00MWNkLWJiZjMtMTU1NTE4M2ZlOTg3Iiwic2NvcGUiOiJJQVQiLCJzY2hlbWFzIjpbInVybjpzaWVtZW5zOm1pbmRzcGhlcmU6djEiXSwidGVuIjoibXMtdDE1IiwiY2xpZW50X2NyZWRlbnRpYWxfcHJvZmlsZSI6IlJTQV8zMDcyIn0.qrZdbwmk7KhUzi403RFDNvFjr7Ov1V3P1E-H8fJn6ArUbnWMW4tOoWp-lylFbp4uMoCaNk8wtCevvEx5vQwbaDl_p9xTK5eZ3OLFDMoT-gNf8CPzZ5aEo3P038S-W9aQa1XMFCU7WLEBmXI657GunOJTXRu5k7MdKXRvSbyRC-l3vxOGIU_GuqxFRhzbjFB7JxGLYPzS8Q-35KIO0hdKC9K_A15WuXRsNw6wI_VwHKdpAu3QKUgnOVkCtdMbxMvZu4DXHDY0Cjmc3PEYa-BsOSb_JFrCRqa2buL9pZcfUooqoEsqeY3L91cesRWuQFj8JUT5vUCi4lL3SLE-OJzYF_sK4Tz3S0NJxwnDfRJqqcNyur4YPexV88QzfJrO6c8dSSywTLkeyYi90DGiqIfKNOQZ8BPHZ42HaEdeKKiJy7gQqESoKXTWL-dpc8jlvOjRzFMfXU1ROhCQClYjFYFfcYML5fSKY034wHDUBpo5T-NSD35IppNPegHTfI__VXmD\n", registration_file);
	fclose(registration_file);
}

void tearDown(void)
{
	string_destroy(&configuration->mindsphere_hostname);
	MCL_FREE(configuration);
	MCL_FREE(jwt);
	string_destroy(&security_handler_with_access_token->access_token);
	MCL_FREE(security_handler_with_access_token);
}

/**
* GIVEN : Both the store path and initial access token - initial registration uri pair are NULL.
* WHEN  : Http processor is initialized.
* THEN  : MCL_NO_ACCESS_TOKEN_PROVIDED is returned.
*/
void test_initialize_001(void)
{
	configuration->store_path = MCL_NULL;
	configuration->initial_access_token = MCL_NULL;
	
	security_handler_t *security_handler;
	MCL_NEW(security_handler);
	security_handler->last_token_time = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);
	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_NO_ACCESS_TOKEN_PROVIDED == result, "http_processor_initialize() does not return MCL_NO_ACCESS_TOKEN_PROVIDED.");
	TEST_ASSERT_NULL_MESSAGE(http_processor, "Http processor is not null after initialization.");

	MCL_FREE(security_handler);
}

/**
* GIVEN : The store path is null but valid initial access token - initial registration uri pair is provided
*         for SHARED_SECRET security profile.
* WHEN  : Http processor is initialized.
* THEN  : MCL_OK is returned.
*/
void test_initialize_002(void)
{
	configuration->store_path = MCL_NULL;
	string_initialize_new("InitialAccessToken", 0, &configuration->initial_access_token);
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	security_handler_t *security_handler = MCL_NULL;
	MCL_NEW(security_handler);
	security_handler->registration_client_uri = MCL_NULL;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_initialize() does not return MCL_OK.");
	TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	string_destroy(&configuration->initial_access_token);
	MCL_FREE(security_handler);
}

/**
* GIVEN : The store path is null but valid initial access token - initial registration uri pair is provided
*         for RSA security profile.
* WHEN  : Http processor is initialized.
* THEN  : MCL_OK is returned.
*/
void test_initialize_003(void)
{
	configuration->store_path = MCL_NULL;
	string_initialize_new("InitialAccessToken", 0, &configuration->initial_access_token);
	configuration->security_profile = MCL_SECURITY_RSA_3072;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// Create security handler mock
	security_handler_t *security_handler = MCL_NULL;
	MCL_NEW(security_handler);
	security_handler->registration_client_uri = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

	security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_initialize() does not return MCL_OK.");
	TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	string_destroy(&configuration->initial_access_token);

	MCL_FREE(security_handler);
}

/**
* GIVEN : The store path is provided for SHARED_SECRET security profile.
* WHEN  : Http processor is initialized.
* THEN  : MCL_OK is returned.
*/
void test_initialize_004(void)
{
	string_initialize_new("StorePath.txt", 0, &configuration->store_path);
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// Create security handler mock
	security_handler_t *security_handler = MCL_NULL;
	MCL_NEW(security_handler);
	security_handler->registration_client_uri = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

	storage_load_shared_secret_registration_information_IgnoreAndReturn(MCL_OK);

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_initialize() does not return MCL_OK.");
	TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	remove(configuration->store_path->buffer);
	string_destroy(&configuration->store_path);
	MCL_FREE(security_handler);
}

/**
* GIVEN : The store path is provided for RSA security profile.
* WHEN  : Http processor is initialized.
* THEN  : MCL_OK is returned.
*/
void test_initialize_005(void)
{
	string_initialize_new("StorePath.txt", 0, &configuration->store_path);
	configuration->security_profile = MCL_SECURITY_RSA_3072;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// Create security handler mock
	security_handler_t *security_handler = MCL_NULL;
	MCL_NEW(security_handler);
	security_handler->registration_client_uri = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

	security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);
	storage_load_rsa_registration_information_IgnoreAndReturn(MCL_OK);

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_initialize() does not return MCL_OK.");
	TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	remove(configuration->store_path->buffer);
	string_destroy(&configuration->store_path);
	MCL_FREE(security_handler);
}

/**
* GIVEN : The store path is provided for RSA security profile without any registration data inside and
*         initial access token - initial registration uri pair is null.
* WHEN  : Http processor is initialized.
* THEN  : MCL_NO_ACCESS_TOKEN_PROVIDED is returned.
*/
void test_initialize_006(void)
{
	string_initialize_new("StorePath.txt", 0, &configuration->store_path);
	configuration->initial_access_token = MCL_NULL;
	configuration->security_profile = MCL_SECURITY_RSA_3072;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);
	security_handler_initialize_IgnoreAndReturn(MCL_OK);
	storage_load_rsa_registration_information_IgnoreAndReturn(MCL_REGISTRATION_INFO_IS_NOT_LOADED);
	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_NO_ACCESS_TOKEN_PROVIDED == result, "http_processor_initialize() does not return MCL_NO_ACCESS_TOKEN_PROVIDED.");
	TEST_ASSERT_NULL_MESSAGE(http_processor, "Http processor is not null after initialization.");

	remove(configuration->store_path->buffer);
	string_destroy(&configuration->store_path);
}

/**
* GIVEN : The store path is provided for RSA security profile without any registration data inside but
*         initial access token - initial registration uri pair is also provided.
* WHEN  : Http processor is initialized.
* THEN  : MCL_OK is returned.
*/
void test_initialize_007(void)
{
	string_initialize_new("StorePath.txt", 0, &configuration->store_path);
	string_initialize_new("InitialAccessToken", 0, &configuration->initial_access_token);
	configuration->security_profile = MCL_SECURITY_RSA_3072;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// Create security handler mock
	security_handler_t *security_handler = MCL_NULL;
	MCL_NEW(security_handler);
	security_handler->registration_client_uri = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

	storage_load_rsa_registration_information_IgnoreAndReturn(MCL_REGISTRATION_INFO_IS_NOT_LOADED);
	security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_initialize() does not return MCL_OK.");
	TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);

	remove(configuration->store_path->buffer);
	string_destroy(&configuration->store_path);
	string_destroy(&configuration->initial_access_token);
	MCL_FREE(security_handler);
}

/**
* GIVEN : Callback functions for loading and saving registration information are provided,
*         initial access token - initial registration uri are both null.
* WHEN  : Http processor is initialized.
* THEN  : MCL_OK is returned.
*/
void test_initialize_008(void)
{
	configuration->store_path = MCL_NULL;
	configuration->initial_access_token = MCL_NULL;
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	configuration->load_function.shared_secret = (mcl_load_shared_secret_callback_t)_custom_load_reg_info_func;
	configuration->save_function.shared_secret = (mcl_save_shared_secret_callback_t)_custom_save_reg_info_func;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// mock security handler
	security_handler_t *security_handler = NULL;
	MCL_NEW(security_handler);
	security_handler->authentication_key = MCL_NULL;
	security_handler->onboarding_key = MCL_NULL;
	security_handler->hmac_key = MCL_NULL;
	security_handler->rsa.private_key = MCL_NULL;
	security_handler->rsa.public_key = MCL_NULL;
	security_handler->rsa.session_key = MCL_NULL;
	security_handler->authentication_key_size = 0;
	security_handler->access_token = MCL_NULL;
	security_handler->registration_access_token = MCL_NULL;
	security_handler->client_secret = MCL_NULL;
	security_handler->registration_client_uri = MCL_NULL;
	security_handler->client_id = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
	security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);
	storage_load_shared_secret_registration_information_IgnoreAndReturn(MCL_REGISTRATION_INFO_IS_NOT_LOADED);

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_initialize() does not use callback function.");
	TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);

	string_destroy(&configuration->store_path);
	string_destroy(&configuration->initial_access_token);
	string_destroy(&security_handler->client_id);
	string_destroy(&security_handler->client_secret);
	string_destroy(&security_handler->registration_access_token);
	string_destroy(&security_handler->registration_client_uri);
	string_destroy(&security_handler->access_token);
	MCL_FREE(security_handler);
}

/**
* GIVEN : User initialized http processor with http_processor_initialize().
* WHEN  : User destroys http processor.
* THEN  : MCL_OK is returned.
*/
void test_destroy_001(void)
{
	configuration->store_path = MCL_NULL;
	string_initialize_new("InitialAccessToken", 0, &configuration->initial_access_token);
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// Create security handler mock
	security_handler_t *security_handler = MCL_NULL;
	MCL_NEW(security_handler);
	security_handler->registration_client_uri = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

	http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);

	TEST_ASSERT_NULL_MESSAGE(http_processor, "Http processor is not null after it is destroyed.");

	string_destroy(&configuration->initial_access_token);
	MCL_FREE(security_handler);
}

/**
* GIVEN : An agent not onboarded, security profile is MCL_SECURITY_SHARED_SECRET.
* WHEN  : http_processor_register is called.
* THEN  : MCL_OK is returned.
*/
void test_register_001(void)
{
	// set necessary configuration parameters
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	configuration->store_path = NULL;
	string_initialize_new("br-smk1", 0, &configuration->tenant);
	string_initialize_new("InitialAccessToken", 0, &configuration->initial_access_token);

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// mock security handler
	security_handler_t *security_handler = NULL;
	MCL_NEW(security_handler);
	security_handler->authentication_key = MCL_NULL;
	security_handler->onboarding_key = MCL_NULL;
	security_handler->hmac_key = MCL_NULL;
	security_handler->rsa.private_key = MCL_NULL;
	security_handler->rsa.public_key = MCL_NULL;
	security_handler->rsa.session_key = MCL_NULL;
	security_handler->authentication_key_size = 0;
	security_handler->access_token = MCL_NULL;
	security_handler->registration_access_token = MCL_NULL;
	security_handler->client_secret = MCL_NULL;
	security_handler->registration_client_uri = MCL_NULL;
	security_handler->client_id = MCL_NULL;
	security_handler->last_token_time = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
	security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

	storage_load_shared_secret_registration_information_IgnoreAndReturn(MCL_OK);

	// initialize http processor
	http_processor_initialize(configuration, &http_processor);

	// mock http request
	http_request_t *http_request = NULL;
	MCL_NEW(http_request);
	http_request->header = MCL_NULL;
	http_request->payload_size = 2;
	http_request->payload = MCL_MALLOC(http_request->payload_size);
	http_request->uri = MCL_NULL;
	http_request->boundary = MCL_NULL;
	http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	http_request_initialize_ReturnThruPtr_http_request(&http_request);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_raw_data_IgnoreAndReturn(MCL_OK);

	// mock http response
	http_response_t *http_response;
	MCL_NEW(http_response);
	http_response->result_code = MCL_HTTP_RESULT_CODE_CREATED;
	http_response_get_result_code_IgnoreAndReturn(MCL_HTTP_RESULT_CODE_CREATED);
	http_response_get_payload_IgnoreAndReturn("{\"client_id\":\"zxc\", \"client_secret\":\"dummy_secret\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}");

	// mock http client
	http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
	http_client_send_ReturnThruPtr_http_response(&http_response);

	// mock destroy functions
	http_request_destroy_Ignore();
	http_response_destroy_Ignore();

	E_MCL_ERROR_CODE code = http_processor_register(http_processor);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "http_processor_register failed.");

	// clean up
	string_destroy(&configuration->tenant);
	string_destroy(&configuration->initial_access_token);
	string_destroy(&security_handler->client_id);
	string_destroy(&security_handler->registration_access_token);
	string_destroy(&security_handler->client_secret);
	string_destroy(&security_handler->registration_client_uri);
	string_destroy(&http_processor->security_handler->access_token);
	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	MCL_FREE(http_response);
	MCL_FREE(http_request->payload);
	MCL_FREE(http_request);
	MCL_FREE(security_handler);
}

/**
* GIVEN : An agent already onboarded, security profile is MCL_SECURITY_SHARED_SECRET.
* WHEN  : http_processor_register is called.
* THEN  : MCL_OK is returned.
*/
void test_register_002(void)
{
	// set necessary configuration parameters
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	string_initialize_new(registration_file_name, 0, &configuration->store_path);
	string_initialize_new("br-smk1", 0, &configuration->tenant);
	string_initialize_new("https://brsm-MindConnectCom.cfapps.industrycloud-staging.siemens.com/api/register", 0, &configuration->initial_access_token);

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// mock security handler
	security_handler_t *security_handler = NULL;
	MCL_NEW(security_handler);
	security_handler->authentication_key = MCL_NULL;
	security_handler->onboarding_key = MCL_NULL;
	security_handler->hmac_key = MCL_NULL;
	security_handler->rsa.private_key = MCL_NULL;
	security_handler->rsa.public_key = MCL_NULL;
	security_handler->rsa.session_key = MCL_NULL;
	security_handler->authentication_key_size = 0;
	security_handler->access_token = MCL_NULL;
	security_handler->registration_access_token = MCL_NULL;
	security_handler->client_secret = MCL_NULL;
	security_handler->registration_client_uri = MCL_NULL;
	security_handler->client_id = MCL_NULL;
	security_handler->last_token_time = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
	security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

	storage_load_shared_secret_registration_information_IgnoreAndReturn(MCL_OK);

	// initialize http processor
	http_processor_initialize(configuration, &http_processor);

	// mock http request
	http_request_t *http_request = NULL;
	MCL_NEW(http_request);
	http_request->header = MCL_NULL;
	http_request->payload_size = 2;
	http_request->payload = MCL_MALLOC(http_request->payload_size);
	http_request->uri = MCL_NULL;
	http_request->boundary = MCL_NULL;
	http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	http_request_initialize_ReturnThruPtr_http_request(&http_request);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_raw_data_IgnoreAndReturn(MCL_OK);

	// mock http response
	http_response_t *http_response;
	MCL_NEW(http_response);
	http_response->result_code = MCL_HTTP_RESULT_CODE_CREATED;
	http_response_get_result_code_IgnoreAndReturn(MCL_HTTP_RESULT_CODE_CREATED);
	http_response_get_payload_IgnoreAndReturn("{\"client_id\":\"zxc\", \"client_secret\":\"dummy_secret\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}");

	// mock http client
	http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
	http_client_send_ReturnThruPtr_http_response(&http_response);

	storage_save_shared_secret_registration_information_IgnoreAndReturn(MCL_OK);

	// mock destroy functions
	http_request_destroy_Ignore();
	http_response_destroy_Ignore();

	E_MCL_ERROR_CODE code = http_processor_register(http_processor);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "http_processor_register failed.");

	// clean up
	string_destroy(&configuration->tenant);
	string_destroy(&configuration->store_path);
	string_destroy(&configuration->initial_access_token);
	string_destroy(&security_handler->client_id);
	string_destroy(&security_handler->registration_access_token);
	string_destroy(&security_handler->client_secret);
	string_destroy(&security_handler->registration_client_uri);
	string_destroy(&http_processor->security_handler->access_token);
	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	MCL_FREE(http_response);
	MCL_FREE(http_request->payload);
	MCL_FREE(http_request);
	MCL_FREE(security_handler);
}

/**
* GIVEN : An agent not onboarded, security profile is MCL_SECURITY_RSA_3072.
* WHEN  : http_processor_register is called.
* THEN  : MCL_OK is returned.
*/
void test_register_003(void)
{
	// set necessary configuration parameters
	configuration->security_profile = MCL_SECURITY_RSA_3072;
	configuration->store_path = MCL_NULL;
	string_initialize_new("br-smk1", 0, &configuration->tenant);
	string_initialize_new("https://brsm-MindConnectCom.cfapps.industrycloud-staging.siemens.com/api/register", 0, &configuration->initial_access_token);

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// mock security handler
	security_handler_t *security_handler = NULL;
	MCL_NEW(security_handler);
	security_handler->authentication_key = MCL_NULL;
	security_handler->onboarding_key = MCL_NULL;
	security_handler->hmac_key = MCL_NULL;
	security_handler->rsa.private_key = MCL_NULL;
	security_handler->rsa.public_key = MCL_NULL;
	security_handler->rsa.session_key = MCL_NULL;
	security_handler->authentication_key_size = 0;
	security_handler->access_token = MCL_NULL;
	security_handler->registration_access_token = MCL_NULL;
	security_handler->client_secret = MCL_NULL;
	security_handler->registration_client_uri = MCL_NULL;
	security_handler->client_id = MCL_NULL;
	security_handler->last_token_time = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
	security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

	storage_load_rsa_registration_information_IgnoreAndReturn(MCL_OK);
	char *modulus = MCL_MALLOC(2);
	char *public_exponent = MCL_MALLOC(2);
	string_util_memcpy(modulus, "3", 2);
	string_util_memcpy(public_exponent, "5", 2);
	security_rsa_get_modulus_and_exponent_ExpectAnyArgsAndReturn(MCL_OK);
	security_rsa_get_modulus_and_exponent_ReturnThruPtr_exponent(&public_exponent);
	security_rsa_get_modulus_and_exponent_ReturnThruPtr_modulus(&modulus);

	// initialize http processor
	http_processor_initialize(configuration, &http_processor);

	// mock http request
	http_request_t *http_request = NULL;
	MCL_NEW(http_request);
	http_request->header = MCL_NULL;
	http_request->payload_size = 63;
	http_request->payload = MCL_MALLOC(http_request->payload_size);
	http_request->uri = MCL_NULL;
	http_request->boundary = MCL_NULL;
	http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	http_request_initialize_ReturnThruPtr_http_request(&http_request);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_raw_data_IgnoreAndReturn(MCL_OK);

	// mock http response
	http_response_t *http_response;
	MCL_NEW(http_response);
	http_response->result_code = MCL_HTTP_RESULT_CODE_CREATED;
	http_response_get_result_code_IgnoreAndReturn(MCL_HTTP_RESULT_CODE_CREATED);
	http_response_get_payload_IgnoreAndReturn("{\"client_id\":\"zxc\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}");

	// mock http client
	http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
	http_client_send_ReturnThruPtr_http_response(&http_response);

	storage_save_rsa_registration_information_IgnoreAndReturn(MCL_OK);

	// mock destroy functions
	http_request_destroy_Ignore();
	http_response_destroy_Ignore();

	E_MCL_ERROR_CODE code = http_processor_register(http_processor);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "http_processor_register failed.");

	// clean up
	string_destroy(&configuration->tenant);
	string_destroy(&configuration->store_path);
	string_destroy(&configuration->initial_access_token);
	string_destroy(&security_handler->client_id);
	string_destroy(&security_handler->registration_access_token);
	string_destroy(&security_handler->client_secret);
	string_destroy(&security_handler->registration_client_uri);
	string_destroy(&http_processor->security_handler->access_token);
	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	MCL_FREE(http_response);
	MCL_FREE(http_request->payload);
	MCL_FREE(http_request);
	MCL_FREE(security_handler);
}

/**
* GIVEN : An agent already onboarded, security profile is MCL_SECURITY_RSA_3072.
* WHEN  : http_processor_register is called.
* THEN  : MCL_OK is returned.
*/
void test_register_004(void)
{
	// set necessary configuration parameters
	configuration->security_profile = MCL_SECURITY_RSA_3072;
	string_initialize_new(registration_file_name, 0, &configuration->store_path);
	string_initialize_new("br-smk1", 0, &configuration->tenant);
	string_initialize_new("https://brsm-MindConnectCom.cfapps.industrycloud-staging.siemens.com/api/register", 0, &configuration->initial_access_token);

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// mock security handler
	security_handler_t *security_handler = NULL;
	MCL_NEW(security_handler);
	security_handler->authentication_key = MCL_NULL;
	security_handler->onboarding_key = MCL_NULL;
	security_handler->hmac_key = MCL_NULL;
	security_handler->rsa.private_key = MCL_NULL;
	security_handler->rsa.public_key = MCL_NULL;
	security_handler->rsa.session_key = MCL_NULL;
	security_handler->authentication_key_size = 0;
	security_handler->access_token = MCL_NULL;
	security_handler->registration_access_token = MCL_NULL;
	security_handler->client_secret = MCL_NULL;
	security_handler->registration_client_uri = MCL_NULL;
	security_handler->client_id = MCL_NULL;
	security_handler->last_token_time = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
	security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

	storage_load_rsa_registration_information_IgnoreAndReturn(MCL_OK);
	char *modulus = MCL_MALLOC(2);
	char *public_exponent = MCL_MALLOC(2);
	string_util_memcpy(modulus, "3", 2);
	string_util_memcpy(public_exponent, "5", 2);
	security_rsa_get_modulus_and_exponent_ExpectAnyArgsAndReturn(MCL_OK);
	security_rsa_get_modulus_and_exponent_ReturnThruPtr_exponent(&public_exponent);
	security_rsa_get_modulus_and_exponent_ReturnThruPtr_modulus(&modulus);

	// initialize http processor
	http_processor_initialize(configuration, &http_processor);

	// mock http request
	http_request_t *http_request = NULL;
	MCL_NEW(http_request);
	http_request->header = MCL_NULL;
	http_request->payload_size = 63;
	http_request->payload = MCL_MALLOC(http_request->payload_size);
	http_request->uri = MCL_NULL;
	http_request->boundary = MCL_NULL;
	http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	http_request_initialize_ReturnThruPtr_http_request(&http_request);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_raw_data_IgnoreAndReturn(MCL_OK);

	// mock http response
	http_response_t *http_response;
	MCL_NEW(http_response);
	http_response->result_code = MCL_HTTP_RESULT_CODE_CREATED;
	http_response_get_result_code_IgnoreAndReturn(MCL_HTTP_RESULT_CODE_CREATED);
	http_response_get_payload_IgnoreAndReturn("{\"client_id\":\"zxc\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}");

	// mock http client
	http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
	http_client_send_ReturnThruPtr_http_response(&http_response);

	storage_save_rsa_registration_information_IgnoreAndReturn(MCL_OK);

	// mock destroy functions
	http_request_destroy_Ignore();
	http_response_destroy_Ignore();

	E_MCL_ERROR_CODE code = http_processor_register(http_processor);
	TEST_ASSERT_EQUAL_MESSAGE(MCL_OK, code, "http_processor_register failed.");

	// clean up
	string_destroy(&configuration->tenant);
	string_destroy(&configuration->store_path);
	string_destroy(&configuration->initial_access_token);
	string_destroy(&security_handler->client_id);
	string_destroy(&security_handler->registration_access_token);
	string_destroy(&security_handler->client_secret);
	string_destroy(&security_handler->registration_client_uri);
	string_destroy(&http_processor->security_handler->access_token);
	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	MCL_FREE(http_response);
	MCL_FREE(http_request->payload);
	MCL_FREE(http_request);
	MCL_FREE(security_handler);
}

/**
* GIVEN : Registration Access Token is present, security profile is MCL_SECURITY_SHARED_SECRET.
* WHEN  : http_processor_register is called and http_request_initialize fails due to some reason.
* THEN  : MCL_FAIL is returned.
*/
void test_register_005(void)
{
	// set necessary configuration parameters
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	configuration->store_path = NULL;

	// mock security handler
	security_handler_t *security_handler = NULL;
	MCL_NEW(security_handler);
	security_handler->authentication_key = MCL_NULL;
	security_handler->onboarding_key = MCL_NULL;
	security_handler->hmac_key = MCL_NULL;
	security_handler->rsa.private_key = MCL_NULL;
	security_handler->rsa.public_key = MCL_NULL;
	security_handler->rsa.session_key = MCL_NULL;
	security_handler->authentication_key_size = 0;
	security_handler->access_token = MCL_NULL;
	security_handler->registration_access_token = MCL_NULL;
	security_handler->client_secret = MCL_NULL;
	security_handler->registration_client_uri = MCL_NULL;
	security_handler->client_id = MCL_NULL;
	string_initialize_new("ClientID", 0, &(security_handler->client_id));
	string_initialize_new("RegistrationAccessToken", 0, &(security_handler->registration_access_token));

	// initialize http processor
	MCL_NEW(http_processor);
	http_processor->configuration = configuration;
	http_processor->security_handler = security_handler;

	http_request_initialize_IgnoreAndReturn(MCL_FAIL);
	http_request_destroy_Ignore();

	E_MCL_ERROR_CODE result = http_processor_register(http_processor);
	TEST_ASSERT_EQUAL(MCL_FAIL, result);

	string_destroy(&(security_handler->registration_access_token));
	string_destroy(&(security_handler->client_id));
	string_destroy(&security_handler->client_secret);
	free(security_handler);
	free(http_processor);
}

/**
* GIVEN : An agent already onboarded, security profile is MCL_SECURITY_SHARED_SECRET.
* WHEN  : http_processor_register is called and storage_save_shared_secret_registration_information() fails.
* THEN  : MCL_REGISTRATION_INFO_IS_NOT_SAVED is returned.
*/
void test_register_006(void)
{
    // set necessary configuration parameters
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    string_initialize_new(registration_file_name, 0, &configuration->store_path);
    string_initialize_new("br-smk1", 0, &configuration->tenant);
    string_initialize_new("https://brsm-MindConnectCom.cfapps.industrycloud-staging.siemens.com/api/register", 0, &configuration->initial_access_token);

    security_initialize_Ignore();
    http_client_initialize_IgnoreAndReturn(MCL_OK);

    // mock security handler
    security_handler_t *security_handler = NULL;
    MCL_NEW(security_handler);
    security_handler->authentication_key = MCL_NULL;
    security_handler->onboarding_key = MCL_NULL;
    security_handler->hmac_key = MCL_NULL;
    security_handler->rsa.private_key = MCL_NULL;
    security_handler->rsa.public_key = MCL_NULL;
    security_handler->rsa.session_key = MCL_NULL;
    security_handler->authentication_key_size = 0;
    security_handler->access_token = MCL_NULL;
    security_handler->registration_access_token = MCL_NULL;
    security_handler->client_secret = MCL_NULL;
    security_handler->registration_client_uri = MCL_NULL;
    security_handler->client_id = MCL_NULL;
    security_handler->last_token_time = MCL_NULL;
    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
    security_handler_generate_rsa_key_IgnoreAndReturn(MCL_OK);

    // Correlation-ID
    security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

    storage_load_shared_secret_registration_information_IgnoreAndReturn(MCL_OK);

    // initialize http processor
    http_processor_initialize(configuration, &http_processor);

    // mock http request
    http_request_t *http_request = NULL;
    MCL_NEW(http_request);
    http_request->header = MCL_NULL;
    http_request->payload_size = 2;
    http_request->payload = MCL_MALLOC(http_request->payload_size);
    http_request->uri = MCL_NULL;
    http_request->boundary = MCL_NULL;
    http_request_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_request_initialize_ReturnThruPtr_http_request(&http_request);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_header_IgnoreAndReturn(MCL_OK);
    http_request_add_raw_data_IgnoreAndReturn(MCL_OK);

    // mock http response
    http_response_t *http_response;
    MCL_NEW(http_response);
    http_response->result_code = MCL_HTTP_RESULT_CODE_CREATED;
    http_response_get_result_code_IgnoreAndReturn(MCL_HTTP_RESULT_CODE_CREATED);
    http_response_get_payload_IgnoreAndReturn("{\"client_id\":\"zxc\", \"client_secret\":\"dummy_secret\", \"registration_access_token\":\"123\", \"registration_client_uri\":\"dummy_host/register\"}");

    // mock http client
    http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
    http_client_send_ReturnThruPtr_http_response(&http_response);

    storage_save_shared_secret_registration_information_IgnoreAndReturn(MCL_REGISTRATION_INFO_IS_NOT_SAVED);

    // mock destroy functions
    http_request_destroy_Ignore();
    http_response_destroy_Ignore();

    E_MCL_ERROR_CODE code = http_processor_register(http_processor);
    TEST_ASSERT_EQUAL_MESSAGE(MCL_REGISTRATION_INFO_IS_NOT_SAVED, code, "http_processor_register failed.");

    // clean up
    string_destroy(&configuration->tenant);
    string_destroy(&configuration->store_path);
    string_destroy(&configuration->initial_access_token);
    string_destroy(&security_handler->client_id);
    string_destroy(&security_handler->registration_access_token);
    string_destroy(&security_handler->client_secret);
    string_destroy(&security_handler->registration_client_uri);
    string_destroy(&http_processor->security_handler->access_token);
    http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    http_processor_destroy(&http_processor);
    MCL_FREE(http_response);
    MCL_FREE(http_request->payload);
    MCL_FREE(http_request);
    MCL_FREE(security_handler);
}

/**
* GIVEN : Valid parameters are provided.
* WHEN  : #http_processor_stream() is called.
* THEN  : #MCL_OK is returned.
*/
void test_stream_001()
{
#if MCL_STREAM_ENABLED
	configuration->store_path = MCL_NULL;
	string_initialize_new("InitialAccessToken", 0, &configuration->initial_access_token);
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;

	security_initialize_Ignore();
	http_client_initialize_IgnoreAndReturn(MCL_OK);

	// mock security handler
	security_handler_t *security_handler = NULL;
	MCL_NEW(security_handler);
	security_handler->authentication_key = MCL_NULL;
	security_handler->onboarding_key = MCL_NULL;
	security_handler->hmac_key = MCL_NULL;
	security_handler->rsa.private_key = MCL_NULL;
	security_handler->rsa.public_key = MCL_NULL;
	security_handler->rsa.session_key = MCL_NULL;
	security_handler->authentication_key_size = 0;
	security_handler->access_token = MCL_NULL;
	security_handler->registration_access_token = MCL_NULL;
	security_handler->client_secret = MCL_NULL;
	security_handler->registration_client_uri = MCL_NULL;
	security_handler->client_id = MCL_NULL;
	security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);
	security_initialize_Ignore();

	// Correlation-ID
	security_generate_random_bytes_ExpectAnyArgsAndReturn(MCL_OK);

	E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

	TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_initialize() does not return MCL_OK.");
	TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

	// set security handler of the http processor
	http_processor->security_handler = security_handler_with_access_token;

	store_t *store = MCL_NULL;
	http_response_t * http_response = MCL_NULL;

	MCL_NEW(store);
	MCL_NEW(http_response);
	http_response->payload = MCL_NULL;
	http_response->payload_size = 0;
	http_response->result_code = MCL_HTTP_RESULT_CODE_SUCCESS;
	list_initialize(&(store->high_priority_list));
	list_initialize(&(store->low_priority_list));

	// just to pass assert condition, no real operation will be performed on this store.
	store->low_priority_list->count = 1;

	event_list_initialize_IgnoreAndReturn(MCL_OK);
	store_get_data_count_IgnoreAndReturn(1);
	http_request_initialize_IgnoreAndReturn(MCL_OK);

	// inside _exchange_initialize_http_request_headers.
	http_request_add_header_IgnoreAndReturn(MCL_OK);
	http_request_add_header_IgnoreAndReturn(MCL_OK);

	// Correlation-ID
	http_request_add_header_IgnoreAndReturn(MCL_OK);

	// inside _add_authentication_header_to_request.
	http_request_add_header_IgnoreAndReturn(MCL_OK);

	http_client_send_ExpectAnyArgsAndReturn(MCL_OK);
	http_client_send_ReturnThruPtr_http_response(&http_response);
	http_request_destroy_Ignore();

	// inside _exchange_evaluate_response.
	http_response_destroy_Ignore();
	event_list_destroy_Ignore();

	store_get_data_count_IgnoreAndReturn(0);
	result = http_processor_stream(http_processor, store, NULL);
	TEST_ASSERT_EQUAL(MCL_OK, result);

	http_client_destroy_Ignore();
	security_handler_destroy_Ignore();
	http_processor_destroy(&http_processor);
	string_destroy(&configuration->mindsphere_hostname);
	string_destroy(&configuration->initial_access_token);

	// correcting the list count before destroying it.
	store->low_priority_list->count = 0;
	list_destroy(&(store->low_priority_list));
	list_destroy(&(store->high_priority_list));
	free(store);
	free(http_response);
	MCL_FREE(security_handler);
#else
	tearDown();
	TEST_IGNORE_MESSAGE("test_stream_001.");
#endif
}

/**
* GIVEN : Http processor is initialized.
* WHEN  : Update security function is called.
* THEN  : MCL_OK is returned.
*/
void test_update_security_001(void)
{
    configuration->store_path = MCL_NULL;
    configuration->load_function.shared_secret = load_for_update_security_test;
    configuration->save_function.shared_secret = save_for_update_security_test;

    string_initialize_new("InitialAccessToken", 0, &configuration->initial_access_token);
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    security_handler_t *security_handler = MCL_NULL;
    MCL_NEW(security_handler);
    memset(security_handler, 0, sizeof(security_handler_t));

    security_initialize_Ignore();
    http_client_initialize_IgnoreAndReturn(MCL_OK);
    security_handler_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    security_handler_initialize_ReturnThruPtr_security_handler(&security_handler);

    update_security_001_state = 0;

    E_MCL_ERROR_CODE result = http_processor_initialize(configuration, &http_processor);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_initialize() does not return MCL_OK.");
    TEST_ASSERT_NOT_NULL_MESSAGE(http_processor, "Http processor is null after initialization.");

    update_security_001_state = 1;

    result = http_processor_update_security_information(http_processor);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "http_processor_update_security_information() does not return MCL_OK.");

    http_client_destroy_Ignore();
    security_handler_destroy_Ignore();
    http_processor_destroy(&http_processor);
    string_destroy(&configuration->initial_access_token);
    string_destroy(&security_handler->client_id);
    string_destroy(&security_handler->client_secret);
    string_destroy(&security_handler->registration_access_token);
    string_destroy(&security_handler->registration_client_uri);
    MCL_FREE(security_handler);
}

static E_MCL_ERROR_CODE _custom_load_reg_info_func(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri)
{
    *client_id = MCL_NULL;
    *client_secret = MCL_NULL;
    *registration_access_token = MCL_NULL;
    *registration_uri = MCL_NULL;

    return MCL_OK;
}

static E_MCL_ERROR_CODE _custom_save_reg_info_func(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
	return MCL_OK;
}

static E_MCL_ERROR_CODE load_for_update_security_test(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri)
{
    *client_id = calloc(5, 1);
    snprintf(*client_id, 5, "1234");

    *registration_access_token = calloc(5, 1);
    snprintf(*registration_access_token, 5, "1234");

    *registration_uri = calloc(5, 1);
    snprintf(*registration_uri, 5, "1234");

    *client_secret = calloc(5, 1);

    if (0 == update_security_001_state)
    {
        snprintf(*client_secret, 5, "1234");
    }
    else
    {
        snprintf(*client_secret, 5, "abcd");
    }

    return MCL_OK;
}

static E_MCL_ERROR_CODE save_for_update_security_test(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri)
{
    return MCL_OK;
}
