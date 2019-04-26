/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_mcl_communication.c
* @date     Aug 1, 2016
* @brief    This file contains test case functions to test mcl module.
*
************************************************************************/

#include "communication.h"
#include "configuration.h"
#include "string_type.h"
#include "unity.h"
#include "memory.h"
#include "string_util.h"
#include "data_types.h"
#include "definitions.h"
#include "mock_http_processor.h"
#include "mcl/mcl_communication.h"
#include "mcl/mcl_configuration.h"
#include "mcl/mcl_list.h"
#include "list.h"

mcl_communication_t *communication = MCL_NULL;
mcl_configuration_t *configuration = MCL_NULL;

void setUp(void)
{
    mcl_configuration_initialize(&configuration);
}

void tearDown(void)
{
    http_processor_destroy_Ignore();
    mcl_communication_destroy(&communication);
    MCL_FREE(configuration);
}

/**
 * GIVEN : User provides valid agent id, host name, port and certificate and security profile as the configuration parameters. Proxy server is not used.
 * WHEN  : User tries to initialize the library with the given configuration parameters.
 * THEN  : Configuration parameters are copied to mcl_t data structure and MCL_OK is returned.
 */
void test_initialize_001(void)
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = NULL;
    configuration->max_http_payload_size = 16384;
    configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
	configuration->tenant = "br-smk1";
    http_processor_initialize_IgnoreAndReturn(MCL_OK);

    E_MCL_ERROR_CODE result = mcl_communication_initialize(configuration, &communication);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("mindsphere", communication->configuration.mindsphere_hostname->buffer, "Host Name Mismatch!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(10, communication->configuration.mindsphere_port, "Host Port Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("3iu2ned298ijdm", communication->configuration.mindsphere_certificate->buffer, "Certificate Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("MCL/" MCL_VERSION_ " (custom agent v1.0)", communication->configuration.user_agent->buffer, "User-Agent doesn't match with expected!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration->initial_access_token, communication->configuration.initial_access_token->buffer, "Initial Access Token mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration->tenant, communication->configuration.tenant->buffer, "Tenant mismatch.");
    TEST_ASSERT_MESSAGE(MCL_SECURITY_SHARED_SECRET == communication->configuration.security_profile, "Security Profile Mismatch!");
    TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_communication_initialize() unexpected return error!");
}

/**
 * GIVEN : User provides agent id, host name, port, certificate, security profile and proxy host name as the configuration parameters.
 * WHEN  : User tries to initialize the library with the given configuration parameters.
 * THEN  : Configuration parameters are copied to mcl_t data structure and MCL_OK is returned.
 */
void test_initialize_002(void)
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "proxy.erlm.siemens.de";
    configuration->proxy_port = 20;
    configuration->max_http_payload_size = 16384;
    configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
    configuration->tenant = "br-smk1";
    http_processor_initialize_IgnoreAndReturn(MCL_OK);

    E_MCL_ERROR_CODE result = mcl_communication_initialize(configuration, &communication);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("mindsphere", communication->configuration.mindsphere_hostname->buffer, "Host Name Mismatch!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(10, communication->configuration.mindsphere_port, "Host Port Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("3iu2ned298ijdm", communication->configuration.mindsphere_certificate->buffer, "Certificate Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("MCL/" MCL_VERSION_ " (custom agent v1.0)", communication->configuration.user_agent->buffer, "User-Agent doesn't match with expected!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration->initial_access_token, communication->configuration.initial_access_token->buffer, "Initial Access Token mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration->tenant, communication->configuration.tenant->buffer, "Tenant mismatch.");
    TEST_ASSERT_MESSAGE(MCL_SECURITY_SHARED_SECRET == communication->configuration.security_profile, "Security Profile Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("proxy.erlm.siemens.de", communication->configuration.proxy_hostname->buffer, "Proxy Host Mismatch!");
    TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_communication_initialize() unexpected return error!");
}

/**
 * GIVEN : User provides agent id, host name, port, certificate, security profile and proxy host name, proxy port and proxy user name as the configuration parameters.
 *         Proxy password is not provided although proxy user is provided.
 * WHEN  : User tries to initialize the library with the given configuration parameters.
 * THEN  : MCL_INVALID_PROXY_PASSWORD is returned.
 */
void test_initialize_003(void)
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "proxy.erlm.siemens.de";
    configuration->proxy_port = 20;
    configuration->proxy_username = "username";
    configuration->max_http_payload_size = 16384;
    configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
    configuration->tenant = "br-smk1";

    E_MCL_ERROR_CODE result = mcl_communication_initialize(configuration, &communication);

    TEST_ASSERT_MESSAGE(MCL_INVALID_PROXY_PASSWORD == result, "mcl_communication_initialize() unexpected return error!");
}

/**
 * GIVEN : User provides valid values for all configuration parameters.
 * WHEN  : User tries to initialize the library with the given configuration parameters.
 * THEN  : Configuration parameters are copied to mcl_t data structure and MCL_OK is returned.
 */
void test_initialize_004(void)
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "proxy.erlm.siemens.de";
    configuration->proxy_port = 20;
    configuration->proxy_username = "username";
    configuration->proxy_password = "password";
    configuration->proxy_type = MCL_PROXY_HTTP;
    configuration->proxy_domain = "domain";
    configuration->max_http_payload_size = 16384;
    configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
    configuration->tenant = "br-smk1";
    http_processor_initialize_IgnoreAndReturn(MCL_OK);

    E_MCL_ERROR_CODE result = mcl_communication_initialize(configuration, &communication);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("mindsphere", communication->configuration.mindsphere_hostname->buffer, "Host Name Mismatch!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(10, communication->configuration.mindsphere_port, "Host Port Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("3iu2ned298ijdm", communication->configuration.mindsphere_certificate->buffer, "Certificate Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("MCL/" MCL_VERSION_ " (custom agent v1.0)", communication->configuration.user_agent->buffer, "User-Agent doesn't match with expected!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration->initial_access_token, communication->configuration.initial_access_token->buffer, "Initial Access Token mismatch.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(configuration->tenant, communication->configuration.tenant->buffer, "Tenant mismatch.");
    TEST_ASSERT_MESSAGE(MCL_SECURITY_SHARED_SECRET == communication->configuration.security_profile, "Security Profile Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("proxy.erlm.siemens.de", communication->configuration.proxy_hostname->buffer, "Proxy Host Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("username", communication->configuration.proxy_username->buffer, "Proxy User Name Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("password", communication->configuration.proxy_password->buffer, "Proxy Password Mismatch!");
    TEST_ASSERT_MESSAGE(MCL_PROXY_HTTP == communication->configuration.proxy_type, "Proxy Type Mismatch!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("domain", communication->configuration.proxy_domain->buffer, "Proxy Domain Mismatch!");
    TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_communication_initialize() unexpected return error!");
}

/**
* GIVEN : User provides max_http_payload_size which is larger than MCL allows.
* WHEN  : User tries to initialize the library with the given configuration parameters.
* THEN  : MCL returns MCL_INVALID_MAX_HTTP_PAYLOAD_SIZE.
*/
void test_initialize_005(void)
{
	configuration->mindsphere_hostname = "mindsphere";
	configuration->mindsphere_port = 10;
	configuration->mindsphere_certificate = "3iu2ned298ijdm";
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	configuration->proxy_hostname = NULL;
	configuration->max_http_payload_size = MCL_MAXIMUM_HTTP_PAYLOAD_SIZE + 1;
	configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
	configuration->tenant = "br-smk1";
	http_processor_initialize_IgnoreAndReturn(MCL_OK);

	E_MCL_ERROR_CODE result = mcl_communication_initialize(configuration, &communication);
	TEST_ASSERT_MESSAGE(MCL_INVALID_MAX_HTTP_PAYLOAD_SIZE == result, "MCL did not return MCL_INVALID_MAX_HTTP_PAYLOAD_SIZE");
}

/**
 * GIVEN : No specific requirement.
 * WHEN  : mcl_communication_onboard() is called with null mcl handle.
 * THEN  : MCL_TRIGGERED_WITH_NULL is returned.
 */
void test_onboard_001(void)
{
    E_MCL_ERROR_CODE result = mcl_communication_onboard(MCL_NULL);

    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == result, "mcl_communication_onboard() unexpected return error!");
}

/**
 * GIVEN : MCL handle is successfully initialized.
 * WHEN  : mcl_communication_onboard() is called.
 * THEN  : MCL_OK is returned.
 */
void test_onboard_002(void)
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "proxy.erlm.siemens.de";
    configuration->proxy_port = 20;
    configuration->proxy_username = "username";
    configuration->proxy_password = "password";
    configuration->proxy_type = MCL_PROXY_HTTP;
    configuration->proxy_domain = "domain";
    configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
    configuration->tenant = "br-smk1";

    // Mock http_processor_initialize and make sure registration access token is NULL.
    http_processor_t *http_processor;
    MCL_NEW(http_processor);
    MCL_NEW(http_processor->security_handler);
    http_processor->security_handler->registration_access_token = MCL_NULL;
    http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_processor_initialize_ReturnThruPtr_http_processor(&http_processor);

    // Initialize mcl_communication.
    mcl_communication_initialize(configuration, &communication);

    // Mock http_processor_onboard.
    http_processor_register_IgnoreAndReturn(MCL_OK);

    // Mock http_processor_get_access_token.
    http_processor_get_access_token_IgnoreAndReturn(MCL_OK);

    // Test.
    E_MCL_ERROR_CODE result = mcl_communication_onboard(communication);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_communication_onboard() unexpected return error!");

    // Clean up.
    MCL_FREE(http_processor->security_handler);
    MCL_FREE(http_processor);
}

/**
 * GIVEN : MCL handle is not null but its state is not-initialized.
 * WHEN  : mcl_communication_onboard() is called.
 * THEN  : MCL_NOT_INITIALIZED is returned.
 */
void test_onboard_003(void)
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "proxy.erlm.siemens.de";
    configuration->proxy_port = 20;
    configuration->proxy_username = "username";
    configuration->proxy_password = "password";
    configuration->proxy_type = MCL_PROXY_HTTP;
    configuration->proxy_domain = "domain";
	configuration->max_http_payload_size = 16384;
    configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
    configuration->tenant = "br-smk1";

    // Mock http_processor_initialize.
    http_processor_t *http_processor;
    MCL_NEW(http_processor);
    MCL_NEW(http_processor->security_handler);
    http_processor->security_handler->registration_access_token = MCL_NULL;
    http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_processor_initialize_ReturnThruPtr_http_processor(&http_processor);

    // Initialize mcl_communication.
    mcl_communication_initialize(configuration, &communication);

    communication->state.initialized = MCL_FALSE;

    // Test.
    E_MCL_ERROR_CODE result = mcl_communication_onboard(communication);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_NOT_INITIALIZED == result, "Return code should be MCL_NOT_INITIALIZED.");

    // Clean up.
    MCL_FREE(http_processor->security_handler);
    MCL_FREE(http_processor);
}

/**
 * GIVEN : MCL handle is successfully initialized.
 * WHEN  : mcl_communication_onboard() is called when the agent is already onboarded.
 * THEN  : MCL_ALREADY_ONBOARD is returned.
 */
void test_onboard_004(void)
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "proxy.erlm.siemens.de";
    configuration->proxy_port = 20;
    configuration->proxy_username = "username";
    configuration->proxy_password = "password";
    configuration->proxy_type = MCL_PROXY_HTTP;
    configuration->proxy_domain = "domain";
	configuration->max_http_payload_size = 16384;
    configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
    configuration->tenant = "br-smk1";

    // Mock http_processor_initialize with http_processor->security_handler->registration_access_token not null.
    http_processor_t *http_processor;
    MCL_NEW(http_processor);
    MCL_NEW(http_processor->security_handler);
    MCL_NEW(http_processor->security_handler->registration_access_token);
    http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_processor_initialize_ReturnThruPtr_http_processor(&http_processor);

    // Initialize mcl_communication.
    mcl_communication_initialize(configuration, &communication);

    // Mock http_processor_get_access_token.
    http_processor_get_access_token_IgnoreAndReturn(MCL_OK);

    // Test.
    E_MCL_ERROR_CODE result = mcl_communication_onboard(communication);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_ALREADY_ONBOARDED == result, "Return code should be MCL_ALREADY_ONBOARDED.");

    // Clean up.
    MCL_FREE(http_processor->security_handler->registration_access_token);
    MCL_FREE(http_processor->security_handler);
    MCL_FREE(http_processor);
}

