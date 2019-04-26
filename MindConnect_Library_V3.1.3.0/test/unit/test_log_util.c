/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_log_util.c
* @date     5 Tem 2017
* @brief    Unit test cases for log_util.
*
************************************************************************/

#include "mcl/mcl_log_util.h"
#include "log_util.h"
#include "unity.h"
#include "file_util.h"
#include "storage.h"

static mcl_bool_t _callback_check;

static void _dummy_log_function(int log_level, const char *tag, const char *message, void *user_context);

void setUp(void)
{

}

void tearDown(void)
{

}

/**
* GIVEN : User provides a valid file path.
* WHEN  : #mcl_log_util_initialize() is called with E_LOG_OUTPUT_FILE.
* THEN  : log_util is initialized and MCL_OK is returned.
*/
void test_initialize_001()
{
    E_MCL_ERROR_CODE result;
    char file_path[] = "dummy.txt";

    result = mcl_log_util_initialize(E_LOG_OUTPUT_FILE, file_path);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_log_util_initialize() does not return MCL_OK.");
    mcl_log_util_finalize();
}

/**
* GIVEN : User provides a valid callback function.
* WHEN  : #mcl_log_util_initialize() is called with E_LOG_OUTPUT_FILE.
* THEN  : log_util is initialized and MCL_OK is returned.
*/
void test_initialize_002()
{
    E_MCL_ERROR_CODE result;

    result = mcl_log_util_initialize(E_LOG_OUTPUT_CALLBACK, _dummy_log_function);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_log_util_initialize() does not return MCL_OK.");

    mcl_log_util_finalize();
}

/**
* GIVEN : No initial condition.
* WHEN  : #mcl_log_util_initialize() is called with E_LOG_OUTPUT_STDERR.
* THEN  : log_util is initialized and MCL_INVALID_PARAMETER is returned.
*/
void test_initialize_003()
{
    E_MCL_ERROR_CODE result;

    result = mcl_log_util_initialize(E_LOG_OUTPUT_STDERR);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == result, "mcl_log_util_initialize() does not return MCL_INVALID_PARAMETER.");
}

/**
* GIVEN : log_util is initialized with a valid callback function.
* WHEN  : #mcl_log_util_set_output_level() is called with LOG_UTIL_LEVEL_ERROR.
* THEN  : Debug message must not be logged.
*/
void test_set_output_level_001()
{
    E_MCL_ERROR_CODE result;

   result = mcl_log_util_initialize(E_LOG_OUTPUT_CALLBACK, _dummy_log_function);
   TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_log_util_initialize() does not return MCL_OK.");

   _callback_check = MCL_FALSE;
   result = mcl_log_util_set_output_level(LOG_UTIL_LEVEL_ERROR);
   TEST_ASSERT_MESSAGE(MCL_OK == result, "mcl_log_util_set_output_level() does not return MCL_OK.");

   // log_util must not call our callback function and _callback_check must remain as MCL_FALSE.
   MCL_DEBUG("Dummy\r\n");

   TEST_ASSERT_MESSAGE(MCL_TRUE != _callback_check, "Debug message is logged when output level is \"LOG_UTIL_LEVEL_ERROR\".");
   mcl_log_util_finalize();
}

/**
* GIVEN : No initial condition.
* WHEN  : #mcl_log_util_set_output_level() is called with non-existent log level.
* THEN  : MCL_INVALID_LOG_LEVEL must be returned.
*/
void test_set_output_level_002()
{
    E_MCL_ERROR_CODE result;

    result = mcl_log_util_set_output_level(LOG_UTIL_LEVEL_NONE - 1);
    TEST_ASSERT_MESSAGE(MCL_INVALID_LOG_LEVEL == result, "mcl_log_util_set_output_level() does not return MCL_INVALID_LOG_LEVEL");
}

/**
* GIVEN : No initial condition.
* WHEN  : #mcl_log_util_convert_error_code_to_string() is called with a valid error code.
* THEN  : The error code is converted to string and returned.
*/
void test_convert_error_code_to_string_001()
{
    // Give an ordinary error code to function as an argument.
    const char *error_string = mcl_log_util_convert_error_code_to_string(MCL_SERVER_CERTIFICATE_NOT_VERIFIED);
    TEST_ASSERT_NOT_NULL(error_string);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("MCL_SERVER_CERTIFICATE_NOT_VERIFIED", error_string, "Wrong error code is returned in string format.");
}

/**
* GIVEN : No initial condition.
* WHEN  : #mcl_log_util_convert_error_code_to_string() is called with an invalid error code which is out of defined range.
* THEN  : User expects to get NULL.
*/
void test_convert_error_code_to_string_002()
{
    // Give an invalid error code to function as an argument.
    const char *error_string = mcl_log_util_convert_error_code_to_string(MCL_ERROR_CODE_END + 1);
    TEST_ASSERT_NULL(error_string);
}

static void _dummy_log_function(int log_level, const char *tag, const char *message, void *user_context)
{
    _callback_check = MCL_TRUE;
}

