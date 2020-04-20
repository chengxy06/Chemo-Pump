#ifdef TEST
#include "greatest.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include "ssz_common.h"
#include "string_data.h"
#include "font_data.h"
#include "image_data.h"
#include "ui_utility.h"
#include "drv_oled.h"
/***********************************************/
//this will call before every case at this suite
static void test_ui_setup(void *arg) {
	(void)arg;

	ui_set_default_draw_context();
	ui_set_fill_color(UI_BLUE);
	ui_set_pen_color(UI_WHITE);
	ui_set_bk_color(UI_BLACK);
	ui_set_canvas_rect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	ui_set_draw_mode(0);

	ui_set_font(get_font(14));
	ui_set_text_color(UI_RED);
	ui_set_text_mode(0);
	ui_set_text_char_space(0);
	ui_set_text_line_space(0);
	ui_clear();
}
//this will call after every case at this suite
static void test_ui_teardown(void *arg) {
	(void)arg;
}
#define DELAY_TO_VIEW_MS  3000
/***********************************************/
static void test_ui_draw_mode() {
}

static void test_ui_set_draw_mode() {
}

static void test_ui_bk_color() {
}

static void test_ui_set_bk_color() {
}

static void test_ui_canvas_rect() {
}

static void test_ui_set_canvas_rect() {

}

static void test_ui_draw_context() {
}

static void test_ui_set_draw_context() {
}

static void test_ui_move_to() {
}

static void test_ui_pen_color() {
}

static void test_ui_set_pen_color() {
}

static void test_ui_fill_color() {
}

static void test_ui_set_fill_color() {
}

static void test_ui_pen_size() {
}

static void test_ui_set_pen_size() {
}

static void test_ui_draw_pixel_at() {
	SKIP();
	for (int i = 0; i < DISPLAY_WIDTH; i++) {
		ui_draw_pixel_at(i, 0);
		ui_draw_pixel_at(i, DISPLAY_HEIGHT - 1);
	}

	for (int i = 1; i < DISPLAY_HEIGHT-1; i++) {
		ui_draw_pixel_at(0, i);
		ui_draw_pixel_at(DISPLAY_WIDTH-1, i);
	}
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
	//ui_draw_pixel_at(0, 0);
	////ssz_delay_ms(DELAY_TO_VIEW_MS);
	//ui_draw_pixel_at(0, DISPLAY_HEIGHT - 1);
	////ssz_delay_ms(DELAY_TO_VIEW_MS);
	//ui_draw_pixel_at(DISPLAY_WIDTH - 1, 0);
	////ssz_delay_ms(DELAY_TO_VIEW_MS);
	//ui_draw_pixel_at(DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1);
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}

static void test_ui_draw_hline_at() {
	SKIP();
	ui_draw_hline_at(0, DISPLAY_HEIGHT/2, DISPLAY_WIDTH);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);

	ui_set_canvas_rect(0, 20, 6, 5);
	ui_draw_hline_at(0, 0, DISPLAY_WIDTH);
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}
static void test_ui_draw_vline_at() {
	SKIP();
	ui_draw_vline_at(DISPLAY_WIDTH/2, 0, DISPLAY_HEIGHT);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);

	ui_set_canvas_rect(70, 10, 100, 20);
	ui_draw_vline_at(0, 0, DISPLAY_HEIGHT);
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}

static void test_ui_draw_rect_at() {
	SKIP();
	ui_draw_rect_at(20, 0, 20, 20, kUIDrawStroke);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
	ui_draw_rect_at(40, 0, 20, 20, kUIDrawFill);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
	ui_draw_rect_at(60, 0, 20, 20, kUIDrawStrokeAndFill);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);

	ui_set_canvas_rect(20, 21, 10, 10);
	ui_set_pen_size(3);
	ui_draw_rect_at(0, 0, 20, 20, kUIDrawStroke);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
	ui_set_canvas_rect(40, 21, 10, 10);
	ui_draw_rect_at(0, 0, 20, 20, kUIDrawFill);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
	ui_set_canvas_rect(60, 21, 10, 10);
	ui_draw_rect_at(0, 0, 20, 20, kUIDrawStrokeAndFill);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
	ui_set_canvas_rect(80, 21, 10, 10);
	ui_set_draw_mode(UI_DRAW_REVERSE);
	ui_draw_rect_at(0, 0, 20, 20, kUIDrawStrokeAndFill);
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}

