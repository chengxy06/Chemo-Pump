/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-10-24 jlli
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "app_popup.h"
#include "screen.h"

/************************************************
* Declaration
************************************************/
extern int g_factory_password;

typedef struct
{
	Screen* src;
    int*  password;
	PopupCallback callback;
}PasswordPopupObj;

#ifdef __cplusplus
extern "C" {
#endif

Screen* scr_password_popup_enter(int* password, Screen* src,PopupCallback callback);


#ifdef __cplusplus
}
#endif

