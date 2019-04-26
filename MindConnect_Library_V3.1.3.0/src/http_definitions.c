/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     http_definitions.c
* @date     Sep 23, 2016
* @brief    HTTP definitions module implementation file.
*
************************************************************************/

#include "http_definitions.h"
#include "definitions.h"

string_t http_header_names[HTTP_HEADER_NAMES_END] =
{
	STRING_CONSTANT("Authorization"),
	STRING_CONSTANT("Content-Type"),
	STRING_CONSTANT("Content-ID"),
	STRING_CONSTANT("Content-Length"),
	STRING_CONSTANT("Transfer-Encoding"),
	STRING_CONSTANT("Transfer-Encoding: chunked"),
	STRING_CONSTANT("User-Agent"),
	STRING_CONSTANT("Accept"),
	STRING_CONSTANT("Host"),
	STRING_CONSTANT("Content-Disposition"),
	STRING_CONSTANT("Range"),
	STRING_CONSTANT("Content-Range"),
	STRING_CONSTANT("Server-Time"),
    STRING_CONSTANT("If-Match"),
    STRING_CONSTANT("ETag"),
    STRING_CONSTANT("Correlation-ID")
};
