#include "ui_statusbar.h"
#include "ssz_common.h"
#include "image_data.h"
#include "ui_define.h"
#include "ui_lable.h"
#include "ui_image.h"

/************************************************
* Declaration
************************************************/
#define STATUSBAR_TEXT_START_X 0
#define STATUSBAR_TEXT_WIDTH 96

#define STATUSBAR_TIME_WIDTH 31
#define STATUSBAR_TIME_START_R_X (STATUSBAR_TIME_WIDTH+1)

#define STATUSBAR_BATTERY_WIDTH 32
#define STATUSBAR_BATTERY_HEIGHT 12
#define STATUSBAR_BATTERY_START_R_X (STATUSBAR_TIME_START_R_X+STATUSBAR_BATTERY_WIDTH+STATUSBAR_GAP_OF_EACH_ICON)
#define STATUSBAR_BATTERY_START_Y 1


#define STATUSBAR_ICON_START_R_X (STATUSBAR_BATTERY_START_R_X)
#define STATUSBAR_ICON_START_Y STATUSBAR_BATTERY_START_Y
//#define STATUSBAR_ICON_ALL_WIDTH (STATUSBAR_WIDTH - STATUSBAR_TEXT_WIDTH - STATUSBAR_TIME_WIDTH-STATUSBAR_TEXT_START_X)
#define STATUSBAR_GAP_OF_EACH_ICON 2


#define STATUSBAR_FONT DEFAULT_FONT

/************************************************
* Variable 
************************************************/
const static int g_statusbar_icon_widget_id_map[kStatusbarIconOrderMax] = 
{ 
	kUI_IDStatusbarLock,
	kUI_IDStatusbarMute
};
const static ImgID g_statusbar_battery_icon_id_map[kBatteryLevelMax]=
{
	kImgBatteryNo,
	kImgBatteryEmpty,
	kImgBatteryEmpty, //low battery
	kImgBatteryOne,
	kImgBatteryTwo,
	kImgBatteryThree,
	kImgBatteryFull
};
/************************************************
* Function 
************************************************/

UIStatusbar::UIStatusbar()
{
	m_text[0] = 0;
	m_time_str[0] = 0;
	ssz_mem_zero(m_icon_imgs, sizeof(m_icon_imgs));
	m_bk_color = DEFAULT_STATUSBAR_BACKGROUND_COLOR;
}

void UIStatusbar::on_create()
{
	UILable* lable;
	int title_x_size = STATUSBAR_TEXT_WIDTH;
	if (xsize()==SCREEN_WIDTH) {
		title_x_size += 64;
	}
	//create title
	lable = (UILable*)ui_view_create(kUILable, kUI_IDStatusbarTitle, this,
		STATUSBAR_TEXT_START_X, STATUSBAR_Y, title_x_size, STATUSBAR_HEIGHT, 0);
	lable->set_font(STATUSBAR_FONT);
	lable->set_text_mode(UI_TEXT_ALIGN_LEFT | UI_TEXT_ALIGN_VCENTER);

	//create time
	lable = (UILable*)ui_view_create(kUILable, kUI_IDStatusbarTime,this,
		xsize()-STATUSBAR_TIME_START_R_X, STATUSBAR_Y, STATUSBAR_TIME_WIDTH, STATUSBAR_HEIGHT,0);
	lable->set_font(STATUSBAR_FONT);
	lable->set_text_mode(UI_TEXT_ALIGN_RIGHT | UI_TEXT_ALIGN_VCENTER);

	//create battery icon
	UIImage* img;
	ui_view_create(kUIImage, kUI_IDStatusbarBattery, this, 
		xsize()- STATUSBAR_BATTERY_START_R_X, STATUSBAR_BATTERY_START_Y, STATUSBAR_BATTERY_WIDTH, STATUSBAR_BATTERY_HEIGHT, 0);

	for (int i = 0; i < kStatusbarIconOrderMax; i++)
	{
		//create icons
		img = (UIImage*)ui_view_create(kUIImage, g_statusbar_icon_widget_id_map[i],this, 0, 0, 0, 0, UI_VIEW_HIDE);
		img->set_auto_size(true);
	}

}

void UIStatusbar::on_draw()
{
	set_draw_attributes();
	ui_clear();
	//if (m_text) {
	//	Rect rc;
	//	area_self(&rc);
	//	ui_disp_text_in_rect(&rc, m_text, m_text_mode);
	//}
}

void UIStatusbar::on_size_change()
{
	UILable* lable;
	//title
	lable = (UILable*)get_child(kUI_IDStatusbarTitle);
	lable->set_pos(STATUSBAR_TEXT_START_X, STATUSBAR_Y);

	//time
	lable = (UILable*)get_child(kUI_IDStatusbarTime);
	lable->set_pos(xsize() - 1 - STATUSBAR_TIME_START_R_X, STATUSBAR_Y);

	//battery icon
	UIImage* img;
	img = (UIImage*)get_child(kUI_IDStatusbarBattery);
	img->set_pos(xsize() - STATUSBAR_BATTERY_START_R_X, STATUSBAR_BATTERY_START_Y);
	update_icons();

}