static void test_ui_draw_rect_ex() {
}


static void test_ui_fill_rect_at() {
}

static void test_ui_fill_rect_ex() {
}

static void test_ui_clear() {
	//ui_draw_rect_at(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, kUIDrawStrokeAndFill);
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
	//ui_clear();
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
}
static void test_ui_draw_img_at() {
	SKIP();
	ui_draw_img_at(0, 0, get_image(kImgSystemSetting));

	ui_set_canvas_rect(40, 0, 10, 10);
	ui_draw_img_at(0, 0, get_image(kImgSystemSetting));

	ui_set_canvas_rect(80, 0, 60, 60);
	ui_draw_img_at(-10, -10, get_image(kImgSystemSetting));
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}

static void test_ui_font() {
}

static void test_ui_set_font() {
}

static void test_ui_text_color() {
}

static void test_ui_set_text_color() {
}

static void test_ui_text_mode() {
}

static void test_ui_set_text_mode() {
}

static void test_ui_disp_char_at() {
	SKIP();
	ui_disp_char_at(0, 0, '1');
	ui_disp_char_at(0, 20, '2');
	ui_set_canvas_rect(0, 40, 8, 8);
	ui_disp_char_at(0, 0, '3');
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}


static void test_ui_disp_text_at() {
	SKIP();
	const char* str = "12345";
	ui_disp_text_at(0, 0, get_string(kStrConfirm));
	ui_disp_text_at(DISPLAY_WIDTH/2, 0, str);
	ui_set_text_mode(UI_TEXT_ALIGN_RIGHT);
	ui_disp_text_at(DISPLAY_WIDTH / 2, 20, str);
	ui_set_text_mode(UI_TEXT_ALIGN_HCENTER);
	ui_disp_text_at(DISPLAY_WIDTH / 2, 40, str);

	ssz_delay_ms(DELAY_TO_VIEW_MS);
	ui_clear();
	str = "123456\n78";
	ui_set_text_mode(UI_TEXT_ALIGN_LEFT);
	ui_disp_text_at(0, 0, str);
	ui_set_text_mode(UI_TEXT_ALIGN_RIGHT);
	ui_disp_text_at(DISPLAY_WIDTH / 2, 0, str);
	ui_set_text_mode(UI_TEXT_ALIGN_HCENTER);
	ui_disp_text_at(DISPLAY_WIDTH / 2+ DISPLAY_WIDTH/4, 0, str);
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}

static void test_ui_disp_text_in_rect() {
	SKIP();
	Rect rc = {0, 0, 59, 59};
	ui_draw_rect_ex(&rc, kUIDrawFill);
	const char* str = "12345";
	ui_disp_text_in_rect(&rc, str, UI_TEXT_ALIGN_LEFT| UI_TEXT_ALIGN_TOP);
	ui_rect_move(&rc, 61, 0);
	ui_draw_rect_ex(&rc, kUIDrawFill);
	ui_disp_text_in_rect(&rc, str, UI_TEXT_ALIGN_RIGHT | UI_TEXT_ALIGN_VCENTER);
	ui_rect_move(&rc, 61, 0);
	ui_set_bk_color(UI_TRANSPARENT);
	ui_draw_rect_ex(&rc, kUIDrawFill);
	ui_disp_text_in_rect(&rc, str, UI_TEXT_ALIGN_HCENTER| UI_TEXT_ALIGN_BOTTOM);
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}

static void test_ui_get_text_xize() {
	const char*str = "123";
	int len = ui_get_text_xsize(str);
	ui_set_text_char_space(3);
	GASSERT(ui_get_text_xsize("123") == len + 2 * 3);
}

static void test_ui_get_char_xize() {

}

static void test_ui_get_font_yize() {

}

static void test_ui_draw_to_dev() {

}


/***********************************************/
#include "ui_suite_def.h"
#endif


