/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_log_util.h
* @date     Jan 30, 2017
* @brief    Log utility module interface header file.
*
* This utility module defines macros and functions for logging purposes.
*
************************************************************************/

#ifndef MCL_LOG_UTIL_H_
#define MCL_LOG_UTIL_H_

#include "mcl/mcl_common.h"
#include <stdarg.h>

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * @brief MCL Log output definitions.
     */
    typedef enum E_LOG_OUTPUT
    {
        E_LOG_OUTPUT_FILE,     //!< Log output channel is file.
        E_LOG_OUTPUT_CALLBACK, //!< Log output channel is callback.
        E_LOG_OUTPUT_SYSLOG,   //!< Log output channel is syslog.
        E_LOG_OUTPUT_STDERR,   //!< Log output channel is stderr.
        E_LOG_OUTPUT_NULL      //!< No log output chosen.
    } E_LOG_OUTPUT;

    /**
     * @brief Log util callback function definition.
     *
     * User's callback can be used with #mcl_log_util_callback_t. Given parameters are @p log_level: Log level of the message,
     * @p tag: Associated tag, @p message: The log message formatted as : Timestamp | PID | Thread ID | Level | Tag | module.function | line | message and
     * @p user_context: User's own context.
     */
    typedef void (*mcl_log_util_callback_t)(int log_level, const char *tag, const char *message, void *user_context);

    /**
     * @brief Sets the global output level. The output level is used during runtime check.
     * No logs are written if given log level is less than this output level.
     *
     * @param [in] log_level The output level to set with.
     * Following log levels can be set.
     * <ul>
     * <li>MCL_LOG_UTIL_LEVEL_VERBOSE.</li>    //!< All other events.
     * <li>MCL_LOG_UTIL_LEVEL_DEBUG.</li>      //!< Minimal set of events that could help to reconstruct the execution path.
     * <li>MCL_LOG_UTIL_LEVEL_INFO.</li>       //!< Happened significant life cycle event or major state transition.
     * <li>MCL_LOG_UTIL_LEVEL_WARN.</li>       //!< happened something that *usually* should not happen and significantly changes application behavior for some period of time.
     * <li>MCL_LOG_UTIL_LEVEL_ERROR.</li>      //!< happened something possible, but highly unexpected. The process is able to recover and continue execution.
     * <li>MCL_LOG_UTIL_LEVEL_FATAL.</li>      //!< Happened something impossible and absolutely unexpected. Process can't continue and must be terminated.
     * <li>MCL_LOG_UTIL_LEVEL_NONE.</li>       //!< None.
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_log_util_set_output_level(const int log_level);

    /**
     * @brief This function initializes logging where output channel is set and configured via variable arguments.
     *
     * Output channel and related variable arguments are as following:
     * <ul>
     * <li>if @p log_output = #E_LOG_OUTPUT_FILE, second argument has to be the file name.</li>
     * @note File name can be a full path. If only name is provided, the log file will be created relative to the executable's working directory.
     * @note Given log file is opened for write operation. Any older log messages will be deleted.
     * @note Given log file remains open until #mcl_log_util_finalize has been called.
     *
     * <li>if @p log_output = #E_LOG_OUTPUT_CALLBACK, second argument is #mcl_log_util_callback_t and third argument is void *user_context</li>
     * <li>if @p log_output = #E_LOG_OUTPUT_SYSLOG, no additional argument is required.</li>
     * <li>if @p log_output = #E_LOG_OUTPUT_STDERR, no additional argument is required.</li>
     * </ul>
     * After initialization any log messages are printed to the chosen output channel.
     *
     * @param [in] log_output Selects the desired output channel.
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_log_util_initialize(E_LOG_OUTPUT log_output, ...);

    /**
     * @brief Performs finalizing operations for logging.
     *
     * @warning
     * <ul>
     * <li>If during initialization a log file is opened, this function must be called in order to close the log file.</li>
     * <li>If @c syslog has been initialized, this function must be called in order to close output to @c syslog.</li>
     * </ul>
     */
    extern MCL_EXPORT void mcl_log_util_finalize(void);

    /**
     * @brief This function converts the given error code to its string value.
     *
     * @param [in] error_code Error code to convert to string.
     * @return Converted error code in string, NULL if @p error_code is invalid. It should not be freed, because its owner is MCL.
     */
    extern MCL_EXPORT const char *mcl_log_util_convert_error_code_to_string(E_MCL_ERROR_CODE error_code);

#ifdef  __cplusplus
}
#endif

#endif //MCL_LOG_UTIL_H_
