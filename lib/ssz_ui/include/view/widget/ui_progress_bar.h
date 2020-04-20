/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_draw_base.h"
/************************************************
* Declaration
************************************************/
#if ENABLE_RUN_TIME_TYPE_INFORMATION
#define M_ui_progress_bar(view) (dynamic_cast<UIProgressBar*>(view))
#else
#define M_ui_progress_bar(view) ((UIProgressBar*)(view))
#endif

class UIProgressBar: public UIDrawBase{
public:
	UIProgressBar();
public://msg handle
	void on_draw();

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);

public://method
	void set_color(color_t progress_bar_color);
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
	color_t m_progress_bar_color;
};



