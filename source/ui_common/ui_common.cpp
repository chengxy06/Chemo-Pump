/************************************************
* DESCRIPTION:
*   
************************************************/
#include "ui_common.h"
#include "ssz_common.h"
#include "string_data.h"
#include "image_data.h"
#include "font_data.h"
#include "app_common.h"
#include "ui_view.h"
#include "ui_statusbar.h"
#include "ui_define.h"
#include "mid_common.h"
#include "screen.h"

/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/


/************************************************
* Function 
************************************************/

void ui_create_widgets(UIView * parent, const int16_t * widgets_define, int widgets_define_size)
{
	UIView* p = parent;
	bool is_find_attr_type = false;
	WidgetAttrType widget_attr_type=kWidgetAttrNull;

	for (int i = 0; i < widgets_define_size; i++) {
		if (!is_find_attr_type) {
			widget_attr_type = (WidgetAttrType)widgets_define[i];
			is_find_attr_type = true;
		}
		else {
			switch (widget_attr_type) {
			case kWidgetAttrBase:
			{
				ssz_assert(widgets_define_size - i + 1 >= 6);
				UIViewTypeID type = (UIViewTypeID)widgets_define[i++];
				int16_t id = widgets_define[i++];
				int16_t x = widgets_define[i++];
				int16_t y = widgets_define[i++];
				int16_t x_size = widgets_define[i++];
				int16_t y_size = widgets_define[i];

				Rect parent_rc;
				parent->area_self(&parent_rc);
				if (x<0) {
					x = ui_rect_xsize(parent_rc) + x;
				}
				if (y<0) {
					y = ui_rect_ysize(parent_rc) + y;
				}
				if (x_size < 0 ) {
					x_size = ui_rect_xsize(parent_rc) - x;
				}
				if (y_size < 0) {
					y_size = ui_rect_ysize(parent_rc) - y;
				}
				p = ui_view_create((UIViewTypeID)type, id, parent, x, y, x_size, y_size, 0);
				if (p->is_enable_focused()) {
					p->set_focus();
				}
				if (type == kUIStatusBar) {
					//screen create a internal statusbar, so need hide the global statusbar
					UIView* tmp = M_ui_statusbar(ui_view_desktop()->get_child(kUI_IDStatusBar));
					tmp->hide();
				}
				break;
			}
			case kWidgetAttrTextID:
			{
				const char* text = get_string((StrID)widgets_define[i]);
				UIMsg msg;
				ssz_mem_zero(&msg, sizeof(msg));
				msg.msg_id = kUIMsgSetText;
				msg.addr_param = text;
				p->on_msg(msg);
				break;
			}
			case kWidgetAttrImgID:
			{
				const Image* img= get_image((ImgID)widgets_define[i]);
				UIMsg msg;
				ssz_mem_zero(&msg, sizeof(msg));
				msg.msg_id = kUIMsgSetImg;
				msg.addr_param = img;
				p->on_msg(msg);
				break;
			}
			case kWidgetAttrFontHeight:
			{
				const Font* font= get_font(widgets_define[i]);
				UIMsg msg;
				ssz_mem_zero(&msg, sizeof(msg));
				msg.msg_id = kUIMsgSetFont;
				msg.addr_param = font;
				p->on_msg(msg);
				break;
			}
			case kWidgetAttrAlign:
			{
				p->send_msg_with_para(p, kUIMsgSetAlign, widgets_define[i]);
				break;
			}
			case kWidgetAttrTitleTextID:
			{
				const char* text = get_string((StrID)widgets_define[i]);
				statusbar()->set_title(text);
				break;
			}
			default:
				ssz_assert_fail();
				break;
			}
			is_find_attr_type = false;
		}
	}
}


void statusbar_create()
{
	//create statusbar
	ui_view_create(kUIStatusBar, kUI_IDStatusBar, NULL,
		STATUSBAR_X, STATUSBAR_Y, STATUSBAR_WIDTH, STATUSBAR_HEIGHT, 0);
}

UIStatusbar * statusbar()
{
	UIStatusbar* p = NULL;
	UIView* parent;
	parent = screen_current_view();
	if (parent) {
		p = M_ui_statusbar(parent->get_child(kUI_IDStatusBar));
	}
	if (!p) {
		p = M_ui_statusbar(ui_view_desktop()->get_child(kUI_IDStatusBar));
	}
	ssz_assert(p);
	return p;
}

void statusbar_init()
{
	UIStatusbar* p;
	p = statusbar();
	p->set_battery(is_use_extern_power(), is_battery_exist(), battery_level());
	p->set_time(ssz_time_now());
	if (is_use_extern_power()) {
		statusbar()->show_icon(kStatusbarIconOrderAC, get_image(kImgAc));
	}
	else {
		statusbar()->hide_icon(kStatusbarIconOrderAC);
	}
	if (is_keypad_locked()) {
		statusbar()->show_icon(kStatusbarIconOrderLock, get_image(kImgLock));
	}
	else {
		statusbar()->hide_icon(kStatusbarIconOrderLock);
	}
}
