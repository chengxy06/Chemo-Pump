/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_text_base.h"
/************************************************
* Declaration
************************************************/

class UIDynamicList: public UITextBase{
public:
	UIDynamicList();
public://msg handle
	void on_draw();

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);

public://method
	void set_text(const char* text);
protected://protected method
protected://property
	const char* m_text;
};



