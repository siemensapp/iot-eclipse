/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_time_util.c
* @date     Dec 22, 2016
* @brief    This file contains test case functions to test time_util module.
*
************************************************************************/

#include "time_util.h"
#include "unity.h"
#include "memory.h"
#include "string_util.h"
#include "string_type.h"
#include "definitions.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : A time value to convert to ISO 8601 date and time format.
 * WHEN  : time_util_convert_to_iso_8601_format() function is called for the given time value.
 * THEN  : MCL_OK is returned and resulting ISO 8601 date and time corresponds to the given time value.
 */
void test_convert_to_iso_8601_format_001(void)
{
    time_t time_value = 1482402748;

    string_t *iso8601_formatted_time = MCL_NULL;
    E_MCL_ERROR_CODE return_code = time_util_convert_to_iso_8601_format(&time_value, &iso8601_formatted_time);

    char *expected_time = "2016-12-22T10:32:28.000Z";
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Time value can not be converted to ISO 8601 date and time format.");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_time, iso8601_formatted_time->buffer, "ISO 8601 time mismatch.");

    string_destroy(&iso8601_formatted_time);
}

/**
 * GIVEN : A timestamp which has a wrong format.
 * WHEN  : time_util_validate_timestamp() function is called for a timestamp in wrong format.
 * THEN  : MCL_FALSE is returned.
 */
void test_validate_timestamp_001(void)
{
    // wrong format T -> X.
    char *timestamp = "2016-04-26X08:06:25.317Z";
    mcl_bool_t result = time_util_validate_timestamp(timestamp);
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "MCL_FALSE didn't return for wrong format.");
}

/**
 * GIVEN : A timestamp which has a wrong length.
 * WHEN  : time_util_validate_timestamp() function is called for a timestamp whose length is wrong.
 * THEN  : MCL_FALSE is returned.
 */
void test_validate_timestamp_002(void)
{
    // wrong length 24 -> 23.
    char *timestamp = "2016-04-26T08:06:25.31";
    mcl_bool_t result = time_util_validate_timestamp(timestamp);
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "MCL_FALSE didn't return for wrong length.");
}

/**
 * GIVEN : A timestamp which has a wrong value.
 * WHEN  : time_util_validate_timestamp() function is called for a timestamp whose date or time value is not valid.
 * THEN  : MCL_FALSE is returned.
 */
void test_validate_timestamp_003(void)
{
    // wrong hour value up to 24 -> 25.
    char *timestamp = "2016-04-26T25:06:25.317Z";
    mcl_bool_t result = time_util_validate_timestamp(timestamp);
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "MCL_FALSE didn't return for wrong time value.");
}

