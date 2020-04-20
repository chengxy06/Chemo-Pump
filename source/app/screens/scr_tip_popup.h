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

#define TIP_POPUP_TITLE_MAX_SIZE 40
#define TIP_POPUP_SHOW_TIME_MS 3000

typedef enum
{
	kTipPopupWarning,
	kTipPopupError, 
	kTipPopupInformation, 
	kTipPopupRemind,
}TipPopupType;

typedef void(*TipPopupCallback)();

typedef struct
{
	TipPopupCallback callback;
	const char* show_text;
	char title[TIP_POPUP_TITLE_MAX_SIZE];
	int auto_close_time_ms;//<1: will not auto close
	TipPopupType popup_type;
}TipPopupObj;

//show a tip popup screen, it will close for timer out, press any key or new screen enter
// when close it will use the callback to notify user, then delete self
// the default tip show time is TIP_POPUP_SHOW_TIME_MS
//return the popup screen
Screen* scr_tip_popup_enter(TipPopupType popup_type, const char* title, const char* show_text, TipPopupCallback callback);
Screen* scr_tip_popup_enter_ex(ScreenID screen_id, TipPopupType popup_type, const char* title, const char* show_text, int auto_close_time_ms, TipPopupCallback callback);


