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
    json_t *root = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_initialize(JSON_OBJECT, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_initialize() failed.");

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    json_util_destroy(&root);
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
    json_t *root = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_initialize(JSON_ARRAY, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_initialize() failed.");

    // Convert root json array to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "[]";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to start an array in root with json_util_start_array().
 * THEN  : User expects to see an array object in root.
 */
void test_start_array_001(void)
{
    // Initialize root
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Start array in root.
    char *array_name = "test array name";
    json_t *json_array = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_start_array(root, array_name, &json_array);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_start_array() failed.");

    // finish array.
    json_util_finish_array(&json_array);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test array name\":[]}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to start an array whose name is already used in the root.
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_start_array_002(void)
{
    // Initialize root
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *array_name = "test array name";
    char *value = "test_value";

    json_util_add_string(root, array_name, value);

    // Start array in root.
    json_t *json_array = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_start_array(root, array_name, &json_array);
    TEST_ASSERT_MESSAGE(MCL_JSON_NAME_DUPLICATION == code, "json_util_start_array() failed.");

    // finish array.
    json_util_finish_array(&json_array);

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to start an object in root with json_util_start_object().
 * THEN  : User expects to see an object in root.
 */
void test_start_object_001(void)
{
    // Initialize root
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Start object in root.
    char *object_name = "test object name";
    json_t *json_object = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_start_object(root, object_name, &json_object);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_start_object() failed.");

    // finish object.
    json_util_finish_object(&json_object);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":{}}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to start an object whose name is already used in the root.
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_start_object_002(void)
{
    // Initialize root
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";
    char *value = "test_value";

    json_util_add_string(root, object_name, value);

    // Start object in root.
    json_t *json_object = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_start_object(root, object_name, &json_object);
    TEST_ASSERT_MESSAGE(MCL_JSON_NAME_DUPLICATION == code, "json_util_start_object() failed.");

    // finish object.
    json_util_finish_object(&json_object);

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add a string to root with json_util_add_string().
 * THEN  : User expects to see a string in root.
 */
void test_add_string_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *name = "test_name";
    char *value = "test_value";

    json_util_add_string(root, name, value);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test_name\":\"test_value\"}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    json_util_destroy(&root);
    MCL_FREE(json_root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add a string whose name is already used in the root.
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_add_string_002(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *name = "test_name";
    char *value_1 = "test_value_1";
    char *value_2 = "test_value_2";

    E_MCL_ERROR_CODE code = json_util_add_string(root, name, value_1);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_add_string() failed.");

    // add the same name twice to get the name duplication error.
    code = json_util_add_string(root, name, value_2);
    TEST_ASSERT_MESSAGE(MCL_JSON_NAME_DUPLICATION == code, "json_util_add_string() failed for name duplication. ");

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls json_util_add_uint() with valid parameters.
 * THEN  : User expects to see the added integer in root json.
 */
void test_add_uint_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";
    mcl_size_t number = 2;

    json_util_add_uint(root, object_name, number);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":2}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add a uint whose name is already used in the root.
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_add_uint_002(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";
    mcl_size_t number = 2;

    E_MCL_ERROR_CODE code = json_util_add_uint(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_add_uint() failed.");

    // add the same name twice to get the name duplication error.
    code = json_util_add_uint(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_JSON_NAME_DUPLICATION == code, "json_util_add_uint() failed for name duplication.");

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls json_util_add_float() with valid parameters.
 * THEN  : It returns MCL_OPERATION_IS_NOT_SUPPORTED.
 */
void test_add_float_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";
    float number = 2.3;

    E_MCL_ERROR_CODE code = json_util_add_float(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OPERATION_IS_NOT_SUPPORTED == code, "Return code should have been MCL_OPERATION_IS_NOT_SUPPORTED.");

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls json_util_add_double() with valid parameters.
 * THEN  : User expects to see the added double in root json.
 */
void test_add_double_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";
    double number = 2.3;

    json_util_add_double(root, object_name, number);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    // We added 2.3 but cJson adds as 2.300000. So thats why we compare with 2.300000.
    char *expected_json_root = "{\"test object name\":2.300000}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add a double whose name is already used in the root.
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_add_double_002(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";
    double number = 2.3;

    E_MCL_ERROR_CODE code = json_util_add_double(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_add_double() failed.");

    // add the same name twice to get the name duplication error.
    code = json_util_add_double(root, object_name, number);
    TEST_ASSERT_MESSAGE(MCL_JSON_NAME_DUPLICATION == code, "json_util_add_double() failed for name duplication.");

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_add_bool() with bool_value = MCL_TRUE.
 * THEN  : User expects to see the bool value in the root.
 */
void test_add_bool_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";
    mcl_bool_t bool_value = MCL_TRUE;

    json_util_add_bool(root, object_name, bool_value);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":true}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add a bool whose name is already used in the root.
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_add_bool_002(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";
    mcl_bool_t bool_value = MCL_TRUE;

    E_MCL_ERROR_CODE code = json_util_add_bool(root, object_name, bool_value);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_add_bool() failed.");

    // add the same name twice to get the name duplication error.
    code = json_util_add_bool(root, object_name, bool_value);
    TEST_ASSERT_MESSAGE(MCL_JSON_NAME_DUPLICATION == code, "json_util_add_bool() failed for name duplication.");

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_add_null() with valid parameters.
 * THEN  : User expects to see the null value in the root.
 */
void test_add_null_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";

    json_util_add_null(root, object_name);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":null}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add a null whose name is already used in the root.
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_add_null_002(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *object_name = "test object name";

    E_MCL_ERROR_CODE code = json_util_add_null(root, object_name);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_add_null() failed.");

    code = json_util_add_null(root, object_name);
    TEST_ASSERT_MESSAGE(MCL_JSON_NAME_DUPLICATION == code, "json_util_add_null() failed for name duplication.");

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_add_object().
 * THEN  : User expects to see the object which is added in the root object.
 */
void test_add_object_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Initialize object to add to root.
    json_t *object = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &object);

    char *object_name = "test object name";

    json_util_add_object(root, object_name, object);

    // finish object.
    json_util_finish_object(&object);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":{}}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to add an object whose name is already used in the root..
 * THEN  : User expects to get the error code of MCL_JSON_NAME_DUPLICATION.
 */
void test_add_object_002(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Initialize object to add to root.
    json_t *object = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &object);

    char *object_name = "test object name";

    json_util_add_object(root, object_name, object);

    // finish object.
    json_util_finish_object(&object);

    // Convert root json object to string.
    char *json_root = MCL_NULL;
    json_util_to_string(root, &json_root);

    char *expected_json_root = "{\"test object name\":{}}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root, "json_root fail.");

    // Clean up.
    MCL_FREE(json_root);
    json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_add_item_to_array().
 * THEN  : User expects to see the objects which is added in the root array.
 */
void test_add_item_to_array_001(void)
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

    json_util_add_item_to_array(array, object_1);

    json_util_add_item_to_array(array, object_2);

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
 * WHEN  : User calls json_util_get_object_item().
 * THEN  : User expects to see the object which is added in the root object.
 */
void test_get_object_item_001(void)
{
    // Initialize json_parent.
    json_t *json_parent = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &json_parent);

    // Add string object to json parent.
    char *object_name = "test_name";
    json_util_add_string(json_parent, object_name, "test_value");

    json_t *json_child = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_get_object_item(json_parent, object_name, &json_child);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_get_object_item() failed.");

    // Convert root json object to string.
    char *json_child_string = MCL_NULL;
    json_util_to_string(json_child, &json_child_string);

    char *expected_json_root = "\"test_value\"";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_child_string, "json_child_string fail.");

    // Clean up.
    MCL_FREE(json_child);
    MCL_FREE(json_child_string);
    json_util_destroy(&json_parent);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_has_child() with root which has child.
 * THEN  : User expects to see result = true.
 */
void test_has_child_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Add string object to root.
    char *object_name = "test_name";
    json_util_add_string(root, object_name, "test_value");

    mcl_bool_t result = json_util_has_child(root);

    TEST_ASSERT_MESSAGE(MCL_TRUE == result, "Result is wrong.");

    json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_has_child() with root which does not have any child.
 * THEN  : User expects to see result = false.
 */
void test_has_child_002(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    mcl_bool_t result = json_util_has_child(root);
    TEST_ASSERT_MESSAGE(MCL_FALSE == result, "Result is wrong.");

    json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_get_number_value() to get the number value.
 * THEN  : User expects to see the number value of the json item.
 */
void test_get_number_value_001(void)
{
    // Initialize json.
    json_t *json = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &json);

    // Add number value to json.
    char *object_name = "number value";
    mcl_size_t number_value_to_be_added = 5;
    json_util_add_uint(json, object_name, number_value_to_be_added);

    // Get the number added child object of json.
    json_t *json_child = MCL_NULL;
    json_util_get_object_item(json, object_name, &json_child);

    // get the number value.
    mcl_int32_t number_value_to_be_received = 0;
    json_util_get_number_value(json_child, &number_value_to_be_received);

    TEST_ASSERT_EQUAL_INT_MESSAGE(number_value_to_be_added, number_value_to_be_received, "Wrong number value received.");

    MCL_FREE(json_child);
    json_util_destroy(&json);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_get_double_value() to get the double_value.
 * THEN  : User expects to see the double_value of the json item.
 */
void test_get_double_value_001(void)
{
    // Initialize json.
    json_t *json = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &json);

    // Add double value to json.
    char *object_name = "double value";
    double double_value_to_be_added = 3.1;
    json_util_add_double(json, object_name, double_value_to_be_added);

    // Get the double value added child object of json.
    json_t *json_child = MCL_NULL;
    json_util_get_object_item(json, object_name, &json_child);

    // get the double value.
    double double_value_to_be_received = 0;
    json_util_get_double_value(json_child, &double_value_to_be_received);

    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(double_value_to_be_added, double_value_to_be_received, "Wrong double value received.");

    MCL_FREE(json_child);
    json_util_destroy(&json);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_get_string() to get the string_value.
 * THEN  : User expects to see the string_value of the json item.
 */
void test_get_string_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Add string to root.
    char *name = "test_name";
    char *string_to_be_added = "test_value";

    json_util_add_string(root, name, string_to_be_added);

    // Get the string added child object of json.
    json_t *json_child = MCL_NULL;
    json_util_get_object_item(root, name, &json_child);

    string_t *string_to_be_received = MCL_NULL;

    // get the string.
    E_MCL_ERROR_CODE code = json_util_get_string(json_child, &string_to_be_received);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_get_string() failed.");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(string_to_be_added, string_to_be_received->buffer, "Wrong string received.");

    MCL_FREE(json_child);
    string_destroy(&string_to_be_received);
    json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_to_string() to convert root json object to string.
 * THEN  : User expects to see the json formatted string of the json item.
 */
void test_to_string_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Add string to root.
    char *name = "test_name";
    char *value = "test_value";

    json_util_add_string(root, name, value);

    char *object_name = "test_number";
    mcl_size_t number = 2;

    json_util_add_uint(root, object_name, number);

    char *json_string = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_to_string(root, &json_string);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_to_string() failed.");

    char *expected_json_root = "{\"test_name\":\"test_value\",\"test_number\":2}";
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_string, "Wrong string received.");

    MCL_FREE(json_string);
    json_util_destroy(&root);
}

/**
 * GIVEN : Uninitialized root
 * WHEN  : User calls json_util_parse() to convert json formatted string to root json object.
 * THEN  : User expects to see the json formatted string of the json item.
 */
void test_parse_001(void)
{
    char *json_to_be_parsed =
        "{\"type\":\"item\",\"version\":\"1.0\",\"payload\":{\"type\":\"standardTimeSeries\",\"version\":\"1.0\",\"details\":{\"configurationId\":\"e3217e2b-7036-49f2-9814-4c38542cd781\"}}}";

    json_t *root = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_parse(json_to_be_parsed, &root);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_parse() failed.");

    // get payload object.
    json_t *json_child_1 = MCL_NULL;
    json_util_get_object_item(root, "payload", &json_child_1);

    // get type object of payload.
    json_t *json_child_2 = MCL_NULL;
    json_util_get_object_item(json_child_1, "type", &json_child_2);

    // json_child_2 => "type\":"standardTimeSeries"
    string_t *string_to_be_received = MCL_NULL;
    json_util_get_string(json_child_2, &string_to_be_received);

    TEST_ASSERT_EQUAL_STRING_MESSAGE("standardTimeSeries", string_to_be_received->buffer, "received json string fail.");

    // Clean up.
    MCL_FREE(json_child_1);
    MCL_FREE(json_child_2);
    json_util_destroy(&root);
    string_destroy(&string_to_be_received);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls json_util_duplicate() (argument of with_children = MCL_TRUE) to duplicate json.
 * THEN  : User expects to get a duplicated json with children.
 */
void test_duplicate_001(void)
{
    // Initialize root in the form of <<{"type":"item","details":{"severity":"minor"}}>>.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *name = "type";
    char *value = "item";

    // Add string to root.
    json_util_add_string(root, name, value);

    // Initialize object to add to root.
    json_t *object = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &object);

    char *object_name = "details";

    json_util_add_object(root, object_name, object);

    char *name_2 = "severity";
    char *value_2 = "minor";

    // Add string to object.
    json_util_add_string(object, name_2, value_2);

    // finish object.
    json_util_finish_object(&object);

    // Convert root json object to string.
    char *json_root_string = MCL_NULL;
    json_util_to_string(root, &json_root_string);

    char *expected_json_root = "{\"type\":\"item\",\"details\":{\"severity\":\"minor\"}}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root_string, "json_root fail.");

    // Duplicate root and check if the duplicated json is the same as root.
    json_t *duplicated_json = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_duplicate(root, MCL_TRUE, &duplicated_json);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_duplicate() failed.");

    // Convert duplicated_json object to string.
    char *duplicated_json_string = MCL_NULL;
    json_util_to_string(duplicated_json, &duplicated_json_string);

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, duplicated_json_string, "duplicated_json_string fail.");

    // Clean up.
    MCL_FREE(json_root_string);
    json_util_destroy(&root);
    MCL_FREE(duplicated_json_string);
    json_util_destroy(&duplicated_json);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User calls json_util_duplicate() (argument of with_children = MCL_FALSE) to duplicate json.
 * THEN  : User expects to get a duplicated json without children.
 */
void test_duplicate_002(void)
{
    // Initialize root in the form of <<{"type":"item","details":{"severity":"minor"}}>>.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *name = "type";
    char *value = "item";

    // Add string to root.
    json_util_add_string(root, name, value);

    // Initialize object to add to root.
    json_t *object = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &object);

    char *object_name = "details";

    json_util_add_object(root, object_name, object);

    char *name_2 = "severity";
    char *value_2 = "minor";

    // Add string to object.
    json_util_add_string(object, name_2, value_2);

    // finish object.
    json_util_finish_object(&object);

    // Convert root json object to string.
    char *json_root_string = MCL_NULL;
    json_util_to_string(root, &json_root_string);

    char *expected_json_root = "{\"type\":\"item\",\"details\":{\"severity\":\"minor\"}}";

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_json_root, json_root_string, "json_root fail.");

    // Duplicate root and check if the duplicated json is the same as root.
    json_t *duplicated_json = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_duplicate(root, MCL_FALSE, &duplicated_json);
    TEST_ASSERT_MESSAGE(MCL_OK == code, "json_util_duplicate() failed.");

    // Convert duplicated_json object to string.
    char *duplicated_json_string = MCL_NULL;
    json_util_to_string(duplicated_json, &duplicated_json_string);

    // Without children.
    TEST_ASSERT_EQUAL_STRING_MESSAGE("{}", duplicated_json_string, "duplicated_json_string fail.");

    // Clean up.
    MCL_FREE(json_root_string);
    json_util_destroy(&root);
    json_util_destroy(&duplicated_json);
    MCL_FREE(duplicated_json_string);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to finish array in root with json_util_finish_array().
 * THEN  : User expects to see json_array destroyed. The array still exists in root. Only json_array struct freed.
 */
void test_finish_array_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Start array in root.
    char *array_name = "test array name";
    json_t *json_array = MCL_NULL;
    json_util_start_array(root, array_name, &json_array);

    json_util_finish_array(&json_array);

    // Check json_array. It should be NULL.
    TEST_ASSERT_NULL_MESSAGE(json_array, "json_array should have been NULL.")

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to finish object in root with json_util_finish_object().
 * THEN  : User expects to see json_object destroyed. The object exists in root. Only json_object struct freed.
 */
void test_finish_object_001(void)
{
    // Initialize root.
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    // Start object in root.
    char *object_name = "test object name";
    json_t *json_object = MCL_NULL;
    json_util_start_object(root, object_name, &json_object);

    json_util_finish_object(&json_object);

    // Check json_object. It should be NULL.
    TEST_ASSERT_NULL_MESSAGE(json_object, "json_object should have been NULL.")

    // Clean up.
    json_util_destroy(&root);
}

/**
 * GIVEN : Initialized root.
 * WHEN  : User requests to destroy root with json_util_destroy().
 * THEN  : User expects to see root destroyed.
 */
void test_destroy_001(void)
{
    // Initialize root
    json_t *root = MCL_NULL;
    json_util_initialize(JSON_OBJECT, &root);

    char *name = "test_name";
    char *value = "test_value";
    json_util_add_string(root, name, value); 

    json_util_destroy(&root);

    // Check root. It should be NULL.
    TEST_ASSERT_NULL_MESSAGE(root, "root should have been NULL.")
}

/**
* GIVEN : Valid JSON array.
* WHEN  : User wants to get the size of the array.
* THEN  : Correct value is returned.
*/
void test_get_array_size_001()
{
	char *json_array_string = "[\"one\",\"two\",\"three\"]";
	json_t *json_array = MCL_NULL;
	json_util_parse(json_array_string, &json_array);
	
	size_t array_size = 0;
	json_util_get_array_size(json_array, &array_size);
	TEST_ASSERT_EQUAL(3, array_size);

    json_util_destroy(&json_array);
}

/**
* GIVEN : Valid JSON array.
* WHEN  : User wants to get the first item of the array.
* THEN  : Correct item is returned.
*/
void test_get_array_item_001()
{
	char *json_array_string = "[\"one\",\"two\",\"three\"]";
	json_t *json_array = MCL_NULL;
	json_util_parse(json_array_string, &json_array);

	json_t *item = MCL_NULL;
	json_util_get_array_item(json_array, 0, &item);

	string_t *expected_item_value = MCL_NULL;
	
	json_util_get_string(item, &expected_item_value);
	TEST_ASSERT_EQUAL_STRING("one", expected_item_value->buffer);

    json_util_destroy(&item);
    json_util_destroy(&json_array);
    string_destroy(&expected_item_value);
}

/**
* GIVEN : Valid JSON array.
* WHEN  : User wants to get the second item of the array.
* THEN  : Correct item is returned.
*/
void test_get_array_item_002()
{
	char *json_array_string = "[\"one\",\"two\",\"three\"]";
	json_t *json_array = MCL_NULL;
	json_util_parse(json_array_string, &json_array);

	json_t *item = MCL_NULL;
	json_util_get_array_item(json_array, 1, &item);

	string_t *expected_item_value = MCL_NULL;

	json_util_get_string(item, &expected_item_value);
	TEST_ASSERT_EQUAL_STRING("two", expected_item_value->buffer);

    json_util_destroy(&item);
    json_util_destroy(&json_array);
    string_destroy(&expected_item_value);
}


