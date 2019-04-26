/*!**********************************************************************
 *
 * @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
 *            All rights reserved.
 *
 *************************************************************************
 *
 * @file     mcl_config_setup.h
 * @date     Jun 30, 2016
 * @brief    Configuration setup module interface header file.
 *
 * Configuration setup header for MCL.
 * This configuration setup should be used by every module which requires external libraries.
 *
 ************************************************************************/

#ifndef MCL_CONFIG_SETUP_H_
#define MCL_CONFIG_SETUP_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#if (defined(_WIN32) || defined(__WIN32__) || defined(_WIN32_WINNT)) && !defined(WIN32)
#define WIN32
#endif

#if (defined(_WIN64) || defined(__WIN64__) || defined(_WIN64_WINNT)) && !defined(WIN64)
#define WIN64
#endif

#if MCL_STATICLIB
#define MCL_EXPORT
#elif defined(WIN32) || defined(WIN64)

#  if MCL_BUILD
#    define MCL_EXPORT  __declspec(dllexport)
#  else
#    define MCL_EXPORT  __declspec(dllimport)
#  endif
#else
#  define MCL_EXPORT
#endif

#if MCL_BUILD
#include "mcl_config.h"
#endif

#ifdef  __cplusplus
}
#endif

#endif //MCL_CONFIG_SETUP_H_
