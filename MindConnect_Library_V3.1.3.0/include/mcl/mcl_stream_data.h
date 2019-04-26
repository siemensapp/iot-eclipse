/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_stream_data.h
* @date     Dec 26, 2016
* @brief    Stream data module interface header file.
*
************************************************************************/

#ifndef MCL_STREAM_DATA_H_
#define MCL_STREAM_DATA_H_

#include "mcl_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * @brief This struct is used for json handling.
     */
    typedef struct mcl_json_t mcl_json_t;

    /**
     * @brief Stream data read callback function definition.
     *
     * @p source parameter has been added in order to accommodate the general callback format. It will be NULL.
     * Should return the amount of actually written bytes.
     * Return 0 if there is no more data to write.
     */
    typedef mcl_size_t (*mcl_stream_data_read_callback_t)(void *destination, void *source, mcl_size_t size, void *user_context);

    /**
     * @brief This struct is used for building the stream data type.
     */
    typedef struct mcl_stream_data_t mcl_stream_data_t;

    /**
     * @brief Adds a custom @p details object to @p stream_data.
     *
     * @param [in] stream_data All meta and payload details, callback etc. stored in this data struct.
     * @param [in] details A custom json object to form meta details.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p stream_data or @p details is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY if any memory allocation fails.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_stream_data_set_meta_details(mcl_stream_data_t *stream_data, const mcl_json_t *details);

#ifdef  __cplusplus
}
#endif

#endif //MCL_STREAM_DATA_H_
