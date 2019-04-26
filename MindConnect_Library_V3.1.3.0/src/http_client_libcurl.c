/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     http_client_libcurl.c
 * @date     Jul 20, 2016
 * @brief    HTTP client libcurl implementation file.
 *
 * HTTP client implementation file using LibCurl.
 *
 ************************************************************************/

#include "http_client_libcurl.h"
#include "log_util.h"
#include "memory.h"
#include "definitions.h"

#if (1 == HAVE_OPENSSL_SSL_H_)
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#define CARRIAGE_RETURN '\r'
#define LINE_FEED '\n'
#define DOMAIN_SEPERATOR '\\'

#define SUPPORTED_CIPHERS_LIST \
  "DHE-DSS-AES128-GCM-SHA256:DHE-DSS-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:DHE-RSA-CHACHA20-POLY1305:" \
  "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:"

// Data structure to be passed as an argument to libcurl callback set with CURLOPT_WRITEFUNCTION option.
typedef struct libcurl_payload_t
{
    mcl_uint8_t *data;
    mcl_size_t size;
} libcurl_payload_t;

static mcl_bool_t curl_global_initialized = MCL_FALSE;

static CURLcode _ssl_context_callback(CURL *curl, void *ssl_context, void *certificate);
static mcl_size_t _response_payload_callback(void *received_data, mcl_size_t size, mcl_size_t count, void *response_payload);
static mcl_size_t _response_header_callback(void *received_data, mcl_size_t size, mcl_size_t count, void *response_header);
static mcl_size_t _request_payload_callback_for_put(char *buffer, mcl_size_t size, mcl_size_t count, void *http_request);
static mcl_bool_t _is_empty_line(char *line);
static struct curl_slist *_set_request_options(CURL *curl, http_request_t *http_request, http_client_send_callback_info_t *callback_info);
static int _curl_debug_callback(CURL *curl, curl_infotype info_type, char *data, mcl_size_t size, void *debug_data);
static E_MCL_ERROR_CODE _convert_to_mcl_error_code(CURLcode curl_code);

E_MCL_ERROR_CODE http_client_initialize(configuration_t *configuration, http_client_t **http_client)
{
    DEBUG_ENTRY("configuration_t *configuration = <%p>, http_client_t *http_client = <%p>", configuration, http_client)

    CURL *curl;

    // Create http client.
    MCL_NEW(*http_client);
    ASSERT_CODE_MESSAGE(MCL_NULL != *http_client, MCL_OUT_OF_MEMORY, "Memory can not be allocated for http client.");

    // Initialize curl memory functions
    if (MCL_FALSE == curl_global_initialized)
    {
        curl_global_initialized = MCL_TRUE;
        curl_global_init_mem(CURL_GLOBAL_DEFAULT, memory_malloc, memory_free, memory_realloc, string_util_strdup, memory_calloc);
    }

    // Initialize curl object.
    (*http_client)->curl = curl_easy_init();
    ASSERT_CODE_MESSAGE(MCL_NULL != (*http_client)->curl, MCL_INITIALIZATION_FAIL, "Libcurl easy interface can not be initialized.");
    MCL_DEBUG("Libcurl easy interface is initialized.");

    // Declare a local curl instance for code clarity.
    curl = (*http_client)->curl;

    // Set remote port number to work with.
    curl_easy_setopt(curl, CURLOPT_PORT, configuration->mindsphere_port);

    // Set timeout values.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)configuration->http_request_timeout);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, (long)configuration->http_request_timeout);

    // Set server certificate.
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, SSL_CERTIFICATE_TYPE_PEM);
    curl_easy_setopt(curl, CURLOPT_SSL_CTX_DATA, configuration->mindsphere_certificate);
    curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, *_ssl_context_callback);
    curl_easy_setopt(curl, CURLOPT_SSL_CIPHER_LIST, SUPPORTED_CIPHERS_LIST);
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

    // Verify the server's SSL certificate.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);

    // enable this if MindSphere supports OCSP
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 1);

