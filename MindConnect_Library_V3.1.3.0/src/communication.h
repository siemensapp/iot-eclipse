/*!**********************************************************************
*
* @copyright Copyright (C) 2016 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     communication.h
* @date     Sep 26, 2016
* @brief    Communication module header file.
*
* This module implements MCL communication interface.
*
************************************************************************/

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "http_processor.h"

/**
 * This data structure holds information about the state of the MCL library.
 */
typedef struct state_t
{
    mcl_bool_t initialized; //!< Status of MCL initialization.
} state_t;

/**
 * This is the main structure to be used for communication with the server.
 */
typedef struct mcl_communication_t
{
    configuration_t configuration;    //!< Configuration handle.
    http_processor_t *http_processor; //!< Http processor handle.
    state_t state;                    //!< MCL state.
} communication_t;

#endif //COMMUNICATION_H_
