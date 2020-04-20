#include "ui_draw_base.h"
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

UIDrawBase::UIDrawBase()
{
	//m_draw_mode = UI_DRAW_NORMAL;
	m_bk_color = DEFAULT_BACKGROUND_COLOR;
}


//void UIDrawBase::set_draw_mode(int draw_mode)
//{
//	if (draw_mode!=m_draw_mode) {
//		m_draw_mode = draw_mode;
//		invalidate();
//	}
//}

void UIDrawBase::set_bk_color(color_t color)
{
	if (color != m_bk_color) {
		m_bk_color = color;
		invalidate();
	}
}

void UIDrawBase::set_draw_attributes()
{
	ui_set_bk_color(m_bk_color);
	//ui_set_draw_mode(m_draw_mode);
}
