/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     data_source_configuration.h
* @date     Nov 13, 2017
* @brief    Data source configuration module header file.
*
* Meta and payload parts of Data source configuration type are initialized and filled in this module.
*
************************************************************************/

#ifndef DATA_SOURCE_CONFIGURATION_H_
#define DATA_SOURCE_CONFIGURATION_H_

#include "data_types.h"

/**
 * This struct is used for building the data source configuration type.
 */
typedef struct mcl_data_source_configuration_t
{
    item_meta_t meta;                            //!< Meta of data source configuration.
    data_source_configuration_payload_t payload; //!< Payload of data_source_configuration.
} data_source_configuration_t;

/**
 * Initializes @p data_source_configuration.
 *
 * @param [in] version Meta version.
 * @param [out] data_source_configuration Initialized #data_source_configuration_t object handle.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE data_source_configuration_initialize(const char *version, data_source_configuration_t **data_source_configuration);

/**
 * Destroys @p data_source_configuration.
 *
 * @param [in] data_source_configuration
 */
void data_source_configuration_destroy(data_source_configuration_t **data_source_configuration);

#endif //DATA_SOURCE_CONFIGURATION_H_