/**
* GIVEN : A valid access token in security handler.
* WHEN  : #mcl_communication_get_token() is called.
* THEN  : MCL_OK is returned.
*/
void test_get_token_001()
{
	communication_t communication;
	http_processor_t processor;
	security_handler_t security_handler;
	char *dummy_token = "dummy_access_token";
	string_initialize_new(dummy_token, 0, &(security_handler.access_token));

	communication.http_processor = &processor;
	communication.http_processor->security_handler = &security_handler;

	char *token = MCL_NULL;
	E_MCL_ERROR_CODE code = mcl_communication_get_last_access_token(&communication, &token);

	TEST_ASSERT_EQUAL(MCL_OK, code);
	TEST_ASSERT_EQUAL_STRING(dummy_token, token);

	string_destroy(&(security_handler.access_token));
	MCL_FREE(token);
}

/**
* GIVEN : A valid access token in security handler.
* WHEN  : #mcl_communication_get_token() is called.
* THEN  : MCL_OK is returned.
*/
void test_get_last_token_time_001()
{
	communication_t communication;
	http_processor_t processor;
	security_handler_t security_handler;
	char *dummy_token_time = "dummy_token_time";
	string_initialize_new(dummy_token_time, 0, &(security_handler.last_token_time));

	communication.http_processor = &processor;
	communication.http_processor->security_handler = &security_handler;

	char *token_time = MCL_NULL;
	E_MCL_ERROR_CODE code = mcl_communication_get_last_token_time(&communication, &token_time);

	TEST_ASSERT_EQUAL(MCL_OK, code);
	TEST_ASSERT_EQUAL_STRING(dummy_token_time, token_time);

	string_destroy(&(security_handler.last_token_time));
	MCL_FREE(token_time);
}

