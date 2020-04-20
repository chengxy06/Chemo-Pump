#include "ui_image.h"
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
UIImage::UIImage()
{
	m_img = NULL;
	m_blink_period_ms = DEFAULT_CURSOR_BLINK_PERIOD_MS;
	m_timer_pos = -1;
	m_flags = 0;
	m_is_hide_at_blink = false;
}

void UIImage::on_draw()
{
	ui_clear();
	if (m_img) {
		set_draw_attributes();
		if (m_flags&UI_IMAGE_REVERSE) {
			ui_set_draw_mode(UI_DRAW_REVERSE);
		}

		if (m_is_hide_at_blink != true) {
			ui_draw_img_at(0, 0, m_img);
		}
	}
}

void UIImage::on_msg(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgDraw:
		on_draw();
		is_handled = true;
		break;
	case kUIMsgSetImg:
		set_img((const Image*)msg.addr_param);
		is_handled = true;
		break;
	case kUIMsgTimer:
		if (m_timer_pos != -1) {
			m_is_hide_at_blink = !m_is_hide_at_blink;
			invalidate();
			ui_view_start_oneshot_after(m_timer_pos, m_blink_period_ms);
		}
		break;
	}
	if (!is_handled) {
		on_msg_default(msg);
	}
}



void UIImage::set_img(const Image * img)
{
	if (m_img != img) {
		m_img = img;
		if (m_flags&UI_IMAGE_AUTOSIZE) {
			if (m_img) {
				set_size(m_img->width, m_img->height);
			}
			else {
				set_size(0, 0);
			}
		}
		invalidate();
		notify_parent(kUINotifyValueChanged);
	}
}

void UIImage::enable_blink()
{
	if (m_timer_pos == -1) {
		m_timer_pos = ui_view_create_timer(this);
		ui_view_start_oneshot_after(m_timer_pos, m_blink_period_ms);
	}
}

void UIImage::disable_blink()
{
	if (m_timer_pos != -1) {
		ui_view_delete_timer(m_timer_pos);
		m_timer_pos = -1;
		m_is_hide_at_blink = false;
		invalidate();
	}
}

void UIImage::set_blink_period_ms(int period_ms)
{
	m_blink_period_ms = period_ms;
	if (m_timer_pos!=-1) {
		ui_view_start_oneshot_after(m_timer_pos, m_blink_period_ms);
	}
}

void UIImage::set_auto_size(bool is_enable)
{
	if (is_enable) {
		m_flags |= UI_IMAGE_AUTOSIZE;
	}
	else {
		m_flags &= ~UI_IMAGE_AUTOSIZE;
	}
}

void UIImage::set_reverse(bool is_reverse)
{
	if (is_reverse && !(m_flags&UI_IMAGE_REVERSE)) {
		m_flags |= UI_IMAGE_REVERSE;
		invalidate();
	}
	else if(!is_reverse && (m_flags&UI_IMAGE_REVERSE)) {
		m_flags &= ~UI_IMAGE_REVERSE;
		invalidate();
	}
}
