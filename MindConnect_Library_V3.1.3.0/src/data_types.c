/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     data_types.c
* @date     Jul 11, 2016
* @brief    Data types module implementation file.
*
************************************************************************/

#include "data_types.h"

string_t meta_field_names[META_FIELD_NAMES_END] =
{
    {"type", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"version", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"meta", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"payload", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"type", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"version", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"details", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"configurationId", 15, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"details", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"routing", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"fileName", 8, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"fileType", 8, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"creationDate", 12, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"totalItems", 10, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"timestamp", 9, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"duration", 8, MCL_STRING_NOT_COPY_NOT_DESTROY},
};

string_t meta_field_values[META_FIELD_VALUES_END] =
{
    {"item", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"1.0", 3, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"1.0", 3, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"standardTimeSeries", 18, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"businessEvent", 13, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"file", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"dataSourceConfiguration", 23, MCL_STRING_NOT_COPY_NOT_DESTROY},
};

string_t payload_field_names[PAYLOAD_FIELD_NAMES_END] =
{
     {"timestamp", 9, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"values", 6, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"dataPointId", 11, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"value", 5, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"qualityCode", 11, MCL_STRING_NOT_COPY_NOT_DESTROY},
	 {"id", 2, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"customEventId", 13, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"correlationId", 13, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"sourceType", 10 , MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"sourceId", 8, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"severity", 8, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"description", 11, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"status", 6, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"type", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"version", 7 , MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"customData", 10 , MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"details", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"message", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"errorCode", 9, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"details", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"value", 5, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"download_link", 13, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"name", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"version", 7, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"description", 11, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"configurationId", 15, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"dataSources", 11, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"name", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"dataPoints", 10, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"id", 2, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"name", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"type", 4, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"unit", 4, MCL_STRING_NOT_COPY_NOT_DESTROY}
};

string_t payload_field_values[PAYLOAD_FIELD_VALUES_END] =
{
    {"1", 1, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"2", 1, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"3", 1, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"asset", 5, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"agent", 5, MCL_STRING_NOT_COPY_NOT_DESTROY},
    {"application", 11, MCL_STRING_NOT_COPY_NOT_DESTROY}
};

string_t content_type_values[CONTENT_TYPE_VALUES_END] =
{
     {"application/octet-stream", 24, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"text/plain", 10, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"application/json", 16, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"multipart/mixed", 15, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"multipart/related", 17, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"application/vnd.siemens.mindsphere.meta+json", 44, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"application/x-www-form-urlencoded", 33, MCL_STRING_NOT_COPY_NOT_DESTROY},
     {"chunked", 7, MCL_STRING_NOT_COPY_NOT_DESTROY}
};

