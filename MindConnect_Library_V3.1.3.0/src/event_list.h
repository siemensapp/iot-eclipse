/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     event_list.h
* @date     Apr 13, 2017
* @brief    Event list module header file.
*
* Various operations can be performed for event lists in this module.
*
************************************************************************/

#ifndef EVENT_LIST_H_
#define EVENT_LIST_H_

#include "data_types.h"
#include "event.h"

/**
 * @brief This struct is used for building the event.
 */
typedef struct event_list_t
{
    item_meta_t *meta;             //!< Meta of event list.
    list_t *events;                //!< Payload of event list.
} event_list_t;

/**
 * @brief This function creates and initializes a data struct of event_list.
 *
 * @param [in] version Version of event.
 * @param [out] event_list Created and initialized event_list data struct.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * </ul>
 */
E_MCL_ERROR_CODE event_list_initialize(const char *version, event_list_t **event_list);

/**
 * @brief This function adds @p event to @p event_list.
 *
 * @param [in] event Event handle which is going to be added to @p event_list.
 * @param [in] event_list Which contains events with the same payload type and payload version.
 * @return
 * <ul>
 * <li>#MCL_OK in case of success.</li>
 * <li>#MCL_OUT_OF_MEMORY in case there is not enough memory in the system to proceed.</li>
 * <li>#MCL_LIMIT_EXCEEDED in case @p event_list is full and @p event can not be added to @p event_list.</li>
 * </ul>
 */
E_MCL_ERROR_CODE event_list_add_event(event_t *event, event_list_t *event_list);

/**
 * @brief To destroy the @c event_list data struct.
 *
 * @param [in] event_list Event set handle to be freed.
 */
void event_list_destroy(event_list_t **event_list);

#endif //event_list_H_
