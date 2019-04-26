/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     string_util.h
* @date     Aug 12, 2016
* @brief    String utility module implementation file.
*
* This module implements utility functions for basic string operations.
*
************************************************************************/

#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include "mcl/mcl_common.h"

/**
 * @brief Standard library <b>strlen</b> wrapper.
 *
 * @param [in] buffer Buffer which length to be calculated. Must be a zero terminated C string.
 * @return the calculated length of buffer.
 */
mcl_size_t string_util_strlen(const char *buffer);

/**
 * @brief Standard library <b>strnlen</b> wrapper.
 *
 * @param [in] buffer Buffer which length to be calculated. Must be a zero terminated C string.
 * @param [in] maximum_length The maximum length to return. If string is not zero terminated this guarantees not accessing unwanted memory.
 * @return the calculated length of buffer.
 */
mcl_size_t string_util_strnlen(const char *buffer, mcl_size_t maximum_length);

/**
 * @brief Standard library <b>strncpy</b> wrapper. Also sets the terminating null char at the end if @p source is not longer than @p count.
 *
 * @param [out] destination Destination string. Must be a zero terminated C string.
 * @param [in] source Source string. Must be a zero terminated C string.
 * @param [in] count Specifies how many chars from @p source will be copied into the @p destination.
 */
void string_util_strncpy(char *destination, const char *source, mcl_size_t count);

/**
 * @brief Standard library <b>strncat</b> wrapper.
 *
 * @param [out] destination Destination string. Must be a zero terminated C string.
 * @param [in] source Source string. Must be a zero terminated C string.
 * @param [in] count Specifies how many chars from @p source will be added at the end of @p destination.
 */
void string_util_strncat(char *destination, const char *source, mcl_size_t count);

/**
 * @brief Standard library <b>strncmp</b> wrapper.
 *
 * @param [in] string_1 Fist string to be compared. Must be a zero terminated C string.
 * @param [in] string_2 Second string to be compared. Must be a zero terminated C string.
 * @param [in] count Specifies how many chars should be compared.
 * @return
 * <ul>
 * <li>#MCL_OK if both strings are equal.</li>
 * <li>#MCL_FAIL if they are not equal.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_util_strncmp(const char *string_1, const char *string_2, mcl_size_t count);

/**
 * @brief Standard library <b>snprintf</b> wrapper.
 *
 * @param [out] string String to be set. Must be a zero terminated C string.
 * @param [in] length Specifies how many chars should be processed.
 * @param [in] format Format string followed by format parameters.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case the operation fails.</li>
 * </ul>
 */
E_MCL_ERROR_CODE string_util_snprintf(char *string, mcl_size_t length, const char *format, ...);

/**
 * @brief Standard library <b>memcmp</b> wrapper.
 *
 * @param [in] block_1 Fist memory block to compare.
 * @param [in] block_2 Second memory block to compare.
 * @param [in] count Size of first memory block.
 * @return
 * <ul>
 * <li>#MCL_TRUE if two memory blocks are the same.</li>
 * <li>#MCL_FALSE if they are not the same.</li>
 * </ul>
 */
mcl_bool_t string_util_memcmp(const void *block_1, const void *block_2, mcl_size_t count);

/**
 * @brief Standard library <b>memcpy</b> wrapper.
 *
 * @param [in] destination Destination memory block.
 * @param [in] source Source memory block.
 * @param [in] count Count of bytes to be copied. Destination should have space for at least @p count bytes.
 */
void string_util_memcpy(void *destination, const void *source, mcl_size_t count);

/**
 * @brief Standard library <b>strdup</b> wrapper.
 *
 * @param [in] string String to duplicate.
 * @return A duplicate of the input @c string.
 */
char *string_util_strdup(const char *string);

/**
 * @brief Finds the first occurence of @p target in @p source and puts it's first index to @p start_index.
 *
 * @param [in] source Null terminated string to be searched in.
 * @param [in] target Null terminated string to be searched for.
 * @param [out] start_index Index of the first character of @p target if @p source contains it.
 * @return If @p target is found then MCL_TRUE, otherwise MCL_FALSE.
 */
mcl_bool_t string_util_find(const char *source, const char *target, mcl_size_t *start_index);

/**
 * @brief Finds the first occurence of @p target in @p source and puts it's first index to @p start_index.
 *
 * @param [in] source Null terminated string to be searched in.
 * @param [in] target Null terminated string to be searched for.
 * @param [out] start_index Index of the first character of @p target if @p source contains it.
 * @return If @p target is found then MCL_TRUE, otherwise MCL_FALSE.
 */
mcl_bool_t string_util_find_case_insensitive(const char *source, const char *target, mcl_size_t *start_index);

/**
 * @brief Checks if a character is whitespace.
 *
 * @param [in] character Character to be checked.
 * @return If @p character is whitespace returns MCL_TRUE. Otherwise, MCL_FALSE.
 */
mcl_bool_t string_util_is_space(char character);

/**
 * @brief Returns the first occurrence of an integral value in @p source string.
 *
 * @param [in] source String that contains the integral value as string.
 * @param [in] base The base that the number will be interpreted.
 * @param [out] end_pointer The pointer that points to the one past the last index of integral value.
 * @return If a number if found it's value is returned. Otherwise it returns 0.
 */
long string_util_strtol(const char *source, int base, char **end_pointer);

#endif //STRING_UTIL_H_
