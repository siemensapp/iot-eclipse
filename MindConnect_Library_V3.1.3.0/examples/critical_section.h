/*!**********************************************************************
*
* @copyright Copyright (C) 2018 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     critical_section.h
* @date     11 Jan 2018
* @brief    Sample critical section callback functions for MCL.
*
************************************************************************/

#ifndef CRITICAL_SECTION_H
#define CRITICAL_SECTION_H

#include "mcl/mcl.h"

#ifdef  __cplusplus
extern "C"
{
#endif

	void initialize_critical_section(void);
	E_MCL_ERROR_CODE enter_critical_section(void);
	void leave_critical_section(void);

#ifdef  __cplusplus
}
#endif

#endif //CRITICAL_SECTION_H
