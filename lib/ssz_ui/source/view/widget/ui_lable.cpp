#include "ui_lable.h"
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

UILable::UILable()
{
	m_text = NULL;
}

void UILable::on_draw()
{
	set_draw_attributes();
	ui_clear();
	if (m_text) {
		Rect rc;
		area_self(&rc);
		ui_disp_text_in_rect(&rc, m_text, m_text_mode);
	}
}

void UILable::on_msg(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgDraw:
		on_draw();
		is_handled = true;
		break;
	case kUIMsgSetText:
		set_text((const char*)msg.addr_param);
		is_handled = true;
		break;
	}
	if (!is_handled) {
		on_msg_default(msg);
	}
}

void UILable::set_text(const char * text)
{
	if (m_text != text) {
		m_text = text;
		invalidate();
		notify_parent(kUINotifyValueChanged);
	}
}

void UILable::set_text_and_force_invalid(const char * text)
{
	m_text = text;
	invalidate();
	notify_parent(kUINotifyValueChanged);
}
