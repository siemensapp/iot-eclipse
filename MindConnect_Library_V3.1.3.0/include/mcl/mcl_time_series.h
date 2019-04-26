/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_time_series.h
* @date     Jul 11, 2016
* @brief    Time series module interface header file.
*
************************************************************************/

#ifndef MCL_TIME_SERIES_H_
#define MCL_TIME_SERIES_H_

#include "mcl/mcl_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * @brief This struct is used for building the time series type.
     */
    typedef struct mcl_time_series_t mcl_time_series_t;

    /**
     * @brief This struct is used for building value set of time series.
     */
    typedef struct mcl_time_series_value_set_t mcl_time_series_value_set_t;

    /**
     * @brief This function creates new value set to be added to #mcl_time_series_t.
     *
     * @param [in] time_series @p value_set is added to @p time_series .
     * @param [in] timestamp Timestamp of the values in YYYY-MM-DDThh:mm:ss.sssZ format. Ex:2016-04-26T08:06:25.317Z.
     * @param [out] value_set Contains a timestamp and a list of values at that timestamp.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if any input argument is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case there is no space to create new #mcl_time_series_value_set_t in store.</li>
     * <li>#MCL_INVALID_PARAMETER in case timestamp is not valid in terms of format, length or time value.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_time_series_new_value_set(mcl_time_series_t *time_series, const char *timestamp, mcl_time_series_value_set_t **value_set);

    /**
     * @brief This function adds @p data_point_id, @p value and @p quality_code to #mcl_time_series_value_set_t.
     *
     * @param [in] value_set Value set to which parameters are added.
     * @param [in] data_point_id Id of the data point the value is read from.
     * @param [in] value The value read.
     * @param [in] quality_code The quality of the value provided. Must represent a valid number compatible with the standard.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if one of the provided parameters is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_LIMIT_EXCEEDED in case there is no space in store to add a new value.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_time_series_add_value(mcl_time_series_value_set_t *value_set, const char *data_point_id, const char *value,
            const char *quality_code);

#ifdef  __cplusplus
}
#endif

#endif //MCL_TIME_SERIES_H_
