/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ssz_def.h"
#include "ui.h"
#include "ssz_ui_view_config.h"
#include "ui_utility.h"
#include "ssz_timer.h"
/************************************************
* Declaration
************************************************/

/************************************************
* view status
************************************************/
//can combination
//#define UI_VIEW_SHOW	0   //show it 
#define UI_VIEW_HIDE	0x1 //not show
#define UI_VIEW_DISABLE		0x2  //will not respond key and touch, can not focus
#define UI_VIEW_DIRTY		0x4  //will need redraw
#define UI_VIEW_ENABLE_ACCEPT_FOCUS 0x8  

/************************************************
* align
************************************************/
#define UI_ALIGN_LEFT	UI_TEXT_ALIGN_LEFT
#define UI_ALIGN_RIGHT	UI_TEXT_ALIGN_RIGHT
#define UI_ALIGN_HCENTER	UI_TEXT_ALIGN_HCENTER //___(horizontal) center
//below is used to get the horizontal align by align&GUI_ALIGN_HORIZONTAL
#define GUI_ALIGN_HORIZONTAL (UI_ALIGN_LEFT|UI_ALIGN_RIGHT|UI_ALIGN_HCENTER)

#define UI_ALIGN_TOP	UI_TEXT_ALIGN_TOP
#define UI_ALIGN_VCENTER	UI_TEXT_ALIGN_VCENTER
#define UI_ALIGN_BOTTOM	UI_TEXT_ALIGN_BOTTOM
//below is used to get the vertical align by align&GUI_ALIGN_VERTICAL
#define GUI_ALIGN_VERTICAL   (UI_ALIGN_TOP|UI_ALIGN_VCENTER|UI_ALIGN_BOTTOM)

#define UI_ALIGN_CENTER (UI_ALIGN_HCENTER|UI_ALIGN_VCENTER) //center at horizontal and vertical 

/************************************************
* layout orientation
************************************************/
enum UIOrientation {
	kUIVertical, //         |
	kUIHorizontal, //      ___
};

/************************************************
* UI key, define the key at the keyboard
************************************************/
#define UI_KEY_INCREASE			 0x01
#define UI_KEY_DECREASE			 0x02
#define UI_KEY_BACKSPACE         0x08         /* ASCII: BACKSPACE */
#define UI_KEY_TAB               0x09         /* ASCII: TAB       */
#define UI_KEY_ENTER             0x0D        /* ASCII: ENTER     */
#define UI_KEY_ESCAPE            0x1B
#define UI_KEY_SPACE             0x20
#define UI_KEY_PGUP  	         0x21
#define VK_KEY_PGDOWN            0x22
#define UI_KEY_END               0x23
#define UI_KEY_HOME              0x24
#define UI_KEY_LEFT              0x25
#define UI_KEY_UP                0x26
#define UI_KEY_RIGHT             0x27
#define UI_KEY_DOWN              0x28
#define UI_KEY_DELETE            0x2E


/*
 * UI_KEY_0 - UI_KEY_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * UI_KEY_A - UI_KEY_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

/************************************************
* UI message
************************************************/
typedef enum {
	//UIView send 
	kUIMsgCreate=1, /* The first message received */
	kUIMsgPosChanged, /* view has been moved*/
	kUIMsgSizeChange,
	kUIMsgEnable,
	kUIMsgDisable,
	kUIMsgDestroy,//view destroy
	kUIMsgDraw, //draw view
	kUIMsgForceDrawAfterAllDraw, //force draw after draw view and it's child
	kUIMsgGetFocus, //view has get focus
	kUIMsgChildGetFocus, //view has get focus
	kUIMsgLostFocus, //view has lost focus
	kUIMsgChildLostFocus, //view has lost focus

	//other send
	kUIMsgNotifyParent, //notify parent about something happen at child.
						//the msg take the UINotifyID
	kUIMsgSetText, //set text, the msg take the text
	kUIMsgSetImg, //set img, the msg take the image
	kUIMsgSetFont, //set font, the msg take the font
	kUIMsgSetAlign, //set align, the msg take the align(e.g. UI_ALIGN_LEFT)

	kUIMsgKeyPress, //Key has been pressed,the msg take the key_info
	kUIMsgKeyRelease, //Key has been released,the msg take the key_info
	kUIMsgChar, //user input a char, the msg take the char
	kUIMsgTimer, //Timer has expired,the msg take the timer pos
	kUIMsgUserStart=1000, //after it are reserved for user messages
}UIMsgID;

//used for kUIMsgNotifyParent
typedef enum {
	kUINotifyClicked = 1,
	kUINotifyReleased,
	kUINotifySelectChanged,
	kUINotifyValueChanged,
	kUINotifyChildDestroy,
	kUINotifyUserStart=100, //after it are reserved for user messages
}UINotifyID;

class UIView;

typedef struct{
	int msg_id;            /* message */
	//UIView *dest;         /* Destination View*/
	UIView *src;      /* Source View  */
	union {
		const void * addr_param;
		int32_t int_param;
		struct {
			int16_t key;
			int16_t repeat_count;
		}key_info_param;
	} ;
}UIMsg;

typedef void UIViewCallback(UIMsg *msg);

class UIView{

public://method
	void attach_to_parent(UIView* parent_p);
	void detach_from_parent();
	void add_child(UIView* child);
	void delete_child(UIView* child);
	void delete_all_child();

	//the view need redraw
	void invalidate();
	void invalidate_by_rect(const Rect& rect);
	void invalidate_by_rect_ex(int x, int y, int x_size, int y_size);
	void invalidate_self_by_rect_at_screen(const Rect& rect_at_screen);

