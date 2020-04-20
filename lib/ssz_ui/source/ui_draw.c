#include "ui.h"
#include "ui_internal.h"
#include "ssz_ui_config.h"
#include "ssz_common.h"
#include "ui_utility.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
const UIDrawContext g_ui_draw_default_context = {
	UI_DRAW_NORMAL,//int draw_mode;
	UI_BLACK,//color_t bk_color;
	1,//int pen_size;
	UI_WHITE,//color_t pen_color;
	UI_WHITE,//color_t fill_color;
	NULL,//Font* font;
	UI_TEXT_NORMAL | UI_TEXT_ALIGN_LEFT | UI_TEXT_ALIGN_VCENTER,//int text_mode;
	UI_WHITE,//color_t text_color;
	0,//int char_space;
	0,//int line_space;
	{ 0,0,DISPLAY_WIDTH-1 ,DISPLAY_HEIGHT-1 },//Rect canvas_rect;
	{ 0,0,DISPLAY_WIDTH-1 ,DISPLAY_HEIGHT-1}//Rect canvas_limit_rect;
};

UIDrawContext g_ui_draw_context_work;// = g_ui_draw_default_context;
UIDrawContext g_ui_draw_context_tmp;
UIDrawContext *g_ui_draw_context = &g_ui_draw_context_work;
bool g_ui_draw_is_use_tmp_context = false;

//cache one line
static uint8_t g_ui_one_line_cache[DISPLAY_WIDTH*DISPLAY_BIT_PER_PIXEL/8];
static int g_ui_line_cache_x_size;
static int g_ui_line_cache_x;
static int g_ui_line_cache_x_min;
static int g_ui_line_cache_x_max;
static int g_ui_line_cache_y;
static int g_ui_line_cache_y_min;
static int g_ui_line_cache_y_max;
static uint32_t g_ui_line_cache_mode;
static bool g_ui_line_cache_is_valid;
/************************************************
* Function 
************************************************/

/************************************************
* Get/Set common draw Attributes
************************************************/
//it not affect text mode
uint32_t ui_draw_mode()
{
	return g_ui_draw_context->draw_mode;
}
void ui_set_draw_mode(uint32_t draw_mode)
{
	g_ui_draw_context->draw_mode = draw_mode;
}
//get background color
color_t ui_bk_color()
{
	return g_ui_draw_context->bk_color;
}
//set background color
void ui_set_bk_color(color_t color)
{
	g_ui_draw_context->bk_color = color;
}

//all draw can only draw at this canvas,
//draw's x and y pos is base this canvas's left top point
//it also set the limit rect same as canvas rect
void ui_canvas_rect(Rect* rect)
{
	*rect = g_ui_draw_context->canvas_rect;
}
// x: xpos at display_dev
void ui_set_canvas_rect(int x, int y, int x_size, int y_size)
{
	Rect rc = {x,y,x+x_size-1,y+y_size-1};
	ui_set_canvas_rect_ex(&rc);
}
void ui_set_canvas_rect_ex(const Rect * rect)
{
	g_ui_draw_context->canvas_rect = *rect;

	g_ui_draw_context->canvas_limit_rect = g_ui_draw_context->canvas_rect;

	Rect display_dev = { 0,0,DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1 };
	if (!ui_rect_intersect(&g_ui_draw_context->canvas_limit_rect, &display_dev)) {
		ssz_mem_zero(&g_ui_draw_context->canvas_limit_rect, sizeof(g_ui_draw_context->canvas_limit_rect));
	}
}
// it must in canvas
// you can not draw out of limit rect
// x: xpos at display_dev
void ui_set_limit_rect_of_canvas(int x, int y, int x_size, int y_size)
{
	Rect rc = { x,y,x + x_size - 1,y + y_size - 1 };
	ui_set_limit_rect_of_canvas_ex(&rc);
}
void ui_set_limit_rect_of_canvas_ex(const Rect * rect)
{
	bool is_right = false;
	g_ui_draw_context->canvas_limit_rect = *rect;
	if (ui_rect_intersect(&g_ui_draw_context->canvas_limit_rect, &g_ui_draw_context->canvas_rect)) {
		Rect display_dev = { 0,0,DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1 };
		if (ui_rect_intersect(&g_ui_draw_context->canvas_limit_rect, &display_dev)) {
			is_right = true;
		}
	}

	if (!is_right) {
		ssz_mem_zero(&g_ui_draw_context->canvas_limit_rect, sizeof(g_ui_draw_context->canvas_limit_rect));
	}

}
void ui_limit_rect_of_canvas(Rect * rect)
{
	*rect = g_ui_draw_context->canvas_limit_rect;
}
void ui_set_default_draw_context()
{
	memcpy(&g_ui_draw_context_work, &g_ui_draw_default_context, sizeof(g_ui_draw_context_work));
}
//get the draw context
void ui_draw_context(UIDrawContext* out_context)
{
	*out_context = *g_ui_draw_context;
}
//set the draw context
void ui_set_draw_context(UIDrawContext* in_context)
{
	*g_ui_draw_context = *in_context;
}

