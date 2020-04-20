/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_list_base.h"
/************************************************
* Declaration
************************************************/
#if ENABLE_RUN_TIME_TYPE_INFORMATION
#define M_ui_page_control(view) (dynamic_cast<UIPageControl*>(view))
#else
#define M_ui_page_control(view) ((UIPageControl*)(view))
#endif
class UIPageControl: public UIListBase{
public:
	UIPageControl();
public://msg handle

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);

	virtual int size();
	//e.g. UI_LIST_ITEM_UNSELECTED
	virtual uint8_t item_status(int index);
	virtual void set_item_status(int index, uint8_t status) {};

	//the item index is set before call, this function need set the item data and status by the item index
	virtual void item_draw_info(UIListItemDrawInfo* draw_info);
	virtual void draw_item(const UIListItemDrawInfo &draw_info);
public://method
	void set_select_pointer_img(const Image* img);
	void set_unselect_pointer_img(const Image* img);
	void set_size(int page_size);

protected://protected method

protected://property
	const Image* m_select_pointer_img;
	const Image* m_unselect_pointer_img;
	int m_size;
};



