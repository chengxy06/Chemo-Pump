/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_draw_base.h"
/************************************************
* Declaration
************************************************/
#define UI_IMAGE_AUTOSIZE 0x1 // Widget size is taken from the attached image
#define UI_IMAGE_REVERSE 0x2 // is draw img reverse

#if ENABLE_RUN_TIME_TYPE_INFORMATION
#define M_ui_image(view) (dynamic_cast<UIImage*>(view))
#else
#define M_ui_image(view) ((UIImage*)(view))
#endif

class UIImage: public UIDrawBase{
public:
	UIImage();
public://msg handle
	void on_draw();

public://virtual method
	   //each child class should implement it
	virtual void on_msg(UIMsg &msg);

public://method
	void set_img(const Image* img);
	void enable_blink();
	void disable_blink();
	void set_blink_period_ms(int period_ms);
	void set_auto_size(bool is_enable);
	void set_reverse(bool is_reverse);

protected://protected method
protected://property
	const Image* m_img;
	int m_blink_period_ms;
	int m_timer_pos;
	uint8_t m_flags;
	bool m_is_hide_at_blink;//false:show, true:hide
};



