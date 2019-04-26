/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     test_json_util.c
 * @date     Feb 22, 2017
 * @brief    This file implements all test cases of json_util module.
 *
 ************************************************************************/

#include "json_util.h"
#include "unity.h"
#include "memory.h"
#include "definitions.h"
#include "mcl/mcl_json_util.h"
#include "json_util.h"
#include "string_util.h"
#include "string_type.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : root is provided without allocated memory.
 * WHEN  : root is initialized as json object.
 * THEN  : It gives the initialized root json struct to the user as json object.
 */
void test_initialize_001(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_initialize(MCL_JSON_OBJECT, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_initialize() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : root is provided without allocated memory.
 * WHEN  : root is initialized as json array.
 * THEN  : It gives the initialized root json struct to the user as json array.
 */
void test_initialize_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_initialize(MCL_JSON_ARRAY, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_initialize() failed.");

    // Convert root json array to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "[]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : User calls mcl_json_util_start_array() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_start_array_001(void)
{
    // Start array in root.
    char *array_name = "test array name";
    mcl_json_t *json_array = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_start_array(MCL_NULL, array_name, &json_array);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_start_array() with array_name = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_start_array_002(void)
{
    // Initialize root
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start array in root.
    mcl_json_t *json_array = MCL_NULL;

    E_MCL_ERROR_CODE code = mcl_json_util_start_array(root, MCL_NULL, &json_array);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for array_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to start an array in root with mcl_json_util_start_array().
 * THEN  : User expects to see an array object in root.
 */
void test_start_array_003(void)
{
    // Initialize root
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start array in root.
    char *array_name = "test array name";
    mcl_json_t *json_array = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_start_array(root, array_name, &json_array);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_start_array() failed.");

    // finish array.
    mcl_json_util_finish_array(&json_array);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test array name\":[]}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : User calls mcl_json_util_start_object() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_start_object_001(void)
{
    // Start object in root.
    char *object_name = "test object name";
    mcl_json_t *json_object = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_start_object(MCL_NULL, object_name, &json_object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_start_object() with object_name = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_start_object_002(void)
{
    // Initialize root
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start object in root.
    mcl_json_t *json_object = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_start_object(root, MCL_NULL, &json_object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to start an object in root with mcl_json_util_start_object().
 * THEN  : User expects to see an object in root.
 */
void test_start_object_003(void)
{
    // Initialize root
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start object in root.
    char *object_name = "test object name";
    mcl_json_t *json_object = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_start_object(root, object_name, &json_object);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_start_object() failed.");

    // finish object.
    mcl_json_util_finish_object(&json_object);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":{}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Uninitialized root.
 * WHEN  : User calls mcl_json_util_add_string() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_string_001(void)
{
    char *name = "test_name";
    char *value = "test_value";

    E_MCL_ERROR_CODE code = mcl_json_util_add_string(MCL_NULL, name, value);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_string() with root whose type is object and name = MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_string_002(void)
{
    // Initialize root
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *value = "test_value";

    E_MCL_ERROR_CODE code = mcl_json_util_add_string(root, MCL_NULL, value);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER when root is object and name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_string() with value = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_string_003(void)
{
    // Initialize root
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *name = "test_name";

    E_MCL_ERROR_CODE code = mcl_json_util_add_string(root, name, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for value = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add a string to root with mcl_json_util_add_string().
 * THEN  : User expects to see a string in root.
 */
void test_add_string_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *name = "test_name";
    char *value = "test_value";

    E_MCL_ERROR_CODE code = mcl_json_util_add_string(root, name, value);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test_name\":\"test_value\"}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_string() with root whose type is array and name is not MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_string_005(void)
{
    // Initialize root
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *name = "test_name";
    char *value = "test_value";

    E_MCL_ERROR_CODE code = mcl_json_util_add_string(root, name, value);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER when root is array and name is not NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add two strings to root array with mcl_json_util_add_string().
 * THEN  : User expects to see these two strings in root array.
 */
void test_add_string_006(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *value_1 = "test_value_1";
    char *value_2 = "test_value_2";

    E_MCL_ERROR_CODE code = mcl_json_util_add_string(root, MCL_NULL, value_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    code = mcl_json_util_add_string(root, MCL_NULL, value_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "[\"test_value_1\",\"test_value_2\"]";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    mcl_json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_uint() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_uint_001(void)
{
    char *object_name = "test object name";
    mcl_size_t number = 2;

    E_MCL_ERROR_CODE code = mcl_json_util_add_uint(MCL_NULL, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_uint() with root whose type is object and name = MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_uint_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    mcl_size_t number = 2;

    E_MCL_ERROR_CODE code = mcl_json_util_add_uint(root, MCL_NULL, number);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_uint() with valid parameters.
 * THEN  : User expects to see the added integer in root json.
 */
void test_add_uint_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";
    mcl_size_t number = 2;

    E_MCL_ERROR_CODE code = mcl_json_util_add_uint(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding integer to root failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":2}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_uint() with root whose type is array and name is not MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_uint_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *object_name = "test object name";
    mcl_size_t number = 2;

    E_MCL_ERROR_CODE code = mcl_json_util_add_uint(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add two integers to root array with mcl_json_util_add_string().
 * THEN  : User expects to see these two added integer in root array.
 */
void test_add_uint_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    mcl_size_t number_1 = 1;
    mcl_size_t number_2 = 2;


    E_MCL_ERROR_CODE code = mcl_json_util_add_uint(root, MCL_NULL, number_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding 1. integer to root failed.");

    code = mcl_json_util_add_uint(root, MCL_NULL, number_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding 2. integer to root failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "[1,2]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_float() with root = MCL_NULL.
 * THEN  : It returns MCL_OPERATION_IS_NOT_SUPPORTED.
 */
void test_add_float_001(void)
{
    char *object_name = "test object name";
    float number = 2.3;

    E_MCL_ERROR_CODE code = mcl_json_util_add_float(MCL_NULL, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OPERATION_IS_NOT_SUPPORTED == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_float() with object_name = MCL_NULL.
 * THEN  : It returns MCL_OPERATION_IS_NOT_SUPPORTED.
 */
void test_add_float_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    float number = 2.3;

    E_MCL_ERROR_CODE code = mcl_json_util_add_float(root, MCL_NULL, number);
    TEST_ASSERT_MESSAGE(MCL_OPERATION_IS_NOT_SUPPORTED == code, "It should have returned MCL_TRIGGERED_WITH_NULL for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_float() with valid parameters.
 * THEN  : It returns MCL_OPERATION_IS_NOT_SUPPORTED.
 */
void test_add_float_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";
    float number = 2.3;

    E_MCL_ERROR_CODE code = mcl_json_util_add_float(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OPERATION_IS_NOT_SUPPORTED == code, "Return code should have been MCL_OPERATION_IS_NOT_SUPPORTED.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_double() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_double_001(void)
{
    char *object_name = "test object name";
    double number = 2.3;

    E_MCL_ERROR_CODE code = mcl_json_util_add_double(MCL_NULL, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_double() with root whose type is object and name = MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_double_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    double number = 2.3;

    E_MCL_ERROR_CODE code = mcl_json_util_add_double(root, MCL_NULL, number);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_double() with valid parameters.
 * THEN  : User expects to see the added double in root json.
 */
void test_add_double_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";
    double number = 2.3;

    E_MCL_ERROR_CODE code = mcl_json_util_add_double(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding double to root failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // We added 2.3 but cJson adds as 2.300000. So thats why we compare with 2.300000.
    char *expected_json_root = "{\"test object name\":2.300000}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls mcl_json_util_add_double() with root whose type is array and name is not null.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_double_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *object_name = "test object name";
    double number = 2.3;

    E_MCL_ERROR_CODE code = mcl_json_util_add_double(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is not NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add two doubles to root array with mcl_json_util_add_double().
 * THEN  : User expects to see these two added doubles in root json array.
 */
void test_add_double_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    double number_1 = 2.3;
    double number_2 = 4.3;

    E_MCL_ERROR_CODE code = mcl_json_util_add_double(root, MCL_NULL, number_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding double to root failed.");

    code = mcl_json_util_add_double(root, MCL_NULL, number_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding double to root failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    // We added 2.3 but cJson adds as 2.300000. So thats why we compare with 2.300000.
    char *expected_json_root = "[2.300000,4.300000]";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_bool() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_bool_001(void)
{
    char *object_name = "test object name";
    mcl_bool_t bool_value = MCL_TRUE;

    E_MCL_ERROR_CODE code = mcl_json_util_add_bool(MCL_NULL, object_name, bool_value);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_bool() with root whose type is object and name = MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_bool_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    mcl_bool_t bool_value = MCL_TRUE;

    E_MCL_ERROR_CODE code = mcl_json_util_add_bool(root, MCL_NULL, bool_value);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is NULL.");

    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_bool() with bool_value = MCL_TRUE.
 * THEN  : User expects to see the bool value in the root.
 */
void test_add_bool_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";
    mcl_bool_t bool_value = MCL_TRUE;

    E_MCL_ERROR_CODE code = mcl_json_util_add_bool(root, object_name, bool_value);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_bool() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":true}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_bool() with root whose type is array and name is not null.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_bool_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *object_name = "test object name";
    mcl_bool_t bool_value = MCL_TRUE;

    E_MCL_ERROR_CODE code = mcl_json_util_add_bool(root, object_name, bool_value);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is not NULL.");

    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User requests to add two bools to root array with mcl_json_util_add_bool().
 * THEN  : User expects to see these two bool values in the root array.
 */
void test_add_bool_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    mcl_bool_t bool_value_1 = MCL_TRUE;
    mcl_bool_t bool_value_2 = MCL_FALSE;


    E_MCL_ERROR_CODE code = mcl_json_util_add_bool(root, MCL_NULL, bool_value_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_bool() failed.");

    code = mcl_json_util_add_bool(root, MCL_NULL, bool_value_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_bool() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "[true,false]";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_null() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_null_001(void)
{
    char *object_name = "test object name";

    E_MCL_ERROR_CODE code = mcl_json_util_add_null(MCL_NULL, object_name);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_null() with root whose type is object and name = MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_null_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    E_MCL_ERROR_CODE code = mcl_json_util_add_null(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is NULL.");

    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_null() with valid parameters.
 * THEN  : User expects to see the null value in the root.
 */
void test_add_null_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";

    E_MCL_ERROR_CODE code = mcl_json_util_add_null(root, object_name);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_null() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":null}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_null() with root whose type is array and name is not MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_null_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    char *object_name = "test object name";

    E_MCL_ERROR_CODE code = mcl_json_util_add_null(root, object_name);
    TEST_ASSERT_MESSAGE(MCL_INVALID_PARAMETER == code, "It should have returned MCL_INVALID_PARAMETER for object_name is not NULL.");

    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_null() with root whose type is object and name = MCL_NULL.
 * THEN  : User expects to see MCL_INVALID_PARAMETER as error code.
 */
void test_add_null_005(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &root);

    E_MCL_ERROR_CODE code = mcl_json_util_add_null(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_null() failed.");

    code = mcl_json_util_add_null(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_null() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "[null,null]";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_object() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_object_001(void)
{
    // Initialize object to add to root.
    mcl_json_t *object = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &object);

    char *object_name = "test object name";

    E_MCL_ERROR_CODE code = mcl_json_util_add_object(MCL_NULL, object_name, object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");

    // Clean up.
    mcl_json_util_destroy(&object);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_object() with object_name = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_object_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Initialize object to add to root.
    mcl_json_t *object = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &object);

    E_MCL_ERROR_CODE code = mcl_json_util_add_object(root, MCL_NULL, object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for object_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
    mcl_json_util_destroy(&object);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_object() with object = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_object_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *object_name = "test object name";

    E_MCL_ERROR_CODE code = mcl_json_util_add_object(root, object_name, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for object = NULL.");

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_object().
 * THEN  : User expects to see the object which is added in the root object.
 */
void test_add_object_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Initialize object to add to root.
    mcl_json_t *object = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &object);

    char *object_name = "test object name";

    E_MCL_ERROR_CODE code = mcl_json_util_add_object(root, object_name, object);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_object() failed.");

    // finish object.
    mcl_json_util_finish_object(&object);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    mcl_json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":{}}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_item_to_array() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_item_to_array_001(void)
{
    // Initialize object to add to root.
    mcl_json_t *object = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &object);

    E_MCL_ERROR_CODE code = mcl_json_util_add_item_to_array(MCL_NULL, object);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_json_util_add_item_to_array() should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");

    // Clean up.
    mcl_json_util_destroy(&object);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_item_to_array() with object = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_item_to_array_002(void)
{
    // Initialize array.
    mcl_json_t *array = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_ARRAY, &array);

    E_MCL_ERROR_CODE code = mcl_json_util_add_item_to_array(array, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "mcl_json_util_add_item_to_array() should have returned MCL_TRIGGERED_WITH_NULL for object = NULL.");

    // Clean up.
    mcl_json_util_destroy(&array);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_add_item_to_array().
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_add_item_to_array_003(void)
{
    // Initialize object_1.
    json_t *object_1 = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &object_1);

    // Initialize object_2.
    json_t *object_2 = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &object_2);

    // Initialize array.
    json_t *array = MCL_NULL;
    json_util_initialize(JSON_ARRAY, &array);

    json_util_add_string(object_1, "name_1", "value_1");

    json_util_add_string(object_2, "name_2", "value_2");

    E_MCL_ERROR_CODE code = mcl_json_util_add_item_to_array(array, object_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_item_to_array() failed.");

    code = mcl_json_util_add_item_to_array(array, object_2);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_item_to_array() failed.");

    // finish object.
    json_util_finish_object(&object_1);
    json_util_finish_object(&object_2);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(array, &json_root);

    char *expected_json_root = "[{\"name_1\":\"value_1\"},{\"name_2\":\"value_2\"}]";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    json_util_destroy(&array);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_object_item() with json_parent = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_object_item_001(void)
{
    char *object_name = "test_name";

    mcl_json_t *json_child = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_get_object_item(MCL_NULL, object_name, &json_child);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_parent = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_object_item() with child_name = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_object_item_002(void)
{
    // Initialize json_parent.
    mcl_json_t *json_parent = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json_parent);

    mcl_json_t *json_child = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_get_object_item(json_parent, MCL_NULL, &json_child);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for child_name = NULL.");

    // Clean up.
    mcl_json_util_destroy(&json_parent);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_object_item() with json_child = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_object_item_003(void)
{
    // Initialize json_parent.
    mcl_json_t *json_parent = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json_parent);

    // Add string object to json parent.
    char *object_name = "test_name";

    E_MCL_ERROR_CODE code = mcl_json_util_get_object_item(json_parent, object_name, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_child = NULL.");

    // Clean up.
    mcl_json_util_destroy(&json_parent);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_object_item().
 * THEN  : User expects to see the object which is added in the root object.
 */
void test_get_object_item_004(void)
{
    // Initialize json_parent.
    mcl_json_t *json_parent = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json_parent);

    // Add string object to json parent.
    char *object_name = "test_name";
    mcl_json_util_add_string(json_parent, object_name, "test_value");

    mcl_json_t *json_child = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_get_object_item(json_parent, object_name, &json_child);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_object_item() failed.");

    // Convert root json object to string.
    char *json_child_string = MCL_NULL;
    mcl_json_util_to_string(json_child, &json_child_string);

    char *expected_json_root = "\"test_value\"";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_child_string, "json_child_string fail.");

    // Clean up.
    MCL_FREE(json_child);
    MCL_FREE(json_child_string);
    mcl_json_util_destroy(&json_parent);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_has_child() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_has_child_001(void)
{
    mcl_bool_t result = MCL_FALSE;

    E_MCL_ERROR_CODE code = mcl_json_util_has_child(MCL_NULL, &result);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_has_child() with result = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_has_child_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    E_MCL_ERROR_CODE code = mcl_json_util_has_child(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for result = NULL.");

    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_has_child() with root which has child.
 * THEN  : User expects to see result = true.
 */
void test_has_child_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add string object to root.
    char *object_name = "test_name";
    mcl_json_util_add_string(root, object_name, "test_value");

    mcl_bool_t result = MCL_FALSE;

    E_MCL_ERROR_CODE code = mcl_json_util_has_child(root, &result);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_has_child() failed.");
    TEST_ASSERT_MESSAGE(MCL_TRUE == result, "Result is wrong.");

    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_has_child() with root which does not have any child.
 * THEN  : User expects to see result = false.
 */
void test_has_child_004(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    mcl_bool_t result = MCL_FALSE;

    E_MCL_ERROR_CODE code = mcl_json_util_has_child(root, &result);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_has_child() failed.");
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "Result is wrong.");

    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_number_value() with json_item = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_number_value_001(void)
{
    // get the number value.
    mcl_int32_t number_value_to_be_received = 0;
    E_MCL_ERROR_CODE code = mcl_json_util_get_number_value(MCL_NULL, &number_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_item = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_number_value() with number_value = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_number_value_002(void)
{
    mcl_json_t json_child;

    // get the number value.
    E_MCL_ERROR_CODE code = mcl_json_util_get_number_value(&json_child, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for number_value = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_number_value() to get the number value.
 * THEN  : User expects to see the number value of the json item.
 */
void test_get_number_value_003(void)
{
    // Initialize json.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add number value to json.
    char *object_name = "number value";
    mcl_size_t number_value_to_be_added = 5;
    mcl_json_util_add_uint(json, object_name, number_value_to_be_added);

    // Get the number added child object of json.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // get the number value.
    mcl_int32_t number_value_to_be_received = 0;
    E_MCL_ERROR_CODE code = mcl_json_util_get_number_value(json_child, &number_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_number_value() failed.");

    TEST_ASSERT_EQUAL_INT_MESSAGE(number_value_to_be_added, number_value_to_be_received, "Wrong number value received.");

    MCL_FREE(json_child);
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_double_value() with json_item = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_double_value_001(void)
{
    // get the double value.
    double double_value_to_be_received = 0;
    E_MCL_ERROR_CODE code = mcl_json_util_get_double_value(MCL_NULL, &double_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_item = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_double_value() with double_value = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_double_value_002(void)
{
    mcl_json_t json_child;

    // get the double value.
    E_MCL_ERROR_CODE code = mcl_json_util_get_double_value(&json_child, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for double_value = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_double_value() to get the double_value.
 * THEN  : User expects to see the double_value of the json item.
 */
void test_get_double_value_003(void)
{
    // Initialize json.
    mcl_json_t *json = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &json);

    // Add double value to json.
    char *object_name = "double value";
    double double_value_to_be_added = 3.1;
    mcl_json_util_add_double(json, object_name, double_value_to_be_added);

    // Get the double value added child object of json.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(json, object_name, &json_child);

    // get the double value.
    double double_value_to_be_received = 0;
    E_MCL_ERROR_CODE code = mcl_json_util_get_double_value(json_child, &double_value_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_double_value() failed.");

    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(double_value_to_be_added, double_value_to_be_received, "Wrong double value received.");

    MCL_FREE(json_child);
    mcl_json_util_destroy(&json);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_string() with json_item = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_string_001(void)
{
    char *string_to_be_received = MCL_NULL;

    // get the string.
    E_MCL_ERROR_CODE code = mcl_json_util_get_string(MCL_NULL, &string_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_item = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_string() with string_value = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_get_string_002(void)
{
    mcl_json_t json_child;

    // get the string.
    E_MCL_ERROR_CODE code = mcl_json_util_get_string(&json_child, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for string_value = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_get_string() to get the string_value.
 * THEN  : User expects to see the string_value of the json item.
 */
void test_get_string_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add string to root.
    char *name = "test_name";
    char *string_to_be_added = "test_value";

    E_MCL_ERROR_CODE code = mcl_json_util_add_string(root, name, string_to_be_added);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    // Get the string added child object of json.
    mcl_json_t *json_child = MCL_NULL;
    mcl_json_util_get_object_item(root, name, &json_child);

    char *string_to_be_received = MCL_NULL;

    // get the string.
    code = mcl_json_util_get_string(json_child, &string_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_get_string() failed.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(string_to_be_added, string_to_be_received, "Wrong string received.");

    MCL_FREE(json_child);
    MCL_FREE(string_to_be_received);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_to_string() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_to_string_001(void)
{
    char *json_string = MCL_NULL;

    E_MCL_ERROR_CODE code = mcl_json_util_to_string(MCL_NULL, &json_string);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_to_string() with json_string = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_to_string_002(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    E_MCL_ERROR_CODE code = mcl_json_util_to_string(root, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_string = NULL.");

    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_to_string() to convert root json object to string.
 * THEN  : User expects to see the json formatted string of the json item.
 */
void test_to_string_003(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Add string to root.
    char *name = "test_name";
    char *value = "test_value";

    E_MCL_ERROR_CODE code = mcl_json_util_add_string(root, name, value);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_add_string() failed.");

    char *object_name = "test_number";
    mcl_size_t number = 2;

    code = mcl_json_util_add_uint(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "Adding integer to root failed.");

    char *json_string = MCL_NULL;
    code = mcl_json_util_to_string(root, &json_string);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_to_string() failed.");

    char *expected_json_root = "{\"test_name\":\"test_value\",\"test_number\":2}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_string, "Wrong string received.");

    MCL_FREE(json_string);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_parse() with json_string = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_parse_001(void)
{
    mcl_json_t *root = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_parse(MCL_NULL, &root);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for json_string = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_parse() with root = MCL_NULL.
 * THEN  : User expects to see MCL_TRIGGERED_WITH_NULL as error code.
 */
void test_parse_002(void)
{
    char *json_to_be_parsed = "{\"type\":\"item\"}";

    E_MCL_ERROR_CODE code = mcl_json_util_parse(json_to_be_parsed, MCL_NULL);
    TEST_ASSERT_MESSAGE(MCL_TRIGGERED_WITH_NULL == code, "It should have returned MCL_TRIGGERED_WITH_NULL for root = NULL.");
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls mcl_json_util_parse() to convert json formatted string to root json object.
 * THEN  : User expects to see the json formatted string of the json item.
 */
void test_parse_003(void)
{
    char *json_to_be_parsed =
        "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"standardTimeSeries\",\"version\":\"1.0\",\"details\":{\"configurationId\":\"e3217e2b-7036-49f2-9814-4c38542cd781\"}}}";

    mcl_json_t *root = MCL_NULL;
    E_MCL_ERROR_CODE code = mcl_json_util_parse(json_to_be_parsed, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "mcl_json_util_parse() failed.");

    // get payload object.
    mcl_json_t *json_child_1 = MCL_NULL;
    mcl_json_util_get_object_item(root, "payload", &json_child_1);

    // get type object of payload.
    mcl_json_t *json_child_2 = MCL_NULL;
    mcl_json_util_get_object_item(json_child_1, "type", &json_child_2);

    // json_child_2 => "type\":"standardTimeSeries"
    char *string_to_be_received = MCL_NULL;
    mcl_json_util_get_string(json_child_2, &string_to_be_received);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("standardTimeSeries", string_to_be_received, "received json string fail.");

    // Clean up.
    MCL_FREE(json_child_1);
    MCL_FREE(json_child_2);
    MCL_FREE(string_to_be_received);
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to finish array in root with mcl_json_util_finish_array().
 * THEN  : User expects to see json_array destroyed. The array still exists in root. Only json_array struct freed.
 */
void test_finish_array_001(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start array in root.
    char *array_name = "test array name";
    mcl_json_t *json_array = MCL_NULL;
    mcl_json_util_start_array(root, array_name, &json_array);

    mcl_json_util_finish_array(&json_array);

    // Check json_array. It should be NULL.
    TEST_ASSERT_NULL_MESSAGE(json_array, "json_array should have been NULL.")

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to finish object in root with mcl_json_util_finish_object().
 * THEN  : User expects to see json_object destroyed. The object exists in root. Only json_object struct freed.
 */
void test_finish_object_001(void)
{
    // Initialize root.
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    // Start object in root.
    char *object_name = "test object name";
    mcl_json_t *json_object = MCL_NULL;
    mcl_json_util_start_object(root, object_name, &json_object);

    mcl_json_util_finish_object(&json_object);

    // Check json_object. It should be NULL.
    TEST_ASSERT_NULL_MESSAGE(json_object, "json_object should have been NULL.")

    // Clean up.
    mcl_json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to destroy root with mcl_json_util_destroy().
 * THEN  : User expects to see root destroyed.
 */
void test_destroy_001(void)
{
    // Initialize root
    mcl_json_t *root = MCL_NULL;
    mcl_json_util_initialize(MCL_JSON_OBJECT, &root);

    char *name = "test_name";
    char *value = "test_value";
    mcl_json_util_add_string(root, name, value);

    mcl_json_util_destroy(&root);

    // Check root. It should be NULL.
    TEST_ASSERT_NULL_MESSAGE(root, "root should have been NULL.")
}
