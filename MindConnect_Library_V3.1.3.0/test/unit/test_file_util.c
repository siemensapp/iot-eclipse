/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_file_util.c
* @date     Oct 5, 2016
* @brief    This file contains test case functions to test file_util module.
*
************************************************************************/

#include "file_util.h"
#include "unity.h"
#include "memory.h"
#include "string_util.h"
#include "definitions.h"

char *file_name = "temp.txt";
char *data_written = "123456";
mcl_size_t data_size = 6;

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : A file name.
 * WHEN  : file_util_fopen and file_util_fclose are called.
 * THEN  : MCL_OK is returned for both function calls. File descriptor is not null after file_util_fopen is called.
 */
void test_fopen_001(void)
{
    void *file_descriptor = MCL_NULL;
    E_MCL_ERROR_CODE return_code = file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    return_code = file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");
}

/**
 * GIVEN : A file is opened in write mode.
 * WHEN  : file_util_fwrite is called.
 * THEN  : MCL_OK is returned.
 */
void test_fwrite_001(void)
{
    void *file_descriptor = MCL_NULL;
    E_MCL_ERROR_CODE return_code = file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    return_code = file_util_fwrite(data_written, sizeof(char), data_size, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be written to file.");

    return_code = file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");
}

/**
 * GIVEN : A file is opened in read mode and data known a priori is already written to the file.
 * WHEN  : file_util_fread is called.
 * THEN  : MCL_OK is returned. Data read equals to data written.
 */
void test_fread_001(void)
{
    void *file_descriptor = MCL_NULL;
    E_MCL_ERROR_CODE return_code = file_util_fopen(file_name, "r", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    char *data_read = MCL_MALLOC(data_size);
    mcl_size_t actual_data_size = 0;
    file_util_fread(data_read, sizeof(char), data_size, file_descriptor, &actual_data_size);
    TEST_ASSERT_MESSAGE(data_size == actual_data_size, "Data size read mismatch the data size written.");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(data_written, data_read, actual_data_size, "Data read is not equal to data written.");

    return_code = file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");

    MCL_FREE(data_read);
}

/**
 * GIVEN : A file is opened in write mode.
 * WHEN  : file_util_fputs is called.
 * THEN  : MCL_OK is returned.
 */
void test_fputs_001(void)
{
    void *file_descriptor = MCL_NULL;
    E_MCL_ERROR_CODE return_code = file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    return_code = file_util_fputs(data_written, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be written to file.");

    return_code = file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");
}

/**
 * GIVEN : A file is opened in read mode and data known a priori is already written to the file.
 * WHEN  : file_util_fgets is called.
 * THEN  : MCL_OK is returned. Data read equals to data written.
 */
void test_fgets_001(void)
{
    void *file_descriptor = MCL_NULL;
    E_MCL_ERROR_CODE return_code = file_util_fopen(file_name, "r", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    char *data_read = MCL_MALLOC(data_size + 1);
    return_code = file_util_fgets(data_read, data_size + 1, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be read from file.");

    mcl_size_t actual_data_size;
    actual_data_size = string_util_strlen(data_read);
    TEST_ASSERT_MESSAGE(data_size == actual_data_size, "Data size read mismatch the data size written.");

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(data_written, data_read, actual_data_size, "Data read is not equal to data written.");

    return_code = file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");

    MCL_FREE(data_read);
}

/**
 * GIVEN : A file is opened in read mode and data known a priori is already written to the file.
 * WHEN  : file_util_fstat is called.
 * THEN  : MCL_OK is returned. Data read equals to data written.
 */
void test_fstat_001(void)
{
    void *file_descriptor = MCL_NULL;
    E_MCL_ERROR_CODE return_code = file_util_fopen(file_name, "w", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    char *data = "0123456789";
    return_code = file_util_fputs(data, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be written to file.");

    return_code = file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");

    return_code = file_util_fopen(file_name, "r", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    struct stat file_attributes;
    file_util_fstat(file_descriptor, &file_attributes);
    mcl_int64_t file_size = file_attributes.st_size;
    TEST_ASSERT_EQUAL_MESSAGE(10, file_size, "File size is not correct.");

    return_code = file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");
}