void UIStatusbar::on_msg(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgCreate:
		on_create();
		break;
	case kUIMsgSizeChange:
		on_size_change();
		break;
	case kUIMsgDraw:
		on_draw();
		is_handled = true;
		break;
	case kUIMsgSetText:
		set_title((const char*)msg.addr_param);
		is_handled = true;
		break;
	}
	if (!is_handled) {
		on_msg_default(msg);
	}
}

void UIStatusbar::set_title(const char * text)
{
	if (text) {
		snprintf(m_text, sizeof(m_text), "%s", text);
	}
	else {
		m_text[0] = 0;
	}
	UILable* handle;
	

	//find it
	handle = (UILable*)get_child(kUI_IDStatusbarTitle);

	if (handle != NULL)
	{
		handle->set_text_and_force_invalid(m_text);
	}
}

void UIStatusbar::set_time(const SszDateTime * date_time)
{
	char tm_str[STATUSBAR_TIME_STR_SIZE];
	if (date_time) {
		snprintf(tm_str, sizeof(tm_str), "%02d:%02d", date_time->hour, date_time->minute);
	}
	else {
		tm_str[0] = 0;
	}
	if (strcmp(m_time_str, tm_str) != 0) {
		strcpy(m_time_str, tm_str);

		UILable* handle;

		//find it
		handle = (UILable*)get_child(kUI_IDStatusbarTime);

		if (handle != NULL)
		{
			handle->set_text_and_force_invalid(m_time_str);
		}
	}
}

void UIStatusbar::set_battery(bool is_ac_connect, bool is_battery_exist, BatteryLevel level)
{
	UIImage* img = (UIImage*)get_child(kUI_IDStatusbarBattery);

	ssz_assert(level < kBatteryLevelMax);
	if (img) {
		img->disable_blink();
		//if (is_ac_connect) {
		//	if (is_battery_exist) {
		//		img->set_img(get_image(kImgBatteryAc));
		//	}
		//	else {
		//		img->set_img(get_image(kImgBatteryNo));
		//	}
		//}
		//else {
			img->set_img(get_image(g_statusbar_battery_icon_id_map[level]));
			if (level == kBatteryLow || level == kBatteryEmpty ) {
				img->enable_blink();
			}
		//}
	}
}

void UIStatusbar::show_icon(StatusbarIconOrderID icon_order, const Image* icon)
{
	set_icon(icon_order, icon, true);
}

void UIStatusbar::hide_icon(StatusbarIconOrderID icon_order)
{
	set_icon(icon_order, NULL, false);
}

void UIStatusbar::set_icon(StatusbarIconOrderID icon_order, const Image* icon, bool is_show)
{
	if (is_show) {
		ssz_assert(icon);
	}


	ssz_assert(icon_order < kStatusbarIconOrderMax);

	if (m_icon_imgs[icon_order] == icon)
	{
		//not change icon, return
		return;
	}
	else if (icon != NULL && m_icon_imgs[icon_order] != NULL)
	{
		//only need change the image
		m_icon_imgs[icon_order] = icon;
		UIImage* tmp;
		tmp = (UIImage*)get_child(g_statusbar_icon_widget_id_map[icon_order]);
		if (tmp) {
			tmp->set_img(icon);
		}
		return;
	}

	//it change to show or hide, need update the icon pos
	m_icon_imgs[icon_order] = icon;

	bool is_need_update_all_icon_pos = false;
	bool is_need_update_the_icon_pos = true;

	//check if it is at leftmost
	//if not need update all icon's pos
	for (int i = icon_order - 1; i >=0; i--)
	{
		if (m_icon_imgs[i] != NULL)
		{
			is_need_update_all_icon_pos = true;
			break;
		}
	}

	if (is_need_update_all_icon_pos)
	{
		//update all icon pos
		update_icons();
	}
	else if (is_need_update_the_icon_pos)
	{
		//update only this icon pos
		int r_x, y;
		const Image* img;
		UIImage* tmp;

		tmp = (UIImage*)get_child(g_statusbar_icon_widget_id_map[icon_order]);

		r_x = STATUSBAR_ICON_START_R_X;

		//calc the icon pos
		for (int i = icon_order-1; i >=0; i--)
		{
			img = m_icon_imgs[i];
			if (img)
			{
				r_x += STATUSBAR_GAP_OF_EACH_ICON + img->width;;
			}
		}

		//show or hide it
		img = m_icon_imgs[icon_order];
		if (img)
		{
			r_x += STATUSBAR_GAP_OF_EACH_ICON + img->width;;
			y = STATUSBAR_ICON_START_Y;
			tmp->set_img(img);
			tmp->set_pos(xsize()-r_x, y);
			tmp->show();
		}
		else
		{
			tmp->hide();
		}

	}
}

void UIStatusbar::update_icons()
{
	UIImage* tmp;
	int r_x;
	int y;
	const Image* img;
	int i;

	r_x = STATUSBAR_ICON_START_R_X;

	y = STATUSBAR_ICON_START_Y;
	for (i = kStatusbarIconOrderMax-1; i>=0; i--)
	{
		img = m_icon_imgs[i];
		tmp = (UIImage*)get_child(g_statusbar_icon_widget_id_map[i]);
		if (img)
		{
			r_x += STATUSBAR_GAP_OF_EACH_ICON + img->width;
			tmp->set_img(img);
			tmp->set_pos(xsize() - r_x - 1, y);
			tmp->show();
		}
		else
		{
			tmp->hide();
		}
	}
}
