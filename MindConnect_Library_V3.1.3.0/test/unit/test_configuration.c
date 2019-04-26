/*!**********************************************************************
*
* @copyright Copyright (C) 2018 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_configuration.c
* @date     Jan 22, 2018
* @brief    This file contains test case functions to test configuration module.
*
************************************************************************/

#include "unity.h"
#include "mcl/mcl_configuration.h"
#include "configuration.h"
#include "definitions.h"
#include "memory.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : Uninitialized configuration object.
 * WHEN  : mcl_configuration_initialize() is called with NULL argument.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_initialize_001(void)
{
    E_MCL_ERROR_CODE code = mcl_configuration_initialize(MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_configuration_initialize() failed when configuration is NULL.");
}

/**
 * GIVEN : Uninitialized configuration object.
 * WHEN  : mcl_new_configuration() is called without any parameter.
 * THEN  : It gives an initialized configuration data struct with mandatory fields.
 */
void test_initialize_002(void)
{
    mcl_configuration_t *configuration;
    configuration = mcl_new_configuration();

    TEST_ASSERT_NOT_NULL_MESSAGE(configuration, "configuration couldn't be initialized.");

    // Test proxy_type
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_PROXY_UNKNOWN, configuration->proxy_type, "proxy_type is wrong.");

    // Test security_profile
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_SECURITY_SHARED_SECRET, configuration->security_profile, "security_profile is wrong.");

    // Test max_http_payload_size
    TEST_ASSERT_EQUAL_INT_MESSAGE(DEFAULT_HTTP_PAYLOAD_SIZE, configuration->max_http_payload_size, "max_http_payload_size is wrong.");

    // Test http_request_timeout
    TEST_ASSERT_EQUAL_INT_MESSAGE(DEFAULT_HTTP_REQUEST_TIMEOUT, configuration->http_request_timeout, "http_request_timeout is wrong.");

    mcl_configuration_destroy(&configuration);
}

/**
 * GIVEN : Initialized configuration object.
 * WHEN  : User requests to destroy configuration object.
 * THEN  : MCL_OK is returned and configuration object is null.
 */
void test_destroy_001(void)
{
    mcl_configuration_t *configuration = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_configuration_initialize(&configuration);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_configuration_initialize() failed.");

    code = mcl_configuration_destroy(&configuration);
    TEST_ASSERT_MESSAGE(MCL_NULL == configuration, "Configuration is not NULL after destroy.");
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_configuration_destroy() failed.");
}
