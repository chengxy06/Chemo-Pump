/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ui_view.h"
#include "ui_statusbar.h"

/************************************************
* Declaration
************************************************/
typedef enum {
	kWidgetAttrNull,
	kWidgetAttrBase, //{type,id,x,y,x_size,y_size}
	kWidgetAttrTextID, //{string_id}
	kWidgetAttrImgID, //{img_id}
	kWidgetAttrFontHeight, //{font_height}
	kWidgetAttrAlign, //{align},e.g. UI_ALIGN_LEFT
	kWidgetAttrTitleTextID, //{string_id},set the screen title
}WidgetAttrType;

void ui_create_widgets(UIView* parent, const int16_t * widgets_define, int widgets_define_size);

#ifdef __cplusplus
extern "C" {
#endif
void statusbar_create();
#ifdef __cplusplus
}
#endif

UIStatusbar* statusbar();
void statusbar_init();