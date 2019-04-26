/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     definitions.h
 * @date     Jul 28, 2016
 * @brief    Definitions module header file.
 *
 * General definitions are stored in this header file.
 *
 ************************************************************************/

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

// MAX value of an mcl_size_t can have :
// The expression below is portable and safe.
#define MCL_SIZE_MAX ((mcl_size_t) - 1)

// MCL_NULL Definition. Refer to C-FAQ Q5.2 - Q5.17 (http://c-faq.com/null/safermacs.html):
#define MCL_NULL ((void *)0)

#define MCL_NULL_CHAR '\0'
#define MCL_NULL_CHAR_SIZE 1

#define NONCE_SIZE 16

// TODO: version size assumed 36 instead of 255 like in document and DEFAULT_ID_SIZE is used. Change later after deciding.
// These sizes come from "Interface Specification MindConnect API V 0.1". Sizes increased by 1 for null termination.
#define DEFAULT_META_TYPE_SIZE                  6
#define DEFAULT_PAYLOAD_TYPE_SIZE               20
#define DEFAULT_QUALITY_CODE_SIZE               9
#define DEFAULT_DATE_SIZE                       23
#define DEFAULT_TIMESTAMP_SIZE                  25
#define DEFAULT_ID_SIZE                         37
#define DEFAULT_VALUES_COUNT                    5

// TODO: value size is 255 in document. if char [255] -> unnecessary memory use:
#define DEFAULT_VALUE_SIZE                      11

#define MAXIMUM_HOST_NAME_LENGTH           256
#define MAXIMUM_PROXY_USER_NAME_LENGTH     32
#define MAXIMUM_PROXY_PASSWORD_LENGTH      32
#define MAXIMUM_PROXY_DOMAIN_LENGTH        256
#define MAXIMUM_USER_AGENT_LENGTH          512

// The practical minimum is about 400 bytes since libcurl uses a buffer of this size as a scratch area.
#define MIN_HTTP_PAYLOAD_SIZE 400

// 16K is default payload size for libcurl since it performs better with this value.
#define DEFAULT_HTTP_PAYLOAD_SIZE 16384

// 300 seconds is default http request timeout value.
#define DEFAULT_HTTP_REQUEST_TIMEOUT (300)

// JWT used in authorization header has an expiration time of 24 hours.
#define JWT_EXPIRATION_TIME 86400

#define MCL_FILE_EXCHANGE_ENABLED 1

#define MCL_ERROR_RETURN_POINTER(return_value, ...) \
    MCL_ERROR(__VA_ARGS__); \
    DEBUG_LEAVE("retVal = <%p>", (return_value)); \
    return (return_value);

#define MCL_ERROR_RETURN(return_value, ...) \
    MCL_ERROR(__VA_ARGS__); \
    DEBUG_LEAVE("retVal = <%d>", (return_value)); \
    return (return_value);

#define ASSERT(condition) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        DEBUG_LEAVE("retVal = <%p>", MCL_NULL); \
        return MCL_NULL; \
    }

#define ASSERT_MESSAGE(condition, ...) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        MCL_ERROR_RETURN_POINTER(MCL_NULL, __VA_ARGS__); \
    }

#define ASSERT_STATEMENT_MESSAGE(condition, statement, ...) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        (statement); \
        MCL_ERROR_RETURN_POINTER(MCL_NULL, __VA_ARGS__); \
    }

#define ASSERT_CODE(condition, return_code) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        DEBUG_LEAVE("retVal = <%d>", (return_code)); \
        return (return_code); \
    }

#define ASSERT_CODE_MESSAGE(condition, return_code, ...) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        MCL_ERROR_RETURN(return_code, __VA_ARGS__); \
    }

#define ASSERT_STATEMENT_CODE(condition, statement, return_code) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        (statement); \
        DEBUG_LEAVE("retVal = <%d>", (return_code)); \
        return (return_code); \
    }

#define ASSERT_STATEMENT_CODE_MESSAGE(condition, statement, return_code, ...) \
    if(!(condition)) \
    { \
        MCL_ERROR_STRING("Assertion failed for condition = <" #condition ">."); \
        (statement); \
        MCL_ERROR_RETURN(return_code, __VA_ARGS__); \
    }

#define ASSERT_NOT_NULL(argument) \
    if(MCL_NULL == argument) \
    { \
        MCL_ERROR_RETURN(MCL_TRIGGERED_WITH_NULL, "Null function argument (" #argument ")."); \
    }

#define STRING_CONSTANT(s) {s, sizeof(s)/sizeof(s[0]) - 1, MCL_STRING_NOT_COPY_NOT_DESTROY}

#endif //DEFINITIONS_H_

