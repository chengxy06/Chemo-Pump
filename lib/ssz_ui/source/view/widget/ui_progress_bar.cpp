#include "ui_progress_bar.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

UIProgressBar::UIProgressBar()
{
	m_max_value = 100;
	m_value = 0;
	m_bk_color = DEFAULT_PROGRESS_BAR_BACKGROUND_COLOR;
	m_progress_bar_color = DEFAULT_PROGRESS_BAR_COLOR;
}

void UIProgressBar::on_draw()
{
	set_draw_attributes();
	ui_set_pen_color(m_progress_bar_color);
	ui_set_fill_color(m_bk_color);
	ui_draw_rect_at(0, 0, xsize(), ysize(), kUIDrawStrokeAndFill);
	if (m_value>0&&m_max_value>0) {
		int progress_width;
		progress_width = (xsize() - 4)*m_value / m_max_value;
		ui_set_fill_color(m_progress_bar_color);
		ui_fill_rect_at(2, 2, progress_width, ysize() - 4);
	}
}

void UIProgressBar::on_msg(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgDraw:
		on_draw();
		is_handled = true;
		break;
	}
	if (!is_handled) {
		on_msg_default(msg);
	}
}

void UIProgressBar::set_color(color_t progress_bar_color)
{
	m_progress_bar_color = progress_bar_color;
	invalidate();
}

void UIProgressBar::set_max_value(int max_value)
{
	m_max_value = max_value;
	if (m_value>m_max_value) {
		m_value = m_max_value;
	}
	invalidate();
}

void UIProgressBar::set_value(int value)
{
	if (m_value==value) {
		return;
	}
	if (value>m_max_value) {
		m_value = m_max_value;
	}
	else if (value < 0) {
		m_value = 0;
	}
	else {
		m_value = value;
	}
	invalidate();
	notify_parent(kUINotifyValueChanged);
}

void UIProgressBar::increase()
{
	set_value(m_value + 1);
}

void UIProgressBar::decrease()
{
	set_value(m_value - 1);
}

