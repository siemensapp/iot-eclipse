/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     custom_data.h
* @date     Sep 26, 2016
* @brief    Custom data module header file.
*
* Meta and payload parts of custom data type are initialized and set by this module.
*
************************************************************************/

#ifndef CUSTOM_DATA_H_
#define CUSTOM_DATA_H_

#include "data_types.h"

/**
 * @brief This struct is used for building the custom data type.
 */
typedef struct mcl_custom_data_t
{
    item_meta_t meta;         //!< Meta of custom data.
    binary_payload_t payload; //!< Payload of custom data.
} custom_data_t;

/**
 * @brief Initializes @p custom_data.
 *
 * @param [in] version Meta version.
 * @param [in] type Meta type.
 * @param [in] routing Routing information in meta description of @p custom_data. This parameter is optional and can be NULL.
 * @param [out] custom_data Custom data handle to operate on.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE custom_data_initialize(const char *version, const char *type, const char *routing, custom_data_t **custom_data);

/**
 * @brief Destroys @p custom_data.
 *
 * @param [in] custom_data All meta and payload details, content etc. stored in this data struct.
 */
void custom_data_destroy(custom_data_t **custom_data);

#endif //CUSTOM_DATA_H_