/**
* GIVEN : Communication is initialized and onboarded.
* WHEN  : #mcl_communication_rotate_key() is called.
* THEN  : MCL_OK is returned.
*/
void test_rotate_key_001()
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "proxy.erlm.siemens.de";
    configuration->proxy_port = 20;
    configuration->proxy_username = "username";
    configuration->proxy_password = "password";
    configuration->proxy_type = MCL_PROXY_HTTP;
    configuration->proxy_domain = "domain";
    configuration->user_agent = "custom agent v1.0";
    configuration->initial_access_token = "InitialAccessToken";
    configuration->tenant = "br-smk1";

    // Mock http_processor_initialize and make sure registration access token is NULL.
    http_processor_t *http_processor;
    MCL_NEW(http_processor);
    MCL_NEW(http_processor->security_handler);
    http_processor->security_handler->registration_access_token = MCL_NULL;
    http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_processor_initialize_ReturnThruPtr_http_processor(&http_processor);

    // Initialize mcl_communication.
    mcl_communication_initialize(configuration, &communication);

    // Mock http_processor_onboard.
    http_processor_register_IgnoreAndReturn(MCL_OK);

    // Mock http_processor_get_access_token
    http_processor_get_access_token_IgnoreAndReturn(MCL_OK);

    E_MCL_ERROR_CODE result = mcl_communication_onboard(communication);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_communication_onboard() unexpected return error!");

    string_t *rat = MCL_NULL;
    result = string_initialize_new("rat", 0, &rat);
    TEST_ASSERT(MCL_OK == result);

    communication->http_processor->security_handler->registration_access_token = rat;

    // Mock http_processor_register.
    http_processor_register_IgnoreAndReturn(MCL_OK);

    // Mock http_processor_get_access_token
    http_processor_get_access_token_IgnoreAndReturn(MCL_OK);

    result = mcl_communication_rotate_key(communication);
    TEST_ASSERT(MCL_OK == result);

    // Clean up.
    string_destroy(&rat);
    MCL_FREE(http_processor->security_handler);
    MCL_FREE(http_processor);
}

