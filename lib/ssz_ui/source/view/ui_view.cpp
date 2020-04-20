#include "ui_view.h"
#include"ui_utility.h"
#include "ssz_common.h"
#include "event.h"
#include "ssz_list.h"
#include "ui_window.h"
#if ENABLE_RUN_TIME_TYPE_INFORMATION
#include <typeinfo>
#endif
#include "ssz_timer.h"
#include "timer.h"
/************************************************
* Declaration
************************************************/
#define USE_EVENT_TO_NOTIFY_DIRTY
//#define ENABLE_OUTPUT_DRAW_INFO
//#define USE_KEY_CONFIRM_TO_TRACE_DRAW
#if DISPLAY_BIT_PER_PIXEL>16
#define TRACE_VIEW_COLOR UI_RED
#define TRACE_VIEW_DRAW_COLOR UI_GREEN
#else
#define TRACE_VIEW_COLOR UI_WHITE
#define TRACE_VIEW_DRAW_COLOR UI_WHITE
#endif

typedef struct {
	UIView* view;
	int16_t one_view_size;
	UIViewTypeID type;
	bool is_used;
}UIViewAllocInfo;

typedef struct {
	SSZ_LIST_NODE_BASE_DEFINE;
	UIView* user_data;
}UIViewListNode;

void ui_view_output_view_self_info(UIView* view, int level);
/************************************************
* Variable 
************************************************/

static UIViewAllocInfo g_ui_view_alloc_arr[UI_VIEW_COUNT_ALLOC_MAX];
static UIViewListNode g_ui_view_list_nodes[UI_VIEW_COUNT_MAX_AT_ONE_SCREEN];
static SszList g_ui_all_view_list;
static UIView* g_ui_desktop_view = NULL;
static UIView* g_ui_focused_view = NULL;
static bool g_ui_is_someone_dirty = false;

static SszTimerManager g_ui_view_timer_manager;
static SszTimerNode g_ui_view_timer_manager_nodes[UI_VIEW_TIMER_MAX];

static bool g_ui_view_is_enable_trace = false;
static bool g_ui_view_is_enable_trace_draw = false;
/************************************************
* Function 
************************************************/

void UIView::attach_to_parent(UIView * parent_p)
{
	if (parent_p) {
		detach_from_parent();
		parent_p->add_child(this);
	}
	else {
		ssz_assert(parent_p);
	}
}

void UIView::detach_from_parent()
{
	if (m_parent) {
		UIView *work = m_parent->m_first_child;
		if (work == this) {
			//first child is this
			m_parent->m_first_child = this->m_next_sibling;
		}
		else {
			while (work) {
				if (work->m_next_sibling == this) {
					//next is this
					work->m_next_sibling = this->m_next_sibling;
					break;
				}
				else {
					work = work->m_next_sibling;
				}
			}
		}
		//remove from view list
		UIView* p = this;
		ssz_list_remove(&g_ui_all_view_list, &p, memcmp);

		if (is_visible()) {
			Rect rect_at_parent;
			area(&rect_at_parent);
			m_parent->invalidate_by_rect(rect_at_parent);
		}
		move_but_no_invalidate(-m_parent->m_rect.x0, -m_parent->m_rect.y0);
		m_parent = NULL;
	}
}

void UIView::add_child(UIView * child)
{
	UIView* pre_view = NULL;
	//add child
	if (m_first_child ==NULL) {
		m_first_child = child;
		pre_view = this;
	}
	else {
		UIView *work = m_first_child;
		while (work) {
			if (work->m_next_sibling == NULL) {
				work->m_next_sibling = child;
				pre_view = work;
				break;
			}
			work = work->m_next_sibling;
		}
	}
	ssz_assert(child->m_parent == NULL);
	child->m_parent = this;
	child->m_next_sibling = NULL;
	child->move_but_no_invalidate(m_rect.x0, m_rect.y0);
	//insert it to views list
	if (pre_view != this) {
		pre_view = pre_view->last_descendant();
	}
	int pos = ssz_list_find(&g_ui_all_view_list, &pre_view, memcmp);
	if (pos != -1) {
		pos = ssz_list_next(&g_ui_all_view_list, pos);
		if (pos == -1) {
			pos = g_ui_all_view_list.max_size;
		}
		if (ssz_list_insert(&g_ui_all_view_list, pos, &child) == -1) {
			ssz_assert_fail();
		}
	}
	//invalid rect
	if (child->is_visible()) {
		child->invalidate();
	}
}

