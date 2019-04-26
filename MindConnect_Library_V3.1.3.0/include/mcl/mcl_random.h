/*!**********************************************************************
*
* @copyright Copyright (C) 2017 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_random.h
* @date     Dec 6, 2017
* @brief    Random number generation module interface header file.
*
************************************************************************/

#ifndef MCL_RANDOM_H_
#define MCL_RANDOM_H_

#include "mcl/mcl_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * @brief Generates globally unique identifier.
     *
     * @param [out] guid Random guid.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_FAIL in case of an internal error in random number generation.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_random_generate_guid(char **guid);

#ifdef  __cplusplus
}
#endif

#endif //MCL_RANDOM_H_
