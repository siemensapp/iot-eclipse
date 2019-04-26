/*!**********************************************************************
*
* @copyright Copyright (C) 2018 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     callbacks.h
* @date     11 Jan 2018
* @brief    Sample callback functions for MCL.
*
************************************************************************/

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include "mcl/mcl.h"

E_MCL_ERROR_CODE custom_load_function(char **client_id, char **client_secret, char **registration_access_token, char **registration_uri);
E_MCL_ERROR_CODE custom_save_function(const char *client_id, const char *client_secret, const char *registration_access_token, const char *registration_uri);

#endif //CALLBACKS_H_
