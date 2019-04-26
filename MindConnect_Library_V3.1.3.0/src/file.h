/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     file.h
* @date     Sep 26, 2016
* @brief    File module header file.
*
* Meta and payload parts of file type are initialized and filled in this module.
*
************************************************************************/

#ifndef FILE_H_
#define FILE_H_

#include "data_types.h"

/**
 * @brief This struct is used for building the complete message of file.
 */
typedef struct mcl_file_t
{
    item_meta_t meta;       //!< Meta of file.
    file_payload_t payload; //!< Payload of file.
    void *descriptor;       //!< Descriptor of file.
} file_t;

/**
 * This function creates and initializes a data structure #file_t for a given file and opens it for reading.
 *
 * @param [in] version Version of the file item meta.
 * @param [in] file_path Absolute path of the file to transfer.
 * @param [in] file_name Name of the file. This name will be assigned to the file that is being uploaded.
 * @param [in] file_type Type of the file.
 * @param [in] routing Routing information of #file_t.
 * @param [out] file Data structure #file_t.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FILE_CANNOT_BE_OPENED in case file does not exist or is not a regular file.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE file_initialize(const char *version, const char *file_path, const char *file_name, const char *file_type, const char *routing, file_t **file);

/**
 * This function closes the file #file_t data structure refers to and destroys the #file_t data structure.
 *
 * @param [in] file Data structure #file_t to destroy.
 */
void file_destroy(file_t **file);

#endif //FILE_H_
