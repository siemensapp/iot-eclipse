/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_custom_data.h
* @date     Jul 27, 2016
* @brief    Custom data module interface header file.
*
* Meta and payload parts of custom data type are initialized and set by this module.
*
************************************************************************/

#ifndef MCL_CUSTOM_DATA_H_
#define MCL_CUSTOM_DATA_H_

#include "mcl_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * @brief This struct is used for building the custom data type.
     */
    typedef struct mcl_custom_data_t mcl_custom_data_t;

    /**
     * @brief This struct is used for json handling.
     */
    typedef struct mcl_json_t mcl_json_t;

    /**
     * @brief Adds a custom @p details object to @p custom_data.
     *
     * @param [in] custom_data All meta and payload details, content etc. stored in this data struct.
     * @param [in] details A custom json object to form meta details of @p custom_data. New memory space will be allocated for this parameter.
     * Ownership passed to caller. Caller must free the space.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p custom_data or @p details is NULL.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_custom_data_set_meta_details(mcl_custom_data_t *custom_data, const mcl_json_t *details);

    /**
     * @brief Sets payload contents.
     *
	 * @warning Destroying content is user's own responsibility. Avoid destroying content unless at least one of the following conditions are met.
	 * <ul>
	 * <li>Successful exchange.</li>
	 * <li>Destroying store (mcl_store_destroy(&store)).</li>
	 * <li>Destroying communication (mcl_communication_destroy(&communication)).</li>
	 * </ul>
	 * @param [in] custom_data All meta and payload details, content etc. stored in this data struct.
	 * @param [in] content Payload content. Payload can contain binary data as well
	 * @param [in] content_size Payload content size.
	 * @return
	 * <ul>
	 * <li>#MCL_OK in case of success.</li>
	 * <li>#MCL_TRIGGERED_WITH_NULL if provided @p custom_data or @p content is NULL.</li>
	 * <li>#MCL_INVALID_PARAMETER if provided @p content_length is not bigger than zero.</li>
	 * </ul>
	 */
	extern MCL_EXPORT E_MCL_ERROR_CODE mcl_custom_data_set_payload(mcl_custom_data_t *custom_data, const mcl_uint8_t *content, mcl_size_t content_size);

#ifdef  __cplusplus
}
#endif

#endif //MCL_CUSTOM_DATA_H_
