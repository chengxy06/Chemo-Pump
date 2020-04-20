/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_normal_list_base.h"

/************************************************
* Declaration
************************************************/
#define UI_IMAGE_LIST_ITEM_MAX_SIZE 10

#if ENABLE_RUN_TIME_TYPE_INFORMATION
#define M_ui_image_list(view) (dynamic_cast<UIImageList*>(view))
#else
#define M_ui_image_list(view) ((UIImageList*)(view))
#endif

class UIImageList: public UINormalListBase{
public:
	struct UIImageListItem:public UIListItem {
	public:
		const Image* img;
		const Image* selected_img;
	};
public:
	UIImageList();
public://msg handle

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);
	virtual void draw_item(const UIListItemDrawInfo &draw_info);

public://method
	//add one item to last
	void push_back(const Image* img, const Image* selected_img);
	//insert item before the index
	void insert(int index, const Image* img, const Image* selected_img);

protected://protected method

protected://property
	UIImageListItem m_list_items[UI_IMAGE_LIST_ITEM_MAX_SIZE];
};



