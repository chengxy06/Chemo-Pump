/************************************************
* DESCRIPTION:
*   
************************************************/
#include "screen.h"
#include "ssz_list.h"
#include "ssz_common.h"
#include "ui_define.h"
#include "ui_common.h"
//#include "timer.h"
//#include "key.h"
#include "stdlib.h"
#include "common.h"
/************************************************
* Declaration
************************************************/
typedef struct {
	SSZ_LIST_NODE_BASE_DEFINE;
	//user should define the real data at below line
	Screen scr;
}ScreenListNode;


/************************************************
* Variable 
************************************************/
static ScreenListNode g_screen_list_nodes[SCREEN_QUEUE_MAX_NUM];
static SszList g_screen_list_inter;
static SszList* g_screen_list = &g_screen_list_inter;
static Screen* g_screen_current = NULL;
//static T_S32 g_screen_current_id = 0;
static ScreenEventFunc g_screen_handlers[kScreenEventIDMax] = { 0 };
static int g_screen_home_id = 0;

/************************************************
* Function 
************************************************/

void screen_window_callback(void* user_data, UIMsg* msg) {
	screen_send_msg((Screen*)user_data, msg);
}
/***********************************************
* Description:
*   notify to active the screen
* Argument:
*   handle:
*
* Return:
*
************************************************/
//lint -e{613}
static void screen_activate(Screen* scr, bool is_need_create_widget)
{
	ssz_assert(scr!=NULL);
	//set current screen
	g_screen_current = scr;

	if (g_screen_handlers[kScreenEventBeforeActivate])
	{
		g_screen_handlers[kScreenEventBeforeActivate](scr);
	}
	if (scr->view == NULL) {
		scr->view = (UIWindow*)ui_view_create(kUIWindow, scr->screen_id, NULL, scr->screen_rect.x0,
			scr->screen_rect.y0,
			ui_rect_xsize((scr->screen_rect)), ui_rect_ysize((scr->screen_rect)), 0);
		ssz_check(scr->view);
		scr->view->m_callback = screen_window_callback;
		scr->view->m_user_data = scr;
	}
	//create widgets
	if (is_need_create_widget && scr->view && scr->widgets_define) {
		ui_create_widgets(scr->view, scr->widgets_define, scr->widgets_define_size);
	}
	//init widgets
	screen_send_msg_no_para(scr, kUIMsgScrWidgetInit, NULL);
	//restore the widget's some status
	screen_restore_context(scr);

	if (scr->view) {
		scr->view->show();
	}
	//give the application a chance to bind the key event, set title,...
	screen_send_msg_no_para(scr, kUIMsgScrActive, NULL);

	if (g_screen_handlers[kScreenEventAfterActivate])
	{
		g_screen_handlers[kScreenEventAfterActivate](scr);
	}
}

/***********************************************
* Description:
*   notify it has deactivate
* Argument:
*   handle:
*
* Return:
*
************************************************/
static void screen_deactivate(Screen* scr, bool is_need_backup_context)
{
	//give the application a chance to close timer,...
	screen_send_msg_no_para(scr, kUIMsgScrDeactivate, NULL);

	if (is_need_backup_context) {
		//backup the widget's some status
		screen_backup_context(scr);
	}

	//delete all widget
	if (scr->view) {
		ui_view_destroy(scr->view);
		scr->view = NULL;
	}

	if (g_screen_handlers[kScreenEventAfterDeactivate])
	{
		g_screen_handlers[kScreenEventAfterDeactivate](scr);
	}
	
	if (g_screen_current == scr) {
		g_screen_current = NULL;
	}
}

void screen_init(int home_screen_id)
{
	g_screen_home_id = home_screen_id;
	ssz_list_init(g_screen_list, (SszListNode*)g_screen_list_nodes, ssz_array_size(g_screen_list_nodes),
		ssz_array_node_size(g_screen_list_nodes));
}

