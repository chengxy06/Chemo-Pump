/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_normal_list_base.h"

/************************************************
* Declaration
************************************************/
#define UI_TEXT_LIST_ITEM_MAX_SIZE 20
#if ENABLE_RUN_TIME_TYPE_INFORMATION
#define M_ui_text_list(view) (dynamic_cast<UITextList*>(view))
#else
#define M_ui_text_list(view) ((UITextList*)(view))
#endif

class UITextList: public UINormalListBase{
public:
	struct UITextListItem:public UIListItem {
	public:
		const char* text;
	};
	UITextList();
public://msg handle
	void on_create();

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);
	virtual void draw_item(const UIListItemDrawInfo &draw_info);

public://method
	//add one item to last
	void push_back(const char* text);
	//insert item before the index
	void insert(int index, const char* text);
	void set_text(int index, const char* text);

	void set_text_color(UIListItemState state, color_t color);
	void set_font(const Font* font);

protected://protected method

protected://property
	UITextListItem m_list_items[UI_TEXT_LIST_ITEM_MAX_SIZE];
	color_t m_item_text_colors[UIListItemStateMax];
	const Font* m_text_font;
};



