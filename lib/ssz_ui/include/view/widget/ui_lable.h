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
#define M_ui_lable(view) (dynamic_cast<UILable*>(view))
#else
#define M_ui_lable(view) ((UILable*)(view))
#endif

class UILable: public UITextBase{
public:
	UILable();
public://msg handle
	void on_draw();

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);

public://method
	void set_text(const char* text);
	void set_text_and_force_invalid(const char* text);
protected://protected method
protected://property
	const char* m_text;
};



