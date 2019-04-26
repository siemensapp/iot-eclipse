/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     json_util.c
* @date     Feb 22, 2017
* @brief    Json util module implementation file.
*
************************************************************************/

#include "cJSON/cJSON.h"
#include "json_util.h"
#include "mcl/mcl_json_util.h"
#include "definitions.h"
#include "log_util.h"
#include "memory.h"
#include "string_util.h" 

// Private Function Prototypes:
static void _finish_json_item(json_t **json_item);
static E_JSON_TYPE _convert_mcl_json_type_to_json_type(E_MCL_JSON_TYPE mcl_json_type);

static cJSON_Hooks cjson_hooks;

void json_util_initialize_json_library()
{
    DEBUG_ENTRY("void")

    cjson_hooks.malloc_fn = memory_malloc;
    cjson_hooks.free_fn = memory_free;
    cJSON_InitHooks(&cjson_hooks);

    DEBUG_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE mcl_json_util_initialize(E_MCL_JSON_TYPE json_type, mcl_json_t **root)
{
    DEBUG_ENTRY("E_MCL_JSON_TYPE json_type = <%d>, mcl_json_t **root = <%p>", json_type, root)

	E_MCL_ERROR_CODE code;

	ASSERT_NOT_NULL(root);

	code = json_util_initialize(_convert_mcl_json_type_to_json_type(json_type), root);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_initialize(E_JSON_TYPE json_type, json_t **root)
{
    VERBOSE_ENTRY("E_JSON_TYPE json_type = <%p>, json_t **root = <%p>", &json_type, root)

    MCL_NEW(*root);
    ASSERT_CODE_MESSAGE(MCL_NULL != *root, MCL_OUT_OF_MEMORY, "root couldn't be allocated in memory.");

    (*root)->root_handle = MCL_NULL;

    switch (json_type)
    {
        case JSON_ARRAY :
            (*root)->root_handle = cJSON_CreateArray();

            break;
        case JSON_OBJECT :
            (*root)->root_handle = cJSON_CreateObject();

            break;
        default :
            MCL_VERBOSE("json_type = <%d> is out of valid types.", json_type);

            break;
    }

    ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != (*root)->root_handle, MCL_FREE(*root), MCL_OUT_OF_MEMORY, "root_handle couldn't be allocated in memory.");

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_json_util_start_array(mcl_json_t *root, const char *array_name, mcl_json_t **json_array)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *array_name = <%p>, mcl_json_t **json_array = <%p>", root, array_name, json_array)

	E_MCL_ERROR_CODE code;

	ASSERT_NOT_NULL(root);
	ASSERT_NOT_NULL(array_name);
	ASSERT_NOT_NULL(json_array);

	code = json_util_start_array(root, array_name, json_array);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_start_array(json_t *root, const char *array_name, json_t **json_array)
{
    VERBOSE_ENTRY("json_t *root = <%p>, const char *array_name = <%p>, json_t **json_array = <%p>", root, array_name, json_array)

    json_t *json_child = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_get_object_item(root, array_name, &json_child);
    ASSERT_CODE_MESSAGE(MCL_OUT_OF_MEMORY != code, code, "json_child couldn't be allocated in memory.");
    MCL_FREE(json_child);

    if (MCL_NON_EXISTING_JSON_CHILD == code)
    {
        code = json_util_initialize(JSON_ARRAY, json_array);
        ASSERT_CODE_MESSAGE(MCL_OK == code, code, "json_array couldn't be allocated in memory.");

        cJSON_AddItemToObject(root->root_handle, array_name, (*json_array)->root_handle);
    }
    else
    {
        VERBOSE_LEAVE("retVal = <%d>", MCL_JSON_NAME_DUPLICATION);
        return MCL_JSON_NAME_DUPLICATION;
    }

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_json_util_get_array_item(mcl_json_t *array, int index, mcl_json_t **item)
{
	DEBUG_ENTRY("mcl_json_t *array = <%p>, int index = <%d>, mcl_json_t **item = <%p>", array, index, item)

	E_MCL_ERROR_CODE code;
	
	ASSERT_NOT_NULL(array);
	ASSERT_NOT_NULL(item);

	code = json_util_get_array_item(array, index, item);

	DEBUG_LEAVE("retVal = <%d>", code);
	return code;
}

E_MCL_ERROR_CODE json_util_get_array_item(json_t *array, int index, json_t **item)
{
	VERBOSE_ENTRY("json_t *root = <%p>, int index = <%d>", array, index, item)

	cJSON *item_cjson;

	MCL_NEW(*item);
	ASSERT_CODE_MESSAGE(MCL_NULL != *item, MCL_OUT_OF_MEMORY, "Could not allocate memory for item!");

	(*item)->root_handle = MCL_NULL;

	item_cjson = cJSON_GetArrayItem(array->root_handle, index);
    (*item)->root_handle = cJSON_Duplicate(item_cjson, MCL_TRUE);
	ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != (*item)->root_handle, MCL_FREE(*item), MCL_FAIL, "Could not get item from json array.");

	VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
	return MCL_OK;
}

E_MCL_ERROR_CODE mcl_json_util_get_array_size(mcl_json_t *array, mcl_size_t *size)
{
	DEBUG_ENTRY("mcl_json_t *array = <%p>, int *size = <%p>", array, size)

	ASSERT_NOT_NULL(array);
	ASSERT_NOT_NULL(size);

	json_util_get_array_size(array, size);

	DEBUG_LEAVE("retVal = <%d>", MCL_OK);
	return MCL_OK;
}

void json_util_get_array_size(json_t *array, mcl_size_t *size)
{
	VERBOSE_ENTRY("json_t *array = <%p>, mcl_size_t *size = <%p>", array, size)

	*size = (mcl_size_t)cJSON_GetArraySize(array->root_handle);

	VERBOSE_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE mcl_json_util_start_object(mcl_json_t *root, const char *object_name, mcl_json_t **json_object)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, mcl_json_t **json_object = <%p>", root, object_name, json_object)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(root);
    ASSERT_NOT_NULL(object_name);
    ASSERT_NOT_NULL(json_object);

	code = json_util_start_object(root, object_name, json_object);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_start_object(json_t *root, const char *object_name, json_t **json_object)
{
    VERBOSE_ENTRY("json_t *root = <%p>, const char *object_name = <%p>, json_t **json_object = <%p>", root, object_name, json_object)

    json_t *json_child = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_get_object_item(root, object_name, &json_child);
    ASSERT_CODE_MESSAGE(MCL_OUT_OF_MEMORY != code, code, "json_child couldn't be allocated in memory.");
    MCL_FREE(json_child);

    if (MCL_NON_EXISTING_JSON_CHILD != code)
    {
        VERBOSE_LEAVE("retVal = <%d>", MCL_JSON_NAME_DUPLICATION);
        return MCL_JSON_NAME_DUPLICATION;
    }

    code = json_util_initialize(JSON_OBJECT, json_object);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "json_object couldn't be allocated in memory.");

    cJSON_AddItemToObject(root->root_handle, object_name, (*json_object)->root_handle);

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_json_util_add_string(mcl_json_t *root, const char *object_name, const char *object_value)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const char *object_value = <%p>", root, object_name, object_value)

	E_MCL_ERROR_CODE code;

	ASSERT_NOT_NULL(root);
	ASSERT_NOT_NULL(object_value);

	code = json_util_add_string(root, object_name, object_value);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_add_string(json_t *root, const char *object_name, const char *object_value)
{
    VERBOSE_ENTRY("json_t *root = <%p>, const char *object_name = <%p>, const char *object_value = <%p>", root, object_name, object_value)

    E_MCL_ERROR_CODE code = MCL_OK;

    if (cJSON_Array == root->root_handle->type && MCL_NULL == object_name)
    {
        cJSON *string = cJSON_CreateString(object_value);
        cJSON_AddItemToArray(root->root_handle, string);
    }
    else if (cJSON_Object == root->root_handle->type && MCL_NULL != object_name)
    {
        json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);
        ASSERT_CODE_MESSAGE(MCL_OUT_OF_MEMORY != code, code, "json_child couldn't be allocated in memory.");
        MCL_FREE(json_child);

        // If object_name is already used in the root, free json_child and return error.
        ASSERT_CODE_MESSAGE(MCL_NON_EXISTING_JSON_CHILD == code, MCL_JSON_NAME_DUPLICATION, "Name duplication fail for object_name.");

        // Add string to object.
        cJSON_AddStringToObject(root->root_handle, object_name, object_value);
        code = MCL_OK;
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_json_util_add_uint(mcl_json_t *root, const char *object_name, const mcl_size_t number)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const mcl_size_t number = <%u>", root, object_name, &number)

	E_MCL_ERROR_CODE code;

	ASSERT_NOT_NULL(root);

	code = json_util_add_uint(root, object_name, number);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_add_uint(json_t *root, const char *object_name, const mcl_size_t number)
{
    VERBOSE_ENTRY("json_t *root = <%p>, char *object_name = <%p>, mcl_size_t number = <%u>", root, object_name, number)

    E_MCL_ERROR_CODE code = MCL_OK;

    if (cJSON_Array == root->root_handle->type && MCL_NULL == object_name)
    {
        cJSON *json_number = cJSON_CreateNumber((double)number);
        cJSON_AddItemToArray(root->root_handle, json_number);
    }
    else if (cJSON_Object == root->root_handle->type && MCL_NULL != object_name)
    {
        json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);
        ASSERT_CODE_MESSAGE(MCL_OUT_OF_MEMORY != code, code, "json_child couldn't be allocated in memory.");
        MCL_FREE(json_child);

        // If object_name is already used in the root, free json_child and return error.
        ASSERT_CODE_MESSAGE(MCL_NON_EXISTING_JSON_CHILD == code, MCL_JSON_NAME_DUPLICATION, "Name duplication fail for object_name.");

        // Add uint to object. This function expects a double and type cast to integer inside the function.
        cJSON_AddNumberToObject(root->root_handle, object_name, (double)number);
        code = MCL_OK;
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_json_util_add_float(mcl_json_t *root, const char *object_name, const float number)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const float number = <%p>", root, object_name, &number)

    E_MCL_ERROR_CODE code = json_util_add_float(root, object_name, number);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_add_float(json_t *root, const char *object_name, const float number)
{
    VERBOSE_ENTRY("json_t *root = <%p>, const char *object_name = <%p>, const float number = <%f>", root, object_name, number)

    //after completing the function, name duplication should be checked.

    VERBOSE_LEAVE("retVal = <%d>", MCL_OPERATION_IS_NOT_SUPPORTED);
    return MCL_OPERATION_IS_NOT_SUPPORTED;
}

E_MCL_ERROR_CODE mcl_json_util_add_double(mcl_json_t *root, const char *object_name, const double number)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const double number = <%f>", root, object_name, &number)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(root);

	code = json_util_add_double(root, object_name, number);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_add_double(json_t *root, const char *object_name, const double number)
{
    VERBOSE_ENTRY("json_t *root = <%p>, const char *object_name = <%p>, const double number = <%f>", root, object_name, number)

    E_MCL_ERROR_CODE code = MCL_OK;

    if (cJSON_Array == root->root_handle->type && MCL_NULL == object_name)
    {
        cJSON *json_number = cJSON_CreateNumber(number);
        cJSON_AddItemToArray(root->root_handle, json_number);
    }
    else if (cJSON_Object == root->root_handle->type && MCL_NULL != object_name)
    {
        json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);
        ASSERT_CODE_MESSAGE(MCL_OUT_OF_MEMORY != code, code, "json_child couldn't be allocated in memory.");
        MCL_FREE(json_child);

        // If object_name is already used in the root, free json_child and return error.
        ASSERT_CODE_MESSAGE(MCL_NON_EXISTING_JSON_CHILD == code, MCL_JSON_NAME_DUPLICATION, "Name duplication fail for object_name.");

        // Add double to object.
        cJSON_AddNumberToObject(root->root_handle, object_name, number);
        code = MCL_OK;
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_json_util_add_bool(mcl_json_t *root, const char *object_name, const mcl_bool_t bool_value)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, const mcl_bool_t bool_value = <%d>", root, object_name, &bool_value)

	E_MCL_ERROR_CODE code;

	ASSERT_NOT_NULL(root);

	code = json_util_add_bool(root, object_name, bool_value);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_add_bool(json_t *root, const char *object_name, const mcl_bool_t bool_value)
{
    VERBOSE_ENTRY("json_t *root = <%p>, const char *object_name = <%p>, const mcl_bool_t bool_value = <%d>", root, object_name, bool_value)

    E_MCL_ERROR_CODE code = MCL_OK;

    if (cJSON_Array == root->root_handle->type && MCL_NULL == object_name)
    {
        cJSON *json_bool = cJSON_CreateBool(bool_value);
        cJSON_AddItemToArray(root->root_handle, json_bool);
    }
    else if (cJSON_Object == root->root_handle->type && MCL_NULL != object_name)
    {
        json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);
        ASSERT_CODE_MESSAGE(MCL_OUT_OF_MEMORY != code, code, "json_child couldn't be allocated in memory.");
        MCL_FREE(json_child);

        // If object_name is already used in the root, free json_child and return error.
        ASSERT_CODE_MESSAGE(MCL_NON_EXISTING_JSON_CHILD == code, MCL_JSON_NAME_DUPLICATION, "Name duplication fail for object_name.");

        // Add bool to object.
        cJSON_AddBoolToObject(root->root_handle, object_name, bool_value);
        code = MCL_OK;
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_json_util_add_null(mcl_json_t *root, const char *object_name)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>", root, object_name)

	E_MCL_ERROR_CODE code;
		
	ASSERT_NOT_NULL(root);
	
	code = json_util_add_null(root, object_name);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_add_null(json_t *root, const char *object_name)
{
    VERBOSE_ENTRY("json_t *root = <%p>, const char *object_name = <%p>", root, object_name)

    E_MCL_ERROR_CODE code = MCL_OK;

    if (cJSON_Array == root->root_handle->type && MCL_NULL == object_name)
    {
        cJSON *json_null = cJSON_CreateNull();
        cJSON_AddItemToArray(root->root_handle, json_null);
    }
    else if (cJSON_Object == root->root_handle->type && MCL_NULL != object_name)
    {
        json_t *json_child = MCL_NULL;
        code = json_util_get_object_item(root, object_name, &json_child);
        ASSERT_CODE_MESSAGE(MCL_OUT_OF_MEMORY != code, code, "json_child couldn't be allocated in memory.");
        MCL_FREE(json_child);

        // If object_name is already used in the root, free json_child and return error.
        ASSERT_CODE_MESSAGE(MCL_NON_EXISTING_JSON_CHILD == code, MCL_JSON_NAME_DUPLICATION, "Name duplication fail for object_name.");

        // Add null to object.
        cJSON_AddNullToObject(root->root_handle, object_name);
        code = MCL_OK;
    }
    else
    {
        code = MCL_INVALID_PARAMETER;
    }

    VERBOSE_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE mcl_json_util_add_object(mcl_json_t *root, const char *object_name, mcl_json_t *object)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, const char *object_name = <%p>, mcl_json_t *object = <%p>", root, object_name, object)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(root);
    ASSERT_NOT_NULL(object_name);
    ASSERT_NOT_NULL(object);

	code = json_util_add_object(root, object_name, object);

    DEBUG_LEAVE("retVal = <%d>", code);
    return MCL_OK;
}

E_MCL_ERROR_CODE json_util_add_object(json_t *root, const char *object_name, json_t *object)
{
    VERBOSE_ENTRY("json_t *root = <%p>, const char *object_name = <%p>, json_t *object = <%p>", root, object_name, object)

    json_t *json_child = MCL_NULL;
    E_MCL_ERROR_CODE code = json_util_get_object_item(root, object_name, &json_child);
    ASSERT_CODE_MESSAGE(MCL_OUT_OF_MEMORY != code, code, "json_child couldn't be allocated in memory.");
    MCL_FREE(json_child);

    if (MCL_NON_EXISTING_JSON_CHILD == code)
    {
        cJSON_AddItemToObject(root->root_handle, object_name, object->root_handle);
    }
    else
    {
        VERBOSE_LEAVE("retVal = <%d>", MCL_JSON_NAME_DUPLICATION);
        return MCL_JSON_NAME_DUPLICATION;
    }

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_json_util_add_item_to_array(mcl_json_t *root, mcl_json_t *object)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, mcl_json_t *object = <%p>", root, object)

    ASSERT_NOT_NULL(root);
    ASSERT_NOT_NULL(object);

    json_util_add_item_to_array(root, object);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void json_util_add_item_to_array(json_t *root, json_t *object)
{
    VERBOSE_ENTRY("json_t *root = <%p>, json_t *object = <%p>", root, object)

    cJSON_AddItemToArray(root->root_handle, object->root_handle);

    VERBOSE_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE mcl_json_util_get_object_item(mcl_json_t *json_parent, const char *child_name, mcl_json_t **json_child)
{
    DEBUG_ENTRY("mcl_json_t *json_parent = <%p>, const char *child_name = <%p>, mcl_json_t **json_child = <%p>", json_parent, child_name, json_child)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(json_parent);
    ASSERT_NOT_NULL(child_name);
    ASSERT_NOT_NULL(json_child);

	code = json_util_get_object_item(json_parent, child_name, json_child);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_get_object_item(json_t *json_parent, const char *child_name, json_t **json_child)
{
    VERBOSE_ENTRY("json_t *json_parent = <%p>, const char *child_name = <%p>, json_t **json_child = <%p>", json_parent, child_name, json_child)

    E_MCL_ERROR_CODE return_code = MCL_OK;

    ASSERT_CODE_MESSAGE(MCL_NULL != MCL_NEW(*json_child), MCL_OUT_OF_MEMORY, "json_child couldn't be allocated in memory.");

    // Returns the address of object item to root handle w/o allocation.
    (*json_child)->root_handle = cJSON_GetObjectItem(json_parent->root_handle, child_name);

    // Check if child object doesn't exist.
    if (MCL_NULL == (*json_child)->root_handle)
    {
       MCL_FREE(*json_child);
       return_code = MCL_NON_EXISTING_JSON_CHILD;
    }

    VERBOSE_LEAVE("retVal = <%d>", return_code);
    return return_code;
}

E_MCL_ERROR_CODE mcl_json_util_has_child(mcl_json_t *root, mcl_bool_t *result)
{
    DEBUG_ENTRY("mcl_json_t *root = <%p>, mcl_bool_t *result = <%p>", root, result)

    ASSERT_NOT_NULL(root);
    ASSERT_NOT_NULL(result);

    *result = json_util_has_child(root);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

mcl_bool_t json_util_has_child(json_t *root)
{
    VERBOSE_ENTRY("json_t *root = <%p>", root)

    mcl_bool_t return_value = MCL_FALSE;

    // If root has child make result true.
    if (MCL_NULL != root->root_handle->child)
    {
        return_value = MCL_TRUE;
    }

    VERBOSE_LEAVE("retVal = <%d>", return_value);
    return return_value;
}

E_MCL_ERROR_CODE mcl_json_util_get_number_value(mcl_json_t *json, mcl_int32_t *number_value)
{
    DEBUG_ENTRY("mcl_json_t *json = <%p>, mcl_int32_t *number_value = <%p>", json, number_value)

    ASSERT_NOT_NULL(json);
    ASSERT_NOT_NULL(number_value);

    json_util_get_number_value(json, number_value);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void json_util_get_number_value(json_t *json, mcl_int32_t *number_value)
{
    VERBOSE_ENTRY("json_t *json = <%p>, mcl_int32_t *number_value = <%p>", json, number_value)

    *number_value = json->root_handle->valueint;

    VERBOSE_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE mcl_json_util_get_double_value(mcl_json_t *json, double *double_value)
{
    DEBUG_ENTRY("mcl_json_t *json = <%p>, double *double_value = <%p>", json, double_value)

    ASSERT_NOT_NULL(json);
    ASSERT_NOT_NULL(double_value);

    json_util_get_double_value(json, double_value);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void json_util_get_double_value(json_t *json, double *double_value)
{
    VERBOSE_ENTRY("json_t *json = <%p>, double *double_value = <%p>", json, double_value)

    *double_value = json->root_handle->valuedouble;

    VERBOSE_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE mcl_json_util_get_bool_value(mcl_json_t *json, mcl_bool_t *bool_value)
{
    DEBUG_ENTRY("json_t *json = <%p>, mcl_bool_t *bool_value = <%p>", json, bool_value)

    ASSERT_NOT_NULL(json);
    ASSERT_NOT_NULL(bool_value);

    json_util_get_bool_value(json, bool_value);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void json_util_get_bool_value(json_t *json, mcl_bool_t *bool_value)
{
    VERBOSE_ENTRY("json_t *json = <%p>, mcl_bool_t *bool_value = <%p>", json, bool_value)

    if(cJSON_True == json->root_handle->type)
    {
        *bool_value = MCL_TRUE;
    }
    else
    {
        *bool_value = MCL_FALSE;
    }

    VERBOSE_LEAVE("retVal = void");
}

E_MCL_ERROR_CODE mcl_json_util_get_string(mcl_json_t *json_item, char **string_value)
{
	DEBUG_ENTRY("mcl_json_t *json_item = <%p>, char **string_value = <%p>", json_item, string_value)

	E_MCL_ERROR_CODE code;
	string_t *string_value_local = MCL_NULL;

    ASSERT_NOT_NULL(json_item);
    ASSERT_NOT_NULL(string_value);

    code = json_util_get_string(json_item, &string_value_local);
    ASSERT_CODE_MESSAGE(MCL_OK == code, code, "Memory can not be allocated for string_value_local.");

    *string_value = string_value_local->buffer;

    // Free only the struct, its buffer should stay.
    MCL_FREE(string_value_local);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE json_util_get_string(json_t *json_item, string_t **string_value)
{
    VERBOSE_ENTRY("json_t *json_item = <%p>, char **string_value = <%p>", json_item, string_value)

    ASSERT_CODE_MESSAGE(MCL_OK == string_initialize_new(json_item->root_handle->valuestring, 0, string_value), MCL_OUT_OF_MEMORY,
                        "Memory can not be allocated for string_value.");

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_json_util_to_string(mcl_json_t *root, char **json_string)
{
	DEBUG_ENTRY("mcl_json_t *root = <%p>, char **json_string = <%p>", root, json_string)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(root);
    ASSERT_NOT_NULL(json_string);

    code = json_util_to_string(root, json_string);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_to_string(json_t *root, char **json_string)
{
    VERBOSE_ENTRY("json_t *root = <%p>, char **json_string = <%p>", root, json_string)

    *json_string = cJSON_PrintUnformatted(root->root_handle);
    ASSERT_CODE_MESSAGE(MCL_NULL != *json_string, MCL_FAIL, "Either the given json object is invalid or memory can not be allocated for root.");

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE mcl_json_util_parse(const char *json_string, mcl_json_t **root)
{
	DEBUG_ENTRY("const char *json_string = <%p>, mcl_json_t **root = <%p>", json_string, root)

	E_MCL_ERROR_CODE code;

    ASSERT_NOT_NULL(json_string);
    ASSERT_NOT_NULL(root);

    code = json_util_parse(json_string, root);

    DEBUG_LEAVE("retVal = <%d>", code);
    return code;
}

E_MCL_ERROR_CODE json_util_parse(const char *json_string, json_t **root)
{
    VERBOSE_ENTRY("const char *json_string = <%p>, json_t **root = <%p>", json_string, root)

    ASSERT_CODE_MESSAGE(MCL_NULL != MCL_NEW(*root), MCL_OUT_OF_MEMORY, "Memory can not be allocated for root.");

    (*root)->root_handle = cJSON_Parse(json_string);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != (*root)->root_handle, MCL_FREE(*root), MCL_FAIL, "json string could not be parsed.");

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE json_util_duplicate(const json_t *source_json, mcl_bool_t with_children, json_t **duplicated_json)
{
    VERBOSE_ENTRY("const json_t *source_json = <%p>, mcl_bool_t with_children = <%d>, json_t **duplicated_json = <%p>", source_json, with_children, duplicated_json)
    ASSERT_CODE_MESSAGE(MCL_NULL != MCL_NEW(*duplicated_json), MCL_OUT_OF_MEMORY, "Memory can not be allocated for duplicated_json.");

    (*duplicated_json)->root_handle = MCL_NULL;
    (*duplicated_json)->root_handle = cJSON_Duplicate(source_json->root_handle, with_children);
    ASSERT_STATEMENT_CODE_MESSAGE(MCL_NULL != (*duplicated_json)->root_handle, MCL_FREE(*duplicated_json), MCL_OUT_OF_MEMORY, "cJSON_Duplicate() failed.");

    VERBOSE_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

void mcl_json_util_finish_array(mcl_json_t **json_array)
{
    DEBUG_ENTRY("json_t **json_array = <%p>", json_array)

    json_util_finish_array(json_array);

    DEBUG_LEAVE("retVal = void");
}

void json_util_finish_array(json_t **json_array)
{
    VERBOSE_ENTRY("json_t **json_array = <%p>", json_array)

    _finish_json_item(json_array);

    VERBOSE_LEAVE("retVal = void");
}

void mcl_json_util_finish_object(mcl_json_t **json_object)
{
    DEBUG_ENTRY("json_t **json_object = <%p>", json_object)

    json_util_finish_object(json_object);

    DEBUG_LEAVE("retVal = void");
}

void json_util_finish_object(json_t **json_object)
{
    VERBOSE_ENTRY("json_t **json_object = <%p>", json_object)

    _finish_json_item(json_object);

    VERBOSE_LEAVE("retVal = void");
}

void mcl_json_util_destroy(mcl_json_t **root)
{
    DEBUG_ENTRY("json_t **root = <%p>", root)

    json_util_destroy(root);

    DEBUG_LEAVE("retVal = void");
}

void json_util_destroy(json_t **root)
{
	VERBOSE_ENTRY("json_t **root = <%p>", root)

	if (MCL_NULL != *root)
	{
		cJSON_Delete((*root)->root_handle);
		MCL_FREE(*root);
	}

	VERBOSE_LEAVE("retVal = void");
}

static void _finish_json_item(json_t **json_item)
{
    VERBOSE_ENTRY("json_t **json_item = <%p>", json_item)

    if (MCL_NULL != *json_item)
    {
        MCL_FREE(*json_item);
    }

    VERBOSE_LEAVE("retVal = void");
}

static E_JSON_TYPE _convert_mcl_json_type_to_json_type(E_MCL_JSON_TYPE mcl_json_type)
{
    VERBOSE_ENTRY("E_MCL_JSON_TYPE mcl_json_type = <%d>", mcl_json_type)

    E_JSON_TYPE json_type = JSON_NULL;

    switch (mcl_json_type)
    {
        case MCL_JSON_ARRAY :
            json_type = JSON_ARRAY;

            break;
        case MCL_JSON_OBJECT :
            json_type = JSON_OBJECT;

            break;
        default :
            MCL_VERBOSE("mcl_json_type = <%d> is out of valid types.", mcl_json_type);

            break;
    }

    VERBOSE_LEAVE("retVal = <%d>", json_type);
    return json_type;
}
