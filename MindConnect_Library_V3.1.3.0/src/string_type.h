/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     string_type.h
* @date     Aug 2, 2016
* @brief    String type module header file.
*
* This module introduces #string_t type which holds the C string inside along with length information.
* Operations that can be performed on this type are also defined in this module.
*
************************************************************************/

#ifndef STRING_TYPE_H_
#define STRING_TYPE_H_

#include "list.h"
#include "string_util.h"

extern const char *hex_table;

/**
 * @brief This type defines the different kind of string behaviors during string initialization and destroy.
 *
 * The type of a string will be determined by which initialize function has called for it.
 *  - #string_initialize_new() will initialize a string with type #MCL_STRING_COPY_DESTROY
 *  - #string_initialize_dynamic() will initialize a string with type #MCL_STRING_NOT_COPY_DESTROY
 *  - #string_initialize_static() will initialize a string with type #MCL_STRING_NOT_COPY_NOT_DESTROY
 */
typedef enum E_MCL_STRING_TYPE
{
    MCL_STRING_COPY_DESTROY, //!< Strings with this type will allocates its buffer during initialization (copies the initial value to its buffer) and free its buffer during destroy.
    MCL_STRING_NOT_COPY_DESTROY, //!< Strings with this type will <b>NOT</b> allocate its buffer during initialization (buffer only points of the initial value) and free its buffer during destroy.
    MCL_STRING_NOT_COPY_NOT_DESTROY //!< Strings with this type will <b>NOT</b> allocate its buffer during initialization (buffer only points of the initial value) and <b>NOT</b> free its buffer during destroy (freeing the buffer will be responsibility of the caller).
} E_MCL_STRING_TYPE;

/**
 * #string_t object definition.
 */
typedef struct string_t
{
    char *buffer;           //!< Buffer of string handle.
    mcl_size_t length;      //!< Length of buffer.
    E_MCL_STRING_TYPE type; //!< Type of copy and destroy.
} string_t;

