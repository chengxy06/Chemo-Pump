#include "ui_image_list.h"
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

UIImageList::UIImageList()
{
	ssz_vector_init(&m_list, m_list_items, ssz_array_size(m_list_items), ssz_array_node_size(m_list_items));
}

void UIImageList::on_msg(UIMsg & msg)
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

void UIImageList::draw_item(const UIListItemDrawInfo & draw_info)
{
	ui_clear();

	const UIImageListItem* p = (UIImageListItem*)draw_info.item_data;
	int x0, y0;
	const Image* img = p->img;

	if (draw_info.status&UI_LIST_ITEM_SELECTED) {
		img = p->selected_img;
	}
	//else if (draw_info.status&UI_LIST_ITEM_DISABLED) {
	//	
	//}

	if (!img) {
		return;
	}
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

void UIImageList::push_back(const Image* img, const Image* selected_img)
{
	UIImageListItem item;
	item.status = 0;
	item.img = img;
	item.selected_img = selected_img;
	if (ssz_vector_push_back(&m_list, &item) != -1) {
		invalidate_item(ssz_vector_size(&m_list) - 1);
		notify_parent(kUINotifyValueChanged);
		select_if_no_select(0);
	}
	else {
		ssz_assert_fail();
	}
}


void UIImageList::insert(int index, const Image* img, const Image* selected_img)
{
	UIImageListItem item;
	item.status = 0;
	item.img = img;
	item.img = selected_img;
	if (ssz_vector_insert(&m_list, index, &item) != -1) {
		invalidate();
		notify_parent(kUINotifyValueChanged);
		select_if_no_select(0);
	}
	else {
		ssz_assert_fail();
	}
}