/**
* GIVEN : Communication is initialized and onboarded.
* WHEN  : #mcl_communication_exchange() is called.
* THEN  : MCL_OK is returned.
*/
void test_exchange_001()
{
    configuration->mindsphere_hostname = "mindsphere";
    configuration->mindsphere_port = 10;
    configuration->mindsphere_certificate = "3iu2ned298ijdm";
    configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
    configuration->proxy_hostname = "proxy.erlm.siemens.de";
    configuration->proxy_port = 20;
    configuration->proxy_username = "username";
    configuration->proxy_password = "password";
    configuration->proxy_type = MCL_PROXY_HTTP;
    configuration->proxy_domain = "domain";
    configuration->user_agent = "custom agent v1.0";
    configuration->initial_access_token = "InitialAccessToken";
    configuration->tenant = "br-smk1";

	// Mock http_processor_initialize with http_processor->security_handler->registration_access_token not null.
    http_processor_t *http_processor = MCL_NULL;
    MCL_NEW(http_processor);
    MCL_NEW(http_processor->security_handler);
    MCL_NEW(http_processor->security_handler->registration_access_token);
    MCL_NEW(http_processor->security_handler->access_token);
    http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
    http_processor_initialize_ReturnThruPtr_http_processor(&http_processor);

    // Initialize mcl_communication.
    mcl_communication_initialize(configuration, &communication);

    // Mock http_processor_get_access_token.
    http_processor_get_access_token_IgnoreAndReturn(MCL_OK);

    E_MCL_ERROR_CODE result = mcl_communication_onboard(communication);

    // Check test result.
    TEST_ASSERT_MESSAGE(MCL_ALREADY_ONBOARDED == result, "mcl_communication_onboard() unexpected return error!");

    mcl_store_t dummy_store;
    dummy_store.streamable = MCL_FALSE;

    // Mock http_processor_exchange.
    http_processor_exchange_IgnoreAndReturn(MCL_OK);

    result = mcl_communication_exchange(communication, &dummy_store, NULL);
    TEST_ASSERT(MCL_OK == result);

    // Clean up.
    MCL_FREE(http_processor->security_handler->access_token);
    MCL_FREE(http_processor->security_handler->registration_access_token);
    MCL_FREE(http_processor->security_handler);
    MCL_FREE(http_processor);
}

