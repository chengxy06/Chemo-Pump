/************************************************
* DESCRIPTION:
* store some screen layout, each screen can select and use the need one
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-15 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "screen.h"

/************************************************
* Declaration
************************************************/

//it have title, left soft key, right soft key
void scr_layout_load_standard(Screen* scr, const char* title, const char* left_soft_key, const char* right_soft_key);

//show screen with a list with one column, the list id is kUI_IDTextList
void scr_layout_load_one_column_list(Screen* scr, const char* title, const char* left_soft_key, const char* right_soft_key);


//it have title, left soft key, right soft key,one process state icon, one progress bar
void scr_layout_load_progress(Screen* scr, const char* title, const char* left_soft_key, const char* right_soft_key);