void UIView::delete_child(UIView * child)
{
	if (child && child->m_parent==this) {
		ui_view_destroy(child);
	}
}

void UIView::delete_all_child()
{
	while (m_first_child) {
		ui_view_destroy(m_first_child);
	}
}

void UIView::set_focus()
{
	if (is_enable_focused() && g_ui_focused_view != this) {
		if (g_ui_focused_view) {
			send_msg_no_para(g_ui_focused_view, kUIMsgLostFocus);
			for (UIView* p = g_ui_focused_view->m_parent; p != NULL; p = p->m_parent) {
				send_msg_no_para(p,kUIMsgChildLostFocus);
			}
		}
		g_ui_focused_view = this;
		send_msg_no_para(this, kUIMsgGetFocus);
		for (UIView* p = m_parent; p != NULL; p=p->m_parent) {
			send_msg_no_para(p,kUIMsgChildGetFocus);
		}
	}
}
void UIView::enable_accept_focus()
{
	m_status |= UI_VIEW_ENABLE_ACCEPT_FOCUS;
}
void UIView::disable_accept_focus()
{
	m_status &= ~UI_VIEW_ENABLE_ACCEPT_FOCUS;
}
void ui_view_invalidate_views(UIView *start, UIView* end, Rect& invalidate_rect_at_screen) {
	UIView* p;
	int pos = ssz_list_find(&g_ui_all_view_list, &start, memcmp);
	if (pos != -1) {
		pos = ssz_list_next(&g_ui_all_view_list, pos);
	}
	while (pos!=-1) {
		p = *(UIView**)ssz_list_at_pos(&g_ui_all_view_list, pos);
		if (p == end) {
			break;
		}
		p->invalidate_self_by_rect_at_screen(invalidate_rect_at_screen);
		pos = ssz_list_next(&g_ui_all_view_list, pos);
	}
}
void UIView::invalidate()
{
	if (invalidate_self()) {
		ui_view_invalidate_views(this, NULL, m_dirty_rect);
	}
}

void UIView::invalidate_by_rect(const Rect & rect)
{
	if (invalidate_self_by_rect(rect)) {
		//Rect screen_rect = rect;
		////to screen client
		//ui_rect_move(&screen_rect, m_rect.x0, m_rect.y0);
		//ui_view_invalidate_views(this, NULL, screen_rect);
		ui_view_invalidate_views(this, NULL, m_dirty_rect);
	}


}

void UIView::invalidate_by_rect_ex(int x, int y, int x_size, int y_size)
{
	Rect rect={ (int16_t)x,(int16_t)y,(int16_t)(x + x_size - 1),(int16_t)(y + y_size - 1) };
	invalidate_by_rect(rect);
}

void UIView::invalidate_self_by_rect_at_screen(const Rect & rect_at_screen)
{
	if (!is_self_and_all_parent_visible()) {
		return;
	}

	Rect new_rect = rect_at_screen;
	//get the right invalid rect
	if (!ui_rect_intersect(&new_rect, &m_rect)) {
		return;
	}
	m_status |= UI_VIEW_DIRTY;
	g_ui_is_someone_dirty = true;
#ifdef USE_EVENT_TO_NOTIFY_DIRTY
	event_set(kEventUIViewDirty);
#endif
	//set to the invalid rect
	if (ui_rect_xsize(m_dirty_rect)<=0|| ui_rect_ysize(m_dirty_rect) <= 0 ){
		//if no invalid rect, assign to it
		m_dirty_rect = new_rect;
	}
	else {
		ui_rect_merge(&m_dirty_rect, &new_rect);
	}
}
void UIView::validate()
{
	m_status &= ~UI_VIEW_DIRTY;
}

