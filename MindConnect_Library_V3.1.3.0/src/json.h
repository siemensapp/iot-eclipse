/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     json.h
 * @date     Jul 14, 2016
 * @brief    Json module header file.
 *
 * This module creates json strings for all data types. Also it builds event list from json.
 *
 ************************************************************************/

#ifndef JSON_H_
#define JSON_H_

#include "data_types.h"
#include "event_list.h"

/**
 * @brief Creates item meta part of all types in json format.
 *
 * @param [in] item_meta Meta fields of the item are stored in this struct.
 * @param [out] json_string Json string filled with meta part of the related item.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE json_from_item_meta(item_meta_t *item_meta, string_t **json_string);

/**
 * @brief Creates payload part of time series in json format.
 *
 * @param [in] payload Payload fields of time series is stored in this struct.
 * @param [out] json_string Json string filled with meta part of the related item.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE json_from_time_series_payload(time_series_payload_t *payload, string_t **json_string);

/**
 * @brief Creates payload part of data source configuration in json format.
 *
 * @param [in] payload Payload fields of data source configuration is stored in this struct.
 * @param [out] json_string Json string filled with meta part of the related item.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE json_from_data_source_configuration_payload(data_source_configuration_payload_t *payload, string_t **json_string);

/**
 * @brief Creates payload part of event set in json format.
 *
 * @param [in] event_list_payload Payload fields of event list is stored in this list.
 * @param [in] json_string Json string filled with meta part of the related item.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE json_from_event_payload(list_t *event_list_payload, string_t **json_string);

#endif //JSON_H_
