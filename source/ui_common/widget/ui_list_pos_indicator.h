/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_text_base.h"
/************************************************
* Declaration
************************************************/
#if ENABLE_RUN_TIME_TYPE_INFORMATION
#define M_ui_list_pos_indicator(view) (dynamic_cast<UIListPosIndicator*>(view))
#else
#define M_ui_list_pos_indicator(view) ((UIListPosIndicator*)(view))
#endif

class UIListPosIndicator: public UITextBase{
public:
	UIListPosIndicator();
public://msg handle
	void on_draw();

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);

public://method
	void set_max_value(int max_value);

	void set_value(int value);
	int value() { return m_value; };
	//increase
	void increase();
	//decrease
	void decrease();
protected://protected method
protected://property
	int m_max_value;
	int m_value;
};



