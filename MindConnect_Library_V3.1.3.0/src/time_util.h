/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     time_util.h
* @date     Dec 21, 2016
* @brief    Time utility module header file.
*
* This utility module implements the time utility functions.
*
************************************************************************/

#ifndef TIME_UTIL_H_
#define TIME_UTIL_H_

#include "string_type.h"

#if HAVE_TIME_H_
#include <time.h>
#endif

/**
 * This function converts time value to ISO 8601 date and time format.
 *
 * @param [in] time_value Number of seconds passed since reference time.
 * @param [out] iso8601_formatted_time Time representation in ISO 8601 format.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_FAIL in case of an internal error in MCL.</li>
 * </ul>
 */
E_MCL_ERROR_CODE time_util_convert_to_iso_8601_format(const time_t *time_value, string_t **iso8601_formatted_time);

/**
 * This function returns the time elapsed in terms of seconds since Jan 1, 1970 UTC until now.
 *
 * @param [in] current_time Time value in seconds.
 */
void time_util_get_time(mcl_time_t *current_time);

/**
 * This function validates timestamp in terms of format, length and time value.
 *
 * @param [in] timestamp Timestamp value to be checked against YYYY-MM-DDThh:mm:ss.sssZ format. Ex:2016-04-26T08:06:25.317Z
 * @return
 * <ul>
 * <li>#MCL_TRUE in case timestamp is valid.</li>
 * <li>#MCL_FALSE in case timestamp is not valid in terms of format, length or time value.</li>
 * </ul>
 */
mcl_bool_t time_util_validate_timestamp(const char *timestamp);

#endif //TIME_UTIL_H_