/**
* GIVEN : Communication is initialized and onboarded.
* WHEN  : #mcl_communication_process() is called.
* THEN  : Store is exchanged successfully.
*/
void test_process_001()
{
	configuration->mindsphere_hostname = "mindsphere";
	configuration->mindsphere_port = 10;
	configuration->mindsphere_certificate = "3iu2ned298ijdm";
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	configuration->proxy_hostname = "proxy.erlm.siemens.de";
	configuration->proxy_port = 20;
	configuration->proxy_username = "username";
	configuration->proxy_password = "password";
	configuration->proxy_type = MCL_PROXY_HTTP;
	configuration->proxy_domain = "domain";
	configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
	configuration->tenant = "br-smk1";

	// Mock http_processor_initialize with http_processor->security_handler->registration_access_token not null.
	http_processor_t *http_processor = MCL_NULL;
	MCL_NEW(http_processor);
	MCL_NEW(http_processor->security_handler);
	MCL_NEW(http_processor->security_handler->registration_access_token);
	MCL_NEW(http_processor->security_handler->access_token);
	http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	http_processor_initialize_ReturnThruPtr_http_processor(&http_processor);

	// Initialize mcl_communication.
	mcl_communication_initialize(configuration, &communication);

	// Mock http_processor_get_access_token.
	http_processor_get_access_token_IgnoreAndReturn(MCL_OK);

	E_MCL_ERROR_CODE result = mcl_communication_onboard(communication);

	// Check test result.
	TEST_ASSERT_MESSAGE(MCL_ALREADY_ONBOARDED == result, "mcl_communication_onboard() unexpected return error!");

	mcl_store_t dummy_store;
	dummy_store.streamable = MCL_FALSE;

	// Mock http_processor_exchange.
	http_processor_exchange_IgnoreAndReturn(MCL_OK);

	result = mcl_communication_process(communication, &dummy_store, NULL);
	TEST_ASSERT_MESSAGE(MCL_OK == result, "Exchange has failed.");

	// Clean up.
	MCL_FREE(http_processor->security_handler->access_token);
	MCL_FREE(http_processor->security_handler->registration_access_token);
	MCL_FREE(http_processor->security_handler);
	MCL_FREE(http_processor);
}

