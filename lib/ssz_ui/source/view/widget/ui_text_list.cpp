#include "ui_text_list.h"
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

UITextList::UITextList()
{
	ssz_vector_init(&m_list, m_list_items, ssz_array_size(m_list_items), ssz_array_node_size(m_list_items));
	m_item_text_colors[kUIListItemUnSelected] = DEFAULT_LIST_ITEM_TEXT_COLOR;
	m_item_text_colors[kUIListItemSelected] = DEFAULT_LIST_ITEM_TEXT_COLOR_WHEN_SELECTED;
	m_item_text_colors[kUIListItemUnSelected] = DEFAULT_LIST_ITEM_TEXT_COLOR_WHEN_DISABLE;

	m_text_font = DEFAULT_FONT;
}

void UITextList::on_msg(UIMsg & msg)
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

void UITextList::draw_item(const UIListItemDrawInfo & draw_info)
{
	int color_index = kUIListItemUnSelected;
	uint32_t text_mode = m_item_align;
	if (draw_info.status&UI_LIST_ITEM_SELECTED) {
		color_index = kUIListItemSelected;
	}
	else if (draw_info.status&UI_LIST_ITEM_DISABLED) {
		color_index = kUIListItemDisabled;
	}



	const UITextListItem* p = (UITextListItem*)draw_info.item_data;

	ui_set_bk_color(m_item_bk_colors[color_index]);
	ui_set_text_color(m_item_text_colors[color_index]);
	ui_set_font(m_text_font);
	ui_clear();
	ui_disp_text_in_rect(&draw_info.item_rect, p->text, text_mode);
}

void UITextList::push_back(const char* text)
{
	UITextListItem item;
	item.status = 0;
	item.text = text;
	if (ssz_vector_push_back(&m_list, &item) != -1) {
		invalidate_item(ssz_vector_size(&m_list) - 1);
		notify_parent(kUINotifyValueChanged);
		select_if_no_select(0);
	}
	else {
		ssz_assert_fail();
	}
}


void UITextList::insert(int index, const char* text)
{
	UITextListItem item;
	item.status = 0;
	item.text = text;
	if (ssz_vector_insert(&m_list, index, &item) != -1) {
		invalidate();
		notify_parent(kUINotifyValueChanged);
		select_if_no_select(0);
	}
	else {
		ssz_assert_fail();
	}
}

void UITextList::set_text(int index, const char * text)
{
	UITextListItem *item;
	item = (UITextListItem*)ssz_vector_at_index(&m_list, index);
	if (item) {
		if (item->text!= text) {
			item->text = text;
			invalidate_item(index);
			notify_parent(kUINotifyValueChanged);
		}
	}
	else {
		ssz_assert_fail();
	}
}

void UITextList::set_text_color(UIListItemState state, color_t color)
{
	m_item_text_colors[state] = color;
	invalidate();
}

void UITextList::set_font(const Font * font)
{
	m_text_font = font;
	invalidate();
}

