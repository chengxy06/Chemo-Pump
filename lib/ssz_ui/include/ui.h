/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ssz_def.h"
#include "ssz_ui_config.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Standard colors
************************************************/
//the color's four byte is alpha,blue,green,red
//alpha: 0->not transparent, 255->transparent
#define UI_BLUE          0x00FF0000
#define UI_GREEN         0x0000FF00
#define UI_RED           0x000000FF
#define UI_CYAN          0x00FFFF00
#define UI_MAGENTA       0x00FF00FF
#define UI_YELLOW        0x0000FFFF
#define UI_LIGHTBLUE     0x00FF8080
#define UI_LIGHTGREEN    0x0080FF80
#define UI_LIGHTRED      0x008080FF
#define UI_LIGHTCYAN     0x00FFFF80
#define UI_LIGHTMAGENTA  0x00FF80FF
#define UI_LIGHTYELLOW   0x0080FFFF
#define UI_DARKBLUE      0x00800000
#define UI_DARKGREEN     0x00008000
#define UI_DARKRED       0x00000080
#define UI_DARKCYAN      0x00808000
#define UI_DARKMAGENTA   0x00800080
#define UI_DARKYELLOW    0x00008080
#define UI_WHITE         0x00FFFFFF
#define UI_LIGHTGRAY     0x00D3D3D3
#define UI_GRAY          0x00808080
#define UI_DARKGRAY      0x00404040
#define UI_BLACK         0x00000000
#define UI_BROWN         0x002A2AA5
#define UI_ORANGE        0x0000A5FF
#define UI_TRANSPARENT   0xFF000000
#define UI_INVALID_COLOR 0xFFFFFFFF

#define UI_RGB(red, green, blue) ( (uint8_t)red&((uint32_t)((uint8_t)green)<<8)&((uint32_t)((uint8_t)blue)<<16) )
#define UI_COLOR_R(color) ((uint8_t)color)
#define UI_COLOR_G(color) ((uint8_t)(color>>8))
#define UI_COLOR_B(color) ((uint8_t)(color>>16))
#define UI_COLOR_ALPHA(color) ((uint8_t)(color>>24))

/************************************************
* draw mode, not affect text
************************************************/
//can combination
#define UI_DRAW_NORMAL 0x000
#define UI_DRAW_REVERSE 0x100

/************************************************
* text Property
************************************************/
//can combination
#define UI_TEXT_NORMAL UI_DRAW_NORMAL
#define UI_TEXT_REVERSE UI_DRAW_REVERSE
//#define UI_TEXT_LINE_BREAK 0x80
#define UI_TEXT_ALIGN_LEFT      0x000
#define UI_TEXT_ALIGN_RIGHT	  	0x001
#define UI_TEXT_ALIGN_HCENTER	0x002 //___(horizontal) cneter
#define UI_TEXT_ALIGN_TOP	    0x000
#define UI_TEXT_ALIGN_VCENTER   0x004
#define UI_TEXT_ALIGN_BOTTOM	0x008
#define UI_TEXT_ALIGN_CENTER (UI_TEXT_ALIGN_HCENTER|UI_TEXT_ALIGN_VCENTER) //center at horizontal and vertical 

typedef uint32_t color_t;
typedef struct  {
	int16_t x0;
	int16_t y0;
	int16_t x1;
	int16_t y1;
}Rect;
typedef struct  {
	int16_t x;
	int16_t y;
}Point;
typedef struct {
	int16_t x_size;
	int16_t y_size;
}Size;
typedef enum {
	kUIDrawStroke, //only draw the outline
	kUIDrawFill, //only fill the area
	kUIDrawStrokeAndFill, //draw the outline and fill the inside area
}UIDrawType;
typedef struct
{
	const unsigned char *data;
	int16_t width;
	int16_t height;
	uint8_t bytes_per_line; //the size of bytes of one line
	int8_t bits_per_pixel;
}Image;
typedef struct
{
	uint16_t char_code;//UTF16 code
	uint8_t width;//char width
	uint8_t bytes_per_line; //the size of bytes of one line
	const uint8_t *data; //char data    
}CharInfo;

typedef struct
{
	int height; //font height
	int char_count;
	const CharInfo* char_info;
}Font;

typedef struct {
	uint32_t draw_mode;
	color_t bk_color;
	int pen_size;
	color_t pen_color;
	color_t fill_color;
	const Font* font;
	uint32_t text_mode;
	color_t text_color;
	int char_space;
	int line_space;
	Rect canvas_rect;//it's pos is based on display device
	Rect canvas_limit_rect;//it's pos is based on display device
}UIDrawContext;

