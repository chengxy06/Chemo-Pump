/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_text_base.h"
#include "ssz_time.h"
#include "image_data.h"
#include "sys_power.h"
/************************************************
* Declaration
************************************************/
//the statusbar's text(used to show title) max size
#define STATUSBAR_TEXT_MAX_SIZE 60
#define STATUSBAR_TIME_STR_SIZE 6

//the icon will show as below order
typedef enum 
{
	kStatusbarIconOrderAC,
    kStatusbarIconOrderLock,
    kStatusbarIconOrderMute,
    kStatusbarIconOrderMax,
}StatusbarIconOrderID;

#ifdef SSZ_TARGET_SIMULATOR
#define M_ui_statusbar(view) (dynamic_cast<UIStatusbar*>(view))
#else
#define M_ui_statusbar(view) ((UIStatusbar*)(view))
#endif

class UIStatusbar: public UITextBase{
public:
	UIStatusbar();
public://msg handle
	void on_create();
	void on_draw();
	void on_size_change();

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);

public://method
	void set_title(const char* text);
	void set_time(const SszDateTime* date_time);
	void set_battery(bool is_ac_connect, bool is_battery_exist, BatteryLevel level);
	void show_icon(StatusbarIconOrderID icon_order, const Image* icon);
	void hide_icon(StatusbarIconOrderID icon_order);

protected://protected method
	void set_icon(StatusbarIconOrderID icon_order, const Image* icon, bool is_show);
	void update_icons();
protected://property
	char m_text[STATUSBAR_TEXT_MAX_SIZE];
	char m_time_str[STATUSBAR_TIME_STR_SIZE];
	const Image* m_icon_imgs[kStatusbarIconOrderMax];
};