void UIView::area(Rect * rc_at_parent)
{
	*rc_at_parent = m_rect;
	if (m_parent) {
		ui_rect_move(rc_at_parent, -m_parent->m_rect.x0, -m_parent->m_rect.y0);
	}
}
void UIView::area_self(Rect * rc)
{
	rc->x0 = 0;
	rc->y0 = 0;
	rc->x1 = ui_rect_xsize(m_rect)-1;
	rc->y1 = ui_rect_ysize(m_rect)-1;
}

void UIView::area_at_screen(Rect * rc_at_screen)
{
	*rc_at_screen = m_rect;
}

void UIView::to_screen_pos(Point * pt_at_screen)
{
	pt_at_screen->x += m_rect.x0;
	pt_at_screen->y += m_rect.y0;
}

UIView*  UIView::get_child(int child_id)
{
	for (UIView *work = this->m_first_child; work != NULL; work = work->m_next_sibling) {
		if (work->m_id == child_id) {
			return work;
		}
	}
	return NULL;
}

UIView*  UIView::get_descendant(int descendant_id)
{
	UIView *tmp;
	for (UIView *work = this->m_first_child; work != NULL; work = work->m_next_sibling) {
		if (work->m_id == descendant_id) {
			return work;
		}
		else {
			tmp = work->get_descendant(descendant_id);
			if (tmp) {
				return tmp;
			}
		}
	}
	return NULL;
}

UIView * UIView::last_descendant()
{
	if (m_first_child) {
		UIView *last_child = m_first_child;
		//find the last child
		while (last_child->m_next_sibling) {
			last_child = last_child->m_next_sibling;
		}
		return last_child->last_descendant();
	}
	else {
		//no descendant, return self
		return this;
	}
}

void UIView::send_msg_no_para(UIView* dest, int msg_id)
{
	UIMsg msg;
	ssz_mem_zero(&msg, sizeof(msg));
	msg.src = this;
	msg.msg_id = msg_id;
	dest->on_msg(msg);
}

void UIView::send_msg_with_para(UIView* dest, int msg_id, int32_t param)
{
	UIMsg msg;
	msg.src = this;
	msg.msg_id = msg_id;
	msg.int_param = param;
	dest->on_msg(msg);
}

void UIView::send_msg(UIView* dest, UIMsg & msg)
{
	msg.src = this;
	dest->on_msg(msg);
}

void UIView::on_msg_default(UIMsg & msg)
{
}

void UIView::notify_parent(int notify_id)
{
	if (m_parent) {
		send_msg_with_para(m_parent, kUIMsgNotifyParent, notify_id);
	}
}

void UIView::set_canvas_for_draw()
{
	ui_set_canvas_rect_ex(&m_rect);
}

int UIView::context_size_with_descendant()
{
	int all_context_size = context_size();

	for (UIView *work = this->m_first_child; work != NULL; work = work->m_next_sibling) {
		all_context_size += work->context_size_with_descendant();
	}
	return all_context_size;
}

bool UIView::backup_context_with_descendant(uint8_t * context_memory, int memory_size, int * used_memory_size)
{
	int used_size = 0;
	int tmp_used_size;

	if (context_size()>0) {
		used_size = backup_context(context_memory, memory_size);
		if (used_size == 0) {
			return false;
		}
	}

	for (UIView *work = this->m_first_child; work != NULL; work = work->m_next_sibling) {
		if (work->backup_context_with_descendant(context_memory+ used_size, memory_size- used_size, &tmp_used_size)
			== false) {
			return false;
		}
		else {
			used_size += tmp_used_size;
		}
	}

	*used_memory_size = used_size;

	return true;
}

bool UIView::restore_context_with_descendant(uint8_t * context_memory, int memory_size, int * used_memory_size)
{
	int used_size = 0;
	int tmp_used_size;

	if (context_size() > 0) {
		used_size = restore_context(context_memory, memory_size);
		if (used_size == 0) {
			return false;
		}
	}

	for (UIView *work = this->m_first_child; work != NULL; work = work->m_next_sibling) {
		if (work->restore_context_with_descendant(context_memory + used_size, memory_size - used_size, &tmp_used_size)
			== false) {
			return false;
		}
		else {
			used_size += tmp_used_size;
		}
	}

	*used_memory_size = used_size;

	return true;
}

