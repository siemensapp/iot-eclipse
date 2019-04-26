/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     time_series.h
* @date     Sep 26, 2016
* @brief    Time series module header file.
*
************************************************************************/

#ifndef TIME_SERIES_H_
#define TIME_SERIES_H_

#include "data_types.h"

/**
 * @brief This struct is used for building the complete message of time series event.
 */
typedef struct mcl_time_series_t
{
    item_meta_t meta;              //!< Meta of time series.
    time_series_payload_t payload; //!< Payload of time series.
} time_series_t;

/**
 * @brief This function creates and initializes a data struct of time_series_t.
 *
 * Initialized meta fields: type, version, payload_type, payload_version and payload_details_configuration_id.
 *
 * @param [in] version Version number of time series event.
 * @param [in] configuration_id Unique identifier of the configuration.
 * @param [in] routing Routing information in meta description of @p time_series. This parameter is optional and can be NULL.
 * @param [out] time_series Pointer address of initialized time series data struct.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE time_series_initialize(const char *version, const char *configuration_id, const char *routing, time_series_t **time_series);

/**
 * @brief To destroy the @c time_series_t data struct.
 *
 * Will release the time_series_t data struct. After destroy() operation, the data struct shouldn't be used.
 *
 * @param [out] time_series Stores all field members of time series json string.
 */
void time_series_destroy(time_series_t **time_series);

#endif //TIME_SERIES_H_
