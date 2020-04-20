/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_draw_base.h"
/************************************************
* Declaration
************************************************/
enum UIListItemState {
	kUIListItemUnSelected = 0,
	kUIListItemSelected,
	kUIListItemDisabled,
	UIListItemStateMax,
};

//item status
#define UI_LIST_ITEM_UNSELECTED 0x0
#define UI_LIST_ITEM_SELECTED 0x1
#define UI_LIST_ITEM_DISABLED 0x2

#define UI_LIST_ITEM_STATE_MAX 3

class UIListBase;
struct UIListItem {
public:
	uint8_t status; //e.g. UI_LIST_ITEM_UNSELECTED
};
struct UIListItemDrawInfo {
	Rect item_rect;
	int item_index;
	uint8_t status; //e.g. UI_LIST_ITEM_UNSELECTED
	const UIListItem* item_data;
};
class UIListBase : public UIDrawBase {
public:
	struct ListBaseContext {
		Point m_show_start_pos;
		int m_select_index;
	};
	UIListBase();
public://msg handle
	void on_draw();
	void on_msg_default(UIMsg &msg);

public://virtual method
	//each child class should implement it
	//virtual void on_msg(UIMsg &msg) =NULL;

	virtual int size() =NULL;
	//e.g. UI_LIST_ITEM_UNSELECTED
	virtual uint8_t item_status(int index)=NULL;
	virtual void set_item_status(int index, uint8_t status)=NULL;

	//the item index is set before call, this function need set the item data and status by the item index
	virtual void item_draw_info(UIListItemDrawInfo* draw_info) =NULL;
	virtual void draw_item(const UIListItemDrawInfo &draw_info) =NULL;

	virtual int context_size() { return sizeof(ListBaseContext); };
	//backup view's context(e.g. select index) to the memory, return the used memory size
	//it used to restore previous screen's state when back to show previous screen
	virtual int backup_context(uint8_t* context_memory, int memory_size);
	//restore view's context(e.g. set the select index), return the used memory size
	virtual int restore_context(uint8_t* context_memory, int memory_size);
public://method
	void item_rect_at_list(int index, Rect *rect);

	//get the selected item index 
	int get_selected();
	//unselect old one and select the index
	void select(int index);
	//unselect old one and select next, it will skip the disable item
	void select_next();
	//unselect old one and select previous, it will skip the disable item
	void select_prev();
	//select the index if now not select any one
	void select_if_no_select(int index);
	UIOrientation layout_orientation();
	void set_layout_orientation(UIOrientation orientation);

	bool is_item_selected(int index);
	//set the item selected, it will not clear other item's select state
	void set_item_select_state(int index, bool is_select);

	bool is_item_disabled(int index);
	//select the index
	void set_item_disable_state(int index, bool is_disable);

	void set_item_bk_color(UIListItemState state, color_t color);
	void set_item_height(int height);
	void set_item_width(int width);
	//e.g. UI_ALIGN_LEFT
	void set_item_align(uint8_t align);

	//invalidate the item
	void invalidate_item(int index);
	//is the item can be view at the list
	bool is_item_visible(int index);

protected://protected method
	bool is_index_valid(int index);
protected://property
	color_t m_item_bk_colors[UIListItemStateMax];
	int m_select_index;//-1: no select
	int m_item_height;
	int m_item_width;
	//the items rect is big than list, this show pos is the pos at items rect
	//the show pos's physics pos is always same as the list's top left corner
	//e.g. list rect:0,0,20*20, items rect:0,0,180*180, the pos may be (0,0) or (15,15) or (20,20)...
	Point m_show_start_pos;
	//it used to show item center at list when item is little and can not full of list
	Point m_first_item_offset_at_list;
	UIOrientation m_layout_orientation;//items show as vertical or horizontal
	uint8_t m_item_align;//refer UI_ALIGN_LEFT
};

