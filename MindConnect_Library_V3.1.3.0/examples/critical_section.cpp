/*!**********************************************************************
*
* @copyright Copyright (C) 2018 Siemens Aktiengesellschaft.\n
*            All rights reserved.
*
*************************************************************************
*
* @file     critical_section.cpp
* @date     11 Jan 2018
* @brief    Sample critical section callback functions for MCL.
*
************************************************************************/

#include "Windows.h"
#include "WinBase.h"
#include <stdio.h>
#include "critical_section.h"

static HANDLE _mcl_mutex = NULL;
static unsigned int _is_initialized = 0;

static wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	int stringLength = strlen(charArray);
	wchar_t* wString = new wchar_t[stringLength + 1];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, stringLength + 1);
	return wString;
}

extern "C" void initialize_critical_section(void)
{
	if(!_is_initialized)
	{
		wchar_t *mutexName = convertCharArrayToLPCWSTR("Global\\MultiProcessMCL");
		_mcl_mutex = CreateMutex(NULL, TRUE, mutexName);
		free(mutexName);
		_is_initialized = 1;
	}
}

extern "C" E_MCL_ERROR_CODE enter_critical_section(void)
{
	// Block until mutex is acquired.

	printf("Waiting for MCL mutex.\n");
	DWORD dwWaitResult = WaitForSingleObject(_mcl_mutex, INFINITE);

	printf("Acquired MCL mutex.\n");
	// If we reach here, it means we got the mutex since we used "INFINITE" parameter.
	return MCL_OK;
}

extern "C" void leave_critical_section(void)
{
	ReleaseMutex(_mcl_mutex);
	printf("Released MCL mutex.\n");
}