#if MCL_LOG_UTIL_LEVEL < LOG_UTIL_LEVEL_INFO

    // set logging if enabled and level set at least DEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, _curl_debug_callback);
#endif

    // Make sure header of the response is processed by a different callback from its payload.
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);

    // Set callback for processing the received header.
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _response_header_callback);

    // Set callback for processing the received payload.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _response_payload_callback);

    // The headers specified in CURLOPT_HEADER will be used in requests to both servers and proxies.
    curl_easy_setopt(curl, CURLOPT_HEADEROPT, CURLHEADER_UNIFIED);

    // Close the connection when done with the transfer.
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);

    // Set proxy options if proxy is used.
    if (MCL_NULL != configuration->proxy_hostname)
    {
        curl_easy_setopt(curl, CURLOPT_PROXY, configuration->proxy_hostname->buffer);
        curl_easy_setopt(curl, CURLOPT_PROXYPORT, configuration->proxy_port);
        curl_easy_setopt(curl, CURLOPT_PROXYTYPE, configuration->proxy_type);

        if (MCL_NULL != configuration->proxy_username)
        {
            if (MCL_NULL != configuration->proxy_domain)
            {
                E_MCL_ERROR_CODE code;
                string_t *proxy_username;
                mcl_size_t proxy_username_length = configuration->proxy_domain->length + configuration->proxy_username->length + 1;
                code = string_initialize_new(MCL_NULL, proxy_username_length, &proxy_username);
                ASSERT_CODE_MESSAGE(MCL_NULL != proxy_username, MCL_OUT_OF_MEMORY,
                                    "Memory can not be allocated for the concatenated string which contains domain and username.");

                code = string_util_snprintf(proxy_username->buffer, proxy_username_length + 1, "%s%c%s", configuration->proxy_domain->buffer, DOMAIN_SEPERATOR,
                                            configuration->proxy_username->buffer);
                curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, proxy_username->buffer);
                string_destroy(&proxy_username);
                ASSERT_CODE_MESSAGE(code == MCL_OK, code, "Domain and username strings could not be concatenated.");
            }
            else
            {
                curl_easy_setopt(curl, CURLOPT_PROXYUSERNAME, configuration->proxy_username->buffer);
            }
            curl_easy_setopt(curl, CURLOPT_PROXYPASSWORD, configuration->proxy_password->buffer);
        }
    }

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

E_MCL_ERROR_CODE http_client_send(http_client_t *http_client, http_request_t *http_request, http_client_send_callback_info_t *callback_info, http_response_t **http_response)
{
    DEBUG_ENTRY("http_client_t *http_client = <%p>, http_request_t *http_request = <%p>, http_client_send_callback_info_t *callback_info = <%p>, http_response_t **http_response = <%p>",
                http_client, http_request, callback_info, http_response)

    E_MCL_ERROR_CODE return_code;
    struct curl_slist *request_header_list;
    string_array_t *response_header = MCL_NULL;
    libcurl_payload_t *response_payload;
    CURLcode curl_code;
    mcl_int64_t response_code;

    // Set request options. If there are no request headers, this function returns null but the other options for the request are set anyway.
    request_header_list = _set_request_options(http_client->curl, http_request, callback_info);

    // Initialize a string array to store response header, clear the list of request headers if this initialization fails and return.
    return_code = string_array_initialize(10, &response_header);
    ASSERT_STATEMENT_CODE_MESSAGE(return_code == MCL_OK, curl_slist_free_all(request_header_list), return_code,
                                  "String array for http response header can not be initialized.");

    // Set pointer passed to the _response_header_callback function as fourth argument.
    curl_easy_setopt(http_client->curl, CURLOPT_HEADERDATA, response_header);

    // Set pointer passed to the _response_callback function as fourth argument.
    MCL_NEW(response_payload);
    response_payload->data = MCL_NULL;
    response_payload->size = 0;
    curl_easy_setopt(http_client->curl, CURLOPT_WRITEDATA, response_payload);

    // Perform the transfer.
    MCL_INFO("Sending HTTP request...");

    curl_code = curl_easy_perform(http_client->curl);
    return_code = _convert_to_mcl_error_code(curl_code);
    MCL_INFO("HTTP request sent. Result code = <%u>", return_code);

    // Free the list of http request header.
    curl_slist_free_all(request_header_list);

    if (MCL_OK != return_code)
    {
        string_array_destroy(&response_header);
        MCL_FREE(response_payload->data);
        MCL_FREE(response_payload);
        MCL_ERROR_RETURN(return_code, "curl_easy_perform() failed: %s", curl_easy_strerror(curl_code));
    }

    // Gather response into http_response object.
    curl_easy_getinfo(http_client->curl, CURLINFO_RESPONSE_CODE, &response_code);

    return_code = http_response_initialize(response_header, response_payload->data, response_payload->size, (E_MCL_HTTP_RESULT_CODE)response_code, http_response);
    if (MCL_OK != return_code)
    {
        string_array_destroy(&response_header);
        MCL_FREE(response_payload->data);
        MCL_FREE(response_payload);
        MCL_ERROR_RETURN(return_code, "Http response can not be initialized.");
    }
    MCL_FREE(response_payload);

    DEBUG_LEAVE("retVal = <%d>", MCL_OK);
    return MCL_OK;
}

