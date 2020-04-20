#ifdef TEST
#include "greatest.h"
#include "ui_lable.h"
#include "ui_image.h"
#include "ui_window.h"
#include "ui_text_list.h"
#include "ui_image_list.h"

#include <stdio.h>
#include <stdlib.h>
#include "string_data.h"
#include "font_data.h"
#include "image_data.h"
#include "ssz_common.h"
/***********************************************/
#define DELAY_TO_VIEW_MS  1000
//this will call before every case at this suite
static void test_ui_view_setup(void *arg) {
	(void)arg;
	ui_view_desktop()->delete_all_child();
	ui_set_canvas_rect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	ui_set_bk_color(UI_BLACK);
	ui_clear();
	//ssz_delay_ms(DELAY_TO_VIEW_MS);
}
//this will call after every case at this suite
static void test_ui_view_teardown(void *arg) {
	(void)arg;
}
/***********************************************/
static void test_ui_view_window() {
	SKIP();
	UILable* lable1,*lable2;
	UIWindow* win1,*win2;
	win1 = (UIWindow*)ui_view_create(kUIWindow, kUI_IDWindow, NULL, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT/2, 0);
	win1->set_bk_color(UI_GRAY);
	win2 = (UIWindow*)ui_view_create(kUIWindow, kUI_IDWindow1, NULL, 0, DISPLAY_HEIGHT / 2, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2, 0);
	win2->set_bk_color(UI_BLUE);
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
	lable1 = (UILable*)ui_view_create(kUILable, kUI_IDLable1, win1, 0, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, 0);
	lable1->set_text("lable1 at windows1");
	lable2 = (UILable*)ui_view_create(kUILable, kUI_IDLable1, win2, DISPLAY_WIDTH/2, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT, 0);
	lable2->set_text("lable2 at windows2");
	ui_view_output_all_view_info();
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
	for (int i = 0; i < DISPLAY_HEIGHT/2; i+=5) {
		lable2->set_pos(DISPLAY_WIDTH / 2, -i);
		ui_view_output_all_view_info();
		ui_view_draw_all_dirty();
		ssz_delay_ms(100);
	}
}
static void test_ui_view_lable() {
	SKIP();
	UILable* p;
	p = (UILable*)ui_view_create(kUILable, kUI_IDLable, NULL, 0, 0, 80, 20, 0);
	p->set_bk_color(UI_GREEN);
	p->set_text_mode(UI_TEXT_ALIGN_LEFT| UI_TEXT_ALIGN_TOP);
	p->set_text("Lable0");
	//ui_view_draw_all_dirty();
	//ssz_delay_ms(DELAY_TO_VIEW_MS);

	p = (UILable*)ui_view_create(kUILable, kUI_IDLable1, NULL, 30, 18, 80, 20, 0);
	p->set_bk_color(UI_GRAY);
	p->set_text_mode(UI_TEXT_ALIGN_HCENTER | UI_TEXT_ALIGN_VCENTER);
	p->set_text("Lable1");
	//ui_view_draw_all_dirty();
	//ssz_delay_ms(DELAY_TO_VIEW_MS);

	p = (UILable*)ui_view_create(kUILable, kUI_IDLable2, NULL, 60, 36, 80, 20, 0);
	p->set_bk_color(UI_BLUE);
	p->set_text_mode(UI_TEXT_ALIGN_RIGHT| UI_TEXT_ALIGN_BOTTOM);
	p->set_text("Lable2");
	ui_view_output_all_view_info();
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
	
	p = (UILable*)ui_view_desktop()->get_child(kUI_IDLable);
	p->set_text_color(UI_RED);
	ui_view_output_all_view_info();
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);

	p = (UILable*)ui_view_desktop()->get_child(kUI_IDLable1);
	p->set_text_color(UI_GREEN);
	ui_view_output_all_view_info();
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);

	p = (UILable*)ui_view_desktop()->get_child(kUI_IDLable2);
	p->set_text_color(UI_CYAN);
	ui_view_output_all_view_info();
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}

static void test_ui_view_image() {
	SKIP();
	UIImage* p;
	const Image* img = get_image(kImgSystemSetting);
	p = (UIImage*)ui_view_create(kUIImage, 0, NULL, 0, 0, img->width, img->height, 0);
	p->set_img(img);
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
	img = get_image(kImgError);
	p->set_img(img);
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
}

static void test_ui_view_text_list() {
	SKIP();
	UITextList* p;
	p = (UITextList*)ui_view_create(kUITextList, 0, NULL, 10, 10, 50, 50, 0);
	p->set_item_height(16);
	p->push_back("item1");
	p->push_back("item2");
	p->push_back("item3");
	p->push_back("item4");
	p->push_back("item5");
	p->push_back("item6");
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
	for (int i = 0; i < p->size(); i++) {
		p->select_next();
		ui_view_draw_all_dirty();
		ssz_delay_ms(DELAY_TO_VIEW_MS);
	}

}

static void test_ui_view_horizontal_text_list() {
	SKIP();
	UITextList* p;
	p = (UITextList*)ui_view_create(kUITextList, 0, NULL, 10, 10, 50, 50, 0);
	p->set_layout_orientation(kUIHorizontal);
	p->set_item_width(16);
	p->push_back("i1");
	p->push_back("i2");
	p->push_back("i3");
	p->push_back("i4");
	p->push_back("i5");
	p->push_back("i6");
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
	for (int i = 0; i < p->size(); i++) {
		p->select_next();
		ui_view_draw_all_dirty();
		ssz_delay_ms(DELAY_TO_VIEW_MS);
	}

}

static void test_ui_view_image_list() {
	SKIP();
	UIImageList* p;
	p = (UIImageList*)ui_view_create(kUIImageList, 0, NULL, 10, 10, 100, 50, 0);
	//p->set_item_height(get_image(kImgSystemSetting)->height);
	p->set_item_width(get_image(kImgSystemSetting)->width+10);
	p->set_layout_orientation(kUIHorizontal);
	p->set_item_align(UI_ALIGN_CENTER);
	p->push_back(get_image(kImgTimeUnselect), get_image(kImgTimeSelect));
	p->push_back(get_image(kImgHistoryLog), get_image(kImgHistoryLog));
	p->push_back(get_image(kImgSystemSetting), get_image(kImgSystemSetting));
	//p->push_back(get_image(kImgHistoryLog));
	ui_view_draw_all_dirty();
	ssz_delay_ms(DELAY_TO_VIEW_MS);
	for (int i = 0; i < p->size(); i++) {
		p->select_next();
		ui_view_draw_all_dirty();
		ssz_delay_ms(DELAY_TO_VIEW_MS);
	}

}
/***********************************************/
#include "ui_view_suite_def.h"
/***********************************************/
#endif


