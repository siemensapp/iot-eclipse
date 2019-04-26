/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     event.h
* @date     Sep 26, 2016
* @brief    Event module header file.
*
* Meta and payload parts of event type are initialized and filled in this module.
*
************************************************************************/

#ifndef EVENT_H_
#define EVENT_H_

#include "data_types.h"

/**
 * @brief This struct is used for building a single event.
 */
typedef struct mcl_event_t
{
    item_meta_t *meta;                             //!< Meta of event.
    event_payload_t *payload;                      //!< Payload of event.
} event_t;

/**
 * @brief This function creates and initializes a data struct of event_t.
 *
 * @param [in] meta Meta of event set which is going to be added.
 * @param [in] type Type of event.
 * @param [in] version Version number of @p event.
 * @param [in] severity Severity level.
 * @param [in] timestamp Timestamp of the event in YYYY-MM-DDThh:mm:ss.sssZ format. Ex:2016-04-26T08:06:25.317Z.
 * @param [out] event Event handle which is going to be initialized.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE event_initialize(item_meta_t *meta, const char* type, const char *version, mcl_int32_t severity, const char* timestamp, event_t **event);

/**
 * @brief To destroy the @c event_t data struct.
 *
 * Will release the event_t data struct. After destroy() operation, the data struct shouldn't be used.
 *
 * @param [in] event Stores all field members of event json string.
 */
void event_destroy(event_t **event);

#endif //EVENT_H_