#ifdef __cplusplus
extern "C" {
#endif

/************************************************
* Get/Set common draw Attributes
************************************************/
//it not affect text mode
uint32_t ui_draw_mode();
void ui_set_draw_mode(uint32_t draw_mode);
//get background color
color_t ui_bk_color();
//set background color
void ui_set_bk_color(color_t color);

//all draw can only draw at this canvas, the canvas can be larger than display device
//draw's x and y pos is base this canvas's left top point
//it also set the limit rect same as(or smaller than) canvas rect
// x: xpos at display device, can negative
void ui_canvas_rect(Rect* rect);
void ui_set_canvas_rect(int x, int y, int x_size, int y_size);
void ui_set_canvas_rect_ex(const Rect *rect);
// it must in canvas, the limit rect must be less than display device
// you can not draw out of limit rect
// x: xpos at display device
void ui_set_limit_rect_of_canvas(int x, int y, int x_size, int y_size);
void ui_set_limit_rect_of_canvas_ex(const Rect *rect);
void ui_limit_rect_of_canvas(Rect* rect);


void ui_set_default_draw_context();
//get the draw context
void ui_draw_context(UIDrawContext* out_context);
//set the draw context
void ui_set_draw_context(UIDrawContext* in_context);

//back current ui draw context and default current draw context
//after use finish, you must call restore context before call it again
void ui_backup_and_set_default_draw_context();
//restore previous context
void ui_restore_draw_context();
/************************************************
* draw graphic
************************************************/
//draw function
color_t ui_pen_color();
void ui_set_pen_color(color_t color);
color_t ui_fill_color();
void ui_set_fill_color(color_t color);
int ui_pen_size();
void ui_set_pen_size(int pen_size);

//it use fixed 1 width pen color
void ui_draw_pixel_at(int x, int y);
//draw ___(horizontal)  line with pen color
void ui_draw_hline_at(int x, int y, int x_size);
//draw |(vertical) line with pen color
void ui_draw_vline_at(int x, int y, int y_size);
//draw the rect with pen and fill color(stroke use pen color, fill use fill color)
void ui_draw_rect_at(int x, int y, int x_size, int y_size, UIDrawType draw_type);
void ui_draw_rect_ex(const Rect *rect, UIDrawType draw_type);
//fill use fill color
void ui_fill_rect_at(int x, int y, int x_size, int y_size);
void ui_fill_rect_ex(const Rect *rect);
//fill rect by the background color
void ui_clear_rect(int x, int y, int x_size, int y_size);
void ui_clear_rect_ex(const Rect *rect);
//this will clear the canvas by the background color
void ui_clear();

/************************************************
* draw image
************************************************/
//img function
void ui_draw_img_at(int x, int y, const Image* img);

/************************************************
* display text
************************************************/
//text out function
const Font* ui_font();
void ui_set_font(const Font* font);
color_t ui_text_color();
void ui_set_text_color(color_t color);
uint32_t ui_text_mode(void);
//refer text Property, e.g. UI_TEXT_NORMAL|UI_TEXT_ALIGN_LEFT
void ui_set_text_mode(uint32_t mode);
int ui_text_char_space();
void ui_set_text_char_space(int char_space);
int ui_text_line_space();
void ui_set_text_line_space(int line_space);

//get the char's width
int ui_get_char_xsize(uint16_t ch);//UTF-16 char
int ui_get_font_ysize();
//get the str's width, if meet line end, it will stop calc xsize
int ui_get_text_xsize(const char* str);
//it not affect by align
void ui_disp_char_at(int x, int y, uint16_t ch);
//it will affect by align(left,right,center)
void ui_disp_text_at(int x, int y, const char * str);
//it will affect by text_mode
void ui_disp_text_in_rect(const Rect * rect, const char * str, uint32_t text_mode);




/************************************************
* draw to dev, user should implement it
************************************************/
void ui_config();
void ui_draw_to_dev(int x, int y, uint8_t *dev_pixels, int pixels_count);
int ui_read_pixel_from_dev(int x, int y);
//convert color to dev color index
int ui_common_color_to_dev_color(color_t color);
//convert dev color index to color
color_t ui_dev_color_to_common_color(int dev_color_index);

#ifdef __cplusplus
}
#endif