bool UIView::invalidate_self()
{
	if (!is_self_and_all_parent_visible()) {
		return false;
	}
	m_dirty_rect = m_rect;
	m_status |= UI_VIEW_DIRTY;
	g_ui_is_someone_dirty = true;
#ifdef USE_EVENT_TO_NOTIFY_DIRTY
	event_set(kEventUIViewDirty);
#endif
	return true;
}

//return: true->the rect is valid
bool UIView::invalidate_self_by_rect(const Rect & rect)
{
	if (!is_self_and_all_parent_visible()) {
		return false;
	}

	Rect client;
	client = rect;
	//to screen rect
	ui_rect_move(&client, m_rect.x0, m_rect.y0);

	//get the right invalid rect
	if (!ui_rect_intersect(&client, &m_rect)) {
		return false;
	}
	m_status |= UI_VIEW_DIRTY;
	g_ui_is_someone_dirty = true;
#ifdef USE_EVENT_TO_NOTIFY_DIRTY
	event_set(kEventUIViewDirty);
#endif
	//set to the invalid rect
	if (ui_rect_xsize(m_dirty_rect) <= 0 || ui_rect_ysize(m_dirty_rect) <= 0) {
		//if no invalid rect, assign to it
		m_dirty_rect = client;
	}
	else {
		ui_rect_merge(&m_dirty_rect, &client);
	}
	return true;
}


void UIView::hide()
{
	if (!(m_status&UI_VIEW_HIDE)) {
		m_status |= UI_VIEW_HIDE;
		if (m_parent) {
			Rect rect_at_parent;
			area(&rect_at_parent);
			m_parent->invalidate_by_rect(rect_at_parent);
		}
	}
}

void UIView::show()
{
	if (m_status&UI_VIEW_HIDE) {
		m_status &= ~UI_VIEW_HIDE;
		invalidate();
	}
}

void UIView::enable()
{
	if ((m_status&UI_VIEW_DISABLE)) {
		m_status &= ~UI_VIEW_DISABLE;
		send_msg_no_para(this, kUIMsgEnable);
	}
}

void UIView::disable()
{
	if (!(m_status&UI_VIEW_DISABLE)) {
		m_status |= UI_VIEW_DISABLE;
		send_msg_no_para(this, kUIMsgDisable);
	}
}

void UIView::draw()
{
	if (is_dirty()) {
		//back current ui draw context and default current draw context
		ui_backup_and_set_default_draw_context();
		//set the canvas and limit rect
		ui_set_canvas_rect_ex(&m_rect);
		Rect real_dirty;
		real_dirty_rect_at_screen(&real_dirty);
		ui_set_limit_rect_of_canvas_ex(&real_dirty);
		if (g_ui_view_is_enable_trace_draw) {
			printf("Draw->");
			ui_view_output_view_self_info(this, 0);
			ui_set_pen_color(TRACE_VIEW_DRAW_COLOR);
			Rect d_rect;
			dirty_rect(&d_rect);
			ui_draw_rect_ex(&d_rect, kUIDrawStroke);
			//ui_draw_rect_at(0, 0, ui_rect_xsize(real_dirty), ui_rect_ysize(real_dirty), kUIDrawStroke);
			ssz_delay_ms(1000);
		}
		send_msg_no_para(this, kUIMsgDraw);
		if (g_ui_view_is_enable_trace_draw) {
			ssz_delay_ms(1000);
		}
		if (g_ui_view_is_enable_trace) {
			m_dirty_rect = m_rect;
			ui_set_canvas_rect_ex(&m_rect);
			real_dirty_rect_at_screen(&real_dirty);
			ui_set_limit_rect_of_canvas_ex(&real_dirty);
			ui_set_pen_color(TRACE_VIEW_COLOR);
			ui_draw_rect_at(0, 0, xsize(), ysize(), kUIDrawStroke);
		}
		ui_restore_draw_context();
		m_status &= ~UI_VIEW_DIRTY;
		m_dirty_rect.x0 = m_rect.x0;
		m_dirty_rect.y0 = m_rect.y0;
		m_dirty_rect.x1 = m_dirty_rect.x0 - 1;
		m_dirty_rect.y1 = m_dirty_rect.y0 - 1;
	}
}

