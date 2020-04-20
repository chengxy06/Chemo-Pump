#include "ui_list_base.h"
#include "ssz_common.h"
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

UIListBase::UIListBase()
{
	m_bk_color = DEFAULT_LIST_BACKGROUND_COLOR;
	m_item_bk_colors[kUIListItemUnSelected] = DEFAULT_LIST_ITEM_BACKGROUND_COLOR;
	m_item_bk_colors[kUIListItemSelected] = DEFAULT_LIST_ITEM_BACKGROUND_COLOR_WHEN_SELECTED;
	m_item_bk_colors[kUIListItemUnSelected] = DEFAULT_LIST_ITEM_BACKGROUND_COLOR_WHEN_DISABLE;

	m_select_index = -1;
	m_item_height = ui_rect_ysize(m_rect);
	m_item_width = ui_rect_xsize(m_rect);
	m_show_start_pos.x = 0;
	m_show_start_pos.y = 0;
	m_layout_orientation = kUIVertical;
	m_item_align = UI_ALIGN_LEFT| UI_ALIGN_VCENTER;
	m_first_item_offset_at_list.x = 0;
	m_first_item_offset_at_list.y = 0;
	enable_accept_focus();
}

void UIListBase::on_draw()
{
	set_draw_attributes();
	//clear list background
	ui_clear();

	int item_start_index;
	int item_end_index;
	int item_num;
	UIListItemDrawInfo draw_info;

	if (m_layout_orientation==kUIVertical) {
		//find the first visible item
		item_start_index = m_show_start_pos.y / m_item_height;
		item_end_index = (m_show_start_pos.y + ui_rect_ysize(m_rect) - 1) / m_item_height;
	}
	else {
		//find the first visible item
		item_start_index = m_show_start_pos.x / m_item_width;
		item_end_index = (m_show_start_pos.x + ui_rect_xsize(m_rect) - 1) / m_item_width;
	}
	item_num = size();

	Rect view_canvas_rect;
	ui_canvas_rect(&view_canvas_rect);
	Rect view_canvas_limit_rect;
	ui_limit_rect_of_canvas(&view_canvas_limit_rect);

	Rect item_canvas_rect;
	Rect item_canvas_limit_rect;
	for (; item_start_index <= item_end_index&&item_start_index < item_num; item_start_index++) {
		draw_info.item_index = item_start_index;
		item_draw_info(&draw_info);

		//set the canvas as the item size
		item_rect_at_list(item_start_index, &item_canvas_rect);
		ui_rect_move(&item_canvas_rect, m_rect.x0, m_rect.y0);
		ui_set_canvas_rect_ex(&item_canvas_rect);
		//set the limit
		item_canvas_limit_rect = item_canvas_rect;
		ui_rect_intersect(&item_canvas_limit_rect, &view_canvas_limit_rect);
		ui_set_limit_rect_of_canvas_ex(&item_canvas_limit_rect);

		ui_rect_init_by_size(draw_info.item_rect, 0, 0, m_item_width, m_item_height);
		draw_item(draw_info);
	}

	//restore
	//ui_set_canvas_rect_ex(&view_canvas_rect);
	//ui_set_limit_rect_of_canvas_ex(&view_canvas_limit_rect);
}

void UIListBase::on_msg_default(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgKeyPress:
		switch (msg.key_info_param.key)
		{
		case UI_KEY_INCREASE:
			select_next();
			is_handled = true;
			break;
		case UI_KEY_DECREASE:
			select_prev();
			is_handled = true;
			break;
		}
		break;
	case kUIMsgSetAlign:
		set_item_align(msg.int_param);
		is_handled = true;
		break;
	}
	if (!is_handled) {
		UIDrawBase::on_msg_default(msg);
	}
}

int UIListBase::backup_context(uint8_t * context_memory, int memory_size)
{
	if (memory_size >= context_size()) {
		ListBaseContext context = { m_show_start_pos, m_select_index };
		memcpy(context_memory, &context, sizeof(context));
		return sizeof(context);
	}
	else {
		return 0;
	}
}

int UIListBase::restore_context(uint8_t * context_memory, int memory_size)
{
	if (memory_size >= context_size()) {
		ListBaseContext context;
		memcpy(&context, context_memory, sizeof(context));
		//m_select_index = context.m_select_index;
		m_show_start_pos = context.m_show_start_pos;
		select(context.m_select_index);
		invalidate();
		return sizeof(context);
	}
	else {
		return 0;
	}
}

