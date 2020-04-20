/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "screen_config.h"
#include "ui_window.h"
#include "timer.h"

/************************************************
* Declaration
************************************************/

#define screen_printfln(...) ssz_printfln_ex(is_module_enable_output(kModuleScreen),SCREEN_MODULE_NAME,__VA_ARGS__)

#define SCREEN_MODE_ENTER_FAIL_IF_EXIST 0x1
#define SCREEN_MODE_DELETE_EXIST_BEFORE_ENTER 0x2
#define SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE 0x4

//it can be used to monitor the screen changed
typedef enum
{
    kScreenEventBeforeActivate,
    kScreenEventAfterActivate,
    kScreenEventAfterDeactivate,
    kScreenEventIDMax
}ScreenEvent;

enum {
	kUIMsgScrCreate = kUIMsgUserStart,
	//used to add can add widget, set widget's attribute(like color, font, text, add item to list)
	//it will be send after screen created and widgets(related with screen) created
	kUIMsgScrWidgetInit,
	//used to set title, bind key, set timer...
	//it will be send after widget init, context restore and view show
	kUIMsgScrActive,
	//used to clear timer...
	//it will be send before context backup, view destroy
	kUIMsgScrDeactivate,
	//used to free memory which malloc at create
	//it will be send after view destroy, deactivate
	kUIMsgScrDestroy,
	//user can add custom msg at below
};


typedef UIMsg ScreenMsg;
struct ScreenStruct;
typedef void (*ScreenCallback)(struct ScreenStruct* scr, ScreenMsg* msg);

typedef struct ScreenStruct {
	Rect screen_rect;
	UIWindow* view;
	const int16_t * widgets_define;
	ScreenCallback callback;
	uint8_t* context;
	void* user_data;
	int16_t context_size;
	int16_t widgets_define_size;
	uint8_t enter_mode;
	ScreenID screen_id;
	//used to indicate current handle state
	//1:create, 2:widget init, 3:active, 4:deactive, 5:destroy
	//int8_t handle_state;
}Screen;
typedef void(*ScreenEventFunc)(Screen* scr);


//init screen, set the home screen's id
void screen_init(int home_screen_id);

//create and show the screen, the screen's default size = LCD size - statusbar size, the pos is at the back of statusbar
//Notice: it will enter fail if the screen is already exist
Screen* screen_enter(ScreenID screen_id, ScreenCallback cb, void* user_data, const int16_t * widgets_define, int widgets_define_size);
Screen* screen_enter_full_screen(ScreenID screen_id, ScreenCallback cb, void* user_data, const int16_t * widgets_define, int widgets_define_size);

//create and show the screen with the screen size
//screen_rect: if NULL, it's size is the default size
//enter_mode:  
//  0 -> enter normal
//	SCREEN_MODE_ENTER_FAIL_IF_EXIST -> fail if the screen is exist
//	SCREEN_MODE_DELETE_EXIST_BEFORE_ENTER -> delete the screen if exist
//	SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE
Screen* screen_enter_ex(ScreenID screen_id,  ScreenCallback cb, void* user_data, Rect* screen_rect, const int16_t * widgets_define, int widgets_define_size, int enter_mode);

//create the screen with the screen size
//screen_rect: if NULL, it's size is the default size
//  enter_mode:  
//  0 -> enter normal
//	SCREEN_MODE_ENTER_FAIL_IF_EXIST -> fail if the screen is exist
//	SCREEN_MODE_DELETE_EXIST_BEFORE_ENTER -> delete the screen if exist
//	SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE
Screen* screen_create(ScreenID screen_id, ScreenCallback cb, void* user_data, Rect* screen_rect, int enter_mode);

//init the screen and show
//is_need_create_widget: true->create widgets from the widget define
void screen_init_and_show(Screen* scr, bool is_need_create_widget);

void screen_send_msg_no_para(Screen* dest, int msg_id, Screen* src);
void screen_send_msg_with_para(Screen* dest, int msg_id, int32_t param, Screen* src);
void screen_send_msg(Screen* dest, ScreenMsg* msg);

void screen_on_msg_default(Screen * scr, ScreenMsg* msg);

//backup the widgets's some status at screen(e.g. widget's select index)
void screen_backup_context(Screen* scr);
//restore the widgets's some status at screen(e.g. widget's select index)
void screen_restore_context(Screen* scr);

//get current screen
Screen* screen_current(void);

//get current screen's id
int screen_current_id(void);

//get current screen view
UIWindow* screen_current_view(void);

//delete current screen and show previous screen
void screen_go_back(void);

//delete screens until meet the screen, then show the screen, if not find, show the home screen
void screen_go_back_to(Screen* scr);

//delete screens until meet the screen, then show the screen, if not find, show the home screen
void screen_go_back_to_id(int screen_id);

//delete screens until meet the home screen
void screen_go_back_to_home();

//delete the screen
void screen_delete(Screen* scr);

//delete the screen
void screen_delete_by_id(int screen_id);

//check if exist the screen
bool screen_is_exist(int screen_id);

//find the screen
Screen * screen_find(int screen_id);

//bind the event handle, the handle can monitor the screen's change
void screen_set_event_Handler(ScreenEvent event, ScreenEventFunc fun);

//set timer handler for timer in screen, the timer will stop when screen deactivate
//you can use timer interface to operate the timer
void screen_set_timer_handler(TimerID id, TimerHandler func);

//set the screen's title
void screen_set_title(Screen* scr, const char* title);


