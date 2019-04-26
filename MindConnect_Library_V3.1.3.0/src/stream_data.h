/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     stream_data.h
* @date     Sep 26, 2016
* @brief    Stream data module header file.
*
* Meta and payload parts of custom data type are initialized and filled in this file.
*
************************************************************************/

#ifndef STREAM_DATA_H_
#define STREAM_DATA_H_

#include "mcl/mcl_stream_data.h"
#include "custom_data.h"
#include "data_types.h"

/**
 * @brief This struct is used for building the complete message of stream data.
 */
typedef struct mcl_stream_data_t
{
    custom_data_t *base;                           //!< Type of base is custom data.
    void *user_context;                            //!< User context.
    mcl_stream_data_read_callback_t read_callback; //!< Read callback of stream data.
} stream_data_t;

/**
 * @brief Initializes @p stream_data.
 *
 * @param [in] version Meta version.
 * @param [in] type Meta type.
 * @param [in] routing Routing information in meta description of @p stream_data.
 * @param [in] stream_data_read_callback The callback function which will be used to fill the payload of this stream object while composing the http request.
 * @param [in] user_context This will be passed to the callback function in each call.
 * @param [out] stream_data Initialized stream data handle to operate on.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE stream_data_initialize(const char *version, const char *type, const char *routing, mcl_stream_data_read_callback_t stream_data_read_callback,
        void *user_context, stream_data_t **stream_data);

/**
 * @brief Destroys @p stream_data.
 *
 * @param [in] stream_data Stream data handle to operate on.
 */
void stream_data_destroy(stream_data_t **stream_data);

#endif //STREAM_DATA_H_
