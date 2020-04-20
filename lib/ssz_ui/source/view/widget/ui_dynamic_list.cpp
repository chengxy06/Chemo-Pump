#include "ui_dynamic_list.h"
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

UIDynamicList::UIDynamicList()
{
	m_text = NULL;
}

void UIDynamicList::on_draw()
{
	set_draw_attributes();
	ui_clear();
	if (m_text) {
		Rect rc;
		area_self(&rc);
		ui_disp_text_in_rect(&rc, m_text, m_text_mode);
	}
}

void UIDynamicList::on_msg(UIMsg & msg)
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

void UIDynamicList::set_text(const char * text)
{
	m_text = text;
	invalidate();
}