/**
* GIVEN : No specific requirement.
* WHEN  : mcl_communication_process() is with null store handle.
* THEN  : MCL_TRIGGERED_WITH_NULL is returned.
*/
void test_process_002()
{
	E_MCL_ERROR_CODE result;

	configuration->mindsphere_hostname = "mindsphere";
	configuration->mindsphere_port = 10;
	configuration->mindsphere_certificate = "3iu2ned298ijdm";
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	configuration->proxy_hostname = "proxy.erlm.siemens.de";
	configuration->proxy_port = 20;
	configuration->proxy_username = "username";
	configuration->proxy_password = "password";
	configuration->proxy_type = MCL_PROXY_HTTP;
	configuration->proxy_domain = "domain";
	configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
	configuration->tenant = "br-smk1";

	http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);

	// Initialize mcl_communication.
	mcl_communication_initialize(configuration, &communication);

	result = mcl_communication_process(communication, MCL_NULL, NULL);
	TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == result, "mcl_communication_process() unexpected return error!");
}

/**
* GIVEN : No specific requirement.
* WHEN  : mcl_communication_process() is with null communication handle.
* THEN  : MCL_TRIGGERED_WITH_NULL is returned.
*/
void test_process_003()
{
	mcl_store_t dummy_store;
	E_MCL_ERROR_CODE result = mcl_communication_process(MCL_NULL, &dummy_store, NULL);
	TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == result, "mcl_communication_process() unexpected return error!");
}

