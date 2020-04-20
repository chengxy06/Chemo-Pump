#include "ui.h"
#include "ui_internal.h"
#include "ssz_common.h"
#include "ssz_str_utility.h"
#include "ui_utility.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

//text out function
const Font* ui_font()
{
	return g_ui_draw_context->font;
}
void ui_set_font(const Font* font)
{
	g_ui_draw_context->font = font;
}
color_t ui_text_color()
{
	return g_ui_draw_context->text_color;
}
void ui_set_text_color(color_t color)
{
	g_ui_draw_context->text_color = color;
}
uint32_t ui_text_mode(void)
{
	return g_ui_draw_context->text_mode;
}
void ui_set_text_mode(uint32_t mode)
{
	g_ui_draw_context->text_mode = mode;
}
int ui_text_char_space()
{
	return g_ui_draw_context->char_space;
}
void ui_set_text_char_space(int char_space)
{
	g_ui_draw_context->char_space = char_space;
}
int ui_text_line_space() {
	return g_ui_draw_context->line_space;
}
void ui_set_text_line_space(int line_space) {
	g_ui_draw_context->line_space = line_space;
}

//get the char's width
int ui_get_char_xsize(uint16_t ch)//UTF-16 char
{
	if (!g_ui_draw_context->font) {
		return 0;
	}
	const CharInfo* char_info = get_char_info(g_ui_draw_context->font, ch);
	//not find the char at the font
	ssz_assert(char_info);
	if (char_info) {
		return char_info->width;
	}
	else {
		return 0;
	}
}
int ui_get_font_ysize()
{
	if (!g_ui_draw_context->font) {
		return 0;
	}
	return g_ui_draw_context->font->height;
}

//get the str's width
int ui_get_text_xsize_internal(const char* str, bool is_break_when_meet_line_end)
{
	if (!g_ui_draw_context->font) {
		return 0;
	}

	int byte_num_of_ch;
	const char * p_work = str;
	uint16_t ch;
	int ret = 0;
	int char_num = 0;//utf16 char num

	while (*p_work != 0) {
		//get one utf16 ch
		ch = ssz_str_utf8_to_one_utf16(p_work, &byte_num_of_ch);
		//set pos to next char
		p_work += byte_num_of_ch;
		char_num++;
		if (byte_num_of_ch == 0) {
			ssz_assert_fail();
			return ret;//lint !e527
		}
		else if (is_break_when_meet_line_end && ch == '\n') {
			char_num--;
			break;
		}
		ret += ui_get_char_xsize(ch);
	}
	if (char_num > 1) {
		ret += (char_num - 1)*g_ui_draw_context->char_space;
	}

	return ret;
}

//get the str's width, if meet line end, it will stop calc xsize
int ui_get_text_xsize(const char* str)
{
	return ui_get_text_xsize_internal(str, true);
}

//return the new x
//x, y is display_dev pos
void ui_disp_char_at_inter(int x, int y, const CharInfo* char_info, int char_height, uint32_t mode)
{
	//draw each line
	for (int line = 0; line < char_height; ++line) {
		ui_start_line_cache(x, y + line, mode);
		//draw one line
		for (int column_byte = 0; column_byte < char_info->bytes_per_line; ++column_byte) {
			uint8_t ch = char_info->data[line*char_info->bytes_per_line + column_byte];
			int column = column_byte * 8;
			//get and set each pixel
			for (int i = 0; i < 8 && column < char_info->width; i++, column++) {
				if (ch & 0x80) {
					ui_set_pixel_at_line_cache(x + column, g_ui_draw_context->text_color);
				}
				else
				{
					ui_set_pixel_at_line_cache(x + column, g_ui_draw_context->bk_color);
				}
				ch <<= 1;
			}
		}
		ui_flush_line_cache();
	}
}
//it not affect by align
void ui_disp_char_at(int x, int y, uint16_t ch)
{
	if (!g_ui_draw_context->font) {
		return;
	}
	const CharInfo* char_info = get_char_info(g_ui_draw_context->font, ch);
	if (char_info) {
		M_ui_canvas_to_display_dev_pos(x, y);
		ui_disp_char_at_inter(x, y, char_info, g_ui_draw_context->font->height, g_ui_draw_context->text_mode);
	}
}