void UIListBase::item_rect_at_list(int index, Rect * rect)
{
	if (m_layout_orientation == kUIVertical) {
		ui_rect_init_by_size(*rect, -m_show_start_pos.x,
			m_item_height*index - m_show_start_pos.y+ m_first_item_offset_at_list.y,
			m_item_width, m_item_height);
	}
	else {
		ui_rect_init_by_size(*rect, m_item_width*index - m_show_start_pos.x + m_first_item_offset_at_list.x,
			-m_show_start_pos.y,
			m_item_width, m_item_height);
	}
}

int UIListBase::get_selected()
{
	return m_select_index;
}

void UIListBase::select(int index)
{
	if (m_select_index != index) {
		//set unselect previous item
		set_item_select_state(m_select_index, false);
		//set select current
		set_item_select_state(index, true);
		int prev_index = m_select_index;
		m_select_index = index;
		if (is_index_valid(m_select_index) && !is_item_visible(m_select_index)) {
			if (m_select_index > prev_index) {
				if (m_layout_orientation == kUIVertical) {
					m_show_start_pos.y = m_select_index*m_item_height -
						(ui_rect_ysize(m_rect) - m_item_height);
				}
				else {
					m_show_start_pos.x = m_select_index*m_item_width -
						(ui_rect_xsize(m_rect) - m_item_width);
				}
			}
			else {
				if (m_layout_orientation == kUIVertical) {
					m_show_start_pos.y = m_select_index*m_item_height;
				}
				else {
					m_show_start_pos.x = m_select_index*m_item_width;
				}
			}
			ssz_assert(m_show_start_pos.x >= 0 && m_show_start_pos.y >= 0);
			invalidate();
		}
		notify_parent(kUINotifySelectChanged);
	}
}

void UIListBase::select_next()
{
	int index = m_select_index + 1;
	while (is_index_valid(index) && is_item_disabled(index)) {
		index++;
	}

	if (is_index_valid(index)) {
		select(index);
	}
}

void UIListBase::select_prev()
{
	int index = m_select_index - 1;
	while (is_index_valid(index) && is_item_disabled(index)) {
		index--;
	}

	if (is_index_valid(index)) {
		select(index);
	}
}

void UIListBase::select_if_no_select(int index)
{
	if (m_select_index <0) {
		select(index);
	}
}

UIOrientation UIListBase::layout_orientation()
{
	return m_layout_orientation;
}

void UIListBase::set_layout_orientation(UIOrientation orientation)
{
	m_layout_orientation = orientation;
	invalidate();
}


bool UIListBase::is_item_selected(int index)
{
	return (item_status(index)&UI_LIST_ITEM_SELECTED) != 0;
}

void UIListBase::set_item_select_state(int index, bool is_select)
{
	if (is_select) {
		set_item_status(index, item_status(index)|UI_LIST_ITEM_SELECTED);
	}
	else {
		set_item_status(index, item_status(index)&(~UI_LIST_ITEM_SELECTED));
	}
	invalidate_item(index);
}

bool UIListBase::is_item_disabled(int index)
{
	return (item_status(index)&UI_LIST_ITEM_DISABLED) != 0;
}

void UIListBase::set_item_disable_state(int index, bool is_disable)
{
	if (is_disable) {
		set_item_status(index, item_status(index) | UI_LIST_ITEM_DISABLED);
	}
	else {
		set_item_status(index, item_status(index)&(~UI_LIST_ITEM_DISABLED));
	}
	invalidate_item(index);
}

void UIListBase::set_item_bk_color(UIListItemState state, color_t color)
{
	m_item_bk_colors[state] = color;
	invalidate();
}

void UIListBase::set_item_height(int height)
{
	m_item_height = height;
	invalidate();
}

void UIListBase::set_item_width(int width)
{
	m_item_width = width;
	invalidate();
}

void UIListBase::set_item_align(uint8_t align)
{
	if (m_item_align != align) {
		m_item_align = align;
		invalidate();
	}
}

void UIListBase::invalidate_item(int index)
{
	if (is_index_valid(index)) {
		Rect item_rect;
		item_rect_at_list(index, &item_rect);
		Rect list_rect;
		area_self(&list_rect);
		if (ui_rect_intersect(&list_rect, &item_rect)) {
			invalidate_by_rect(list_rect);
		}
	}
}

bool UIListBase::is_item_visible(int index)
{
	if (is_index_valid(index)) {
		Rect item_rect;
		item_rect_at_list(index, &item_rect);
		Rect list_rect;
		area_self(&list_rect);
		if (ui_rect_is_include(&list_rect, &item_rect)) {
			return true;
		}
	}
	return false;
}

bool UIListBase::is_index_valid(int index)
{
	if (index >= 0 && index < size()) {
		return true;
	}
	else {
		return false;
	}
}