/**
* GIVEN : Communication is initialized and access token is expired.
* WHEN  : #mcl_communication_process() is called.
* THEN  : New access token is acquired, exchange is done successfully.
*/
void test_process_004()
{
	E_MCL_ERROR_CODE result;

	configuration->mindsphere_hostname = "mindsphere";
	configuration->mindsphere_port = 10;
	configuration->mindsphere_certificate = "3iu2ned298ijdm";
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	configuration->proxy_hostname = "proxy.erlm.siemens.de";
	configuration->proxy_port = 20;
	configuration->proxy_username = "username";
	configuration->proxy_password = "password";
	configuration->proxy_type = MCL_PROXY_HTTP;
	configuration->proxy_domain = "domain";
	configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
	configuration->tenant = "br-smk1";

	// Mock http_processor_initialize with http_processor->security_handler->registration_access_token not null.
	http_processor_t *http_processor = MCL_NULL;
	MCL_NEW(http_processor);
	MCL_NEW(http_processor->security_handler);
	MCL_NEW(http_processor->security_handler->registration_access_token);

	char *dummy_token = "dummy_access_token";
	string_initialize_new(dummy_token, 0, &(http_processor->security_handler->access_token));

	http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	http_processor_initialize_ReturnThruPtr_http_processor(&http_processor);

	// Initialize mcl_communication.
	mcl_communication_initialize(configuration, &communication);

	// Mock http_processor_get_access_token.
	http_processor_get_access_token_IgnoreAndReturn(MCL_OK);

	// Mock http_processor_exchange.
	http_processor_exchange_IgnoreAndReturn(MCL_UNAUTHORIZED);
	http_processor_exchange_IgnoreAndReturn(MCL_OK);

	mcl_store_t dummy_store;
	dummy_store.streamable = MCL_FALSE;

	result = mcl_communication_process(communication, &dummy_store, NULL);
	TEST_ASSERT_MESSAGE(MCL_OK == result, "Exchange has failed.");

	// Clean up.
	string_destroy(&(http_processor->security_handler->access_token));
	MCL_FREE(http_processor->security_handler->access_token);
	MCL_FREE(http_processor->security_handler->registration_access_token);
	MCL_FREE(http_processor->security_handler);
	MCL_FREE(http_processor);
}

/**
* GIVEN : Communication is initialized, both access token and key expired.
* WHEN  : #mcl_communication_process() is called.
* THEN  : Key is rotated, new access token is obtained and store is exchanged successfully.
*/
void test_process_005()
{
	E_MCL_ERROR_CODE result;

	configuration->mindsphere_hostname = "mindsphere";
	configuration->mindsphere_port = 10;
	configuration->mindsphere_certificate = "3iu2ned298ijdm";
	configuration->security_profile = MCL_SECURITY_SHARED_SECRET;
	configuration->proxy_hostname = "proxy.erlm.siemens.de";
	configuration->proxy_port = 20;
	configuration->proxy_username = "username";
	configuration->proxy_password = "password";
	configuration->proxy_type = MCL_PROXY_HTTP;
	configuration->proxy_domain = "domain";
	configuration->user_agent = "custom agent v1.0";
	configuration->initial_access_token = "InitialAccessToken";
	configuration->tenant = "br-smk1";

	// Mock http_processor_initialize with http_processor->security_handler->registration_access_token not null.
	http_processor_t *http_processor = MCL_NULL;
	MCL_NEW(http_processor);
	MCL_NEW(http_processor->security_handler);
	MCL_NEW(http_processor->security_handler->registration_access_token);

	char *dummy_token = "dummy_access_token";
	string_initialize_new(dummy_token, 0, &(http_processor->security_handler->access_token));

	http_processor_initialize_ExpectAnyArgsAndReturn(MCL_OK);
	http_processor_initialize_ReturnThruPtr_http_processor(&http_processor);

	// Initialize mcl_communication.
	mcl_communication_initialize(configuration, &communication);

	// Mock http_processor_get_access_token.
	http_processor_get_access_token_IgnoreAndReturn(MCL_BAD_REQUEST);
	http_processor_get_access_token_IgnoreAndReturn(MCL_OK);

	// Mock http_processor_register.
	http_processor_register_IgnoreAndReturn(MCL_OK);

	// Mock http_processor_exchange.
	http_processor_exchange_IgnoreAndReturn(MCL_UNAUTHORIZED);
	http_processor_exchange_IgnoreAndReturn(MCL_OK);

	mcl_store_t dummy_store;
	dummy_store.streamable = MCL_FALSE;

	result = mcl_communication_process(communication, &dummy_store, NULL);
	TEST_ASSERT_MESSAGE(MCL_OK == result, "Exchange has failed.");

	// Clean up.
	string_destroy(&(http_processor->security_handler->access_token));
	MCL_FREE(http_processor->security_handler->access_token);
	MCL_FREE(http_processor->security_handler->registration_access_token);
	MCL_FREE(http_processor->security_handler);
	MCL_FREE(http_processor);
}
