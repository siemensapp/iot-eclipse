/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     log_util.h
 * @date     Jun 27, 2016
 * @brief    Log utility module header file.
 *
 * This utility module defines macros and functions for logging purposes.
 *
 ************************************************************************/

#ifndef LOG_UTIL_H_
#define LOG_UTIL_H_

// Definition of default and module specific log level.
// Modules log level overrides the default log level.
// This log level is set during compile time check and has no runtime overhead.

#if defined(MCL_LOG_UTIL_LEVEL)
#define ZF_LOG_LEVEL MCL_LOG_UTIL_LEVEL
#elif defined(LOG_UTIL_DEFAULT_LEVEL)
#define ZF_LOG_DEF_LEVEL LOG_UTIL_DEFAULT_LEVEL
#else

#ifdef NDEBUG
#define MCL_LOG_UTIL_LEVEL LOG_UTIL_LEVEL_INFO
#else
#define MCL_LOG_UTIL_LEVEL LOG_UTIL_LEVEL_INFO
#endif

#define ZF_LOG_LEVEL MCL_LOG_UTIL_LEVEL

#endif

// Definition of logging tags
#define LOG_UTIL_TAG_DEFAULT "[..]"
#define LOG_UTIL_TAG_ENTRY "[->]"
#define LOG_UTIL_TAG_LEAVE "[<-]"
#define ZF_LOG_DEF_TAG LOG_UTIL_TAG_DEFAULT

#include "zf_log/zf_log.h"

#define LOG_UTIL_LEVEL_VERBOSE ZF_LOG_VERBOSE
#define LOG_UTIL_LEVEL_DEBUG   ZF_LOG_DEBUG
#define LOG_UTIL_LEVEL_INFO    ZF_LOG_INFO
#define LOG_UTIL_LEVEL_WARN    ZF_LOG_WARN
#define LOG_UTIL_LEVEL_ERROR   ZF_LOG_ERROR
#define LOG_UTIL_LEVEL_FATAL   ZF_LOG_FATAL
#define LOG_UTIL_LEVEL_NONE    ZF_LOG_NONE

// Definition of logging macros
// MCL_VERBOSE
#define MCL_VERBOSE(...) ZF_LOGV(__VA_ARGS__)
#define MCL_VERBOSE_STRING(data) ZF_LOGV_STR((data))
#define MCL_VERBOSE_MEMORY(data, data_size, ...) ZF_LOGV_MEM(data, data_size, __VA_ARGS__)

#if ZF_LOG_ENABLED(LOG_UTIL_LEVEL_VERBOSE)
#define VERBOSE_ENTRY(...) ZF_LOG_WRITE(LOG_UTIL_LEVEL_VERBOSE, LOG_UTIL_TAG_ENTRY, __VA_ARGS__);
#define VERBOSE_LEAVE(...) ZF_LOG_WRITE(LOG_UTIL_LEVEL_VERBOSE, LOG_UTIL_TAG_LEAVE, __VA_ARGS__)
#else
#define VERBOSE_ENTRY(...)
#define VERBOSE_LEAVE(...) _ZF_LOG_UNUSED(__VA_ARGS__)
#endif

// MCL_DEBUG
#define MCL_DEBUG(...) ZF_LOGD(__VA_ARGS__)
#define MCL_DEBUG_STRING(data) ZF_LOGD_STR((data))
#define MCL_DEBUG_MEMORY(data, data_size, ...) ZF_LOGD_MEM(data, data_size, __VA_ARGS__)

#if ZF_LOG_ENABLED(LOG_UTIL_LEVEL_DEBUG)
#define DEBUG_ENTRY(...) ZF_LOG_WRITE(LOG_UTIL_LEVEL_DEBUG, LOG_UTIL_TAG_ENTRY, __VA_ARGS__);
#define DEBUG_LEAVE(...) ZF_LOG_WRITE(LOG_UTIL_LEVEL_DEBUG, LOG_UTIL_TAG_LEAVE, __VA_ARGS__)
#define DEBUG_MEMORY_ENTRY(...) ZF_LOG_WRITE_MEM(LOG_UTIL_LEVEL_DEBUG, LOG_UTIL_TAG_ENTRY, __VA_ARGS__)
#define DEBUG_MEMORY_LEAVE(...) ZF_LOG_WRITE_MEM(LOG_UTIL_LEVEL_DEBUG, LOG_UTIL_TAG_LEAVE, __VA_ARGS__)
#else
#define DEBUG_ENTRY(...)
#define DEBUG_LEAVE(...) _ZF_LOG_UNUSED(__VA_ARGS__)
#define DEBUG_MEMORY_ENTRY(...) _ZF_LOG_UNUSED(__VA_ARGS__)
#define DEBUG_MEMORY_LEAVE(...) _ZF_LOG_UNUSED(__VA_ARGS__)
#endif

// MCL_INFO
#define MCL_INFO(...) ZF_LOGI(__VA_ARGS__)
#define MCL_INFO_STRING(data) ZF_LOGI_STR((data))
#define MCL_INFO_MEMORY(data, data_size, ...) ZF_LOGI_MEM(data, data_size, __VA_ARGS__)

// MCL_WARN
#define MCL_WARN(...) ZF_LOGW(__VA_ARGS__)
#define MCL_WARN_STRING(data) ZF_LOGW_STR((data))
#define MCL_WARN_MEMORY(data, data_size, ...) ZF_LOGW_MEM(data, data_size, __VA_ARGS__)

// ERROR
#define MCL_ERROR(...) ZF_LOGE(__VA_ARGS__)
#define MCL_ERROR_STRING(data) ZF_LOGE_STR((data))
#define MCL_ERROR_MEMORY(data, data_size, ...) ZF_LOGE_MEM(data, data_size, __VA_ARGS__)

// MCL_FATAL
#define MCL_FATAL(...) ZF_LOGF(__VA_ARGS__)
#define MCL_FATAL_STRING(data) ZF_LOGF_STR((data))
#define MCL_FATAL_MEMORY(data, data_size, ...) ZF_LOGF_MEM(data, data_size, __VA_ARGS__)

#endif //LOG_UTIL_H_

