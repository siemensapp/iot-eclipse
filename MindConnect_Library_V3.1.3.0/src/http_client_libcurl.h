/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_client_libcurl.h
 * @date     Jul 29, 2016
 * @brief    HTTP client libcurl module header file.
 *
 * Implements the functions defined in http_client.h file. This implementation actively uses "libcurl" for HTTP Send/Receive operations.
 * For targets in which libcurl cannot be used, another implementation file should be created to implement the http_client.h functions.
 *
 ************************************************************************/

#ifndef HTTP_CLIENT_LIBCURL_H_
#define HTTP_CLIENT_LIBCURL_H_

#include "http_client.h"

// define CURL_STATICLIB before including curl.h otherwise linker wont be able to find __impl_* functions

#if MCL_STATICLIB
#define CURL_STATICLIB 1
#endif
#include "curl/curl.h"

struct http_client_t
{
    CURL *curl; //!< Curl handle.
};

#endif //HTTP_CLIENT_LIBCURL_H_