	//the view no need redraw
	void validate();

	void set_focus();
	void enable_accept_focus();
	void disable_accept_focus();
	void hide();
	void show();
	void enable();
	void disable();
	void draw();
	//force draw after all child and self draw
	void force_draw_after_draw_all();

	//the x and y are based on the parent
	void set_pos(int x, int y);
	void set_size(int x_size, int y_size);
	void set_area(int x, int y, int x_size, int y_size);
	void set_id(int id) { m_id = id; };

	bool is_enable();
	bool is_self_and_all_parent_visible();//it will check parents
	bool is_visible();
	bool is_focused();
	bool is_enable_focused();
	bool is_dirty();
	void pos_at_screen(Point* pos) { pos->x = m_rect.x0; pos->y = m_rect.y0; };
	void size(Size* sz) { sz->x_size = ui_rect_xsize(m_rect); sz->y_size = ui_rect_ysize(m_rect); };
	int xsize() {return ui_rect_xsize(m_rect);	}
	int ysize() { return ui_rect_ysize(m_rect); }
	void dirty_rect(Rect* dest);
	void dirty_rect_at_screen(Rect* dest) { *dest = m_dirty_rect; }
	//this will return the rect which limit by all parents
	void real_dirty_rect_at_screen(Rect* dest);
	UIView* first_child() { return m_first_child; };
	UIView* parent() { return m_parent; };
	UIView* next_sibling() { return m_next_sibling; };
	int id() { return m_id; }
	//the x and y are based on the parent
	void area(Rect* rc_at_parent);
	//the x and y are always 0
	void area_self(Rect *rc);
	//the x and y are based on the screen
	void area_at_screen(Rect *rc_at_screen);

	void to_screen_pos(Point* pt_at_screen);
	//find child
	UIView* get_child(int child_id);
	//find descendant
	UIView* get_descendant(int descendant_id);
	UIView* last_descendant();

	void send_msg_no_para(UIView* dest, int msg_id);
	void send_msg_with_para(UIView* dest, int msg_id, int32_t param);
	void send_msg(UIView* dest, UIMsg& msg);
	void on_msg_default(UIMsg &msg);
	void notify_parent(int notify_id);
	void set_canvas_for_draw();

	int context_size_with_descendant();
	//backup view and descendant's context(e.g. select index) to the memory
	//it used to restore previous screen's state when back to show previous screen
	bool backup_context_with_descendant(uint8_t* context_memory, int memory_size, int *used_memory_size);
	//restore view and descendant's context(e.g. set the select index) 
	bool restore_context_with_descendant(uint8_t* context_memory, int memory_size, int *used_memory_size);

private:
	//the view need redraw
	bool invalidate_self();
	bool invalidate_self_by_rect(const Rect& rect);
	void move_but_no_invalidate(int x_size, int y_size);

public:
	//each child class should implement it
	virtual void on_msg(UIMsg &msg) =NULL;
	virtual int context_size() { return 0; };
	//backup view's context(e.g. select index) to the memory, return the used memory size
	//it used to restore previous screen's state when back to show previous screen
	virtual int backup_context(uint8_t* context_memory, int memory_size) { return 0; };
	//restore view's context(e.g. set the select index), return the used memory size
	virtual int restore_context(uint8_t* context_memory, int memory_size) { return 0; };



public: //friend function
	friend  UIView* ui_view_create(UIViewTypeID type, int id, UIView* parent, int x, int y, int x_size, int y_size, uint32_t status);
	friend void ui_view_destroy(UIView* view);

protected: //property
	Rect m_rect; //the view size and pos at screen
	Rect m_dirty_rect; //the view invalid rect which need draw to screen, based on screen
	uint32_t m_status;

	UIView* m_next_sibling;
	UIView* m_parent;
	UIView* m_first_child;
	int m_id;
};

//create a view, the view must be register first and must have free view memory to create
UIView* ui_view_create(UIViewTypeID type, int id, UIView* parent, int x, int y, int x_size, int y_size, uint32_t status);
//delete the view
void ui_view_destroy(UIView* view);
//regist the view
void ui_view_regist(UIViewTypeID type, int one_view_size, void* one_view_memory);

void ui_view_send_msg(UIView* dest, UIMsg& msg);

//the root view, UI only have one root view and it is the desktop
void ui_view_create_desktop_if_not();
UIView* ui_view_desktop();
UIView* ui_view_focused();


//return the timer, 
//	-1: create fail
SszTimer ui_view_create_timer(UIView* view);
void ui_view_delete_timer(SszTimer timer);
//delete all timer associated with the view
void ui_view_delete_associated_timer(UIView* view);
//start a timer(previous timer will be clear),
//when milliseconds from now is pass, the handle function will be called.
//Note: the timer only run once,
void ui_view_start_oneshot_after(SszTimer timer, int32_t milliseconds);
//stop the timer
void ui_view_stop_timer(SszTimer timer);
void ui_view_handle_all_timer();
#ifdef __cplusplus
extern "C" {
#endif
void ui_view_output_all_view_info();
bool ui_view_is_someone_dirty();
void ui_view_draw_all_dirty();
void ui_view_enable_trace();
void ui_view_disable_trace();
void ui_view_enable_trace_draw();
void ui_view_disable_trace_draw();

void ui_view_init();

void ui_view_start_scan_timer();
void ui_view_stop_scan_timer();
//user must implement it,
//it need regist all used view,
void ui_view_config();
//user must implement it, user should call the class's constructed function
void ui_view_init_obj(UIViewTypeID view_type, UIView* view, int one_view_size);

#ifdef __cplusplus
}
#endif


