/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     log_util.c
 * @date     Jun 27, 2016
 * @brief    Log utility module implementation file.
 *
 ************************************************************************/

#include "mcl/mcl_config_setup.h"
#include "log_util.h"
#include "mcl/mcl_log_util.h"
#include "file_util.h"
#include "definitions.h"
#include "memory.h"

#include <stdio.h>
FILE *log_file = NULL;

#if HAVE_SYSLOG_H_
#include <syslog.h>
#endif

#define LOG_UTIL_MCL_SYSLOG "mcl_syslog"

mcl_log_util_callback_t user_callback = MCL_NULL;

// log_output_global is set to E_LOG_OUTPUT_STDERR as default.
E_LOG_OUTPUT log_output_global = E_LOG_OUTPUT_STDERR;

const char *error_code_strings[MCL_ERROR_CODE_END] =
{
	"MCL_OK",
	"MCL_FAIL",
	"MCL_TRIGGERED_WITH_NULL",
	"MCL_OUT_OF_MEMORY",
	"MCL_INVALID_PARAMETER",
	"MCL_NO_FILE_SUPPORT",
	"MCL_NO_STORAGE_MEDIUM",
	"MCL_INVALID_LOG_LEVEL",
	"MCL_OPERATION_IS_NOT_SUPPORTED",
	"MCL_FILE_CANNOT_BE_OPENED",
	"MCL_JSON_NAME_DUPLICATION",
	"MCL_INDEX_OUT_OF_RANGE",
	"MCL_ITEM_ALREADY_EXISTS",
	"MCL_INVALID_EVENT_SEVERITY_LEVEL",
	"MCL_NO_SERVER_TIME",
	"MCL_NO_ACCESS_TOKEN_EXISTS",
	"MCL_INVALID_HOST_NAME",
	"MCL_INVALID_CERTIFICATE",
	"MCL_INVALID_PROXY_HOST_NAME",
	"MCL_INVALID_PROXY_USER_NAME",
	"MCL_INVALID_PROXY_PASSWORD",
	"MCL_INVALID_PROXY_DOMAIN",
	"MCL_INVALID_SECURITY_PROFILE",
	"MCL_INVALID_USER_AGENT",
	"MCL_INVALID_TENANT",
	"MCL_INVALID_MAX_HTTP_PAYLOAD_SIZE",
	"MCL_NO_ACCESS_TOKEN_PROVIDED",
	"MCL_PATH_NOT_ACCESSIBLE",
	"MCL_COULD_NOT_RESOLVE_PROXY",
	"MCL_COULD_NOT_RESOLVE_HOST",
	"MCL_COULD_NOT_CONNECT",
	"MCL_SSL_HANDSHAKE_FAIL",
	"MCL_NETWORK_SEND_FAIL",
	"MCL_NETWORK_RECEIVE_FAIL",
	"MCL_SERVER_CERTIFICATE_NOT_VERIFIED",
	"MCL_CA_CERTIFICATE_AUTHENTICATION_FAIL",
	"MCL_IMPROPER_CERTIFICATE",
	"MCL_REQUEST_TIMEOUT",
	"MCL_SERVER_FAIL",
	"MCL_CREATED",
	"MCL_PARTIAL_CONTENT",
	"MCL_BAD_REQUEST",
	"MCL_UNAUTHORIZED",
	"MCL_FORBIDDEN",
	"MCL_NOT_FOUND",
	"MCL_CONFLICT",
	"MCL_PRECONDITION_FAIL",
	"MCL_REQUEST_PAYLOAD_TOO_LARGE",
	"MCL_UNEXPECTED_RESULT_CODE",
	"MCL_NOT_INITIALIZED",
	"MCL_NOT_ONBOARDED",
	"MCL_ALREADY_ONBOARDED",
	"MCL_STORE_IS_EMPTY",
	"MCL_EXCHANGE_STREAMING_IS_ACTIVE",
	"MCL_SECURITY_UP_TO_DATE",
	"MCL_CANNOT_ENTER_CRITICAL_SECTION",
	"MCL_INITIALIZATION_FAIL",
	"MCL_STORE_ITEM_EXCEEDS_MAX_HTTP_REQUEST_SIZE",
	"MCL_UNDEFINED_ENDPOINT",
	"MCL_BAD_CONTENT_ENCODING",
	"MCL_NON_EXISTING_JSON_CHILD",
	"MCL_SHA256_CALCULATION_FAIL",
	"MCL_SERVER_PROOF_FAIL",
	"MCL_REGISTRATION_INFO_IS_NOT_SAVED",
	"MCL_REGISTRATION_INFO_IS_NOT_LOADED",
	"MCL_BUFFER_OVERFLOW_ERROR",
	"MCL_HTTP_REQUEST_NO_MORE_SPACE",
	"MCL_LIMIT_EXCEEDED",
	"MCL_ARRAY_IS_EMPTY",
	"MCL_PARTIALLY_WRITTEN",
	"MCL_HTTP_REQUEST_FINALIZE_FAILED"
};

