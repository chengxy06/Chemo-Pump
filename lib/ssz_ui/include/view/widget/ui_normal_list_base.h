/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_list_base.h"
#include "ui_draw_base.h"
#include "ssz_vector.h"
/************************************************
* Declaration
************************************************/

class UINormalListBase: public UIListBase{
public:

public://msg handle

public://virtual method
	//each child class should implement it
	//virtual void on_msg(UIMsg &msg) {};

	virtual int size();
	//e.g. UI_LIST_ITEM_UNSELECTED
	virtual uint8_t item_status(int index);
	virtual void set_item_status(int index, uint8_t status);

	//the item index is set before call, this function need set the item data and status by the item index
	virtual void item_draw_info(UIListItemDrawInfo* draw_info);
	//virtual void draw_item(const UIListItemDrawInfo &draw_info) {};
public://method
	//erase one item
	void erase(int index);
	//get the item at the index;
	UIListItem* at(int index);
	void clear();

protected://protected method

protected://property
	SszVector m_list;
};



