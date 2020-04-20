#include "ui_page_control.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

UIPageControl::UIPageControl()
{
	m_select_pointer_img = NULL;
	m_unselect_pointer_img = NULL;
	m_size = 0;
	m_layout_orientation = kUIHorizontal;
	m_item_align = UI_ALIGN_HCENTER | UI_ALIGN_BOTTOM;
	m_item_width = DEFAULT_PAGE_CONTROL_ITEM_WIDTH;
	m_select_pointer_img = DEFAULT_PAGE_CONTROL_SELECT_POINTER_IMG;
	m_unselect_pointer_img = DEFAULT_PAGE_CONTROL_UNSELECT_POINTER_IMG;
}

void UIPageControl::on_msg(UIMsg & msg)
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

int UIPageControl::size()
{
	return m_size;
}

uint8_t UIPageControl::item_status(int index)
{
	if (index == m_select_index) {
		return UI_LIST_ITEM_SELECTED;
	}
	else {
		return 0;
	}
}

void UIPageControl::item_draw_info(UIListItemDrawInfo * draw_info)
{
	draw_info->item_data = NULL;
	draw_info->status = item_status(draw_info->item_index);
}

void UIPageControl::draw_item(const UIListItemDrawInfo & draw_info)
{
	ui_clear();
	const Image* img;
	
	if (draw_info.status&UI_LIST_ITEM_SELECTED) {
		img = m_select_pointer_img;
	}
	else {
		img = m_unselect_pointer_img;
	}
	if (!img) {
		return;
	}
	int x0, y0;

	//calc x pos 
	switch (m_item_align&GUI_ALIGN_HORIZONTAL) {
	case UI_ALIGN_LEFT:
		x0 = draw_info.item_rect.x0;
		break;
	case UI_ALIGN_RIGHT:
		x0 = draw_info.item_rect.x1 + 1 - img->width;
		break;
	case UI_ALIGN_HCENTER:
		x0 = draw_info.item_rect.x0 +
			(ui_rect_xsize(draw_info.item_rect) - img->width) / 2;
		break;
	}

	//calc y pos 
	switch (m_item_align&GUI_ALIGN_VERTICAL) {
	case UI_ALIGN_TOP:
		y0 = draw_info.item_rect.y0;
		break;
	case UI_ALIGN_BOTTOM:
		y0 = draw_info.item_rect.y1 + 1 - img->height;
		break;
	case UI_ALIGN_VCENTER:
		y0 = draw_info.item_rect.y0 +
			(ui_rect_ysize(draw_info.item_rect) - img->height) / 2;
		break;
	}

	ui_draw_img_at(x0, y0, img);
}

void UIPageControl::set_select_pointer_img(const Image * img)
{
	m_select_pointer_img = img;
	invalidate();
}

void UIPageControl::set_unselect_pointer_img(const Image * img)
{
	m_unselect_pointer_img = img;
	invalidate();
}

void UIPageControl::set_size(int page_size)
{
	m_size = page_size;
	if (m_layout_orientation == kUIHorizontal) {
		int x0;
		x0 = (ui_rect_xsize(m_rect) - m_size*m_item_width) / 2;
		m_first_item_offset_at_list.x = x0;
	}
	else {
		int y0;
		y0 = (ui_rect_xsize(m_rect) - m_size*m_item_height) / 2;
		m_first_item_offset_at_list.y = y0;
	}
	if (m_select_index<0 || m_select_index>=m_size) {
		m_select_index = 0;
	}
	notify_parent(kUINotifyValueChanged);
	invalidate();
}
