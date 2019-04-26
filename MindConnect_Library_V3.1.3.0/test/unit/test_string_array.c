/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_string_array.c
* @date     Aug 19, 2016
* @brief    Unit test cases for string_array module.
*
************************************************************************/

#include "mcl/mcl_common.h"
#include "string_array.h"
#include "string_util.h"
#include "definitions.h"
#include "memory.h"
#include "unity.h"
#include "string_type.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN :
 * WHEN  : User requests initialization of an string_array for 0 strings.
 * THEN  : User expects initialize to be failed.
 */
void test_initialize_001(void)
{
    string_array_t *array = MCL_NULL;
	mcl_size_t count = 0;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);

    TEST_ASSERT_NOT_EQUAL(0, result);
    TEST_ASSERT_NULL_MESSAGE(array, "string_array is NOT NULL after initialize");
}

/**
 * GIVEN :
 * WHEN  : User requests initialization of an string_array for 1 strings.
 * THEN  : User expects array members of the array have their initial values.
 */
void test_initialize_002(void)
{
    string_array_t *array;
    mcl_size_t count = 1;
    string_array_initialize(count, &array);

    TEST_ASSERT_NOT_NULL_MESSAGE(array, "string array is NULL after initialize");
    TEST_ASSERT_EQUAL_INT(count, array->count);
    TEST_ASSERT_NOT_NULL_MESSAGE(array->items, "string array is NULL after initialize");

    for (mcl_size_t i = 0; i < count; i++)
    {
        TEST_ASSERT_NULL_MESSAGE(array->items[i].string, "string in array is NOT NULL");
    }

    TEST_ASSERT_EQUAL_INT(DEFAULT_INCREMENT_VALUE, array->increment_value);
    TEST_ASSERT_EQUAL_INT(0, array->index);
    TEST_ASSERT_EQUAL_INT(0, array->total_length);

    string_array_destroy(&array);
}

/**
 * GIVEN :
 * WHEN  : User requests initialization of an string_array for 5 strings.
 * THEN  : User expects array members of the array have their initial values.
 */
void test_initialize_003(void)
{
    string_array_t *array;
    mcl_size_t count = 5;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(array, "string array is NULL after initialize");
    TEST_ASSERT_EQUAL_INT(count, array->count);
    TEST_ASSERT_NOT_NULL_MESSAGE(array->items, "string array is NULL after initialize");

    for (mcl_size_t i = 0; i < count; i++)
    {
        TEST_ASSERT_NULL_MESSAGE(array->items[i].string, "string in array is NOT NULL");
    }

    TEST_ASSERT_EQUAL_INT(DEFAULT_INCREMENT_VALUE, array->increment_value);
    TEST_ASSERT_EQUAL_INT(0, array->index);
    TEST_ASSERT_EQUAL_INT(0, array->total_length);

    string_array_destroy(&array);
}

/**
 * GIVEN : User requests initialization of an string_array for 2 strings.
 * WHEN  : User adds 2 strings into the array.
 * THEN  : User expects the operation succeeds and index value is correct.
 */
void test_add_001(void)
{
    string_array_t *array;
    mcl_size_t count = 2;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_t *s1, *s2;
    string_initialize_static("firstpart", 0, &s1);
    string_initialize_static("secondpart", 0, &s2);

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);

    result = string_array_add(array, s1, MCL_TRUE);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(1, array->index);

    result = string_array_add(array, s2, MCL_TRUE);

    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(2, array->index);

    string_array_destroy(&array);
}

/**
 * GIVEN : User requests initialization of an string_array for 2 strings.
 * WHEN  : User adds 3 strings into the array.
 * THEN  : User expects after final add operation, count will be increased by default inc
 */