//it will affect by align(left,right,center)
//x,y is display_dev pos
void ui_disp_text_at_internal(int x, int y, const char * str, int text_mode)
{
	if (!g_ui_draw_context->font) {
		return;
	}
	int x0 = x;
	int y0 = y;
	int byte_num_of_ch;
	const char * p_work = str;
	const CharInfo* char_info;

	if (text_mode& UI_TEXT_ALIGN_RIGHT) {
		int all_xsize = ui_get_text_xsize_internal(p_work, true);
		x0 -= all_xsize;
	}
	else if (text_mode& UI_TEXT_ALIGN_HCENTER) {
		int all_xsize = ui_get_text_xsize_internal(p_work, true);
		x0 -= all_xsize / 2;
	}

	uint16_t ch;
	while (*p_work != 0) {
		//get one utf16 ch
		ch = ssz_str_utf8_to_one_utf16(p_work, &byte_num_of_ch);
		//set pos to next char
		p_work += byte_num_of_ch;
		if (byte_num_of_ch == 0) {
			ssz_assert_fail();
			return;//lint !e527
		}
		else if (ch == '\n') {
			//when move to next line, need recalc the start xpos
			if (text_mode& UI_TEXT_ALIGN_RIGHT) {
				int all_xsize = ui_get_text_xsize_internal(p_work, true);
				x0 =x- all_xsize;
			}
			else if (text_mode& UI_TEXT_ALIGN_HCENTER) {
				int all_xsize = ui_get_text_xsize_internal(p_work, true);
				x0 =x- all_xsize / 2;
			}
			else {
				x0 = x;
			}
			y0 += g_ui_draw_context->font->height + g_ui_draw_context->line_space;
			continue;
		}


		//get char info
		char_info = get_char_info(g_ui_draw_context->font, ch);
		//not find the char at the font
		ssz_assert(char_info);
		//display the ch
		if (char_info) {
			ui_disp_char_at_inter(x0, y0, char_info, g_ui_draw_context->font->height,
				text_mode);
			//set next char display pos
			x0 += char_info->width + g_ui_draw_context->char_space;
		}
	}

}

//it will affect by align(left,right,center)
void ui_disp_text_at(int x, int y, const char * str)
{
	M_ui_canvas_to_display_dev_pos(x, y);
	ui_disp_text_at_internal(x, y, str, g_ui_draw_context->text_mode);
}

//it will affect by text_mode
void ui_disp_text_in_rect(const Rect * rect, const char * str, uint32_t text_mode)
{
	if (!g_ui_draw_context->font) {
		return;
	}
	int x0 ;//the pos at display_dev
	int y0 ;//the pos at display_dev

	Rect rect_at_display_dev = *rect;
	//calc the rect pos at display_dev
	ui_rect_move(&rect_at_display_dev, g_ui_draw_context->canvas_rect.x0, g_ui_draw_context->canvas_rect.y0);
	M_ui_return_if_rect_invalid(rect_at_display_dev.x0, rect_at_display_dev.y0, ui_rect_xsize(rect_at_display_dev), ui_rect_ysize(rect_at_display_dev));

	//calc x pos at display_dev
	if (text_mode& UI_TEXT_ALIGN_RIGHT) {
		x0 = rect_at_display_dev.x1;
	}
	else if (text_mode& UI_TEXT_ALIGN_HCENTER) {
		x0 = (rect_at_display_dev.x0+ rect_at_display_dev.x1)/2;
	}
	else {//left
		x0 = rect_at_display_dev.x0;
	}

	//calc y pos at display_dev
	if ((text_mode& UI_TEXT_ALIGN_BOTTOM) || (text_mode& UI_TEXT_ALIGN_VCENTER)) {
		int line_num = ssz_str_line_num(str);
		int all_ysize = line_num*g_ui_draw_context->font->height;
		if (line_num>1) {
			all_ysize += (line_num - 1)*g_ui_draw_context->line_space;
		}
		if (text_mode& UI_TEXT_ALIGN_BOTTOM) {
			y0 = rect_at_display_dev.y1+1 - all_ysize;
		}
		else {
			y0 = rect_at_display_dev.y0 + (ui_rect_ysize(rect_at_display_dev) - all_ysize) / 2;
		}
	}
	else {
		y0 = rect_at_display_dev.y0;
	}

	Rect limit_rect_to_draw;
	//calc the limit rect to draw
	if (ui_rect_intersect_to(&limit_rect_to_draw, &rect_at_display_dev, &g_ui_draw_context->canvas_limit_rect)) {
		//back canvas
		Rect old_rect = g_ui_draw_context->canvas_limit_rect;
		//set the canvas as the limit rect
		ui_set_limit_rect_of_canvas_ex(&limit_rect_to_draw);
		ui_disp_text_at_internal(x0, y0, str, text_mode);
		//restore the canvas
		g_ui_draw_context->canvas_limit_rect = old_rect;
	}
}