/**
 * @brief Initializes an #string_t object with another one.
 *
 * The content of the buffer of the @p other string will be copied into the new string's buffer. The type of the string will be #MCL_STRING_COPY_DESTROY.
 *
 * @param [in] other Other #string_t object to initialize from.
 * @param [out] string Will point to the initialized #string_t object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_initialize(const string_t *other, string_t **string);

/**
 * @brief Initializes a <i>new</i> #string_t object with the given value and length.
 *
 * A <b>New</b> string means that it will copy the content of value into its buffer and frees the buffer when #string_destroy() is called.
 * If received length is zero, length of the string will be calculated.
 * <br>This is good for usage like this :
 * @code {.c}
 * string_t *string;
 * string_initialize_new("Hello World!", 0, &string);
 * @endcode
 *
 * @warning The given @p value must be a zero-terminated C string.
 *
 * @param [in] value The content to initialize the string with. If it is not NULL it will be copied into the buffer of the string. If it is NULL, an empty string with NULL buffer will be created.
 * @param [in] value_length The length of the @p value. If this length is non zero it will be accepted as the correct length of the string. If the length is not known can be passed as zero and initialize function will calculate the length itself.
 * @param [out] string Will point to the initialized #string_t object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_initialize_new(const char *value, mcl_size_t value_length, string_t **string);

/**
 * @brief Initializes a <i>dynamic</i> #string_t object with the given value and length.
 *
 * A <b>Dynamic</b> string means that it will not allocate any memory space for its buffer. Its buffer will point to the address of the initial @p value but it frees the buffer when #string_destroy() is called.
 * <br>This is good for usage like this :
 * @code {.c}
 * string_t *string;
 * string_initialize_dynamic(cJSON_Print(), 0, &string);
 * @endcode
 *
 * If received length is zero, length of the string will be calculated.
 *
 * @warning The given @p value must be a zero-terminated C string.
 *
 * @param [in] value The content to initialize the string with. If it is not NULL it will be copied into the buffer of the string. If it is NULL, an empty string with NULL buffer will be created.
 * @param [in] value_length The length of the @p value. If this length is non zero it will be accepted as the correct length of the string. If the length is not known can be passed as zero and initialize function will calculate the length itself.
 * @param [out] string Will point to the initialized #string_t object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_initialize_dynamic(const char *value, mcl_size_t value_length, string_t **string);

/**
 * @brief Initializes a <i>static</i> #string_t object with the given value and length.
 *
 * A <b>Static</b> string means that it will not allocate any memory space for its buffer. Its buffer will point to the address of the initial @p value and also it will not free the buffer when #string_destroy() is called.
 * If received length is zero, length of the string will be calculated.
 * <br>This is good for usage like this :
 * @code {.c}
 * void func(char *in_cstr)
 * {
 *   string_t *string;
 *   string_initialize_static(in_cstr, 0, &string);
 * }
 * @endcode
 *
 * @warning The given @p value must be a zero-terminated C string.
 *
 * @param [in] value The content to initialize the string with. If it is not NULL it will be copied into the buffer of the string. If it is NULL, an empty string with NULL buffer will be created.
 * @param [in] value_length The length of the @p value. If this length is non zero it will be accepted as the correct length of the string. If the length is not known can be passed as zero and initialize function will calculate the length itself.
 * @param [out] string Will point to the initialized #string_t object.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_initialize_static(const char *value, mcl_size_t value_length, string_t **string);

/**
 * @brief Sets the buffer of the string with a new value.
 *
 * First #string_release() is called and the buffer is released with or without freeing it depending on the string type. Then buffer assigned to the new value again either by copying or by referencing depending on string type.
 * String type and behavior will not be changed with this function.
 *
 * @param [in] string String handle to be set.
 * @param [in] value The new value to be set to the string. This must be a zero terminated C string.
 * @param [in] value_length If value length is known, by passing it with this parameter would reduce operation cost by avoiding to calculate it again. If this parameter is given as 0, #string_set will calculate the length.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_set(string_t *string, const char *value, mcl_size_t value_length);

/**
 * @brief Compare the contents of two string_t's.
 *
 * @param [in] string String handler to compare.
 * @param [in] other Other string handler to compare.
 * @return #MCL_OK if two are the same. MCL_FAIL if they are not.
 * @return
 * <ul>
 * <li>#MCL_OK in case @p string is the same as @p other.</li>
 * <li>#MCL_FAIL  in case @p string is not the same as @p other.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_compare(const string_t *string, const string_t *other);

/**
 * @brief Compare the contents of string_t with a C string.
 *
 * @param [in] string String handler to compare.
 * @param [in] other Zero terminating C string to compare.
 * @return
 * <ul>
 * <li>#MCL_OK in case @p string is the same as @p other.</li>
 * <li>#MCL_FAIL in case @p string is not the same as @p other.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_compare_with_cstr(const string_t *string, const char *other);

/**
 * @brief Splits the string with the given char and returns the result as an #list_t of #string_t's.
 *
 * @param [in] string String handler to split.
 * @param [in] token Char value to split the string.
 * @param [out] string_list The prepared list containing the splitted strings.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_LIMIT_EXCEEDED in case @p string_list has no space for an additional split.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_split(string_t *string, const char token, list_t **string_list);

/**
 * Converts given buffer to its hex representation as string.
 * Allocates a new memory for resulting string with the size of @code 2*buffer_size + 1@endcode.
 * The returned string is zero terminated.
 *
 * @param [in] buffer The buffer which contains data to be converted.
 * @param [in] buffer_size The length of the @p buffer.
 * @param [out] hex_data The newly allocated string which contains conversion result and which is zero terminated.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_convert_binary_to_hex(const mcl_uint8_t *buffer, mcl_size_t buffer_size, string_t **hex_data);

/**
 * @brief Clears the content of the string. And deallocates the memory.
 *
 * Clears the content of the string, sets the buffer to #MCL_NULL and the length to zero.
 *
 * @param [in] string String to be released.
 */
void string_release(string_t *string);

/**
 * @brief Destroys the allocated resources of the string.
 *
 * After destroy operation, string handler shouldn't be used.
 *
 * @param [in] string Address of the string handler to destroy. It's value will be set to #MCL_NULL after destroying.
 */
void string_destroy(string_t **string);

/**
 * @brief Concatenates two strings of type @c string_t into a @c string_t.
 *
 * @param [in]  string String to which the @p cstring will be concatenated.
 * @param [in]  c_string Char array which will be concatenated to @p string.
 * @param [out] result The string that is result of the concatenation.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_concatenate_cstr(string_t *string, char *c_string, string_t **result);

/**
 * @brief Concatenates a C string (i.e. a char array)
 *
 * @param [in]  string_1 String to which the @p string_2 will be concatenated.
 * @param [in]  string_2 String which will be concatenated to @p string_1.
 * @param [out] result The string that is result of the concatenation.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_concatenate(string_t *string_1, string_t *string_2, string_t **result);

/**
* @brief Replaces @p old_string with @p new_string.
*
* @param [in]  source The string in which the replacement will be done.
* @param [in]  old_string Part of @p source that will be replaced.
* @param [in]  new_string New string which will be replaced with @p old_string.
* @param [out] result String that is created as result of the replacement.
* @return
* <ul>
* <li>#MCL_OK in case of success.</li>
* <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
* </ul>
*/
E_MCL_ERROR_CODE string_replace(string_t *source, const char *old_string, const char *new_string, string_t **result);

#endif //STRING_TYPE_H_