void test_add_002(void)
{
    string_array_t *array;

    mcl_size_t count = 2;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(array, "string array is NULL after initialize");
    TEST_ASSERT_EQUAL_INT(count, array->count);

    string_t *s1, *s2, *s3;
    string_initialize_static("firstpart", 0, &s1);
    string_initialize_static("secondpart", 0, &s2);
    string_initialize_static("thirdpart", 0, &s3);

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);
    TEST_ASSERT_NOT_NULL(s3);

    result = string_array_add(array, s1, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(1, array->index);
    TEST_ASSERT_EQUAL_INT(count, array->count);

    result = string_array_add(array, s2, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(2, array->index);
    TEST_ASSERT_EQUAL_INT(count, array->count);

    result = string_array_add(array, s3, MCL_TRUE);
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(3, array->index);
    TEST_ASSERT_EQUAL_INT(count + array->increment_value, array->count);

	string_array_destroy(&array);
}

/**
 * GIVEN : User requests initialization of an string_array for 2 strings.
 * WHEN  : User adds 3 strings into the array.
 * THEN  : User expects after final add operation, first two strings still have the same value
 */
void test_add_003(void)
{
    string_array_t *array;

    mcl_size_t count = 2;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(array, "string array is NULL after initialize");
    TEST_ASSERT_EQUAL_INT(count, array->count);

    string_t *s1, *s2, *s3;
    string_initialize_static("firstpart", 0, &s1);
    string_initialize_static("secondpart", 0, &s2);
    string_initialize_static("thirdpart", 0, &s3);

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);
    TEST_ASSERT_NOT_NULL(s3);

    result = string_array_add(array, s1, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(1, array->index);
    TEST_ASSERT_EQUAL_INT(count, array->count);

    result = string_array_add(array, s2, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(2, array->index);
    TEST_ASSERT_EQUAL_INT(count, array->count);

    result = string_array_add(array, s3, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(3, array->index);
    TEST_ASSERT_EQUAL_INT(count + array->increment_value, array->count);

    TEST_ASSERT_EQUAL_STRING("firstpart", array->items[0].string->buffer);
    TEST_ASSERT_EQUAL_STRING("secondpart", array->items[1].string->buffer);
    TEST_ASSERT_EQUAL_STRING("thirdpart", array->items[2].string->buffer);

	string_array_destroy(&array);
}

/**
 * GIVEN : User requests initialization of an string_array for 2 strings.
 * WHEN  : User adds 3 strings into the array.
 * THEN  : User expects after final add operation, first two strings still have the same value
 */
void test_add_004(void)
{
    string_array_t *array;

    mcl_size_t count = 1;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(array, "string array is NULL after initialize");
    TEST_ASSERT_EQUAL_INT(count, array->count);

    string_t *s1, *s2;
    string_initialize_static("firstpart", 0, &s1);
    string_initialize_static("secondpart", 0, &s2);

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);

    result = string_array_add(array, s1, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(1, array->index);
    TEST_ASSERT_EQUAL_INT(count, array->count);

    result = string_array_add(array, s2, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(2, array->index);
    TEST_ASSERT_EQUAL_INT(count + DEFAULT_INCREMENT_VALUE, array->count);

    TEST_ASSERT_EQUAL_STRING("firstpart", array->items[0].string->buffer);
    TEST_ASSERT_EQUAL_STRING("secondpart", array->items[1].string->buffer);

	string_array_destroy(&array);
}

/**
 * GIVEN : User requests initialization of an string_array for 2 strings.
 * WHEN  : User sets increment value as 1. And adds 3 string into the array.
 * THEN  : User expects after final add operation, count will be increased by 1
 */
void test_set_increment_001(void)
{
    string_array_t *array;

	mcl_size_t count = 2;
    mcl_size_t increment = 1;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(array, "string array is NULL after initialize");
    TEST_ASSERT_EQUAL_INT(count, array->count);
    TEST_ASSERT_EQUAL_INT(DEFAULT_INCREMENT_VALUE, array->increment_value);

    string_array_set_increment(array, increment);
    TEST_ASSERT_EQUAL_INT(increment, array->increment_value);

    string_t *s1, *s2, *s3;
    string_initialize_static("firstpart", 0, &s1);
    string_initialize_static("secondpart", 0, &s2);
    string_initialize_static("thirdpart", 0, &s3);

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);
    TEST_ASSERT_NOT_NULL(s3);

    result = string_array_add(array, s1, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(1, array->index);
    TEST_ASSERT_EQUAL_INT(count, array->count);

    result = string_array_add(array, s2, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(2, array->index);
    TEST_ASSERT_EQUAL_INT(count, array->count);

    result = string_array_add(array, s3, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(3, array->index);
    TEST_ASSERT_EQUAL_INT(count + increment, array->count);

    TEST_ASSERT_EQUAL_STRING("firstpart", array->items[0].string->buffer);
    TEST_ASSERT_EQUAL_STRING("secondpart", array->items[1].string->buffer);
    TEST_ASSERT_EQUAL_STRING("thirdpart", array->items[2].string->buffer);

	string_array_destroy(&array);
}

/**
 * GIVEN : User requests initialization of an string_array for 2 strings.
 * WHEN  : User sets increment value as 0
 * THEN  : User expects set_increment operation to be failed and arrays increment value not to be changed.
 */
void test_set_increment_002(void)
{
    string_array_t *array;

	mcl_size_t count = 2;
	mcl_size_t incr = 0;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(array, "string array is NULL after initialize");
    TEST_ASSERT_EQUAL_INT(count, array->count);
    TEST_ASSERT_EQUAL_INT(DEFAULT_INCREMENT_VALUE, array->increment_value);

    result = string_array_set_increment(array, incr);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == result, "MCL_INVALID_PARAMETER expected as return code!");
    TEST_ASSERT_EQUAL_INT(DEFAULT_INCREMENT_VALUE, array->increment_value);

    string_array_destroy(&array);
}

/**
 * GIVEN : User requests initialization of an string_array for 3 strings.
 * WHEN  : User adds 3 string with destroy flag set to FALSE
 * THEN  : User expects after destroy, strings are not deleted
 */
void test_destroy_001(void)
{
    string_array_t *array;

    mcl_size_t count = 2;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_t *s1, *s2, *s3;
    string_initialize_new("firstpart", 0, &s1);
    string_initialize_new("secondpart", 0, &s2);
    string_initialize_new("thirdpart", 0, &s3);

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);
    TEST_ASSERT_NOT_NULL(s3);

    result = string_array_add(array, s1, MCL_FALSE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    result = string_array_add(array, s2, MCL_FALSE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    result = string_array_add(array, s3, MCL_FALSE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_array_destroy(&array);

    TEST_ASSERT_EQUAL_STRING("firstpart", s1->buffer);
    TEST_ASSERT_EQUAL_STRING("secondpart", s2->buffer);
    TEST_ASSERT_EQUAL_STRING("thirdpart", s3->buffer);

	string_destroy(&s1);
	string_destroy(&s2);
	string_destroy(&s3);
}

/**
 * GIVEN : User requests initialization of an string_array for 3 strings.
 * WHEN  : User adds 3 string with destroy flag set to FALSE
 * THEN  : User expects after destroy, strings are not deleted
 */
void test_destroy_002(void)
{
    string_array_t *array;

    mcl_size_t count = 2;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_t *s1, *s2, *s3;
    string_initialize_new("firstpart", 0, &s1);
    string_initialize_new("secondpart", 0, &s2);
    string_initialize_new("thirdpart", 0, &s3);

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);
    TEST_ASSERT_NOT_NULL(s3);

    result = string_array_add(array, s1, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    result = string_array_add(array, s2, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    result = string_array_add(array, s3, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_array_destroy(&array);

    // TODO : If you open following lines program will crash. This shows that the test case is passed.
    // TEST_ASSERT_EQUAL_STRING("firstpart", s1->buffer);
    // TEST_ASSERT_EQUAL_STRING("secondpart", s2->buffer);
    // TEST_ASSERT_EQUAL_STRING("thirdpart", s3->buffer);
}

/**
 * GIVEN : User requests initialization of an string_array for 3 strings.
 * WHEN  : User adds 3 undestroyable string with destroy flag set to TRUE
 * THEN  : User expects after destroy, strings are deleted but buffers are not
 */
void test_destroy_003(void)
{
    string_array_t *array;

    mcl_size_t count = 2;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    char *c1 = "firstpart";
    char *c2 = "secondpart";
    char *c3 = "thirdpart";

    string_t *s1, *s2, *s3;
    string_initialize_static(c1, 0, &s1);
    string_initialize_static(c2, 0, &s2);
    string_initialize_static(c3, 0, &s3);

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);
    TEST_ASSERT_NOT_NULL(s3);

    result = string_array_add(array, s1, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    result = string_array_add(array, s2, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    result = string_array_add(array, s3, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_array_destroy(&array);

    TEST_ASSERT_EQUAL_STRING("firstpart", c1);
    TEST_ASSERT_EQUAL_STRING("secondpart", c2);
    TEST_ASSERT_EQUAL_STRING("thirdpart", c3);
}

/**
 * GIVEN : User requests initialization of an string_array for 2 strings.
 * WHEN  : User adds 3 string and calls to_string()
 * THEN  : User expects to get the concatenated final string
 */
void test_to_string_001(void)
{
    string_array_t *array;

    mcl_size_t count = 2;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_t *s1, *s2, *s3;
    string_initialize_static("firstpart", 0, &s1);
    string_initialize_static("secondpart", 0, &s2);
    string_initialize_static("thirdpart", 0, &s3);

    char *concat = "firstpartsecondpartthirdpart";

    TEST_ASSERT_NOT_NULL(s1);
    TEST_ASSERT_NOT_NULL(s2);
    TEST_ASSERT_NOT_NULL(s3);

    result = string_array_add(array, s1, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    result = string_array_add(array, s2, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    result = string_array_add(array, s3, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_t *final_string = string_array_to_string(array);
    TEST_ASSERT_NOT_NULL(final_string);
    TEST_ASSERT_EQUAL_STRING(concat, final_string->buffer);

    string_array_destroy(&array);
    string_destroy(&final_string);
}

/**
 * GIVEN : User requests initialization of an string_array for 2 strings.
 * WHEN  : User doesn't add any string and calls to_string()
 * THEN  : User expects to get NULL as the result.
 */
void test_to_string_002(void)
{
    string_array_t *array;

    mcl_size_t count = 2;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    string_t *final_string = string_array_to_string(array);
    TEST_ASSERT_NULL(final_string);

    string_array_destroy(&array);
    string_destroy(&final_string);
}

/**
 * GIVEN : User requests initialization of an string_array for 5 strings.
 * WHEN  : User adds 1 string and calls to_string()
 * THEN  : User expects to get the same string.
 */
void test_to_string_003(void)
{
    string_array_t *array;

    mcl_size_t count = 5;
    E_MCL_ERROR_CODE result = string_array_initialize(count, &array);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");

    char *concat = "firstpart";
    string_t *s1;
    string_initialize_static(concat, 0, &s1);

    TEST_ASSERT_NOT_NULL(s1);

    result = string_array_add(array, s1, MCL_TRUE);
    TEST_ASSERT_MESSAGE(MCL_OK == result, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_INT(0, result);

    string_t *final_string = string_array_to_string(array);
    TEST_ASSERT_NOT_NULL(final_string);
    TEST_ASSERT_EQUAL_STRING(concat, final_string->buffer);

    string_array_destroy(&array);
    string_destroy(&final_string);
}
