/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     data_types.h
 * @date     Jun 27, 2016
 * @brief    Data types module header file.
 *
 * This module contains data type definitions.
 *
 ************************************************************************/

#ifndef DATA_TYPES_H_
#define DATA_TYPES_H_

#include "string_type.h"

typedef struct mcl_time_series_t time_series_t;

typedef struct mcl_json_t mcl_json_t;

// ----------------- item_meta_t -----------------------------

/**
 * @brief This struct is used for building item.meta.details structure.
 */
typedef struct item_meta_details_t
{
    string_t *routing;            //!< Information helping the server-side routing mechanism.
} item_meta_details_t;

/**
 * @brief This struct is used for the time_series.meta.payload.details structure.
 */
typedef struct item_meta_payload_details_time_series_t
{
    string_t *configuration_id; //!< Unique identifier of the configuration.
} item_meta_payload_details_time_series_t;

/**
 * @brief This struct is used for building file.meta.payload.details structure.
 */
typedef struct item_meta_payload_details_file_t
{
    string_t *file_name;          //!< Name of the file transferred.
    string_t *creation_date;      //!< Date and time when the file was created. ISO 8601 date and time format.
    string_t *file_type;          //!< Type of the file transferred.
} item_meta_payload_details_file_t;

/**
 * @brief This struct is used for building custom.meta.payload.details structure.
 */
typedef struct item_meta_payload_details_custom_t
{
    mcl_json_t *details_json; //!< Any details.
} item_meta_payload_details_custom_t;

/**
 * @brief This union is used for building item.meta.payload.details for time series, file and custom data.
 */
typedef union item_meta_payload_details_union_t
{
    item_meta_payload_details_time_series_t time_series_details; //!< Time series details.
    item_meta_payload_details_file_t file_details;               //!< File details.
    item_meta_payload_details_custom_t custom_details;           //!< Custom details.
} item_meta_payload_details_union_t;

/**
 * @brief This struct is used for building item.meta.payload structure.
 */
typedef struct item_meta_payload_t
{
    string_t *type;                            //!< Type of payload.
    string_t *version;                         //!< Version of payload.
    item_meta_payload_details_union_t details; //!< Type and version specific meta information about the payload.
} item_meta_payload_t;

/**
 * @brief This struct is used for building item.meta structure.
 */
typedef struct item_meta_t
{
    string_t *content_id;        //!< Unique identifier of the group.
    string_t *type;              //!< Type of meta.
    string_t *version;           //!< Version of meta.

    item_meta_details_t details; //!< Type and version-specific meta-information.
    item_meta_payload_t payload; //!< Information describing the payload part following this meta or a collection of tuples referencing it.
} item_meta_t;

// ----------------- time_series_payload_t -----------------------------

/**
 * @brief This struct is used for building time_series.payload.values structure.
 */
typedef struct mcl_time_series_value_t
{
    string_t *data_point_id; //!< Id of the datapoint the value is read from.
    string_t *value;         //!< The value read.
    string_t *quality_code;  //!< The quality of the value provided.
} time_series_value_t;

/**
 * @brief This struct is used for building value set of time series.
 */
typedef struct mcl_time_series_value_set_t
{
    string_t *timestamp; //!< Time of values in YYYY-MM-DDThh:mm:ss.sssZ format.

    // time_series_value_t
    list_t *values; //!< List of measurements made at the given timestamp.
    time_series_t *parent; //!< Parent of this time series.
} time_series_value_set_t;

/**
 * @brief This struct is used for building the payload of time series which is list of value sets.
 */
typedef struct time_series_payload_t
{
    // time_series_value_set_t
    list_t *value_sets; //!< List of value sets.
} time_series_payload_t;

// ----------------- event_payload_t -----------------------------

/**
 * @brief This struct is used for building event.payload structure.
 */