mcl_size_t http_client_get_callback_termination_code()
{
    DEBUG_ENTRY("void")

    mcl_size_t termination_code = CURL_READFUNC_ABORT;

    DEBUG_LEAVE("retVal = <%u>", termination_code);
    return termination_code;
}

void http_client_destroy(http_client_t **http_client)
{
    DEBUG_ENTRY("http_client_t **http_client = <%p>", http_client)

    if (MCL_NULL != *http_client)
    {
        curl_easy_cleanup((*http_client)->curl);
        MCL_FREE(*http_client);

        MCL_DEBUG("Http client handle is destroyed.");
    }
    else
    {
        MCL_DEBUG("Http client is already null.");
    }

    DEBUG_LEAVE("retVal = void");
}

// This function is the callback which is called to set the SSL certificate given by the parameter "certificate".
static CURLcode _ssl_context_callback(CURL *curl, void *ssl_context, void *certificate)
{
    DEBUG_ENTRY("CURL *curl = <%p>, void *ssl_context = <%p>, void *certificate = <%p>", curl, ssl_context, certificate)

    string_t *certificate_local;
    X509_STORE *store;
    BIO *bio;
    struct stack_st_X509_INFO *certificate_info;
    mcl_size_t index;


    if (MCL_NULL == certificate)
    {
        MCL_INFO("No certificate is provided by the user for peer verification. Continuing with the existing CA certificate store.");
        return CURLE_OK;
    }

    certificate_local = (string_t *)certificate;

    // Get a pointer to the X509 certificate store (which may be empty!).
    store = SSL_CTX_get_cert_store((SSL_CTX *)ssl_context);

    // Get BIO.
    bio = BIO_new_mem_buf(certificate_local->buffer, (int)certificate_local->length);
    ASSERT_CODE_MESSAGE(MCL_NULL != bio, CURLE_OUT_OF_MEMORY, "Memory for BIO could not be allocated!");

    // Read certificate info
    certificate_info = PEM_X509_INFO_read_bio(bio, MCL_NULL, MCL_NULL, MCL_NULL);

    BIO_free(bio);
    ASSERT_CODE_MESSAGE(MCL_NULL != certificate_info, CURLE_SSL_CERTPROBLEM, "Certificate info can not be read from memory");

    // Read all PEM formatted certificates from memory into an X509 structure that SSL can use.
    for (index = 0; index < sk_X509_INFO_num(certificate_info); index++)
    {
        X509_INFO *temp_info = sk_X509_INFO_value(certificate_info, index);
        if (temp_info->x509)
        {
            if (!X509_STORE_add_cert(store, temp_info->x509))
            {
                // Ignore error X509_R_CERT_ALREADY_IN_HASH_TABLE which means the certificate is already in the store.
                // That could happen if libcurl already loaded the certificate from a ca cert bundle set at libcurl build-time or runtime.
                unsigned long error = ERR_peek_last_error();

                if(ERR_GET_LIB(error) != ERR_LIB_X509 || ERR_GET_REASON(error) != X509_R_CERT_ALREADY_IN_HASH_TABLE)
                {
                    MCL_ERROR_RETURN(CURLE_SSL_CERTPROBLEM, "Certificate can not be added to store.");
                }
                else
                {
                    MCL_INFO("Did not add the certificate to store since it is already in the store.");
                }
            }
        }
        else if (temp_info->crl)
        {
            ASSERT_CODE_MESSAGE(0 != X509_STORE_add_crl(store, temp_info->crl), CURLE_SSL_CERTPROBLEM, "Certificate can not be added to store.");
        }
    }

    // Clean up.
    sk_X509_INFO_pop_free(certificate_info, X509_INFO_free);

    DEBUG_LEAVE("retVal = <%d>", CURLE_OK);
    return CURLE_OK;
}

