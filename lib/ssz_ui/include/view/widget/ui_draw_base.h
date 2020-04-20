/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_view.h"
/************************************************
* Declaration
************************************************/

class UIDrawBase: public UIView {
public:
	UIDrawBase();
public://msg handle
public://virtual method
	//each child class should implement it
	//virtual void on_msg(UIMsg &msg) =NULL;

public://method
	//void set_draw_mode(int draw_mode);
	void set_bk_color(color_t color);
	
protected://protected method
	//set draw mode, bk color
	void set_draw_attributes();

protected://property
	//int m_draw_mode;//default: UI_DRAW_NORMAL
	color_t m_bk_color;
};



