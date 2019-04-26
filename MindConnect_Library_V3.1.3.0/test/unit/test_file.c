/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     test_file.c
* @date     Nov 30, 2016
* @brief    This file contains test case functions to test file module.
*
************************************************************************/

#include "unity.h"
#include "file.h"
#include "file_util.h"
#include "memory.h"
#include "string_util.h"
#include "time_util.h"
#include "string_type.h"
#include "list.h"
#include "data_types.h"
#include "definitions.h"

void setUp(void)
{
}

void tearDown(void)
{
}

/**
 * GIVEN : An existing binary file.
 * WHEN  : file_initialize() is called.
 * THEN  : MCL_OK is returned.
 */
void test_initialize_001(void)
{
    const char *version = "1.0";
    const char *file_path = "temp.bin";
	const char *file_name = "MyFile";
    const char *file_type = "binary file";
    const char *routing = "vnd.kuka.FingerprintAnalizer";
    file_t *file = MCL_NULL;

    // Create a file for this test case.
    void *file_descriptor = MCL_NULL;
    E_MCL_ERROR_CODE return_code = file_util_fopen(file_path, "w", &file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "No support for file handling.");
    TEST_ASSERT_NOT_NULL_MESSAGE(file_descriptor, "File descriptor is Null.");

    // Write data to file.
    char *data = "0123456789";
    return_code = file_util_fwrite(data, 1, 10, file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "Data can not be written to file.");

    // Close the file.
    return_code = file_util_fclose(file_descriptor);
    TEST_ASSERT_MESSAGE(MCL_OK == return_code, "File can not be closed.");

    // Initialize a file item.
    E_MCL_ERROR_CODE code = file_initialize(version, file_path, file_name, file_type, routing, &file);

    // Test.
    TEST_ASSERT_MESSAGE(MCL_OK == code, "File initialization failed.");

    file_destroy(&file);
    remove(file_path);
}

/**
 * GIVEN : file_path is path of a directory.
 * WHEN  : file_initialize() is called.
 * THEN  : MCL_FILE_CANNOT_BE_OPENED is returned.
 */
void test_initialize_002(void)
{
    const char *version = "1.0";
    const char *file_path = "temp.bin";
    const char *file_name = "MyFile";
    const char *file_type = "binary file";
    const char *routing = "vnd.kuka.FingerprintAnalizer";
    file_t *file = MCL_NULL;

    // Initialize a file item.
    E_MCL_ERROR_CODE code = file_initialize(version, file_path, file_name, file_type, routing, &file);

    // Test.
    TEST_ASSERT_MESSAGE(MCL_FILE_CANNOT_BE_OPENED == code, "Regular file check fails.");

    file_destroy(&file);
}
