/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-04 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

//flush the display cache to lcd
void display_flush(int x0, int y0, int x1, int y1);
//flush the dirty area to lcd
void display_flush_dirty();
//is exist dirty area
bool display_is_dirty();
//set dirty area
void display_set_dirty(int x, int y, int width, int height);

//open display for show, when need show, it will auto open
void display_open();

//close display for reduce power
void display_close();



void display_data_at(int x, int y, const uint8_t *pixels, int pixels_count);
void display_init();


#ifdef __cplusplus
}
#endif