// This function is the callback which is called once for the payload of the received response.
// The function initializes an string_t from the received data in the buffer "received_data"
// composed of "count" elements of each "size" bytes long and copies this string to the string array "response_payload".
static mcl_size_t _response_payload_callback(void *received_data, mcl_size_t size, mcl_size_t count, void *response_payload)
{
    DEBUG_ENTRY("void *received_data = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *response_payload = <%p>", received_data, size, count, response_payload)

    mcl_size_t received_data_size = size * count;
    libcurl_payload_t *payload = (libcurl_payload_t *)response_payload;

    // ASSERT_CODE_MESSAGE(MCL_NULL == payload->data, 0, "Payload data was already allocated!");
    if (payload->data)
    {
        MCL_RESIZE(payload->data, payload->size + received_data_size);
        ASSERT_CODE_MESSAGE(MCL_NULL != payload->data, 0, "Memory re-allocation for payload data failed!");
    }
    else
    {
        payload->data = MCL_MALLOC(received_data_size);
        ASSERT_CODE_MESSAGE(MCL_NULL != payload->data, 0, "Memory allocation for payload data failed!");
    }

    string_util_memcpy(payload->data + payload->size, received_data, received_data_size);

    payload->size += received_data_size;

    DEBUG_LEAVE("retVal = <%d>", received_data_size);
    return received_data_size;
}

// This function is the callback which is called once for every header line of the received http response.
// The function initializes a string_t from the received data in the buffer "received_data"
// composed of "count" elements of each "size" bytes long and copies this string to the string array "response_header".
static mcl_size_t _response_header_callback(void *received_data, mcl_size_t size, mcl_size_t count, void *response_header)
{
    DEBUG_ENTRY("void *received_data = <%p>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *response_header = <%p>", received_data, size, count, response_header)

    string_array_t *header_array = (string_array_t *)response_header;

    string_t *header_line = MCL_NULL;
    mcl_size_t received_data_size = size * count;

    // Eliminate empty line
    if ((2 == received_data_size) && (MCL_TRUE == _is_empty_line((char *)received_data)))
    {
        DEBUG_LEAVE("retVal = <%d>", received_data_size);
        return received_data_size;
    }

    // Create an mcl string for the received header line (CRLF excluded).
    if (MCL_OK != string_initialize_new(received_data, received_data_size - 2, &header_line))
    {
        MCL_ERROR("MCL string can not be initialized for the response header line.");
        return 0;
    }

    // Add header line to header array, destroy header line if the add operation is unsuccessful.
    if (MCL_OK != string_array_add(header_array, header_line, MCL_TRUE))
    {
        string_destroy(&header_line);
        MCL_ERROR("MCL string of response header line can not be added to string array of response header.");
        DEBUG_LEAVE("retVal = <0>");
        return 0;
    }

    DEBUG_LEAVE("retVal = <%d>", received_data_size);
    return received_data_size;
}