void UIView::force_draw_after_draw_all()
{
	//back current ui draw context and default current draw context
	ui_backup_and_set_default_draw_context();
	//set the canvas and limit rect
	ui_set_canvas_rect_ex(&m_rect);
	send_msg_no_para(this, kUIMsgForceDrawAfterAllDraw);

	ui_restore_draw_context();
}

bool UIView::is_enable()
{
	if (m_status&UI_VIEW_DISABLE) {
		return false;
	}
	else {
		return true;
	}
}

bool UIView::is_self_and_all_parent_visible()
{
	UIView* tmp = this;
	while (tmp!=NULL) {
		if (tmp->m_status&UI_VIEW_HIDE) {
			return false;
		}
		else {
			tmp = tmp->m_parent;
		}
	}
	return true;
}

bool UIView::is_visible()
{
	if (m_status&UI_VIEW_HIDE) {
		return false;
	}
	else {
		return true;
	}
}

bool UIView::is_focused()
{
	return this==g_ui_focused_view?true:false;
}

bool UIView::is_enable_focused()
{
	if (m_status&UI_VIEW_ENABLE_ACCEPT_FOCUS) {
		return true;
	}
	else {
		return false;
	}
}

bool UIView::is_dirty()
{
	if (!(m_status&UI_VIEW_HIDE) && (m_status&UI_VIEW_DIRTY)) {
		return true;
	}
	else {
		return false;
	}
}

void UIView::dirty_rect(Rect * dest)
{
	*dest = m_dirty_rect;
	ui_rect_move(dest, -m_rect.x0, -m_rect.y0);
}

void UIView::real_dirty_rect_at_screen(Rect * dest)
{
	*dest = m_dirty_rect;
	UIView* p = m_parent;
	while (p) {
		ui_rect_intersect(dest, &p->m_rect);
		p = p->parent();
	}
}

void UIView::set_pos(int x, int y)
{
	Rect old_rect_at_parent;
	area(&old_rect_at_parent);

	int x_size = x - old_rect_at_parent.x0;
	int y_size = y - old_rect_at_parent.y0;
	if (x_size != 0 || y_size != 0) {
		move_but_no_invalidate(x_size, y_size);
		if (is_self_and_all_parent_visible() && m_parent) {
			////invalidate new area
			//m_parent->invalidate_by_rect_ex(x,y,ui_rect_xsize(m_rect), ui_rect_ysize(m_rect));
			////invalid old area
			//m_parent->invalidate_by_rect(old_rect_at_parent);
			//invalid merged rect
			Rect  curr_rect;
			area(&curr_rect);
			ui_rect_merge(&old_rect_at_parent, &curr_rect);
			m_parent->invalidate_by_rect(old_rect_at_parent);
		}
		send_msg_no_para(this, kUIMsgPosChanged);
	}
}

void UIView::set_size(int x_size, int y_size)
{
	Rect old_rect;
	area(&old_rect);
	m_rect.x1 = x_size+m_rect.x0-1;
	m_rect.y1 = y_size + m_rect.y0 - 1;
	m_dirty_rect = m_rect;
	if (is_self_and_all_parent_visible() && m_parent) {
		////invalidate new area
		//m_parent->invalidate_by_rect_ex(old_rect.x0, old_rect.y0, x_size, y_size);
		////invalid old area
		//m_parent->invalidate_by_rect(old_rect);
		//invalid merged rect
		Rect  curr_rect;
		area(&curr_rect);
		ui_rect_merge(&old_rect, &curr_rect);
		m_parent->invalidate_by_rect(old_rect);
	}
	send_msg_no_para(this, kUIMsgSizeChange);
}

