#include "ui_window.h"
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

UIWindow::UIWindow()
{
	m_callback = NULL;
	m_user_data = NULL;
}

void UIWindow::on_draw()
{
	set_draw_attributes();
	ui_clear();
}

void UIWindow::on_msg_default(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgDraw:
		on_draw();
		is_handled = true;
		break;
	}
	if (!is_handled) {
		UIView::on_msg_default(msg);
	}
}

void UIWindow::on_msg(UIMsg & msg)
{
	if (m_callback) {
		m_callback(m_user_data, &msg);
	}
	else {
		on_msg_default(msg);
	}
}
