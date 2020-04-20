/************************************************
* DESCRIPTION:
*	store some screen layout, each screen can select and use the need one
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-15 xqzhao
* Initial revision.
*
************************************************/
#include "scr_layout.h"
#include "ui_define.h"
#include "ui_common.h"
#include "string_data.h"
#include "ui_lable.h"
#include "ui_text_list.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//standard layout
static const int16_t g_layout_standard[] = {
	kWidgetAttrBase,kUILable, kUI_IDLeftSoftKey, LEFT_SOFT_KEY_X,LEFT_SOFT_KEY_Y,LEFT_SOFT_KEY_WIDTH,LEFT_SOFT_KEY_HEIGHT,
		kWidgetAttrAlign,UI_ALIGN_LEFT| UI_ALIGN_VCENTER,
	kWidgetAttrBase,kUILable, kUI_IDRightSoftKey, RIGHT_SOFT_KEY_X,RIGHT_SOFT_KEY_Y,RIGHT_SOFT_KEY_WIDTH,RIGHT_SOFT_KEY_HEIGHT,
		kWidgetAttrAlign,UI_ALIGN_RIGHT| UI_ALIGN_VCENTER
};

//one list with one column
static const int16_t g_layout_list_with_one_column[] = {
	kWidgetAttrBase,kUITextList, kUI_IDTextList, 32,FIRST_LINE_Y,SCREEN_WINDOW_WIDTH - 64,ONE_LINE_HEIGHT * 2,
	kWidgetAttrBase,kUIListPosIndicator, kUI_IDListPosIndicator, 85,LEFT_SOFT_KEY_Y,85,LEFT_SOFT_KEY_HEIGHT,
};

//progress bar and a icon
static const int16_t g_layout_progress_bar_and_icon[] = {
	kWidgetAttrBase,kUIImage, kUI_IDImage,
		RUN_INDICATOR_X,RUN_INDICATOR_Y - STATUSBAR_HEIGHT,RUN_INDICATOR_WIDTH,RUN_INDICATOR_HEIGHT,
	kWidgetAttrBase,kUIProgressBar, kUI_IDProgressBar,
		PROGRESS_BAR_X,PROGRESS_BAR_Y - STATUSBAR_HEIGHT,PROGRESS_BAR_WIDTH,PROGRESS_BAR_HEIGHT,
};

/************************************************
* Function 
************************************************/


//it have title, left soft key, right soft key
void scr_layout_load_standard(Screen* scr, const char* title, const char* left_soft_key, const char* right_soft_key) {
	ui_create_widgets(scr->view, g_layout_standard, ssz_array_size(g_layout_standard));
	screen_set_title(scr, title);
	UILable*  p = M_ui_lable(scr->view->get_child(kUI_IDLeftSoftKey));
	if (left_soft_key) {
		p->set_text(left_soft_key);
	}

	p = M_ui_lable(scr->view->get_child(kUI_IDRightSoftKey));
	if (right_soft_key) {
		p->set_text(right_soft_key);
	}

}

//show screen with a list with one column, the list id is kUI_IDTextList
void scr_layout_load_one_column_list(Screen* scr, const char* title, const char* left_soft_key, const char* right_soft_key) {
	ui_create_widgets(scr->view, g_layout_list_with_one_column, ssz_array_size(g_layout_list_with_one_column));
	scr_layout_load_standard(scr, title, left_soft_key, right_soft_key);
	UITextList* p = M_ui_text_list(scr->view->get_child(kUI_IDTextList));
	p->set_item_height(ONE_LINE_HEIGHT);
	p->set_item_bk_color(kUIListItemSelected, UI_WHITE);
	p->set_text_color(kUIListItemSelected, UI_BLACK);
}

void scr_layout_load_progress(Screen * scr, const char * title, const char * left_soft_key, const char * right_soft_key)
{
	ui_create_widgets(scr->view, g_layout_progress_bar_and_icon, ssz_array_size(g_layout_progress_bar_and_icon));
	scr_layout_load_standard(scr, title, left_soft_key, right_soft_key);
}
