/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     file_util.h
* @date     Oct 5, 2016
* @brief    File utility module header file.
*
* This utility includes the functions for file handling.
*
************************************************************************/

#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include "mcl/mcl_common.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

typedef struct stat mcl_stat_t;

/**
 * This function is used to open a file.
 *
 * @param [in] file_name File name to open.
 * @param [in] mode Mode of the file to open. Please refer to the modes of fopen() function in ANSI standard.
 * @param [out] file_descriptor File descriptor for the file opened. This descriptor is used to process the file.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case the file can not be opened.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fopen(const char *file_name, const char *mode, void **file_descriptor);

/**
 * This function is used to open a file.
 *
 * @note There are no debug entry and debug leave functions in this function.
 *
 * @param [in] file_name File name to open.
 * @param [in] mode Mode of the file to open. Please refer to the modes of fopen() function in ANSI standard.
 * @param [out] file_descriptor File descriptor for the file opened. This descriptor is used to process the file.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case the file can not be opened.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fopen_without_log(const char *file_name, const char *mode, void **file_descriptor);

/**
 * This function is used to close the file opened beforehand.
 *
 * @param [in] file_descriptor File descriptor of the file to close.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fclose(void *file_descriptor);

/**
 * This function is used to close the file opened beforehand. It has the same functionality as #file_util_fclose
 * except that it does not log any debug messages.
 *
 * @param [in] file_descriptor File descriptor of the file to close.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fclose_without_log(void *file_descriptor);

/**
 * This function is used to write data in memory to a file.
 *
 * @param [in] data Pointer to the data to write to file.
 * @param [in] size Size of each element in @p data.
 * @param [in] count Number of elements in @p data.
 * @param [in] file_descriptor File descriptor of the file to write to.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fwrite(const void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor);

/**
 * This function is used to write data in memory to a file.
 *
 * @note There are no debug entry and debug leave functions in this function.
 *
 * @param [in] data Pointer to the data to write to file.
 * @param [in] size Size of each element in @p data.
 * @param [in] count Number of elements in @p data.
 * @param [in] file_descriptor File descriptor of the file to write to.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fwrite_without_log(const void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor);

/**
 * This function is used to read data from a file into memory.
 *
 * @param [in] data Pointer to the memory (@p size x @p count bytes allocated a priori) to store the data from the file.
 * @param [in] size Size of each element requested.
 * @param [in] count Number of elements requested.
 * @param [in] file_descriptor File descriptor of the file to read from.
 * @param [out] actual_count Number of elements actually read from file.
 */
void file_util_fread(void *data, mcl_size_t size, mcl_size_t count, void *file_descriptor, mcl_size_t *actual_count);

/**
 * This function is used to write a string to a file. The terminating null character is not written.
 *
 * @param [in] data Pointer to the string to write to file.
 * @param [in] file_descriptor File descriptor of the file to write to.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fputs(const char *data, void *file_descriptor);

/**
 * This function is used to read a string from a file. A maximum of @p data_size - 1 characters are read from the file
 * and a terminating null character is immediately added after the last character of @p data. No additional characters are
 * read after the new line character of after end of file.
 *
 * @param [out] data Pointer to the string to read from file.
 * @param [in] data_size Maximum number of characters to read from file including the null terminator.
 * @param [in] file_descriptor File descriptor of the file to read from.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fgets(char *data, mcl_size_t data_size, void *file_descriptor);

/**
 * This function is used to get the attributes of a file.
 *
 * @param [in] file_descriptor File descriptor obtained by opening the file.
 * @param [in] file_attributes Structure holding the file attributes.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fstat(void *file_descriptor, mcl_stat_t *file_attributes);

/**
 * This function flushes pending content to @p file_descriptor.
 *
 * @param [in] file_descriptor File descriptor obtained by opening the file.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fflush(void *file_descriptor);

/**
 * This function flushes pending content to @p file_descriptor.
 *
 * @note There are no debug entry and debug leave functions in this function.
 *
 * @param [in] file_descriptor File descriptor obtained by opening the file.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_FAIL in case of failure.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_util_fflush_without_log(void *file_descriptor);

/**
 * This function is used to check if file is a regular file.
 *
 * @param [in] file_attributes Structure holding the file attributes.
 * @return
 * <ul>
 * <li>#MCL_TRUE if file is a regular file.</li>
 * <li>#MCL_FALSE if file is not a regular file.</li>
 * </ul>
 */
mcl_bool_t file_util_check_if_regular_file(const mcl_stat_t *file_attributes);

#endif //FILE_UTIL_H_
