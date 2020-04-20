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
#include "app_popup.h"
#include "screen.h"
/************************************************
* Declaration
************************************************/

#define APP_ALTERNATIVE_POPUP_TITLE_MAX_SIZE 40


typedef void(*AlternativePopupCallback)(PopupReturnCode ret_code, int select_index);

typedef struct
{
	AlternativePopupCallback callback;
	const char* select1_text;
	const char* select2_text;
	Screen* src;
	char title[APP_ALTERNATIVE_POPUP_TITLE_MAX_SIZE];
	int select_index;
}AlternativePopupObj;

//show a alternative popup screen, when press ok or back, 
// it will use the callback to notify user, then delete self
//return the popup screen
Screen* app_alternative_popup_enter(const char* title,
	const char* select1_text, const char* select2_text, int first_select_index,
	AlternativePopupCallback callback);

