/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-20 jcwu
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"

//all system's set mode
typedef enum
{
	kSystemSetModeDate,
	kSystemSetModeVersion,
}SystemSetMode;
	
typedef enum
{
	kAskEverytime,
	kSetSpeedMode,
	kSetTimeMode,
}InfusionMode;

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


	

void app_set_menu_enter(void);




#ifdef __cplusplus
}
#endif