void ui_backup_and_set_default_draw_context()
{
	ssz_assert(g_ui_draw_is_use_tmp_context == false);
	g_ui_draw_is_use_tmp_context = true;
	g_ui_draw_context = &g_ui_draw_context_tmp;
	memcpy(g_ui_draw_context, &g_ui_draw_default_context, sizeof(*g_ui_draw_context));
}

void ui_restore_draw_context()
{
	g_ui_draw_is_use_tmp_context = false;
	g_ui_draw_context = &g_ui_draw_context_work;
}

//convert to display_dev pos according canvas 
// and the x_size and y_size will change according the canvas limit
//return: true->the rect is valid, false:->the rect is not valid 
//bool ui_to_display_dev_rect(int *x, int *y, int *x_size, int *y_size)
//{
//	if(*x_size>g_ui_draw_context->canvas_rect.x_size){
//		*x_size = g_ui_draw_context->canvas_rect.x_size;	
//	}
//	if(*y_size>g_ui_draw_context->canvas_rect.y_size){
//		*y_size = g_ui_draw_context->canvas_rect.y_size;	
//	}
//	*x += g_ui_draw_context->canvas_rect.x;
//	*y += g_ui_draw_context->canvas_rect.y;
//
//	if(*x>=DISPLAY_WIDTH || *x<0 || *y>=DISPLAY_HEIGHT || *y<0){
//		//ssz_assert_fail();
//		return false;	
//	}
//	if (*x_size<=0 || *y_size<=0) {
//		//ssz_assert_fail();
//		return false;
//	}
//
//	return true;
//}
/************************************************
* draw 
************************************************/
//common function, affect draw, img, text
//void ui_move_to(int x, int y)
//{
//	int x_size = 1;
//	int y_size = 1;
//	if (ui_to_display_dev_rect(&x, &y, &x_size, &y_size)) {
//		g_ui_draw_pos.x = x;
//		g_ui_draw_pos.y = y;
//	}
//
//}

//draw function
color_t ui_pen_color()
{
	return g_ui_draw_context->pen_color;
}
void ui_set_pen_color(color_t color)
{
	g_ui_draw_context->pen_color = color;
}

color_t ui_fill_color()
{
	return g_ui_draw_context->fill_color;
}
void ui_set_fill_color(color_t color)
{
	g_ui_draw_context->fill_color = color;
}
int ui_pen_size()
{
	return g_ui_draw_context->pen_size;
}
void ui_set_pen_size(int pen_size)
{
	if (pen_size<1) {
		pen_size = 1;
	}
	g_ui_draw_context->pen_size = pen_size;
}

//convert the color to no alpha and use the draw mode
//the x and y is display_dev pos(not relate to canvas)
color_t ui_convet_color_to_real(color_t color, int x_of_display_dev, int y_of_display_dev, int draw_mode) 
{
	if ((color&0xFF000000)!=0) {
		color_t curr_color;
		curr_color = ui_dev_color_to_common_color(ui_read_pixel_from_dev(x_of_display_dev, y_of_display_dev));
		uint8_t r, g, b;
		r = (UI_COLOR_R(color)*(255 - UI_COLOR_ALPHA(color)) +
			UI_COLOR_R(curr_color)*UI_COLOR_ALPHA(curr_color)) / 255;
		g = (UI_COLOR_G(color)*(255 - UI_COLOR_ALPHA(color)) +
			UI_COLOR_G(curr_color)*UI_COLOR_ALPHA(curr_color)) / 255;
		b = (UI_COLOR_B(color)*(255 - UI_COLOR_ALPHA(color)) +
			UI_COLOR_B(curr_color)*UI_COLOR_ALPHA(curr_color)) / 255;
		color = UI_RGB(r, g, b);
	}
	if (draw_mode&UI_DRAW_REVERSE) {
		color = (~color)&0x00FFFFFF;
	}

	return color;
}


