/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_edit_base.h"
#include "ui_text_base.h"
/************************************************
* Declaration
************************************************/
#if ENABLE_RUN_TIME_TYPE_INFORMATION
#define M_ui_number_edit(view) (dynamic_cast<UINumberEdit*>(view))
#else
#define M_ui_number_edit(view) ((UINumberEdit*)(view))
#endif

class UINumberEdit: public UITextBase{
public:
	struct NumberEditContext {
		int m_value;
	};
	UINumberEdit();
public://msg handle
	void on_draw();

public://virtual method
	//each child class should implement it
	virtual void on_msg(UIMsg &msg);
	virtual int context_size() { return sizeof(NumberEditContext); };
	//backup view's context(e.g. select index) to the memory, return the used memory size
	//it used to restore previous screen's state when back to show previous screen
	virtual int backup_context(uint8_t* context_memory, int memory_size);
	//restore view's context(e.g. set the select index), return the used memory size
	virtual int restore_context(uint8_t* context_memory, int memory_size);
public://method
	void set_bk_color(UIEditState state, color_t color);
	void set_text_color(UIEditState state, color_t color);
	//e.g. range 0~999, if show all digit, it will show as 000 or show as 0
	void set_show_all_digit_state(bool is_show_all_digit);
	//e.g. init(0,200,10,1)=0~20.0, 0.1 step
	void init(int min_value, int max_value, int enlargement_factor, int one_step_value);

	void set_value(int value);
	int value() { return m_value; };
	void set_step_value(int step_value) { m_step_value = step_value; }
	int step_value() { return m_step_value; }
	//increase one step
	void increase_one_step();
	//decrease one step
	void decrease_one_step();

protected://protected method
protected://property
	color_t m_bk_colors[UIEditStateMax];
	color_t m_text_colors[UIEditStateMax];
	int m_min_value;
	int m_max_value;
	int m_value;
	int m_enlargement_factor;
	int m_step_value;
	bool m_is_show_all_digit;
};



