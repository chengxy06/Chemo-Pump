/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "screen.h"
/************************************************
* Declaration
************************************************/

#define APP_POPUP_TITLE_MAX_SIZE 40

typedef enum
{
	kPopupWarning, //this popup only return OK
	kPopupError, //this popup only return OK
	kPopupInformation, //this popup only return OK
	kPopupAsk, //this popup return OK or cancel
	kPopupRemind,
}PopupType;

typedef enum
{
	kPopupReturnOK,
	kPopupReturnCancel,
	kPopupReturnWrongPassword,
}PopupReturnCode;

typedef void(*PopupCallback)(PopupReturnCode ret_code);

typedef struct
{
	Screen* src;
	PopupCallback callback;
	const char* show_text;
	char title[APP_POPUP_TITLE_MAX_SIZE];
	int auto_close_time_ms;
	PopupType popup_type;
	bool is_auto_close;
}PopupObj;

//show a popup screen, when press ok or back, 
// it will use the callback to notify user, then delete self
//return the popup screen
Screen* app_popup_enter(PopupType popup_type, const char* title, const char* show_text, PopupCallback callback);