//it use fixed 1 width pen color
void ui_draw_pixel_at(int x, int y)
{
	//int dev_color;
	M_ui_canvas_to_display_dev_pos(x, y);
	M_ui_return_if_rect_invalid(x, y, 1, 1);
	//dev_color = ui_common_color_to_dev_color(ui_convet_color_to_real(g_ui_draw_context->pen_color, x, y,
	//	g_ui_draw_context->draw_mode));
	//ui_draw_to_dev(x, y, (uint8_t*)&dev_color, 1);
	ui_start_line_cache(x, y, g_ui_draw_context->draw_mode);
	ui_set_pixel_at_line_cache(x, g_ui_draw_context->pen_color);
	ui_flush_line_cache();
}
//draw ___(horizontal)  line with pen color
void ui_draw_hline_at(int x, int y, int x_size)
{
	int y_size = g_ui_draw_context->pen_size;
	M_ui_canvas_to_display_dev_pos(x, y);
	M_ui_return_if_rect_invalid(x, y, x_size, y_size);
	//draw each line
	for (int line = 0; line < y_size; ++line) {
		ui_start_line_cache(x, y + line, g_ui_draw_context->draw_mode);
		//draw one line
		ui_set_pixels_at_line_cache(x, x_size, g_ui_draw_context->pen_color);
		ui_flush_line_cache();
	}
}
//draw |(vertical) line with pen color
void ui_draw_vline_at(int x, int y, int y_size)
{
	int x_size = g_ui_draw_context->pen_size;
	M_ui_canvas_to_display_dev_pos(x, y);
	M_ui_return_if_rect_invalid(x, y, x_size, y_size);
	//draw each line
	for (int line = 0; line < y_size; ++line) {
		ui_start_line_cache(x, y + line, g_ui_draw_context->draw_mode);
		//draw one line
		ui_set_pixels_at_line_cache(x,x_size, g_ui_draw_context->pen_color);
		ui_flush_line_cache();
	}
}
//draw the rect with pen and fill color(stroke use pen color, fill use fill color)
void ui_draw_rect_at(int x, int y, int x_size, int y_size, UIDrawType draw_type)
{
	M_ui_canvas_to_display_dev_pos(x, y);
	M_ui_return_if_rect_invalid(x, y, x_size, y_size);
	int border_size = g_ui_draw_context->pen_size;
	int content_xsize = x_size;
	//int content_ysize = y_size;
	switch (draw_type) {
	case kUIDrawStroke:
		content_xsize = 0;
		//content_ysize = 0;
		break;
	case kUIDrawFill:
		border_size = 0;
		break;
	case kUIDrawStrokeAndFill:
		content_xsize -= border_size * 2;
		//content_ysize -= border_size * 2;
		break;
	default:
		ssz_assert_fail();
		break;
	}
	if (draw_type==kUIDrawFill) {
		//int stop_watch_index;
		for (int line = 0; line < y_size; ++line) {
			//stop_watch_index = stop_watch_child_start("one line");
			ui_start_line_cache(x, y + line, g_ui_draw_context->draw_mode);
			//draw each line
			ui_set_pixels_at_line_cache(x, x_size, g_ui_draw_context->fill_color);
			ui_flush_line_cache();
			//stop_watch_child_stop(stop_watch_index);
		}
		return;
	}
	//int stop_watch_index;
	int right_board_start_x = x + x_size - border_size;
	int content_start_x = x + border_size;
	int bottom_board_start_y = y_size - border_size;
	for (int line = 0; line < y_size; ++line) {
		//stop_watch_index = stop_watch_child_start("one line");
		ui_start_line_cache(x, y + line, g_ui_draw_context->draw_mode);
		if (line < border_size || line >= bottom_board_start_y) {
			//draw top and bottom border
			ui_set_pixels_at_line_cache(x,x_size, g_ui_draw_context->pen_color);
		}
		else {
			if (border_size>0) {
				//draw left border
				ui_set_pixels_at_line_cache(x, border_size, g_ui_draw_context->pen_color);
				//draw right border
				ui_set_pixels_at_line_cache(right_board_start_x, border_size, g_ui_draw_context->pen_color);
			}
			if (content_xsize>0) {
				//draw content
				ui_set_pixels_at_line_cache(content_start_x, content_xsize, g_ui_draw_context->fill_color);
			}
		}
		ui_flush_line_cache();
		//stop_watch_child_stop(stop_watch_index);
	}
}
void ui_draw_rect_ex(const Rect *rect, UIDrawType draw_type)
{
	ui_draw_rect_at(rect->x0, rect->y0, ui_rect_xsize(*rect), ui_rect_ysize(*rect), draw_type);
}
//fill use fill color
void ui_fill_rect_at(int x, int y, int x_size, int y_size)
{
	ui_draw_rect_at(x, y, x_size, y_size, kUIDrawFill);
}
void ui_fill_rect_ex(const Rect *rect)
{
	ui_draw_rect_at(rect->x0, rect->y0, ui_rect_xsize(*rect), ui_rect_ysize(*rect), kUIDrawFill);
}

