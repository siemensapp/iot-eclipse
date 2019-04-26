/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     string_array.h
* @date     Aug 19, 2016
* @brief    String array module header file.
*
* String Array is used to collect different #string_t string objects.
* By #string_array_to_string() function it gives the capability to concatenate the strings.
*
************************************************************************/

#ifndef STRING_ARRAY_H_
#define STRING_ARRAY_H_

#include "string_type.h"

/**
 * @brief Default increment value of the array.
 *
 * Incrementing the array means if during an add operation, there initialized array count is not enough to hold the additional item,
 * array increasses it's size by this value. To override this default value #string_array_set_increment() can be used.
 */
#define DEFAULT_INCREMENT_VALUE 5

/**
 * String array item. Holds the #string_t string object in the array and the destroy information of the item.
 * Destroy flag is used when destroying the string array to decide whether call #string_destroy() for specific item in the array.
 */
typedef struct string_array_item_t
{
    string_t *string;   //!< String object.
    mcl_bool_t destroy; //!< To decide whether to call #string_destroy() for specific item in the array when destroying the string array.
} string_array_item_t;

/**
 * String array handle object. Holds array specific information. String array functions will operate on this handle.
 */
typedef struct string_array_t
{
    string_array_item_t *items; //!< Item of string array.
    mcl_size_t count;           //!< Item count.
    mcl_size_t index;           //!< Item index.
    mcl_size_t increment_value; //!< Increment value.
    mcl_size_t total_length;    //!< Total length of string array.
} string_array_t;

/**
 * @brief String array initialize method.
 *
 * Initializes a string array handle suitable to hold @p count items.
 *
 * @param [in] count Initial item count that this string array will hold. This count value might be changed if it is not enough for future add operations.
 * @param [out] array Initialized string array handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_INVALID_PARAMETER in case @p count is zero.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_array_initialize(mcl_size_t count, string_array_t **array);

/**
 * @brief Sets the increment value of the array.
 *
 * Incrementing the array means if during an add operation, there initialized array count is not enough to hold the additional item,
 * array increasses it's size by its increment value. With this function this value can be set. For its default value refer to #DEFAULT_INCREMENT_VALUE.
 *
 * @param [in] array String array handle to operate.
 * @param [in] increment_value The increment value. Have to be greater than 0.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case @p increment_value is zero.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_array_set_increment(string_array_t *array, mcl_size_t increment_value);

/**
 * @brief Adds an #string_t string object into the array.
 *
 * Caller also have to decide wheter this string object should be destroyed or not when string array is destroyed.
 *
 * @param [in] array String array handle to operate.
 * @param [in] string The #string_t string object to be added into the array.
 * @param [in] destroy Destroy flag is used to decide during #string_array_destroy() operation wheter call #string_destroy() for this string item or not.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_array_add(string_array_t *array, string_t *string, mcl_bool_t destroy);

/**
 * @brief To set the string item at a specified index in the string array.
 *
 * @param [in] array String array hadnle to operate.
 * @param [in] index Index value. It has to be greater than 0 and smaller than arrays current count value.
 * @param [in] string The #string_t string object to be set into the array at @p index.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_INVALID_PARAMETER in case @p index is out of bounds.</li>
 * </ul>
 *
 * @warning The strings added with this function at different indexes might be overriden if #string_array_add() operation is called. Either use #string_array_add() or this function to manually manage.
 *
 */
E_MCL_ERROR_CODE string_array_set(string_array_t *array, mcl_size_t index, string_t *string);

/**
 * @brief To get the #string_t string object at a specified index.
 * @param [in] array String array handle to operate.
 * @param [in] index Specifes the index in the array of the reqeusted string object. Index has to be greater than 0 and smaller then array's current index.
 * @return Returns the string object on success. MCL_NULL if index is invalid.
 */
string_t *string_array_get(string_array_t *array, mcl_size_t index);

/**
 * @brief To concatenate the strings in the array.
 *
 * It collects the strings in its array and concatenates them into a final string and returns it.
 *
 * @param [in] array String array handle to operate.
 * @return Returns pointer to #string_t string object which holds the concatenated form of the strings in the array, MCL_NULL on failure.
 */
string_t *string_array_to_string(string_array_t *array);

/**
 * @brief Destroys the string array handle.
 *
 * With a loop over its items, check @c destroy flag of each of the items and calls #string_destroy() if the flag is MCL_TRUE.
 *
 * @warning Even though this function calls #string_destroy() buffer of that string might not be freed. It is depended on how the string is initialized. Please @see string_initialize() functions.
 *
 * @param [in] array Address of string array handle. String array will be MCL_NULL after this operation.
 */
void string_array_destroy(string_array_t **array);

#endif //STRING_ARRAY_H_