// This function is the callback which is called when HTTP PUT is requested.
// The function takes in parameter "http_request" of type http_request_t and copies its payload to "buffer".
static mcl_size_t _request_payload_callback_for_put(char *buffer, mcl_size_t size, mcl_size_t count, void *http_request)
{
    DEBUG_ENTRY("char *buffer = <%s>, mcl_size_t size = <%u>, mcl_size_t count = <%u>, void *http_request = <%p>", buffer, size, count, http_request)

    http_request_t *request = (http_request_t *)http_request;

    mcl_size_t buffer_size = size * count;
    mcl_size_t remaining_size = request->payload_size - request->payload_offset;
    mcl_size_t payload_size = (buffer_size > remaining_size) ? remaining_size : buffer_size;

    // TODO Maximum size check. Shall we check it where we set it CURLOPT_INFILESIZE option or here.
    if (payload_size == 0)
    {
        MCL_DEBUG("Remaining payload size is zero. Nothing will be copied.");
    }
    else
    {
        string_util_memcpy(buffer, request->payload + request->payload_offset, payload_size);
    }
    request->payload_offset += payload_size;

    DEBUG_LEAVE("retVal = <%u>", payload_size);
    return payload_size;
}

// This function checks if the given line is an empty line or not.
static mcl_bool_t _is_empty_line(char *line)
{
    DEBUG_ENTRY("char *line = <%s>", line)

    mcl_bool_t is_empty = (CARRIAGE_RETURN == line[0]) && (LINE_FEED == line[1]) ? MCL_TRUE : MCL_FALSE;

    DEBUG_LEAVE("retVal = <%d>", is_empty);
    return is_empty;
}

// This function sets the options for the http request and returns the curl list of request headers.
static struct curl_slist *_set_request_options(CURL *curl, http_request_t *http_request, http_client_send_callback_info_t *callback_info)
{
    DEBUG_ENTRY("CURL *curl = <%p>, http_request_t *http_request = <%p>, http_client_send_callback_info_t *callback_info = <%p>", curl, http_request, callback_info)

    struct curl_slist *request_header_list = MCL_NULL;
    string_t *request_header_line = MCL_NULL;
	mcl_size_t index;

    // Set the URL to use in the request.
    // Reminder: Request URI is an absolute path including the host name.
    curl_easy_setopt(curl, CURLOPT_URL, http_request->uri->buffer);

    // Set http method and data for post and put methods.
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 0);
    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 0);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE, -1);
    curl_easy_setopt(curl, CURLOPT_READDATA, MCL_NULL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, MCL_NULL);

    switch (http_request->method)
    {
        case MCL_HTTP_GET :
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

            break;

        case MCL_HTTP_POST :
            curl_easy_setopt(curl, CURLOPT_POST, 1);

            // If a callback function is present, use Transfer-Encoding : chunked:
            if (MCL_NULL == callback_info)
            {
                // Normal http transfer without chunked encoding
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (void *)http_request->payload);

                // if post field size is not set, you have to use transfer-encoding:chunked otherwise no data will be send.
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, http_request->payload_size);
            }
            else
            {
                // Transfer-Encoding : chunked
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, callback_info->read_callback);
                curl_easy_setopt(curl, CURLOPT_READDATA, callback_info->user_context);
                request_header_list = curl_slist_append(request_header_list, http_header_names[HTTP_HEADER_TRANSFER_ENCODING_CHUNKED].buffer);
            }

            break;

        case MCL_HTTP_PUT :

            // set payload_offset to 0 to indicate for read callback what part of the payload is already written to libcurl
            http_request->payload_offset = 0;

            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, _request_payload_callback_for_put);
            curl_easy_setopt(curl, CURLOPT_READDATA, http_request);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE, http_request->payload_size);

            // TODO Use CURLOPT_INFILESIZE_LARGE for data larger than 2GB. Shall we check it here?

            break;

        default :
            MCL_ERROR_RETURN(MCL_NULL, "Unsupported HTTP method is requested.");

            break;
    }

    // Set request header.
    for (index = 0; index < http_request->header->index; index++)
    {
        request_header_line = string_array_get(http_request->header, index);
        request_header_list = curl_slist_append(request_header_list, request_header_line->buffer);
    }

    // Add expect header to prevent that libcurl adds it automatically (HTTP continue bug)
    request_header_list = curl_slist_append(request_header_list, "Expect:");

    // Separate HTTP headers for MindSphere and Proxy
    curl_easy_setopt(curl, CURLOPT_HEADEROPT, CURLHEADER_SEPARATE);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_header_list);
    curl_easy_setopt(curl, CURLOPT_PROXYHEADER, MCL_NULL);

    DEBUG_LEAVE("retVal = <%p>", request_header_list);
    return request_header_list;
}