void UIView::set_area(int x, int y, int x_size, int y_size)
{
	Rect old_rect;
	area(&old_rect);


	int dx = x - m_rect.x0;
	int dy = y - m_rect.y0;
	//set pos
	if (dx != 0 || dy != 0) {
		move_but_no_invalidate(dx, dy);
		send_msg_no_para(this, kUIMsgPosChanged);
	}

	//set size
	m_rect.x1 = x_size + m_rect.x0 - 1;
	m_rect.y1 = y_size + m_rect.y0 - 1;
	m_dirty_rect = m_rect;
	//invalid
	if (is_self_and_all_parent_visible() && m_parent) {
		//Rect new_rc;
		//area(&new_rc);
		////invalidate new area
		//m_parent->invalidate_by_rect(new_rc);
		////invalid old area
		//m_parent->invalidate_by_rect(old_rect);

		//invalid merged rect
		Rect  curr_rect;
		area(&curr_rect);
		ui_rect_merge(&old_rect, &curr_rect);
		m_parent->invalidate_by_rect(old_rect);
	}
	send_msg_no_para(this, kUIMsgSizeChange);
}

void UIView::move_but_no_invalidate(int x_size, int y_size)
{
	ui_rect_move(&m_rect, x_size, y_size);
	m_dirty_rect = m_rect;
	for (UIView* p = m_first_child; p !=NULL; p=p->m_next_sibling) {
		p->move_but_no_invalidate(x_size, y_size);
	}
}

static UIView* ui_view_malloc(UIViewTypeID type, int *view_size)
{
	int i;
	for (i = 0; i < ssz_array_size(g_ui_view_alloc_arr) && g_ui_view_alloc_arr[i].view; i++) {
		if (g_ui_view_alloc_arr[i].type == type &&
			g_ui_view_alloc_arr[i].is_used == false) {
			g_ui_view_alloc_arr[i].is_used = true;
			*view_size = g_ui_view_alloc_arr[i].one_view_size;
			return g_ui_view_alloc_arr[i].view;
		}
	}

	ssz_assert_fail();
	return NULL;//lint !e527
}
//free the view
void ui_view_free(UIView* view)
{
	int i;
	for (i = 0; i < ssz_array_size(g_ui_view_alloc_arr) && g_ui_view_alloc_arr[i].view; i++) {
		if (g_ui_view_alloc_arr[i].view == view) {
			g_ui_view_alloc_arr[i].is_used = false;
			break;
		}
	}
}


//create a view, the view must be register first and must have free view memory to create
UIView* ui_view_create(UIViewTypeID type, int id, UIView* parent, int x, int y, int x_size, int y_size, uint32_t status)
{
	if (!parent) {
		parent = g_ui_desktop_view;
	}
	int view_size;
	UIView* ret = ui_view_malloc(type, &view_size);
	if (ret ) {
		ret->m_status = status;
		ret->m_rect.x0 = x;
		ret->m_rect.y0 = y;
		ret->m_rect.x1 = x + x_size - 1;
		ret->m_rect.y1 = y + y_size - 1;
		ret->m_dirty_rect = ret->m_rect;
		ret->m_id = id;
		ret->m_first_child = NULL;
		ret->m_next_sibling = NULL;
		ret->m_parent = NULL;
		ui_view_init_obj(type, ret, view_size);

		if (parent) {
			//attach to parent
			parent->add_child(ret);
		}
		UIMsg msg;
		ssz_mem_zero(&msg, sizeof(msg));
		msg.msg_id = kUIMsgCreate;
		ret->on_msg(msg);
	}


	return ret;
}

void ui_view_destroy(UIView * view)
{
	if(view==NULL){
		return;
	}
	UIMsg msg;
	ssz_mem_zero(&msg, sizeof(msg));
	msg.msg_id = kUIMsgDestroy;
	view->on_msg(msg);
	// Delete associated timers
	ui_view_delete_associated_timer(view);

	if (g_ui_focused_view == view) {
		g_ui_focused_view = NULL;
	}

	//notify parent
	if (view->m_parent) {
		view->notify_parent(kUINotifyChildDestroy);
	}
	//delete all child
	while (view->m_first_child) {
		ui_view_destroy(view->m_first_child);
	}
	//detach from parent
	view->detach_from_parent();
	//free it
	ui_view_free(view);
}