typedef struct event_payload_t
{
    string_t *id;             //!< Unique identifier of the event.
    string_t *correlation_id; //!< Parent event id.

    // string_ts
    string_t *timestamp;			//!< Creation time of the event in ISO format.
    mcl_int32_t severity;           //!< Severity level.
    string_t *description;			//!< Event description.
    string_t *type;					//!< Type of the event.
    string_t *version;				//!< Version of the event/alarm type.
    mcl_json_t *details;			//!< Event/alarm details.
} event_payload_t;

// ----------------- file_payload_t -----------------------------

/**
 * @brief This struct is used for building file.payload structure.
 */
typedef struct file_payload_t
{
    mcl_uint8_t *buffer;   //!< File payload buffer.
    mcl_int64_t size;      //!< File payload size.
    mcl_size_t start_byte; //!< File payload start byte.
    mcl_size_t end_byte;   //!< File payload end byte.
} file_payload_t;

// ----------------- custom_data_payload_t -----------------------------

/**
 * @brief This struct is used for building custom_data.payload structure.
 */
typedef struct binary_payload_t
{
    mcl_uint8_t *buffer; //!< Binary payload buffer.
    mcl_size_t size;     //!< Binary payload size.
} binary_payload_t;

// ----------------- data_source_configuration_payload_t -----------------------------

/**
 * @brief This struct is used for building data_source_configuration.data_points structure.
 */
typedef struct data_point_t
{
    string_t *id;            //!< Agent-unique identifier of the data point.
    string_t *name;          //!< Name of the data point.
    string_t *description;   //!< Description of the data point.
    string_t *type;          //!< Type of data point.
    string_t *unit;          //!< Measurement unit of the data point.
    mcl_json_t *custom_data; //!< Custom data.
} data_point_t;

/**
 * @brief This struct is used for building data_source_configuration.data_source structure.
 */
typedef struct mcl_data_source_t
{
    string_t *name;        //!< Name of the data source.
    string_t *description; //!< Description of the data source.

    // data_point_t *;
    list_t *data_points;     //!< List of data points definitions.
    mcl_json_t *custom_data; //!< Custom data.
} data_source_t;

/**
 * @brief This struct is used for building data_source_configuratio.payload structure.
 */
typedef struct data_source_configuration_payload_t
{
    string_t *configuration_id; //!< Unique identifier of the configuration.

    // data_source_t *
    list_t *data_sources; //!< List of data sources definitions.
} data_source_configuration_payload_t;

/**
 * @brief Used with meta_field_names[E_META_FIELD_NAMES] to get the related string.
 * ex: meta_field_names[META_FIELD_TYPE] returns the string of "type".
 */
typedef enum E_META_FIELD_NAMES
{
    META_FIELD_TYPE,                               //!< Type of meta field.
    META_FIELD_VERSION,                            //!< Version of meta field.
    META_FIELD_PAYLOAD_META,                       //!< Meta of meta field payload.
    META_FIELD_PAYLOAD,                            //!< Meta field payload.
    META_FIELD_PAYLOAD_TYPE,                       //!< Type of meta field payload.
    META_FIELD_PAYLOAD_VERSION,                    //!< Version of meta field payload.
    META_FIELD_PAYLOAD_DETAILS,                    //!< Meta field payload details.
    META_FIELD_PAYLOAD_DETAILS_CONFIGURATION_ID,   //!< Configuration id of meta field payload details.
    META_FIELD_DETAILS,                            //!< Meta field details.
    META_FIELD_DETAILS_ROUTING,                    //!< Routing of meta field details.
    META_FIELD_PAYLOAD_DETAILS_FILE_NAME,          //!< File name of meta field details.
    META_FIELD_PAYLOAD_DETAILS_FILE_TYPE,          //!< File type of meta field details.
    META_FIELD_PAYLOAD_DETAILS_CREATION_DATE,      //!< Creation date of meta field details.
    META_FIELD_DETAILS_TOTAL_ITEMS,                //!< Total items of meta field details.
    META_FIELD_PAYLOAD_DETAILS_TIMESTAMP,          //!< Timestamp of meta field payload details.
    META_FIELD_PAYLOAD_DETAILS_DURATION,           //!< Duration of meta field payload details.
    META_FIELD_NAMES_END                           //!< End of meta field names.
} E_META_FIELD_NAMES;

