#include "ui_text_base.h"
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

UITextBase::UITextBase()
{
	m_text_mode = UI_TEXT_NORMAL | UI_TEXT_ALIGN_HCENTER | UI_TEXT_ALIGN_VCENTER;
	m_text_color = DEFAULT_TEXT_COLOR;
	m_text_font = DEFAULT_FONT;
#if ENABLE_TEXT_BASE_SPACE_ATTRIBUTE
	m_text_char_space = 0;
	m_text_line_space = 0;
#endif
}

void UITextBase::on_msg_default(UIMsg & msg)
{
	bool is_handled = false;
	switch (msg.msg_id) {
	case kUIMsgSetFont:
		set_font((const Font*)msg.addr_param);
		is_handled = true;
		break;
	case kUIMsgSetAlign:
		m_text_mode &= 0xFFFFFF00;
		m_text_mode |= (msg.int_param);
		invalidate();
		is_handled = true;
		break;
	}
	if (!is_handled) {
		UIDrawBase::on_msg_default(msg);
	}
}

void UITextBase::set_text_mode(int mode)
{
	if (m_text_mode!= mode) {
		m_text_mode = mode;
		invalidate();
	}
}

void UITextBase::set_text_color(color_t color)
{
	if (m_text_color != color) {
		m_text_color = color;
		invalidate();
	}
}

void UITextBase::set_font(const Font * font)
{
	if (m_text_font != font) {
		m_text_font = font;
		invalidate();
	}	
}
#if ENABLE_TEXT_BASE_SPACE_ATTRIBUTE
void UITextBase::set_text_char_space(int char_space)
{
	if (m_text_char_space != char_space) {
		m_text_char_space = char_space;
		invalidate();
	}	
}

void UITextBase::set_text_line_space(int line_space)
{
	if (m_text_line_space != line_space) {
		m_text_line_space = line_space;
		invalidate();
	}		
}
#endif

void UITextBase::set_draw_attributes()
{
	UIDrawBase::set_draw_attributes();
	ui_set_text_mode(m_text_mode);
	ui_set_text_color(m_text_color);
	ui_set_font(m_text_font);
#if ENABLE_TEXT_BASE_SPACE_ATTRIBUTE
	ui_set_text_char_space(m_text_char_space);
	ui_set_text_line_space(m_text_line_space);
#endif
}
