/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui.h"

/************************************************
* Declaration
************************************************/
#define ui_rect_xsize(rect) ((rect).x1-(rect).x0 + 1)
#define ui_rect_ysize(rect) ((rect).y1-(rect).y0 + 1)
#define ui_rect_init_by_size(rect,x,y,xsize,ysize) {(rect).x0=(x);(rect).y0=(y); \
	(rect).x1=(x)+(xsize)-1;(rect).y1=(y)+(ysize)-1;}


#ifdef __cplusplus
extern "C" {
#endif

//get the intersect(min),return if intersect
bool ui_rect_intersect(Rect* dest, const Rect* src);
//get the intersect(min) of two rect, return if intersect
bool ui_rect_intersect_to(Rect* dest, const Rect* src1, const Rect* src2);
//get the merged rect which can contain two rect
void ui_rect_merge(Rect* dest, const Rect* src);
void ui_rect_merge_to(Rect* dest, const Rect* src1, const Rect* src2);
void ui_rect_move(Rect* dest, int x_size, int y_size);
void ui_rect_move_to(Rect* dest, int x, int y);
bool ui_rect_is_intersect(const Rect* src1, const Rect* src2);
bool ui_rect_is_intersect_ex(const Rect* src1, int x, int y, int x_size, int y_size);
bool ui_rect_is_include(const Rect* dest, const Rect* src);

#ifdef __cplusplus
}
#endif