//regist the view
void ui_view_regist(UIViewTypeID type, int one_view_size, void* one_view_memory)
{
	int i;
	for (i = 0; i < ssz_array_size(g_ui_view_alloc_arr); i++) {
		if (g_ui_view_alloc_arr[i].view ==NULL) {
			g_ui_view_alloc_arr[i].is_used = false;
			g_ui_view_alloc_arr[i].type = type;
			g_ui_view_alloc_arr[i].view = (UIView*)one_view_memory;
			g_ui_view_alloc_arr[i].one_view_size = one_view_size;
			break;
		}
	}
	ssz_assert(i < ssz_array_size(g_ui_view_alloc_arr));
}

void ui_view_send_msg(UIView * dest, UIMsg & msg)
{
	dest->on_msg(msg);
}

void ui_view_desktop_callback(void* user_data, UIMsg* msg) {

}
//the root view, UI only have one root view and it is the desktop
void ui_view_create_desktop_if_not()
{
	if (!g_ui_desktop_view) {
		g_ui_desktop_view = ui_view_create(kUIWindow, 0, NULL,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0);
		((UIWindow*)g_ui_desktop_view)->m_callback = ui_view_desktop_callback;
		g_ui_focused_view = g_ui_desktop_view;
		ssz_list_init(&g_ui_all_view_list, (SszListNode*)g_ui_view_list_nodes, ssz_array_size(g_ui_view_list_nodes),
			ssz_array_node_size(g_ui_view_list_nodes));
		ssz_list_push_back(&g_ui_all_view_list, &g_ui_desktop_view);
		ssz_timer_init(&g_ui_view_timer_manager, g_ui_view_timer_manager_nodes,
			ssz_array_size(g_ui_view_timer_manager_nodes));
	}
}
UIView* ui_view_desktop()
{
	return g_ui_desktop_view;
}

UIView * ui_view_focused()
{
	return g_ui_focused_view;
}

void ui_view_print_space(int space_count) {
	for (int i = 0; i < space_count; i++) {
		printf(" ");
	}
}
void ui_view_output_view_self_info(UIView* view, int level) {
	Rect frame;
	Rect invalid_rc;
	view->area_at_screen(&frame);
	view->dirty_rect_at_screen(&invalid_rc);
	ui_view_print_space(level * 2);
#if ENABLE_RUN_TIME_TYPE_INFORMATION
	printf("Name:%s, ", typeid(*view).name());
#endif
	printf("ID:%d, [%d,%d,%dx%d]", view->id(),
		frame.x0, frame.y0, ui_rect_xsize(frame), ui_rect_ysize(frame));
	if (view->is_dirty()) {
		printf(", dirty:[%d,%d,%dx%d]",
			invalid_rc.x0, invalid_rc.y0, ui_rect_xsize(invalid_rc), ui_rect_ysize(invalid_rc));
	}
	printf("\n");
}
//return next pos at view list
int ui_view_output_view_info(UIView* view, int level, int pos) {
	ui_view_output_view_self_info(view, level);

	if (pos != -1) {
		if (*(UIView**)ssz_list_at_pos(&g_ui_all_view_list, pos) != view) {
			printf("the pos not same with the view");
			ui_view_output_view_self_info(*(UIView**)ssz_list_at_pos(&g_ui_all_view_list, pos), level);
		}
		pos = ssz_list_next(&g_ui_all_view_list, pos);
	}
	else {
		printf("pos not valid\n");
		ssz_assert_fail();
	}


	for (UIView* p = view->first_child(); p != NULL;p=p->next_sibling()) {
		pos = ui_view_output_view_info(p, level + 1, pos);
	}

	return pos;
}
void ui_view_output_all_view_info()
{
	int level = 0;
	printf("--views--\n");
	int pos = ui_view_output_view_info(ui_view_desktop(), level, ssz_list_begin(&g_ui_all_view_list));
	//ui_view_check_all_view_list();
	ssz_assert(pos == -1);
}



