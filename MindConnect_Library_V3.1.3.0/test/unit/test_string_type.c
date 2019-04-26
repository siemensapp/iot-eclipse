/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_string_type.c
* @date     Aug 3, 2016
* @brief    Unit test cases for mcl_string module
*
************************************************************************/

#include "list.h"
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
 * GIVEN : No initial condition
 * WHEN  : User requests initialization of an string_t with MCL_NULL.
 * THEN  : User expects the string is initialized with 0 length and buffer is NULL.
 */
void test_initialize_new_001(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : No initial condition
 * WHEN  : User requests initialization of an string_t with content.
 * THEN  : User expects the string is initialized with correct length and correct buffer content.
 */
void test_initialize_new_002(void)
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 11;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : No initial condition
 * WHEN  : User requests initialization of an string_t with content and length.
 * THEN  : User expects the string is initialized with correct length and correct buffer content.
 */
void test_initialize_new_003(void)
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 11;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : No initial condition
 * WHEN  : Initialize NEW String - Content : not NULL , Length : Non-Zero.
 * THEN  : Expect string->buffer : address different from content, string->length : 0. After destroy, content is not destroyed.
 */
void test_initialize_new_004(void)
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 11;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(test_content, string->buffer,
                                  "After initializing with value, string buffer and content string are pointing to the same memory address while it must be different.");

    string_destroy(&string);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test_string", test_content, "After destroy , content doesn't have the right content!");
}

/**
 * GIVEN : No initial condition
 * WHEN  : Initialize NEW String - Content : not NULL , Length : 0.
 * THEN  : Expect string->buffer : address different from content, string->length : 0. After destroy, content is not destroyed.
 */
void test_initialize_new_005(void)
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 11;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(test_content, string->buffer,
                                  "After initializing with value, string buffer and content string are pointing to the same memory address while it must be different.");

    string_destroy(&string);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test_string", test_content, "After destroy , content doesn't have the right content!");
}

/**
* GIVEN : No initial condition
* WHEN  : Initialize NEW String - Content : NULL , Length : Non-Zero.
* THEN  : Expect string->buffer : address is not NULL, string->length : equal to given length.
*/
void test_initialize_new_006(void)
{
	char *test_content = MCL_NULL;
	mcl_size_t test_content_length = 11;

	string_t *string;
	E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

	TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
	TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
	TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "Buffer is not allocated!");
	TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

	string_destroy(&string);
}

/**
 * GIVEN : No initial condition
 * WHEN  : User requests initialization of an string_t with another string_t.
 * THEN  : User expects the string is initialized with correct length and correct buffer content.
 */
void test_initialize_001(void)
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 11;

    string_t *string1;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, 0, &string1);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string1, "Initialized string1 is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string1->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string1->length, "After initializing with value, value of length in string1 is wrong!");

    string_t *string2;
    init_result = string_initialize(string1, &string2);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string2, "Initialized string2 is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(string1->buffer, string2->buffer, "After initializing with other string, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(string1->length, string2->length, "After initializing with other string, value of length in string2 is wrong!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_STRING_COPY_DESTROY, string2->type, "After initializing with other string, type of string2 is wrong!");

    string_destroy(&string1);
    string_destroy(&string2);
}

/**
 * GIVEN : No initial condition
 * WHEN  : User requests initialization of a static string_t with content, then destroys the string.
 * THEN  : User expects the content is not destroyed.
 */
void test_initialize_static_001()
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 11;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_static(test_content, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);

    TEST_ASSERT_NULL_MESSAGE(string, "Destroyed string is NOT NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test_string", test_content, "After destroy original string doesn't have the right content!");
}

/**
 * GIVEN : No initial condition
 * WHEN  : User requests initialization of a static string_t with content and length, then destroys the string.
 * THEN  : User expects the content is not destroyed.
 */
void test_initialize_static_002()
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 11;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_static(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);

    TEST_ASSERT_NULL_MESSAGE(string, "Destroyed string is NOT NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test_string", test_content, "After destroy original string doesn't have the right content!");
}

/**
 * GIVEN : No initial condition
 * WHEN  : Initialize STATIC  String - Content : NULL , Length : Non-Zero.
 * THEN  : Expect string->buffer : NULL, string->length : 0
 */