/**
 * @brief Used with payload_field_names[E_PAYLOAD_FIELD_NAMES] to get the related string.
 * ex: payload_field_names[PAYLOAD_FIELD_TIMESTAMP] returns the string of "timeStamp".
 */
typedef enum E_PAYLOAD_FIELD_NAMES
{
    PAYLOAD_FIELD_TIMESTAMP,                                     //!< Timestamp of payload field.
    PAYLOAD_FIELD_VALUES,                                        //!< Payload field values.
    PAYLOAD_FIELD_VALUES_DATA_POINT_ID,                          //!< Data point id of payload field values.
    PAYLOAD_FIELD_VALUES_VALUE,                                  //!< Value of payload field values.
    PAYLOAD_FIELD_VALUES_QUALITY_CODE,                           //!< Quality code of payload field values.
    PAYLOAD_FIELD_ID,                                            //!< Payload field id.
    PAYLOAD_FIELD_CUSTOM_EVENT_ID,                               //!< Custom event id of payload field.
    PAYLOAD_FIELD_CORRELATION_ID,                                //!< Correlation id of payload field.
    PAYLOAD_FIELD_SOURCE_TYPE,                                   //!< Source type of payload field.
    PAYLOAD_FIELD_SOURCE_ID,                                     //!< Source id of payload field.
    PAYLOAD_FIELD_SEVERITY,                                      //!< Severity of payload field.
    PAYLOAD_FIELD_DESCRIPTION,                                   //!< Description of payload field.
    PAYLOAD_FIELD_STATUS,                                        //!< Status of payload field.
    PAYLOAD_FIELD_TYPE,                                          //!< Type of payload field.
    PAYLOAD_FIELD_VERSION,                                       //!< Version of payload field.
    PAYLOAD_FIELD_CUSTOM_DATA,                                   //!< Custom data of payload field.
    PAYLOAD_FIELD_DETAILS,                                       //!< Details of payload field.
    PAYLOAD_FIELD_DETAILS_MESSAGE,                               //!< Message of payload field details.
    PAYLOAD_FIELD_DETAILS_ERROR_CODE,                            //!< Error code of payload field details.
    PAYLOAD_FIELD_DETAILS_DETAILS,                               //!< Details of payload field details.
    PAYLOAD_FIELD_DETAILS_VALUE,                                 //!< Value of payload field details.
    PAYLOAD_FIELD_DETAILS_DOWNLOAD_LINK,                         //!< Download link of payload field details.
    PAYLOAD_FIELD_DETAILS_NAME,                                  //!< Name of payload field details.
    PAYLOAD_FIELD_DETAILS_VERSION,                               //!< Version of payload field details.
    PAYLOAD_FIELD_DETAILS_DESCRIPTION,                           //!< Description of payload field details.
    PAYLOAD_FIELD_CONFIGURATION_ID,                              //!< Configuration id of payload field.
    PAYLOAD_FIELD_DATA_SOURCES,                                  //!< Payload field data sources.
    PAYLOAD_FIELD_DATA_SOURCES_NAME,                             //!< Name of payload field data sources.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS,                      //!< Payload field data sources data points.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_ID,                   //!< Data point id of payload field assets data sources data points.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_NAME,                 //!< Name of payload field data sources data points.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_TYPE,                 //!< Type of payload field data sources data points.
    PAYLOAD_FIELD_DATA_SOURCES_DATA_POINTS_UNIT,                 //!< Unit of payload field data sources data points.
    PAYLOAD_FIELD_NAMES_END                                      //!< End of payload field names.
} E_PAYLOAD_FIELD_NAMES;

