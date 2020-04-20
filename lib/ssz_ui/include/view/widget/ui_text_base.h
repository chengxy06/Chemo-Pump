/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_draw_base.h"
/************************************************
* Declaration
************************************************/

class UITextBase: public UIDrawBase {
public:
	UITextBase();
public://msg handle
	void on_msg_default(UIMsg &msg);
public://virtual method
	//each child class should implement it
	//virtual void on_msg(UIMsg &msg)=NULL;

public://method
	//e.g. UI_TEXT_ALIGN_HCENTER|UI_TEXT_ALIGN_VCENTER|UI_TEXT_NORMAL
	void set_text_mode(int mode);
	void set_text_color(color_t color);
	void set_font(const Font* font);
#if ENABLE_TEXT_BASE_SPACE_ATTRIBUTE
	void set_text_char_space(int char_space);
	void set_text_line_space(int line_space);
#endif

protected://protected method
	//set draw mode,bk color,text attributes...
	void set_draw_attributes();

protected://property
	int m_text_mode;//default: UI_TEXT_ALIGN_HCENTER|UI_TEXT_ALIGN_VCENTER|UI_TEXT_NORMAL
	color_t m_text_color;
	const Font* m_text_font;

#if ENABLE_TEXT_BASE_SPACE_ATTRIBUTE
	int m_text_char_space;
	int m_text_line_space;
#endif
};



