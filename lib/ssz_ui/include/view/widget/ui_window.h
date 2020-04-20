/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_view.h"
#include "ui_draw_base.h"
/************************************************
* Declaration
************************************************/
#if ENABLE_RUN_TIME_TYPE_INFORMATION
#define M_ui_window(view) (dynamic_cast<UIWindow*>(view))
#else
#define M_ui_window(view) ((UIWindow*)(view))
#endif

typedef void(*WindowCallback)(void* user_data, UIMsg* msg);
class UIWindow: public UIDrawBase{
public:
	UIWindow();
public://msg handle
	void on_draw();
	void on_msg_default(UIMsg &msg);

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);

public://method

public://property
	WindowCallback m_callback;
	void* m_user_data;

protected://protected method
protected://property

};



