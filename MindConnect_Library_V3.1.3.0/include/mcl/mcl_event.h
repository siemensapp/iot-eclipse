/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     mcl_event.h
* @date     Jul 27, 2016
* @brief    Event module interface header file.
*
************************************************************************/

#ifndef MCL_EVENT_H_
#define MCL_EVENT_H_

#include "mcl/mcl_common.h"

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     * @brief This struct is used for building only one message of event.
     */
    typedef struct mcl_event_t mcl_event_t;

    /**
     * @brief This function is used to set optional payload fields of event.
     *
     * @param [in] event Event to set its option.
     * @param [in] option One of the options listed in #E_MCL_EVENT_OPTION.
     * @param [in] value New value of the @p option.
     * @return
     * <ul>
     * <li>#MCL_OK in case of success.</li>
     * <li>#MCL_TRIGGERED_WITH_NULL if provided @p event or @p value is NULL.</li>
     * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
     * <li>#MCL_INVALID_PARAMETER if provided @p option is invalid.</li>
     * </ul>
     */
    extern MCL_EXPORT E_MCL_ERROR_CODE mcl_event_set_option(mcl_event_t *event, E_MCL_EVENT_OPTION option, const void *value);

#ifdef  __cplusplus
}
#endif

#endif //MCL_EVENT_H_