/**
 * @brief Used with meta_field_values[E_META_FIELD_VALUES] to get the related string.
 * ex: meta_field_values[META_FIELD_TYPE_ITEM] returns the string of "item".
 * Please refer to the MindConnectAPI Specifications for detailed info.
 */
typedef enum E_META_FIELD_VALUES
{
    META_FIELD_TYPE_ITEM,                               //!< Item type of meta field.
    META_FIELD_VERSION_CURRENT,                         //!< Current version of meta field.
    META_FIELD_PAYLOAD_VERSION_CURRENT,                 //!< Current version of meta field payload.
    META_FIELD_PAYLOAD_TYPE_TIME_SERIES,                //!< Time series type of meta field payload.
    META_FIELD_PAYLOAD_TYPE_BUSINESS_EVENT,             //!< Business event type of meta field payload.
    META_FIELD_PAYLOAD_TYPE_FILE,                       //!< File type of meta field payload.
    META_FIELD_PAYLOAD_TYPE_DATA_SOURCE_CONFIGURATION,  //!< Data source configuration type of meta field payload.
    META_FIELD_VALUES_END                               //!< End of meta field values.
} E_META_FIELD_VALUES;

/**
 * @brief Used with payload_field_values[E_PAYLOAD_FIELD_VALUES] to get the related string.
 * ex: payload_field_values[PAYLOAD_FIELD_SEVERITY_URGENT] returns the string of "1".
 */
typedef enum E_PAYLOAD_FIELD_VALUES
{
    PAYLOAD_FIELD_SEVERITY_URGENT,         //!< Severity level of payload field is urgent.
    PAYLOAD_FIELD_SEVERITY_IMPORTANT,      //!< Severity level of payload field is important.
    PAYLOAD_FIELD_SEVERITY_INFORMATION,    //!< Severity level of payload field is information.
    PAYLOAD_FIELD_SOURCE_TYPE_ASSET,       //!< Source type of payload is asset.
    PAYLOAD_FIELD_SOURCE_TYPE_AGENT,       //!< Source type of payload is agent.
    PAYLOAD_FIELD_SOURCE_TYPE_APPLICATION, //!< Source type of payload is application.
    PAYLOAD_FIELD_VALUES_END               //!< End of payload field values.
} E_PAYLOAD_FIELD_VALUES;

/**
 * @brief Used with http_header_values[E_HTTP_HEADER_VALUES] to get the related string.
 * ex: http_header_values[HTTP_HEADER_CONTENT_TYPE_APPLICATION_OCTET_STREAM] returns the string of "application/octet-stream".
 */
typedef enum E_CONTENT_TYPE_VALUES
{
    CONTENT_TYPE_APPLICATION_OCTET_STREAM, //!< Content type is application octet stream.
    CONTENT_TYPE_TEXT_PLAIN,               //!< Content type is text plain.
    CONTENT_TYPE_APPLICATION_JSON,         //!< Content type is application json.
    CONTENT_TYPE_MULTIPART_MIXED,          //!< Content type is multipart mixed.
    CONTENT_TYPE_MULTIPART_RELATED,        //!< Content type is multipart related.
    CONTENT_TYPE_META_JSON,                //!< Content type is meta json.
    CONTENT_TYPE_URL_ENCODED,              //!< Content type is application/x-www-form-urlencoded.
    TRANSFER_ENCODING_CHUNKED,             //!< Transfer encoding chunked.
    CONTENT_TYPE_VALUES_END                //!< End of content type values.
} E_CONTENT_TYPE_VALUES;

extern string_t meta_field_names[META_FIELD_NAMES_END];
extern string_t meta_field_values[META_FIELD_VALUES_END];
extern string_t payload_field_values[PAYLOAD_FIELD_VALUES_END];
extern string_t payload_field_names[PAYLOAD_FIELD_NAMES_END];
extern string_t content_type_values[CONTENT_TYPE_VALUES_END];
#endif //MCL_DATA_TYPES_H_
