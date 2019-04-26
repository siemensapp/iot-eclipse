/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_json_util.h
* @date     Feb 2, 2017
* @brief    Json utility module interface header file.
*
* This module is used for json handling like json parsing and json generating.
*
************************************************************************/

#ifndef MCL_JSON_UTIL_H_
#define MCL_JSON_UTIL_H_

#include "mcl/mcl_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * @brief This struct is used for json handling.
     */
    typedef struct mcl_json_t mcl_json_t;

    /**
     * @brief Json type definitions.
     */
    typedef enum E_MCL_JSON_TYPE
    {
        MCL_JSON_ARRAY,  //!< Json array.
        MCL_JSON_OBJECT, //!< Json object.
    } E_MCL_JSON_TYPE;

    /**
     * @brief This function initializes the given @p root json.
     *
     * @param [in] json_type Type of the json.
     * @param [out] root Root json.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_initialize(E_MCL_JSON_TYPE json_type, mcl_json_t **root);

    /**
     * @brief This function creates an array in @p root.
     *
     * @param [in] root Root json object.
     * @param [in] array_name Name of the array which is going to be created in @p root.
     * @param [out] json_array Json array which is going to be created in @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root, @p array_name or @p json_array is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_JSON_NAME_DUPLICATION if the same @p array_name is already used in @p root as object name.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_start_array(mcl_json_t *root, const char *array_name, mcl_json_t **json_array);

    /**
     * @brief This function gets the item at given index from @p array.
     *
     * @param [in] array Array json object.
     * @param [in] index Index of the item to get from @p array.
     * @param [out] item Result json object item.New memory space will be allocated for this parameter.
     * Ownership passed to caller. Caller must free the space using MCL_FREE().
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_get_array_item(mcl_json_t *array, int index, mcl_json_t **item);

    /**
     * @brief This function returns the size of @p array.
     *
     * @param [in]  array Array json object.
     * @param [out] size Size of @p array.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_get_array_size(mcl_json_t *array, mcl_size_t *size);

    /**
     * @brief This function creates an object in @p root.
     *
     * @param [in] root Root json object.
     * @param [in] object_name Name of the object which is going to be created in @p root.
     * @param [out] json_object Json object which is going to be created in @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root, @p object_name or @p json_object is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_JSON_NAME_DUPLICATION if the same @p object_name is already used in @p root as object name.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_start_object(mcl_json_t *root, const char *object_name, mcl_json_t **json_object);

    /**
     * @brief This function adds string to @p root which can be object or array.
     *
     * @note if @p root is array, @p object_name must be null.
     *
     * @param [in] root Root json. It can be object or array.
     * @param [in] object_name Name of the name/value pair which is going to be added to @p root.
     * @param [in] object_value Value of the name/value pair which is going to be added to @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root or @p object_value is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_JSON_NAME_DUPLICATION if the same @p object_name is already used in @p root as object name.</li>
     * <li>#MCL_INVALID_PARAMETER if @p root type is array and @p object_name is not null, or @p root type is object and @p object_name is null.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_add_string(mcl_json_t *root, const char *object_name, const char *object_value);

    /**
     * @brief This function adds integer number to @p root which can be object or array.
     *
     * @note if @p root is array, @p object_name must be null.
     *
     * @param [in] root Root json. It can be object or array.
     * @param [in] object_name Name of the name/value pair which is going to be added to @p root.
     * @param [in] number Value of the name/value pair which is going to be added to @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root is NULL.</li>
     * <li>#MCL_JSON_NAME_DUPLICATION if the same @p object_name is already used in @p root as object name.</li>
     * <li>#MCL_INVALID_PARAMETER if @p root type is array and @p object_name is not null, or @p root type is object and @p object_name is null.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_add_uint(mcl_json_t *root, const char *object_name, const mcl_size_t number);

    /**
     * @brief This function adds floating number to @p root.
     *
     * @warning If cJson library is used, this function is not supported.
     *
     * @param [in] root Root json object.
     * @param [in] object_name Name of the name/value pair which is going to be added to @p root.
     * @param [in] number Value of the name/value pair which is going to be added to @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root or @p object_name is NULL.</li>
     * <li>#MCL_OPERATION_IS_NOT_SUPPORTED if the json library which is used does not support adding float.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_add_float(mcl_json_t *root, const char *object_name, const float number);

    /**
     * @brief This function adds double number to @p root which can be object or array.
     *
     * @param [in] root Root json. It can be object or array.
     * @param [in] object_name Name of the name/value pair which is going to be added to @p root.
     * @param [in] number Value of the name/value pair which is going to be added to @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root is NULL.</li>
     * <li>#MCL_JSON_NAME_DUPLICATION if the same @p object_name is already used in @p root as object name.</li>
     * <li>#MCL_INVALID_PARAMETER if @p root type is array and @p object_name is not null, or @p root type is object and @p object_name is null.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_add_double(mcl_json_t *root, const char *object_name, const double number);

    /**
     * @brief This function adds @p bool_value to @p root which can be object or array.
     *
     * @param [in] root Root json. It can be object or array.
     * @param [in] object_name Name of the bool value which is going to be added to @p root.
     * @param [in] bool_value Bool value to be added to @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root is NULL.</li>
     * <li>#MCL_JSON_NAME_DUPLICATION if the same @p object_name is already used in @p root as object name.</li>
     * <li>#MCL_INVALID_PARAMETER if @p root type is array and @p object_name is not null, or @p root type is object and @p object_name is null.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_add_bool(mcl_json_t *root, const char *object_name, const mcl_bool_t bool_value);

    /**
     * @brief This function adds null to @p root which can be object or array.
     *
     * @param [in] root Root json. It can be object or array.
     * @param [in] object_name Name of the null value which is going to be added to @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root is NULL.</li>
     * <li>#MCL_JSON_NAME_DUPLICATION if the same @p object_name is already used in @p root as object name.</li>
     * <li>#MCL_INVALID_PARAMETER if @p root type is array and @p object_name is not null, or @p root type is object and @p object_name is null.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_add_null(mcl_json_t *root, const char *object_name);

    /**
     * @brief This function adds @p object to @p root.
     *
     * @param [in] root Root json object.
     * @param [in] object_name Name of @p object which is going to be going to be added to @p root.
     * @param [in] object @p object which is going to be added to @p root.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root, @p object_name or @p object is NULL.</li>
     * <li>#MCL_JSON_NAME_DUPLICATION if the same @p object_name is already used in @p root as object name.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_add_object(mcl_json_t *root, const char *object_name, mcl_json_t *object);

    /**
     * @brief This function adds @p object to @p root array.
     *
     * @param [in] root Root json array.
     * @param [in] object @p object which is going to be added to @p root array.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root, @p object_name or @p object is NULL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_add_item_to_array(mcl_json_t *root, mcl_json_t *object);

    /**
     * @brief This function gives the value of @p json_child object, when the @p child_name in @p json_parent object is given.
     *
     * @param [in] json_parent Root json object of @p json_child.
     * @param [in] child_name Name of the @p json_child object.
     * @param [out] json_child The json object which is going to be received. New memory space will be allocated for this parameter.
     * Ownership passed to caller. Caller must free the space using MCL_FREE().
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p json_parent, @p child_name or @p json_child is NULL.</li>
     * <li>#MCL_NON_EXISTING_JSON_CHILD if @p json_child doesn't exist in @p json_parent.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_get_object_item(mcl_json_t *json_parent, const char *child_name, mcl_json_t **json_child);

    /**
     * @brief This function checks whether root object has child object or not.
     *
     * @param [in] root Json object which is going to be checked whether it has child or not.
     * @param [out] result This bool value is going to be made true, if @ root has child. Otherwise it is false.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root or @p result is NULL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_has_child(mcl_json_t *root, mcl_bool_t *result);

    /**
     * @brief This function gets the number value of a given json object.
     *
     * @param [in] json Root object of @p number_value.
     * @param [in] number_value Value of @p json.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p json or @p number_value is NULL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_get_number_value(mcl_json_t *json, mcl_int32_t *number_value);

    /**
     * @brief This function gets the double value of a given json object.
     *
     * @param [in] json Root object of @p double_value.
     * @param [in] double_value Value of @p json.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p json or @p double_value is NULL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_get_double_value(mcl_json_t *json, double *double_value);
    
    /**
    * @brief This function gets the boolean value of a given json object.
    *
    * @param [in] json Root object of @p bool_value.
    * @param [in] bool_value Boolean value of @p json.
    * @return
    * <ul>
    * <li>#MCL_OK in case of success.</li>
    * <li>#MCL_TRIGGERED_WITH_NULL if provided @p json or @p bool_value is NULL.</li>
    * </ul>
    */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_get_bool_value(mcl_json_t *json, mcl_bool_t *bool_value);

    /**
     * @brief This function gets the string value of a given json object.
     *
     * @param [in] json_item Root object of @p string_value.
     * @param [in] string_value String value of @p json_item. New memory space will be allocated for this parameter. Ownership passed to caller. Caller must free the space.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p json or @p string_value is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_get_string(mcl_json_t *json_item, char **string_value);

    /**
     * @brief This function gives the string of @p root in json format.
     *
     * @param [in] root Root json object.
     * @param [out] json_string The string of @p root in json format. New memory space will be allocated for this parameter. Ownership passed to caller. Caller must free the space.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p root or @p json_string is NULL.</li>
     * <li>#MCL_FAIL either the given json is invalid or memory can not be allocated for root.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_to_string(mcl_json_t *root, char **json_string);

    /**
     * @brief This function parses the given string to the given json object.
     *
     * @param [in] json_string String in json format. New memory space will be allocated for this parameter. Ownership passed to caller. Caller must free the space.
     * @param [out] root  @p json_string is going to be parsed to this root object.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p json_string or @p root is NULL.</li>
     * <li>#MCL_FAIL If given json has invalid format.</li>
     * <li>#MCL_OUT_OF_MEMORY If memory cannot be allocated for root.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_json_util_parse(const char *json_string, mcl_json_t **root);

    /**
     * @brief This function destroys @p json_array data struct. But the array still exists in root json object.
     *
     * @note To remove @p json_array from root, #mcl_json_util_destroy must be called for root json object.
     *
     * @param [out] json_array Json array which exists in @p root.
     */
    extern MCL_EXPORT void mcl_json_util_finish_array(mcl_json_t **json_array);

    /**
     * @brief This function destroys @p json_object data struct. But the object still exists in root json object.
     *
     * @note To remove @p json_object from root, #mcl_json_util_destroy must be called for root json object.
     *
     * @param [out] json_object Json object which exists in @p root.
     */
    extern MCL_EXPORT void mcl_json_util_finish_object(mcl_json_t **json_object);

    /**
     * @brief This function destroys @p root.
     *
     * @note If @p root is destroyed, all child json objects in @p root are also destroyed.
     *
     * @param [out] root Root json object.
     */
    extern MCL_EXPORT void mcl_json_util_destroy(mcl_json_t **root);

#ifdef  __cplusplus
}
#endif

#endif //MCL_JSON_UTIL_H_