static void ui_view_on_timer_expire(int timer_pos, void* user_data) {
	UIView* p = (UIView*)user_data;
	p->send_msg_with_para(p, kUIMsgTimer, timer_pos);
}
SszTimer ui_view_create_timer(UIView * view)
{
	return ssz_timer_create(&g_ui_view_timer_manager, ui_view_on_timer_expire, view);
}

void ui_view_delete_timer(SszTimer timer)
{
	ssz_timer_delete(&g_ui_view_timer_manager, timer);
}
static int ui_view_compare_timer(const void* data1, const void* data2, size_t data_size) {
	SszTimerObj *p = (SszTimerObj*)data1;
	if (p->user_data == data2) {
		return 0;
	}
	else {
		return 1;
	}
}
void ui_view_delete_associated_timer(UIView * view)
{
	ssz_list_remove(&g_ui_view_timer_manager, view, ui_view_compare_timer);
}

void ui_view_start_oneshot_after(SszTimer timer, int32_t milliseconds)
{
	ssz_timer_start_oneshot_after(&g_ui_view_timer_manager, timer, milliseconds);
}

void ui_view_stop_timer(SszTimer timer)
{
	ssz_timer_stop(&g_ui_view_timer_manager, timer);
}

bool ui_view_is_someone_dirty()
{
	return g_ui_is_someone_dirty;
}

//return is self or child draw
static bool ui_view_draw_self_and_descendant(UIView* view) {
	if (!view->is_visible()) {
		return false;
	}
	bool ret = false;
	if (view->is_dirty()) {
#ifdef ENABLE_OUTPUT_DRAW_INFO
		printf("Draw->");
		ui_view_output_view_self_info(view, 0);
#endif
		//draw self
		view->draw();
		ret = true;
#ifdef USE_KEY_CONFIRM_TO_TRACE_DRAW
		printf("press any key to draw next:");
		ssz_get_char();
		printf("\n");
#endif
	}
	//draw each descendant
	for (UIView *work = view->first_child(); work != NULL; work = work->next_sibling()) {
		if (ret==false) {
			ret = ui_view_draw_self_and_descendant(work);
		}
		else {
			ui_view_draw_self_and_descendant(work);
		}
	}
	if (ret) {
		view->force_draw_after_draw_all();
	}

	return ret;
}
void ui_view_draw_all_dirty()
{
#ifdef ENABLE_OUTPUT_DRAW_INFO
	printf("--draw start--\n");
#endif
	if (g_ui_desktop_view) {
		ui_view_draw_self_and_descendant(g_ui_desktop_view);
	}
#ifdef ENABLE_OUTPUT_DRAW_INFO
	printf("**draw end**\n");
#endif
	g_ui_is_someone_dirty = false;
}

void ui_view_enable_trace()
{
	if (!g_ui_view_is_enable_trace) {
		g_ui_view_is_enable_trace = true;
		ui_view_desktop()->invalidate();
	}
}

void ui_view_disable_trace()
{
	if (g_ui_view_is_enable_trace) {
		g_ui_view_is_enable_trace = false;
		ui_view_desktop()->invalidate();
	}
}
void ui_view_enable_trace_draw()
{
	if (!g_ui_view_is_enable_trace_draw) {
		g_ui_view_is_enable_trace_draw = true;
	}
}

void ui_view_disable_trace_draw()
{
	if (g_ui_view_is_enable_trace_draw) {
		g_ui_view_is_enable_trace_draw = false;
	}
}

void ui_view_handle_all_timer() {
	ssz_timer_handle_all(&g_ui_view_timer_manager);
}
void ui_view_init()
{

	event_set_handler(kEventUIViewDirty, ui_view_draw_all_dirty);

	ui_view_create_desktop_if_not();
	timer_set_handler(kTimerHandleUIViewTimer, ui_view_handle_all_timer);
}
void ui_view_start_scan_timer(){
	timer_start_periodic_every(kTimerHandleUIViewTimer, 5);
	ui_view_handle_all_timer();
}

void ui_view_stop_scan_timer()
{
	timer_stop(kTimerHandleUIViewTimer);
}