//create and show the screen, the screen's default size = LCD size - statusbar size, the pos is at the back of statusbar
//Notice: it will enter fail if the screen is already exist
Screen * screen_enter(ScreenID screen_id, ScreenCallback cb, void* user_data, const int16_t * widgets_define, int widgets_define_size)
{
	return screen_enter_ex(screen_id, cb, user_data, NULL, widgets_define, widgets_define_size, SCREEN_MODE_ENTER_FAIL_IF_EXIST);
}

Screen * screen_enter_full_screen(ScreenID screen_id, ScreenCallback cb, void* user_data, const int16_t * widgets_define, int widgets_define_size)
{
	Rect rc;
	ui_rect_init_by_size(rc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	return screen_enter_ex(screen_id, cb, user_data, &rc, widgets_define, widgets_define_size, SCREEN_MODE_ENTER_FAIL_IF_EXIST);
}

//screen_rect: if NULL, it's size is the default size
//enter_mode:  
//  0 -> enter normal
//	SCREEN_MODE_ENTER_FAIL_IF_EXIST -> fail if the screen is exist
//	SCREEN_MODE_DELETE_EXIST_BEFORE_ENTER -> delete the screen if exist
//	SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE
Screen * screen_enter_ex(ScreenID screen_id, ScreenCallback cb, void* user_data, Rect * screen_rect, const int16_t * widgets_define, int widgets_define_size, int enter_mode)
{
	if (screen_create(screen_id, cb, user_data, screen_rect, enter_mode)) {
		Screen *scr = (Screen *)ssz_list_back(g_screen_list);
		ssz_assert(scr);
		//lint -e{613}
		scr->widgets_define = widgets_define;
		//lint -e(613)
		scr->widgets_define_size = widgets_define_size;
		screen_init_and_show(scr, true);
		return scr;
	}
	else {
		screen_printfln("screen [%d] create fail!", screen_id);
		ssz_assert_fail();

		return NULL; //lint !e527
	}
}

//create the screen with the screen size
//screen_rect: if NULL, it's size is the default size
//  enter_mode:  
//  0 -> enter normal
//	SCREEN_MODE_ENTER_FAIL_IF_EXIST -> fail if the screen is exist
//	SCREEN_MODE_DELETE_EXIST_BEFORE_ENTER -> delete the screen if exist
//	SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE -> auto delete when deactive
Screen * screen_create(ScreenID screen_id, ScreenCallback cb, void* user_data, Rect * screen_rect, int enter_mode)
{
	if (enter_mode & SCREEN_MODE_ENTER_FAIL_IF_EXIST) {
		if (screen_is_exist(screen_id)) {
			screen_printfln("screen [%d] already exist!", screen_id);
			ssz_assert_fail();
			return NULL; //lint !e527
		}
	}
	else if (enter_mode & SCREEN_MODE_DELETE_EXIST_BEFORE_ENTER) {
		Screen* p = screen_find(screen_id);
		if (p) {
			screen_delete(p);
		}
	}
	Rect area;
	if (screen_rect) {
		area = *screen_rect;
	}
	else {
		area.x0 = SCREEN_WINDOW_X;
		area.y0 = SCREEN_WINDOW_Y;
		area.x1 = SCREEN_WINDOW_X + SCREEN_WINDOW_WIDTH - 1;
		area.y1 = SCREEN_WINDOW_Y + SCREEN_WINDOW_HEIGHT - 1;
	}
	Screen scr;
	ssz_mem_zero(&scr, sizeof(scr));
	scr.screen_rect = area;
	scr.screen_id = screen_id;
	scr.callback = cb;
	scr.user_data = user_data;
	scr.enter_mode = enter_mode;
	if (ssz_list_push_back(g_screen_list, &scr) != -1) {
		Screen* p =(Screen *)ssz_list_back(g_screen_list);
		return p;
	}
	else {
		screen_printfln("no memory to alloc screen [%d]!", screen_id);
		ssz_assert_fail();
		return NULL; //lint !e527
	}
}

void screen_init_and_show(Screen * scr, bool is_need_create_widget)
{
	//notify current screen from activate to deactivate
	Screen * tmp = g_screen_current;
	if (tmp)
	{
		screen_deactivate(tmp, true);
		if (tmp->enter_mode&SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE) {
			screen_delete(tmp);
		}
	}

	screen_send_msg_no_para(scr, kUIMsgScrCreate, NULL);

	//active it
	screen_activate(scr, is_need_create_widget);
}

void screen_send_msg_no_para(Screen * scr, int msg_id, Screen* src)
{
	UIMsg msg;
	ssz_mem_zero(&msg, sizeof(msg));
	if (src) {
		msg.src = src->view;
	}
	msg.msg_id = msg_id;
	screen_send_msg(scr, &msg);
}

void screen_send_msg_with_para(Screen * scr, int msg_id, int32_t param, Screen* src)
{
	UIMsg msg;
	ssz_mem_zero(&msg, sizeof(msg));
	if (src) {
		msg.src = src->view;
	}
	msg.msg_id = msg_id;
	msg.int_param = param;

	screen_send_msg(scr, &msg);
}

void screen_send_msg(Screen * scr, ScreenMsg * msg)
{
	if (scr&&scr->callback) {
		scr->callback(scr, msg);
	}
}

void screen_on_msg_default(Screen * scr, ScreenMsg * msg)
{
	if (scr->view) {
		scr->view->on_msg_default(*msg);
	}
}

void screen_backup_context(Screen * scr)
{
	if (scr->context) {
		free(scr->context);
		scr->context = NULL;
		scr->context_size = 0;
	}
	if (scr->view == NULL)
	{
		return;
	}
	int context_size = scr->view->context_size_with_descendant();
	if (context_size == 0) {
		return;
	}
	scr->context = (uint8_t*)malloc(context_size);
	if (!scr->context) {
		screen_printfln("no memory to alloc context of screen [%d]!", scr->screen_id);
		ssz_assert_fail();
		return;
	}
	scr->context_size = context_size;
	int used_size;
	if (!scr->view->backup_context_with_descendant(scr->context, context_size, &used_size))
	{
		screen_printfln("backup screen[%d] context fail!", scr->screen_id);
		ssz_assert_fail();
	}
}

void screen_restore_context(Screen * scr)
{
	if (scr->context) {
		if (scr->view)
		{
			int used_size;
			if (!scr->view->restore_context_with_descendant(scr->context, scr->context_size, &used_size)) {
				screen_printfln("restore screen[%d] context fail!", scr->screen_id);
				ssz_assert_fail();
			}
		}
		free(scr->context);
		scr->context = NULL;
		scr->context_size = 0;
	}
}

Screen * screen_current(void)
{
	return g_screen_current;
}

int screen_current_id(void)
{
	return g_screen_current?g_screen_current->screen_id:0;
}

UIWindow * screen_current_view(void)
{
	return g_screen_current?g_screen_current->view:NULL;
}

void screen_go_back(void)
{
	screen_delete(g_screen_current);
}

void screen_go_back_to(Screen * scr)
{
	int work_id;
	while (g_screen_current)
	{
		work_id = g_screen_current->screen_id;
		//if now is home, break
		if (work_id == g_screen_home_id )
		{
			break;
		}
		else if (g_screen_current == scr)//if find it, break
		{
			break;
		}
		else //if not the specify screen id, continue goback
		{
			screen_go_back();
		}
	}
}

void screen_go_back_to_id(int screen_id)
{
	int work_id;

	while (g_screen_current)
	{
		work_id = g_screen_current->screen_id;
		//if now is home, break
		if (work_id == g_screen_home_id)
		{
			break;
		}
		else if (work_id == screen_id)//if find it, break
		{
			break;
		}
		else //if not the specify screen id, continue goback
		{
			screen_go_back();
		}
	}
}

void screen_go_back_to_home()
{
	screen_go_back_to_id(g_screen_home_id);
}

void screen_delete_by_pos(int pos)
{
	Screen* p = (Screen*)ssz_list_at_pos(g_screen_list, pos);
	Screen* curr = g_screen_current;
	if (p == curr) {
		screen_deactivate(p, false);
	}
	screen_send_msg_no_para(p, kUIMsgScrDestroy, p);
	if (p->view) {
		ui_view_destroy(p->view);
	}
	if (p->context) {
		free(p->context);
		p->context = NULL;
		p->context_size = 0;
	}
	p->screen_id = kScreenNull;
	ssz_list_erase(g_screen_list, pos);

	if (p == curr) {
		p = (Screen*)ssz_list_back(g_screen_list);
		if (p) {
			screen_activate(p, true);
		}
	}
	
}
void screen_delete(Screen * scr)
{
	ssz_assert(scr);
	//check if it is already deleted, if yes, return
	if (scr == NULL || scr->screen_id == kScreenNull) {
		return;
	}

	Screen* p;
	uint8_t* node_addr = (uint8_t*)scr - 4;
	int pos = (node_addr - (uint8_t*)g_screen_list_nodes) / sizeof(g_screen_list_nodes[0]);
	p = (Screen*)ssz_list_at_pos(g_screen_list, pos);
	if (p && p==scr ) {
		screen_delete_by_pos(pos);
	}
	else {
		screen_printfln("delete screen[%d] fail!", scr->screen_id);
		ssz_assert_fail();
	}

	//for (int pos = ssz_list_begin(g_screen_list); pos != -1; pos = ssz_list_next(g_screen_list, pos)) {
	//	p = (Screen*)ssz_list_at_pos(g_screen_list, pos);
	//	if (p == scr) {
	//		screen_delete_by_pos(pos);
	//		break;
	//	}
	//}
}

void screen_delete_by_id(int screen_id)
{
	Screen* p;
	for (int pos = ssz_list_begin(g_screen_list); pos != -1; pos = ssz_list_next(g_screen_list, pos)) {
		p = (Screen*)ssz_list_at_pos(g_screen_list, pos);
		if (p->screen_id == screen_id) {
			screen_delete_by_pos(pos);
			break;
		}
	}
}

//void screen_delete_by_view(UIWindow * screen_view)
//{
//	Screen* p;
//	for (int pos = ssz_list_begin(g_screen_list); pos != -1; pos = ssz_list_next(g_screen_list, pos)) {
//		p = (Screen*)ssz_list_at_pos(g_screen_list, pos);
//		if (p->view == screen_view) {
//			screen_delete_by_pos(pos);
//			break;
//		}
//	}
//}

bool screen_is_exist(int screen_id)
{
	Screen* p;
	for (int pos = ssz_list_begin(g_screen_list); pos !=-1; pos=ssz_list_next(g_screen_list, pos)) {
		p = (Screen*)ssz_list_at_pos(g_screen_list, pos);
		if (p->screen_id == screen_id) {
			return true;
		}
	}
	return false;
}

Screen * screen_find(int screen_id)
{
	Screen* p;
	for (int pos = ssz_list_begin(g_screen_list); pos != -1; pos = ssz_list_next(g_screen_list, pos)) {
		p = (Screen*)ssz_list_at_pos(g_screen_list, pos);
		if (p->screen_id == screen_id) {
			return p;
		}
	}
	return NULL;
}

void screen_set_event_Handler(ScreenEvent event, ScreenEventFunc fun)
{
	g_screen_handlers[event] = fun;
}

void screen_set_timer_handler(TimerID id, TimerHandler func)
{
	timer_set_handler_and_group(id, func, TIMER_GROUP_SCREEN);
}

void screen_set_title(Screen * scr, const char * title)
{
	UIStatusbar* p = NULL;
	UIView* parent = scr->view;
	if (parent) {
		p = M_ui_statusbar(parent->get_child(kUI_IDStatusBar));
	}
	if (!p) {
		p = M_ui_statusbar(ui_view_desktop()->get_child(kUI_IDStatusBar));
	}
	ssz_assert(p);
	p->set_title(title);
}