#if (1 == HAVE_SYSLOG_H_)
static int log_util_convert_to_syslog_level(const int lvl)
{
    switch (lvl)
    {
        case ZF_LOG_VERBOSE :
            return LOG_DEBUG;
        case ZF_LOG_DEBUG :
            return LOG_DEBUG;
        case ZF_LOG_INFO :
            return LOG_INFO;
        case ZF_LOG_WARN :
            return LOG_WARNING;
        case ZF_LOG_ERROR :
            return LOG_ERR;
        case ZF_LOG_FATAL :
            return LOG_EMERG;
        default :
            return LOG_EMERG;
    }
}
#endif

static void log_util_default_callback(const zf_log_message * const message, void *user_context)
{
    switch (log_output_global)
    {
        case E_LOG_OUTPUT_FILE :
            if (MCL_NULL != log_file)
            {
                // Terminate message with new line
                *message->p = '\n';

                file_util_fwrite_without_log(message->buf, message->p - message->buf + 1, 1, log_file);

                file_util_fflush_without_log(log_file);
            }

            break;
        case E_LOG_OUTPUT_CALLBACK :
            if (MCL_NULL != user_callback)
            {
                user_callback(message->lvl, message->tag, message->buf, user_context);
            }

            break;
        case E_LOG_OUTPUT_SYSLOG :

        #if (1 == HAVE_SYSLOG_H_)

            // Terminate message string
            *message->p = '\0';

            syslog(log_util_convert_to_syslog_level(message->lvl), "%s", message->buf);
        #endif

            break;
        case E_LOG_OUTPUT_STDERR :
            zf_log_out_stderr_callback(message, user_context);

            break;
    }
}

E_MCL_ERROR_CODE mcl_log_util_set_output_level(const int log_level)
{
    if (((LOG_UTIL_LEVEL_VERBOSE <= log_level) && (log_level <= LOG_UTIL_LEVEL_FATAL)) || (LOG_UTIL_LEVEL_NONE == log_level))
    {
        zf_log_set_output_level(log_level);
        return MCL_OK;
    }

    return MCL_INVALID_LOG_LEVEL;
}

E_MCL_ERROR_CODE mcl_log_util_initialize(E_LOG_OUTPUT log_output, ...)
{
	va_list valist;
	void *user_context = MCL_NULL;
	
	log_output_global = log_output;

    va_start(valist, log_output);

    if (E_LOG_OUTPUT_FILE == log_output_global)
    {
        // Get file name as variable argument.
        if (MCL_OK != file_util_fopen_without_log(va_arg(valist, char*), "w", (void **)&log_file))
        {
            log_file = NULL;
            va_end(valist);
            return MCL_FILE_CANNOT_BE_OPENED;
        }
    }
    else if (E_LOG_OUTPUT_SYSLOG == log_output_global)
    {
    #if (1 == HAVE_SYSLOG_H_)
        openlog(LOG_UTIL_MCL_SYSLOG, LOG_PERROR, LOG_USER);
    #else
        va_end(valist);
        return MCL_INVALID_PARAMETER;
    #endif
    }
    else if (E_LOG_OUTPUT_CALLBACK == log_output_global)
    {
        user_callback = (mcl_log_util_callback_t)va_arg(valist, void*);
        user_context = va_arg(valist, void*);
    }
    else
    {
        va_end(valist);
        return MCL_INVALID_PARAMETER;
    }

    zf_log_set_output_v(ZF_LOG_PUT_STD, user_context, log_util_default_callback);

    va_end(valist);
    return MCL_OK;
}

void mcl_log_util_finalize(void)
{
    if (E_LOG_OUTPUT_FILE == log_output_global)
    {
        file_util_fclose_without_log(log_file);
    }
    else if (E_LOG_OUTPUT_CALLBACK == log_output_global)
    {
        user_callback = MCL_NULL;
    }
    else if (E_LOG_OUTPUT_SYSLOG == log_output_global)
    {
    #if (1 == HAVE_SYSLOG_H_)
        closelog();
    #endif
    }

    log_output_global = E_LOG_OUTPUT_NULL;
}

const char *mcl_log_util_convert_error_code_to_string(E_MCL_ERROR_CODE error_code)
{
    DEBUG_ENTRY("E_MCL_ERROR_CODE error_code = <%d>", error_code)

    const char *error_string = MCL_NULL;
    if(MCL_OK <= error_code && error_code < MCL_ERROR_CODE_END)
    {
        error_string = error_code_strings[error_code];
    }
    else
    {
        MCL_ERROR("Given code is invalid.");
    }

    DEBUG_LEAVE("retVal = <%p>", error_string);
    return error_string;
}
