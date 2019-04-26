/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     store.h
* @date     Sep 26, 2016
* @brief    Store module header file.
*
************************************************************************/

#ifndef STORE_H_
#define STORE_H_
#include "data_types.h"

/**
 * Data type of a data in the store.
 */
typedef enum E_STORE_DATA_TYPE
{
    STORE_DATA_TIME_SERIES,              //!< Type of the data in the store is time series.
    STORE_DATA_EVENT_LIST,               //!< Type of the data in the store is event list.
    STORE_DATA_FILE,                     //!< Type of the data in the store is file.
    STORE_DATA_CUSTOM,                   //!< Type of the data in the store is custom data.
    STORE_DATA_STREAM,                   //!< Type of the data in the store is stream.
    STORE_DATA_DATA_SOURCE_CONFIGURATION //!< Type of the data in the store is data source configuration.
} E_STORE_DATA_TYPE;

/**
 * Store data states.
 */
typedef enum E_STORE_DATA_STATE
{
    DATA_STATE_INITIAL,         //!< Initial state of a store data. Means this data is not processed before.
    DATA_STATE_PREPARED,        //!< Prepared. Means its meta and payload strings has been prepared.
    DATA_STATE_WRITTEN,         //!< Current data has been written to the current http request as a whole.
    DATA_STATE_STREAMING,       //!< Streaming is active. Current data has been partially written to the current http request or about to be written.
    DATA_STATE_SENT //!< This data has been successfully sent to the server. Can be deleted from the store.
} E_STORE_DATA_STATE;

typedef struct store_data_stream_info_t
{
    mcl_size_t meta_stream_index;    //!< Index of meta stream.
    mcl_size_t payload_stream_index; //!< Index of payload stream.
    string_t *tuple_subboundary;     //!< Subboundary of tuple.
    int meta_state;                  //!< Meta state.
    int payload_state;               //!< Payload state.
} store_data_stream_info_t;

/**
 * General data type for storage in store_t.
 * Data can be one of the following (distinguish by type):
 * <ul>
 * <li>time_series_t *</li>
 * <li>event_t *</li>
 * <li>file_t *</li>
 * <li>custom_data_t *</li>
 * <li>stream_data_t *</li>
 * </ul>
 */
typedef struct store_data_t
{
    void *data;                            //!< Data to be added to the store.
    E_STORE_DATA_TYPE type;                //!< Type of data in the store.
    E_STORE_DATA_STATE state;              //!< State of data in the store.
    string_t *meta;                        //!< Meta of the store as json string.
    mcl_uint8_t *payload_buffer;           //!< Payload of the store.
    mcl_size_t payload_size;               //!< Size of the payload in the store.
    store_data_stream_info_t *stream_info; //!< Stream information.
} store_data_t;

/**
 * This struct holds references to data to exchange via MCL communication interface.
 */
typedef struct mcl_store_t
{
    mcl_bool_t streamable;      //!< Defines if the data in store is streamable or not.

    list_t *high_priority_list; //!< Contains high priority store_data_t data.

    list_t *low_priority_list;  //!< Contains low priority store_data_t data.
} store_t;

/**
 * This function is used to set the state of the store data.
 *
 * @param [in] store_data Current store data.
 * @param [in] state New state of the data.
 */
void store_data_set_state(store_data_t *store_data, E_STORE_DATA_STATE state);

/**
 * This function is used to get the state of the data.
 *
 * @param [in] store_data Current store data.
 * @return Returns the state of the data.
 */
E_STORE_DATA_STATE store_data_get_state(store_data_t *store_data);

/**
 * This function is used to remove a store data from the store.
 *
 * Needs to know in which list ( high_priority_list or low_priority_list ) this data resides.
 *
 * @param [in] store_list The list in the store holding the data.
 * @param [in] store_data_node List node of the data requested to be removed.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_ARRAY_IS_EMPTY in case @p store_list is empty.</li>
 * </ul>
 */
E_MCL_ERROR_CODE store_data_remove(list_t *store_list, list_node_t *store_data_node);

/**
 * This function is used to get the count of items in store.
 *
 * @param [in] store The store handle.
 * @return Returns the number of store data items.
 */
mcl_size_t store_get_data_count(store_t *store);

#endif //STORE_H_
