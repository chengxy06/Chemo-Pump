#include "ui_number_edit.h"
#include "ssz_common.h"
#include "limits.h"
#include "ssz_utility.h"

/************************************************
* Declaration
************************************************/
#define M_number_edit_is_value_invalid(value) ((value)<m_min_value || (value)>m_max_value)

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

UINumberEdit::UINumberEdit()
{
	m_bk_colors[kUIEditUnFocused] = DEFAULT_EDIT_BACKGROUND_COLOR;
	m_bk_colors[kUIEditFocused] = DEFAULT_EDIT_BACKGROUND_COLOR_WHEN_FOCUS;
	m_bk_colors[kUIEditDisabled] = DEFAULT_EDIT_BACKGROUND_COLOR_WHEN_DISABLE;
	m_text_colors[kUIEditUnFocused] = DEFAULT_EDIT_TEXT_COLOR;
	m_text_colors[kUIEditFocused] = DEFAULT_EDIT_TEXT_COLOR_WHEN_FOCUS;
	m_text_colors[kUIEditDisabled] = DEFAULT_EDIT_TEXT_COLOR_WHEN_DISABLE;
	m_min_value = 0;
	m_max_value = INT_MAX;
	m_value = 0;
	m_enlargement_factor = 0;
	m_step_value = 1;
	m_is_show_all_digit = false;
	enable_accept_focus();
}

void UINumberEdit::on_draw()
{
	set_draw_attributes();
	int color_index = kUIEditUnFocused;
	if (is_focused()) {
		color_index = kUIEditFocused;
	}
	else if (!is_enable()) {
		color_index = kUIEditDisabled;
	}
	ui_set_bk_color(m_bk_colors[color_index]);
	ui_set_text_color(m_text_colors[color_index]);
	ui_clear();
	char sz[20];
	int decimal_char_num = ssz_get_char_num_to_display(m_enlargement_factor) - 1;
	int inter_char_num = ssz_get_char_num_to_display(m_max_value/m_enlargement_factor);
	if (decimal_char_num > 0) {
		if (m_is_show_all_digit) {
			snprintf(sz, sizeof(sz), "%0.*d.%0.*d", inter_char_num, m_value / m_enlargement_factor,
				decimal_char_num, m_value%m_enlargement_factor);
		}
		else {
			snprintf(sz, sizeof(sz), "%d.%0.*d", m_value / m_enlargement_factor,
				decimal_char_num, m_value%m_enlargement_factor);
		}
	}
	else {
		if (m_is_show_all_digit) {
			snprintf(sz, sizeof(sz), "%0.*d", inter_char_num,m_value);
		}
		else {
			snprintf(sz, sizeof(sz), "%d", m_value);
		}
	}
	Rect rc;
	area_self(&rc);
	ui_disp_text_in_rect(&rc, sz, m_text_mode);
}

void UINumberEdit::on_msg(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgDraw:
		on_draw();
		is_handled = true;
		break;
	case kUIMsgKeyPress:
		switch (msg.key_info_param.key)
		{
		case UI_KEY_INCREASE:
			increase_one_step();
			is_handled = true;
			break;
		case UI_KEY_DECREASE:
			decrease_one_step();
			is_handled = true;
			break;
		}
		break;
	}
	if (!is_handled) {
		on_msg_default(msg);
	}
}

int UINumberEdit::backup_context(uint8_t * context_memory, int memory_size)
{
	if (memory_size >= context_size()) {
		NumberEditContext context = { m_value};
		memcpy(context_memory, &context, sizeof(context));
		return sizeof(context);
	}
	else {
		return 0;
	}
}

int UINumberEdit::restore_context(uint8_t * context_memory, int memory_size)
{
	if (memory_size >= context_size()) {
		NumberEditContext context;
		memcpy(&context, context_memory, sizeof(context));
		m_value = context.m_value;
		invalidate();
		return sizeof(context);
	}
	else {
		return 0;
	}
}

void UINumberEdit::set_bk_color(UIEditState state, color_t color)
{
	m_bk_colors[state] = color;
	invalidate();
}

void UINumberEdit::set_text_color(UIEditState state, color_t color)
{
	m_text_colors[state] = color;
	invalidate();
}

void UINumberEdit::set_show_all_digit_state(bool is_show_all_digit)
{
	m_is_show_all_digit = is_show_all_digit;
	invalidate();
}

void UINumberEdit::init(int min_value, int max_value, int enlargement_factor, int one_step_value)
{
	m_min_value = min_value;
	m_max_value = max_value;
	m_enlargement_factor = enlargement_factor;
	if (m_enlargement_factor<1) {
		m_enlargement_factor = 1;
	}
	m_step_value = one_step_value;
	if (m_value<m_min_value ||m_value>m_max_value) {
		m_value = m_min_value;
		notify_parent(kUINotifyValueChanged);
		invalidate();
	}
}

void UINumberEdit::set_value(int value)
{
	if (value>=m_min_value && value<=m_max_value) {
		m_value = value;
		notify_parent(kUINotifyValueChanged);
		invalidate();
	}
}

void UINumberEdit::increase_one_step()
{
	if (m_value < m_max_value) {
		m_value += m_step_value;
		if (M_number_edit_is_value_invalid(m_value)) {
			m_value = m_max_value;
		}
		notify_parent(kUINotifyValueChanged);
		invalidate();
	}
}

void UINumberEdit::decrease_one_step()
{
	if (m_value > m_min_value) {
		m_value -= m_step_value;
		if (M_number_edit_is_value_invalid(m_value)) {
			m_value = m_min_value;
		}
		notify_parent(kUINotifyValueChanged);
		invalidate();
	}
}

