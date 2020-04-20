/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "font_data.h"
#include "image_data.h"
#include "ssz_ui_config.h"
#include "ui.h"
#include "ui_utility.h"
/************************************************
* Declaration
************************************************/

#define M_ui_return_if_rect_invalid(x,y,x_size2,y_size2) \
	if(!ui_rect_is_intersect_ex(&g_ui_draw_context->canvas_limit_rect,(x),(y),(x_size2),(y_size2)) )return

#define M_ui_canvas_to_display_dev_pos(xpos,ypos) ((xpos)+=g_ui_draw_context->canvas_rect.x0, \
	(ypos)+=g_ui_draw_context->canvas_rect.y0)
extern UIDrawContext* g_ui_draw_context;

#ifdef __cplusplus
extern "C" {
#endif

//convert to display_dev pos according canvas 
// and the x_size and y_size will change according the canvas limit
//return: true->the rect is valid, false:->the rect is not valid 
//bool ui_to_display_dev_rect(int *x, int *y, int *x_size, int *y_size);

void ui_start_line_cache(int x, int y, uint32_t draw_mode);
void ui_set_pixel_at_line_cache(int x, color_t color);
void ui_set_pixels_at_line_cache(int x, int x_size, color_t color);
void ui_flush_line_cache();

#ifdef __cplusplus
}
#endif