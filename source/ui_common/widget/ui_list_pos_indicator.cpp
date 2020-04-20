#include "ui_list_pos_indicator.h"
#include "ssz_common.h"
#include "image_data.h"

/************************************************
* Declaration
************************************************/
#define ICON_INDICATOR_GAP_TO_TEXT 4

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

UIListPosIndicator::UIListPosIndicator()
{
	m_max_value = 100;
	m_value = 1;
}

void UIListPosIndicator::on_draw()
{
	set_draw_attributes();
	ui_clear();
	char tmp[20];
	snprintf(tmp, sizeof(tmp), "%d/%d", m_value, m_max_value);

	Rect rc;
	area_self(&rc);
	ui_disp_text_in_rect(&rc, tmp, UI_TEXT_ALIGN_CENTER);

	const Image* img;
	if (m_value>1) {
		img = get_image(kImgLeftArrow);
		int text_xsize = ui_get_text_xsize(tmp);
		int x, y;
		y = (ui_rect_ysize(rc) - img->height) / 2;
		x = (ui_rect_xsize(rc) - text_xsize) / 2 - ICON_INDICATOR_GAP_TO_TEXT - img->width;
		ui_draw_img_at(x, y, img);
	}

	if (m_value<m_max_value) {
		img = get_image(kImgRightArrow);
		int text_xsize = ui_get_text_xsize(tmp);
		int x, y;
		y = (ui_rect_ysize(rc) - img->height) / 2;
		x = (ui_rect_xsize(rc) - text_xsize) / 2 + ICON_INDICATOR_GAP_TO_TEXT + text_xsize ;
		ui_draw_img_at(x, y, img);
	}

}

void UIListPosIndicator::on_msg(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgDraw:
		on_draw();
		is_handled = true;
		break;
	}
	if (!is_handled) {
		on_msg_default(msg);
	}
}


void UIListPosIndicator::set_max_value(int max_value)
{
	m_max_value = max_value;
	if (m_value>m_max_value) {
		m_value = m_max_value;
	}
	invalidate();
}

void UIListPosIndicator::set_value(int value)
{
	if (m_value==value) {
		return;
	}
	if (value>m_max_value) {
		m_value = m_max_value;
	}
	else if (value < 1) {
		m_value = 1;
	}
	else {
		m_value = value;
	}
	invalidate();
	notify_parent(kUINotifyValueChanged);
}

void UIListPosIndicator::increase()
{
	set_value(m_value + 1);
}

void UIListPosIndicator::decrease()
{
	set_value(m_value - 1);
}