#if MCL_LOG_UTIL_LEVEL < LOG_UTIL_LEVEL_INFO
static int _curl_debug_callback(CURL *curl, curl_infotype info_type, char *data, mcl_size_t size, void *debug_data)
{
    const char *text;

    switch (info_type)
    {
        case CURLINFO_TEXT :
            MCL_DEBUG("== Info: %s", data);
            return CURLE_OK;
        case CURLINFO_HEADER_OUT :
            text = "=> Send header";

            break;
        case CURLINFO_DATA_OUT :
            text = "=> Send data";

            break;
        case CURLINFO_SSL_DATA_OUT :
            text = "=> Send SSL data";

            break;
        case CURLINFO_HEADER_IN :
            text = "<= Recv header";

            break;
        case CURLINFO_DATA_IN :
            text = "<= Recv data";

            break;
        case CURLINFO_SSL_DATA_IN :
            text = "<= Recv SSL data";

            break;
        default :
            return CURLE_OK;
    }

    MCL_DEBUG_MEMORY(data, (const unsigned int)size, "%s, %10.10d bytes (0x%8.8X)\n", text, size, size);
    return CURLE_OK;
}
#endif

static E_MCL_ERROR_CODE _convert_to_mcl_error_code(CURLcode curl_code)
{
    DEBUG_ENTRY("CURLcode curl_code = <%d>", curl_code)

    E_MCL_ERROR_CODE mcl_code;

    switch (curl_code)
    {
        case CURLE_OK :
            mcl_code = MCL_OK;

            break;
        case CURLE_COULDNT_RESOLVE_PROXY :
            mcl_code = MCL_COULD_NOT_RESOLVE_PROXY;

            break;
        case CURLE_COULDNT_RESOLVE_HOST :
            mcl_code = MCL_COULD_NOT_RESOLVE_HOST;

            break;
        case CURLE_COULDNT_CONNECT :
            mcl_code = MCL_COULD_NOT_CONNECT;

            break;
        case CURLE_OUT_OF_MEMORY :
            mcl_code = MCL_OUT_OF_MEMORY;

            break;
        case CURLE_SSL_CONNECT_ERROR :
            mcl_code = MCL_SSL_HANDSHAKE_FAIL;

            break;
        case CURLE_PEER_FAILED_VERIFICATION :
            mcl_code = MCL_SERVER_CERTIFICATE_NOT_VERIFIED;

            break;
        case CURLE_SEND_ERROR :
            mcl_code = MCL_NETWORK_SEND_FAIL;

            break;
        case CURLE_RECV_ERROR :
            mcl_code = MCL_NETWORK_RECEIVE_FAIL;

            break;
        case CURLE_SSL_CERTPROBLEM :
            mcl_code = MCL_IMPROPER_CERTIFICATE;

            break;
        case CURLE_SSL_CACERT :
            mcl_code = MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL;

            break;
        case CURLE_OPERATION_TIMEDOUT  :
            mcl_code = MCL_REQUEST_TIMEOUT;

            break;
        default :
            mcl_code = MCL_FAIL;

            break;
    }

    DEBUG_LEAVE("retVal = <%d>", mcl_code);
    return mcl_code;
}