void test_initialize_static_003()
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 11;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_static(MCL_NULL, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "string->buffer is NOT NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);

    TEST_ASSERT_NULL_MESSAGE(string, "Destroyed string is NOT NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test_string", test_content, "After destroy original string doesn't have the right content!");
}

/**
 * GIVEN : No initial condition
 * WHEN  : Initialize STATIC  String - Content : NULL , Length : 0.
 * THEN  : Expect string->buffer : NULL, string->length : 0
 */
void test_initialize_static_004()
{
    char *test_content = "test_string";
    mcl_size_t test_content_length = 0;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_static(MCL_NULL, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "string->buffer is NOT NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);

    TEST_ASSERT_NULL_MESSAGE(string, "Destroyed string is NOT NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test_string", test_content, "After destroy original string doesn't have the right content!");
}

/**
 * GIVEN : No initial condition
 * WHEN  : User requests initialization of a dynamic string_t with content and length, then destroys the string.
 * THEN  : User expects the string->buffer and content is pointing the same space and after strings destroy, the content is not destroyed.
 */
void test_initialize_dyanmic_001()
{
    mcl_size_t test_content_length = 2;
    char *test_content = MCL_MALLOC(test_content_length + 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_content, "Couldn't allocated for test content");

    // to not be depend on strncpy or other string lib :
    test_content[0] = (char)':';
    test_content[1] = (char)')';
    test_content[2] = (char)'\0';

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_dynamic(test_content, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    // same buffer
    TEST_ASSERT_EQUAL_MESSAGE(string->buffer, test_content, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);

    TEST_ASSERT_NULL_MESSAGE(string, "Destroyed string is NOT NULL");

    // if you open following line test exe crushes which prooves that destroy works :)
    // TEST_ASSERT_EQUAL_STRING_MESSAGE("test_string", test_content, "After destroy original string doesn't have the right content!");
}

/**
 * GIVEN : No initial condition
 * WHEN  : User requests initialization of a dynamic string_t with content and length, then destroys the string.
 * THEN  : User expects the string->buffer and content is pointing the same space and after strings destroy, the content is not destroyed.
 */
void test_initialize_dyanmic_002()
{
    mcl_size_t test_content_length = 2;
    char *test_content = MCL_MALLOC(test_content_length + 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_content, "Couldn't allocated for test content");

    // to not be depend on strncpy or other string lib :
    test_content[0] = (char)':';
    test_content[1] = (char)')';
    test_content[2] = (char)'\0';

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_dynamic(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    // same buffer
    TEST_ASSERT_EQUAL_MESSAGE(string->buffer, test_content, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);

    TEST_ASSERT_NULL_MESSAGE(string, "Destroyed string is NOT NULL");

    // if you open following line test exe crushes which prooves that destroy works :)
    // TEST_ASSERT_EQUAL_STRING_MESSAGE("test_string", test_content, "After destroy original string doesn't have the right content!");
}

/**
 * GIVEN : No initial condition
 * WHEN  : Initialize DYNAMIC String - Content : NULL , Length : Non-Zero.
 * THEN  : Expect string->buffer : NULL, string->length : 0
 */
void test_initialize_dyanmic_003()
{
    mcl_size_t test_content_length = 2;
    char *test_content = MCL_MALLOC(test_content_length + 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_content, "Couldn't allocated for test content");

    // to not be depend on strncpy or other string lib :
    test_content[0] = ':';
    test_content[1] = ')';
    test_content[2] = '\0';

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_dynamic(MCL_NULL, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "string->buffer is NOT NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);

    TEST_ASSERT_NULL_MESSAGE(string, "Destroyed string is NOT NULL");
    MCL_FREE(test_content);
}

/**
 * GIVEN : No initial condition
 * WHEN  : Initialize DYNAMIC String - Content : NULL , Length : 0.
 * THEN  : Expect string->buffer : NULL, string->length : 0
 */
void test_initialize_dyanmic_004()
{
    mcl_size_t test_content_length = 2;
    char *test_content = MCL_MALLOC(test_content_length + 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_content, "Couldn't allocated for test content");

    // to not be depend on strncpy or other string lib :
    test_content[0] = ':';
    test_content[1] = ')';
    test_content[2] = '\0';

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_dynamic(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "string->buffer is NOT NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);

    TEST_ASSERT_NULL_MESSAGE(string, "Destroyed string is NOT NULL");
    MCL_FREE(test_content);
}

/**
 * GIVEN : String object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with test content.
 * THEN  : User expects the string content and  length is correct after set operation.
 */
void test_set_001(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    char *test_content = "test_string";
    int test_content_size = 11;
    E_MCL_ERROR_CODE ret = string_set(string, test_content, 0);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");

    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "After set string->buffer is still NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After set with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_size, string->length, "After set, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with MCL_NULL.
 * THEN  : User expects the set operation will return MCL_OK.
 */
void test_set_002(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    char *test_content = MCL_NULL;

    E_MCL_ERROR_CODE ret = string_set(string, test_content, 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with zero length string ("").
 * THEN  : User expects the string is NULL
 */
void test_set_003(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After set operation, buffer length is not right!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with "test" string.
 * WHEN  : User sets the string with zero length string ("").
 * THEN  : User expects the string NULL.
 */
void test_set_004(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After set operation, buffer length is not right!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with "test" string. After that user sets again with empty string ("")
 * THEN  : User expects the string is NULL.
 */
void test_set_005(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_MESSAGE(MCL_OK == ret, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After set operation, value of length in string is wrong!");

    ret = string_set(string, "", 0);

    // 14 for MCL_INVALID_PARAMETER
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After set operation, buffer length is not right!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with "test" string.
 * WHEN  : User sets the string with "test" string again.
 * THEN  : User expects the string buffer content is not changed.
 */
void test_set_006(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_MESSAGE(MCL_OK == ret, "MCL_OK expected as return code!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After set operation, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with "test" string. And then sets with "test" string again.
 * THEN  : User expects the string buffer content is not changed.
 */
void test_set_007(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After set operation, value of length in string is wrong!");

    // set with the same content:
    ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After set operation, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with "test" string. And then sets with a longer "longer_test" string again.
 * THEN  : User expects the string buffer content is the last set value and length is correct.
 */
void test_set_008(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After set operation, value of length in string is wrong!");

    // set with the same content:
    ret = string_set(string, "longer_test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("longer_test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(11, string->length, "After set operation, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with "longer_test" string. And then sets with a shorter "test" string again.
 * THEN  : User expects the string buffer content is the last set value and length is correct.
 */
void test_set_009(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "longer_test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("longer_test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(11, string->length, "After set operation, value of length in string is wrong!");

    // set with the same content:
    ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After set operation, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with "test" string.
 * WHEN  : User sets the string with a longer "longer_test" string.
 * THEN  : User expects the string buffer content is the last set value and length is correct.
 */
void test_set_010(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    // set with the same content:
    E_MCL_ERROR_CODE ret = string_set(string, "longer_test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("longer_test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(11, string->length, "After set operation, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : String object is successfully initialized with "longer_test" string.
 * WHEN  : User sets the string with a shorter "test" string.
 * THEN  : User expects the string buffer content is the last set value and length is correct.
 */
void test_set_011(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("longer_test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("longer_test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(11, string->length, "After initializing with value, value of length in string is wrong!");

    // set with the same content:
    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After set operation, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : DYNAMIC String object is successfully initialized with content.
 * WHEN  : User sets the string with a another content.
 * THEN  : User expects string has the correct content and first content is freed.
 */
void test_set_012(void)
{
    mcl_size_t test_content_length = 2;
    char *test_content = MCL_MALLOC(test_content_length + 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_content, "Couldn't allocated for test content");

    // to not be depend on strncpy or other string lib :
    test_content[0] = ':';
    test_content[1] = ')';
    test_content[2] = '\0';

    char *test_content2 = MCL_MALLOC(test_content_length + 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_content2, "Couldn't allocated for test content");

    // to not be depend on strncpy or other string lib :
    test_content2[0] = ':';
    test_content2[1] = '(';
    test_content2[2] = '\0';

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_dynamic(test_content, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_STRING_NOT_COPY_DESTROY, string->type, "string type is wrong!");

    // set with the same content:
    E_MCL_ERROR_CODE ret = string_set(string, test_content2, 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content2, string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After set operation, value of length in string is wrong!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_STRING_NOT_COPY_DESTROY, string->type, "string type is wrong!");

    // first content shouldn't be exists anymore. If you open following line test exe will crush :)
    // TEST_ASSERT_EQUAL_STRING_MESSAGE(":)", test_content, "After set operation, buffer doesn't have the right content!");

    string_destroy(&string);
}

/**
 * GIVEN : STATIC String object is successfully initialized with content.
 * WHEN  : User sets the string with a another content.
 * THEN  : User expects string has the correct content and first content is freed.
 */
void test_set_013(void)
{
    mcl_size_t test_content_length = 2;
    char *test_content = MCL_MALLOC(test_content_length + 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_content, "Couldn't allocated for test content");

    // to not be depend on strncpy or other string lib :
    test_content[0] = ':';
    test_content[1] = ')';
    test_content[2] = '\0';

    char *test_content2 = MCL_MALLOC(test_content_length + 1);
    TEST_ASSERT_NOT_NULL_MESSAGE(test_content2, "Couldn't allocated for test content");

    // to not be depend on strncpy or other string lib :
    test_content2[0] = ':';
    test_content2[1] = '(';
    test_content2[2] = '\0';

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_static(test_content, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_STRING_NOT_COPY_NOT_DESTROY, string->type, "string type is wrong!");

    // set with the same content:
    E_MCL_ERROR_CODE ret = string_set(string, test_content2, 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content2, string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After set operation, value of length in string is wrong!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(MCL_STRING_NOT_COPY_NOT_DESTROY, string->type, "string type is wrong!");

    // first content shouldn't be exists anymore. If you open following line test exe will crush :)
    TEST_ASSERT_EQUAL_STRING_MESSAGE(":)", test_content, "After set operation, buffer doesn't have the right content!");

    string_destroy(&string);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(":(", test_content2, "After set operation, buffer doesn't have the right content!");

    MCL_FREE(test_content);
    MCL_FREE(test_content2);
}

void _destroy_list_with_content(list_t *list)
{
    if (MCL_NULL != list)
    {
        list_node_t *node;
        while (MCL_NULL != (node = list_next(list)))
        {
			string_t* string = (string_t *)node->data;
            string_destroy(&string);
        }
        list_destroy(&list);
    }
}

/**
 * GIVEN : String object is successfully initialized with content with 1 token.
 * WHEN  : User splits the string by token.
 * THEN  : User expects 2 split part.
 */
void test_split_001(void)
{
    char *test_content = "test_content";
    char *test = "test";
    char *content = "content";
    char token = '_';
    mcl_size_t test_content_length = 12;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    list_t *list = MCL_NULL;
    E_MCL_ERROR_CODE split_result = string_split(string, token, &list);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, split_result, "Split operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "list is NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, list->count, "List count is wrong!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head, "list->head is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head->next, "list->head->next is NULL");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(test, ((string_t *)(list->head->data))->buffer, "First split part is wrong");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(content, ((string_t *)(list->head->next->data))->buffer, "Second split part is wrong");

    string_destroy(&string);
    _destroy_list_with_content(list);
}

/**
 * GIVEN : String object is successfully initialized with content with 0 token.
 * WHEN  : User splits the string by token.
 * THEN  : User expects 1 split part which equals to the original string.
 */
void test_split_002(void)
{
    char *test_content = "test_content";

    // char *test = "test";
    // char *content = "content";
    char token = '@';
    mcl_size_t test_content_length = 12;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    list_t *list = MCL_NULL;
    E_MCL_ERROR_CODE split_result = string_split(string, token, &list);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, split_result, "Split operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "list is NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "List count is wrong!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head, "list->head is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, ((string_t *)(list->head->data))->buffer, "First split part is wrong");

    string_destroy(&string);
    _destroy_list_with_content(list);
}

/**
 * GIVEN : String object is successfully initialized with content with 2 token with other sting between.
 * WHEN  : User splits the string by token.
 * THEN  : User expects 1 split part which equals to the original string.
 */
void test_split_003(void)
{
    char *test_content = "test_content_other";
    char *test = "test";
    char *content = "content";
    char *other = "other";
    char token = '_';
    mcl_size_t test_content_length = 18;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    list_t *list = MCL_NULL;
    E_MCL_ERROR_CODE split_result = string_split(string, token, &list);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, split_result, "Split operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "list is NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(3, list->count, "List count is wrong!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head, "list->head is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head->next, "list->head->next is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head->next->next, "list->head->next->next is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test, ((string_t *)(list->head->data))->buffer, "First split part is wrong");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(content, ((string_t *)(list->head->next->data))->buffer, "Second split part is wrong");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(other, ((string_t *)(list->head->next->next->data))->buffer, "Third split part is wrong");

    string_destroy(&string);
    _destroy_list_with_content(list);
}

/**
 * GIVEN : String object is successfully initialized with content with 2 token consecutively in the middle.
 * WHEN  : User splits the string by token.
 * THEN  : User expects 2 split part which equals to the original string.
 */
void test_split_004(void)
{
    char *test_content = "test__content";
    char *test = "test";
    char *content = "content";
    char token = '_';
    mcl_size_t test_content_length = 13;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    list_t *list = MCL_NULL;
    E_MCL_ERROR_CODE split_result = string_split(string, token, &list);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, split_result, "Split operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "list is NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, list->count, "List count is wrong!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head, "list->head is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head->next, "list->head->next is NULL");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(test, ((string_t *)(list->head->data))->buffer, "First split part is wrong");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(content, ((string_t *)(list->head->next->data))->buffer, "Second split part is wrong");

    string_destroy(&string);
    _destroy_list_with_content(list);
}

/**
 * GIVEN : String object is successfully initialized with content with 1 token in the beginning.
 * WHEN  : User splits the string by token.
 * THEN  : User expects 1 split part.
 */
void test_split_005(void)
{
    char *test_content = "_testcontent";
    char *test = "testcontent";
    char token = '_';
    mcl_size_t test_content_length = 12;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    list_t *list = MCL_NULL;
    E_MCL_ERROR_CODE split_result = string_split(string, token, &list);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, split_result, "Split operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "list is NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "List count is wrong!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head, "list->head is NULL");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(test, ((string_t *)(list->head->data))->buffer, "First split part is wrong");

    string_destroy(&string);
    _destroy_list_with_content(list);
}

/**
 * GIVEN : String object is successfully initialized with content with 2 token consecutively in beginning.
 * WHEN  : User splits the string by token.
 * THEN  : User expects 1 split part .
 */
void test_split_006(void)
{
    char *test_content = "__testcontent";
    char *test = "testcontent";
    char token = '_';
    mcl_size_t test_content_length = 13;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    list_t *list = MCL_NULL;
    E_MCL_ERROR_CODE split_result = string_split(string, token, &list);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, split_result, "Split operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "list is NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "List count is wrong!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head, "list->head is NULL");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(test, ((string_t *)(list->head->data))->buffer, "First split part is wrong");

    string_destroy(&string);
    _destroy_list_with_content(list);
}

/**
 * GIVEN : String object is successfully initialized with content with 2 token consecutively in the end.
 * WHEN  : User splits the string by token.
 * THEN  : User expects 1 split part .
 */
void test_split_007(void)
{
    char *test_content = "testcontent__";
    char *test = "testcontent";
    char token = '_';
    mcl_size_t test_content_length = 13;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    list_t *list = MCL_NULL;
    E_MCL_ERROR_CODE split_result = string_split(string, token, &list);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, split_result, "Split operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "list is NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "List count is wrong!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head, "list->head is NULL");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(test, ((string_t *)(list->head->data))->buffer, "First split part is wrong");

    string_destroy(&string);
    _destroy_list_with_content(list);
}

/**
 * GIVEN : String object is successfully initialized with content with 1 token consecutively in the end.
 * WHEN  : User splits the string by token.
 * THEN  : User expects 1 split part .
 */
void test_split_008(void)
{
    char *test_content = "testcontent_";
    char *test = "testcontent";
    char token = '_';
    mcl_size_t test_content_length = 12;

    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(test_content, test_content_length, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(test_content, string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(test_content_length, string->length, "After initializing with value, value of length in string is wrong!");

    list_t *list = MCL_NULL;
    E_MCL_ERROR_CODE split_result = string_split(string, token, &list);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, split_result, "Split operation failed!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list, "list is NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, list->count, "List count is wrong!");
    TEST_ASSERT_NOT_NULL_MESSAGE(list->head, "list->head is NULL");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(test, ((string_t *)(list->head->data))->buffer, "First split part is wrong");

    string_destroy(&string);
    _destroy_list_with_content(list);
}

/**
 * GIVEN : One string object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with "test" and compares it with MCL_NULL
 * THEN  : User expects compare result to be failed since string is MCL_NULL.
 */
void test_compare_with_cstr_001(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After set operation, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After set operation, value of length in string is wrong!");

    // Expect MCL_FAIL
    ret = string_compare_with_cstr(string, MCL_NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ret, "string_compare_with_cstr operation resulted with unexpected error!");

    string_destroy(&string);
}

/**
 * GIVEN : One string object is successfully initialized with "test".
 * WHEN  : User compares it with MCL_NULL.
 * THEN  : User expects compare result to be failed.
 */
void test_compare_with_cstr_002(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    // expecting MCL_FAIL
    E_MCL_ERROR_CODE ret = string_compare_with_cstr(string, MCL_NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ret, "string_compare_with_cstr operation resulted with unexpected error!");

    string_destroy(&string);
}

/**
 * GIVEN : One string object is successfully initialized with MCL_NULL.
 * WHEN  : User compares it with "".
 * THEN  : User expects compare result to be failed.
 */
void test_compare_with_cstr_003(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    // Expecting MCL_FAIL
    E_MCL_ERROR_CODE ret = string_compare_with_cstr(string, "");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ret, "string_compare_with_cstr operation resulted with unexpected error!");

    string_destroy(&string);
}

/**
 * GIVEN : One string object is successfully initialized with "test".
 * WHEN  : User compares it with "test".
 * THEN  : User expects compare result to be successful.
 */
void test_compare_with_cstr_004(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_RETURN_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_RETURN_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    // expecting MCL_OK
    E_MCL_ERROR_CODE ret = string_compare_with_cstr(string, "test");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_compare_with_cstr operation resulted with unexpected error!");

    string_destroy(&string);
}

/**
 * GIVEN : One string object is successfully initialized with MCL_NULL.
 * WHEN  : User sets the string with "test" and compares it with "test".
 * THEN  : User expects compare result to be successful.
 */
void test_compare_with_cstr_005(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    ret = string_compare_with_cstr(string, "test");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_compare_with_cstr operation resulted with unexpected error!");

    string_destroy(&string);
}

/**
 * GIVEN : One string object is successfully initialized with MCL_NULL. And another one with MCL_NULL
 * WHEN  : User sets the first string with "test" and compares it with the second string
 * THEN  : User expects compare result to be failed since compare string is MCL_NULL.
 */
void test_compare_001(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    string_t *string_2;
    init_result = string_initialize_new(MCL_NULL, 0, &string_2);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string_2, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string_2->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string_2->length, "After initializing with value, value of length in string is wrong!");

    ret = string_compare(string, string_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ret, "string_compare operation resulted with unexpected error!");

    string_destroy(&string);
    string_destroy(&string_2);
}

/**
 * GIVEN : One string object is successfully initialized with "test". And another one with MCL_NULL.
 * WHEN  : User compares two string.
 * THEN  : User expects compare result to be failed.
 */
void test_compare_002(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    string_t *string_2;
    init_result = string_initialize_new(MCL_NULL, 0, &string_2);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string_2, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string_2->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string_2->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_compare(string, string_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ret, "string_compare operation resulted with unexpected error!");

    string_destroy(&string);
    string_destroy(&string_2);
}

/**
 * GIVEN : One string object is successfully initialized with MCL_NULL. And another one with "".
 * WHEN  : User compares two string.
 * THEN  : User expects compare result to be failed.
 */
void test_compare_003(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    string_t *string_2;
    init_result = string_initialize_new("", 0, &string_2);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string_2, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string_2->buffer, "Initialized string->buffer is  NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string_2->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_compare(string, string_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, ret, "string_compare operation resulted with unexpected error!");

    string_destroy(&string);
    string_destroy(&string_2);
}

/**
 * GIVEN : One string object is successfully initialized with "test" and another one with "test".
 * WHEN  : User compares two strings.
 * THEN  : User expects compare result to be successful.
 */
void test_compare_004(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    string_t *string_2;
    init_result = string_initialize_new("test", 0, &string_2);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string_2, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string_2->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string_2->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string_2->length, "After initializing with value, value of length in string is wrong!");

    // expecting MCL_OK
    E_MCL_ERROR_CODE ret = string_compare(string, string_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_compare operation resulted with unexpected error!");

    string_destroy(&string);
    string_destroy(&string_2);
}

/**
 * GIVEN : One string object is successfully initialized with MCL_NULL and a second one with "test".
 * WHEN  : User sets the first string with "test" and compares it with the second one.
 * THEN  : User expects compare result to be successful.
 */
void test_compare_005(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    string_t *string_2;
    init_result = string_initialize_new("test", 0, &string_2);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string_2, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string_2->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string_2->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string_2->length, "After initializing with value, value of length in string is wrong!");

    ret = string_compare(string, string_2);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_compare operation resulted with unexpected error!");

    string_destroy(&string);
    string_destroy(&string_2);
}

/**
 * GIVEN : One string object is successfully initialized with MCL_NULL .
 * WHEN  : User sets the first string with "test" and requests to clear the string.
 * THEN  : User expects the string buffer is NULL and it's length is zero.
 */
void test_clear_001(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new(MCL_NULL, 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After initializing with value, value of length in string is wrong!");

    E_MCL_ERROR_CODE ret = string_set(string, "test", 0);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "string_set operation resulted with error!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    string_release(string);

    TEST_ASSERT_NULL_MESSAGE(string->buffer, "After clear string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After clear, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : One string object is successfully initialized with "test".
 * WHEN  : User requests to clear the string.
 * THEN  : User expects the string buffer is NULL and it's length is zero.
 */
void test_clear_002(void)
{
    string_t *string;
    E_MCL_ERROR_CODE init_result = string_initialize_new("test", 0, &string);

    TEST_ASSERT_MESSAGE(MCL_OK == init_result, "MCL_OK expected as return code!");
    TEST_ASSERT_NOT_NULL_MESSAGE(string, "Initialized string is NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(string->buffer, "Initialized string->buffer is not NULL");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("test", string->buffer, "After initializing with value, buffer doesn't have the right content!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, string->length, "After initializing with value, value of length in string is wrong!");

    string_release(string);

    TEST_ASSERT_NULL_MESSAGE(string->buffer, "After clear string->buffer is not NULL");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, string->length, "After clear, value of length in string is wrong!");

    string_destroy(&string);
}

/**
 * GIVEN : Buffer to convert to hex string is given.
 * WHEN  : Conversion to hex string string called.
 * THEN  : Expected content and length are checked.
 */
void test_convert_binary_to_hex_001(void)
{
    mcl_uint8_t buffer[] =
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
    };
    string_t *hex_data = MCL_NULL;

    E_MCL_ERROR_CODE code = string_convert_binary_to_hex(buffer, 8, &hex_data);

    TEST_ASSERT_MESSAGE(MCL_OK == code, "Expected result was not returned!");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("0123456789ABCDEF", hex_data->buffer, "Wrong content of hex string returned!");
    TEST_ASSERT_EQUAL_INT_MESSAGE(16, hex_data->length, "Wrong length of hex string returned!");

    string_destroy(&hex_data);
}

/**
* GIVEN : Two strings, "hello " and "world", are provided.
* WHEN  : #string_concatenate() is called.
* THEN  : Result is correct (i.e. "hello world").
*/
void test_concatenate_001()
{
	string_t first = STRING_CONSTANT("hello ");
	string_t second = STRING_CONSTANT("world");
	string_t *result = MCL_NULL;

	string_concatenate(&first, &second, &result);

	TEST_ASSERT_EQUAL_STRING("hello world", result->buffer);

    string_destroy(&result);
}

/**
* GIVEN : A string, "hello ", and a char array, "world", are provided.
* WHEN  : #string_concatenate_cstr() is called.
* THEN  : Result is correct (i.e. "hello world").
*/
void test_concatenate_cstr_001()
{
	string_t first = STRING_CONSTANT("hello ");
	string_t *result = MCL_NULL;

	string_concatenate_cstr(&first, "world", &result);

	TEST_ASSERT_EQUAL_STRING("hello world", result->buffer);

    string_destroy(&result);
}

/**
* GIVEN : A string is provided.
* WHEN  : #string_replace() is called.
* THEN  : Result is correct (i.e. "Result is MCL_OK").
*/
void test_replace_001()
{
    string_t test_string = STRING_CONSTANT("Result is MCL_FAIL");
    string_t *result = MCL_NULL;

    E_MCL_ERROR_CODE code = string_replace(&test_string, "MCL_FAIL", "MCL_OK", &result);
    TEST_ASSERT_EQUAL(MCL_OK, code);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_STRING("Result is MCL_OK", result->buffer);

    string_destroy(&result);
}