//fill rect by the background color
void ui_clear_rect(int x, int y, int x_size, int y_size) {
	M_ui_canvas_to_display_dev_pos(x, y);
	//draw each line
	for (int line = 0; line < y_size; ++line) {
		ui_start_line_cache(x, y+line, g_ui_draw_context->draw_mode);
		//draw one line
		ui_set_pixels_at_line_cache(x,x_size, g_ui_draw_context->bk_color);
		ui_flush_line_cache();
	}
}
void ui_clear_rect_ex(const Rect *rect)
{
	ui_clear_rect(rect->x0, rect->y0, ui_rect_xsize(*rect), ui_rect_ysize(*rect));
}
//this will clear the canvas by the background color
void ui_clear()
{
	ui_clear_rect(0,0, ui_rect_xsize(g_ui_draw_context->canvas_rect), ui_rect_ysize(g_ui_draw_context->canvas_rect));
}

//the x and y should based on display_dev
void ui_start_line_cache(int x, int y, uint32_t draw_mode) {
	g_ui_line_cache_mode = draw_mode;
	g_ui_line_cache_x_size = 0;
	g_ui_line_cache_x = x;
	g_ui_line_cache_y = y;
	g_ui_line_cache_is_valid = true;

	g_ui_line_cache_x_min = g_ui_draw_context->canvas_limit_rect.x0;
	g_ui_line_cache_x_max = g_ui_draw_context->canvas_limit_rect.x1;
	g_ui_line_cache_y_min = g_ui_draw_context->canvas_limit_rect.y0;
	g_ui_line_cache_y_max = g_ui_draw_context->canvas_limit_rect.y1;
	//check if x is valid
	if ( x > g_ui_line_cache_x_max) {
		g_ui_line_cache_is_valid = false;
	}
	
	//check if the y is valid
	if (y<g_ui_line_cache_y_min || y>g_ui_line_cache_y_max) {
		g_ui_line_cache_is_valid = false;
	}

}

void ui_set_pixel_at_line_cache(int x, color_t color) {
	if (!g_ui_line_cache_is_valid) {
		//not at valid canvas
		return;
	}
	if (x < g_ui_line_cache_x_min || x > g_ui_line_cache_x_max) {
		//x is not valid
		//if have cache, need flush dev
		if (g_ui_line_cache_x_size > 0) {
			ui_flush_line_cache();
		}
		return;
	}
	if (x != g_ui_line_cache_x+g_ui_line_cache_x_size || (color&0xFF000000)== UI_TRANSPARENT) {
		//the pixel is not next with previous pixel, 
		//need flush previous cache to dev
		if (g_ui_line_cache_x_size > 0) {
			ui_flush_line_cache();
		}
		if ((color & 0xFF000000) == UI_TRANSPARENT) {
			//if it is transparent, skip
			return;
		}
		g_ui_line_cache_x = x;
	}


	//save to cache
	int dev_color;
#if SSZ_UI_SUPPORT_ALPHA_COLOR
	dev_color = ui_common_color_to_dev_color(ui_convet_color_to_real(color,
		x,
		g_ui_line_cache_y, g_ui_line_cache_mode));
#else
	if (g_ui_line_cache_mode&UI_DRAW_REVERSE) {
		color = (~color) & 0x00FFFFFF;
	}
	dev_color = ui_common_color_to_dev_color(color);
#endif
	x = x - g_ui_line_cache_x;
#if DISPLAY_BIT_PER_PIXEL==4
	if (x % 2 == 0) {
		//set high 4 bit of one byte(set low four bit all as high)
		g_ui_one_line_cache[x / 2] = ((uint8_t)dev_color<< DISPLAY_BIT_PER_PIXEL)|0x0F;
	}
	else {
		//set low four bit of one byte
		g_ui_one_line_cache[x / 2] &= (0xF0|(uint8_t)dev_color);
	}
#elif DISPLAY_BIT_PER_PIXEL==24
	memcpy(g_ui_one_line_cache + x * 3, &dev_color, 3);
#else
#error "not support current bit width"
#endif

	g_ui_line_cache_x_size++;

}
void ui_set_pixels_at_line_cache(int x, int x_size, color_t color) {
	if (!g_ui_line_cache_is_valid) {
		//not at valid canvas
		return;
	}
	//if have cache, need flush dev
	if (g_ui_line_cache_x_size > 0) {
		ui_flush_line_cache();
	}
#if SSZ_UI_SUPPORT_ALPHA_COLOR
	if (color  == UI_TRANSPARENT) {
		//if it is transparent, skip
		return;
	}
#endif
	int x0=x;
	int x1=x+x_size-1;
	if (x0<g_ui_line_cache_x_min) {
		x0 = g_ui_line_cache_x_min;
	}
	if (x1>g_ui_line_cache_x_max) {
		x1 = g_ui_line_cache_x_max;
	}
	x_size = x1 - x0 + 1;
	if (x_size<=0) {
		//x is not valid
		return;
	}

	//save to cache
	int dev_color;
#if SSZ_UI_SUPPORT_ALPHA_COLOR
	dev_color = ui_common_color_to_dev_color(ui_convet_color_to_real(color,
		x,
		g_ui_line_cache_y, g_ui_line_cache_mode));
	ssz_assert(0);//not implement
#else
	if (g_ui_line_cache_mode&UI_DRAW_REVERSE) {
		color = (~color) & 0x00FFFFFF;
	}
	dev_color = ui_common_color_to_dev_color(color);
#endif
	uint8_t* line_cache_to_write;

	x = 0;
#if DISPLAY_BIT_PER_PIXEL==4
	line_cache_to_write = g_ui_one_line_cache + x / 2;
	uint8_t one_byte_color = ((uint8_t)dev_color << DISPLAY_BIT_PER_PIXEL) | dev_color;
	//if x not at edge of 2, set it 
	if (x%2==1) {
		//clear back four bit
		*line_cache_to_write &= 0xF0;
		//set back four bit
		*line_cache_to_write |= (uint8_t)dev_color;
		x++;
		x_size--;
		line_cache_to_write++;
	}
	if (x_size>1) {
		memset(line_cache_to_write, one_byte_color, x_size / 2);
		line_cache_to_write += x_size / 2;
	}
	if (x_size%2==1) {
		//clear front four bit
		*line_cache_to_write &= 0x0F;
		//set front four bit
		*line_cache_to_write |= ((uint8_t)dev_color << DISPLAY_BIT_PER_PIXEL);
	}
#elif DISPLAY_BIT_PER_PIXEL==24
	line_cache_to_write = g_ui_one_line_cache+x*3;
	for (; x <= x1; x++) {
		memcpy(line_cache_to_write, &dev_color, 3);
		line_cache_to_write += 3;
	}
#else
#error "not support current bit width"
#endif
	g_ui_line_cache_x = x0;
	g_ui_line_cache_x_size = x1 - x0 + 1;

}
void ui_flush_line_cache() {
	//flush cache to dev
	if (g_ui_line_cache_x_size > 0) {
		ui_draw_to_dev(g_ui_line_cache_x,
			g_ui_line_cache_y , g_ui_one_line_cache,
			g_ui_line_cache_x_size);
		g_ui_line_cache_x_size = 0;
		g_ui_line_cache_x = 0;
	}
}