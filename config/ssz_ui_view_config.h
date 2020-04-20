#pragma once
#include "ui.h"
#include "image_data.h"
#include "font_data.h"

#define SCREEN_WIDTH DISPLAY_WIDTH
#define SCREEN_HEIGHT DISPLAY_HEIGHT

//the max count view to alloc
#define UI_VIEW_COUNT_ALLOC_MAX (40)

//the max count view at one screen
#define UI_VIEW_COUNT_MAX_AT_ONE_SCREEN (30)

//the max count timer for ui view system
#define UI_VIEW_TIMER_MAX 4

//is enable char space and line space's attribute of UITextBase
#define ENABLE_TEXT_BASE_SPACE_ATTRIBUTE 0

//if enable RTTI, it will use dynamic_cast to convert the point by the macro M_ui_lable,M_ui_image,...
//and it will output class name by ui_view_output_all_view_info
#ifdef SSZ_TARGET_SIMULATOR
#define ENABLE_RUN_TIME_TYPE_INFORMATION 1
#else
#ifdef DEBUG
#define ENABLE_RUN_TIME_TYPE_INFORMATION 0
#else
#define ENABLE_RUN_TIME_TYPE_INFORMATION 0
#endif
#endif

//define all widget(include your custom widget)
typedef enum {
	kUIWindow, //which can contain some widget, usually a window is a screen
	kUILable,
	kUIImage,
	// kUIButton,
	kUINumberEdit,
	//kUIEdit,
	kUITextList,
	kUIImageList,
	// kUISwitch,
	// kUISlider,
	kUIPageControl,
	// kUISegmentedControl,
	kUIProgressBar,
	kUIStatusBar, //at top, have time, battery, notify...
	//kUINavigationBar, //at bottom, have back, menu, home...
	// kUIToolBar,  //put some quick operator for user
	kUIListPosIndicator,


	kUIViewTypeIDMax
}UIViewTypeID;

//define default view ID to easy use
typedef enum {
	//skip ui msg id
	kUI_IDWindow = 3000,
	kUI_IDWindow1,
	kUI_IDWindow2,
	kUI_IDWindow3,
	kUI_IDLable,
	kUI_IDLable1,
	kUI_IDLable2,
	kUI_IDLable3,
	kUI_IDLable4,
	kUI_IDLable5,
	kUI_IDLable6,
	kUI_IDLable7,
	kUI_IDLable8,
	kUI_IDLable9,
	kUI_IDImage,
	kUI_IDImage1,
	kUI_IDImage2,
	kUI_IDImage3,
	kUI_IDButton,
	kUI_IDNumberEdit,
	kUI_IDNumberEdit1,
	kUI_IDNumberEdit2,
	kUI_IDNumberEdit3,
	kUI_IDEdit,
	kUI_IDTextList,
	kUI_IDTextList1,
	kUI_IDTextList2,
	kUI_IDTextList3,
	kUI_IDImageList,
	kUI_IDImageList1,
	kUI_IDImageList2,
	kUI_IDPageControl,
	kUI_IDProgressBar,
	kUI_IDStatusBar,
	kUI_IDStatusBar1,
	kUI_IDListPosIndicator,
	kUI_IDUserStart = 4000
}UIViewID;

#include "ui_theme_define.h"
/************************************************
* common font
************************************************/
#ifndef DEFAULT_FONT
#define DEFAULT_FONT_HEIGHT 14
#define DEFAULT_FONT get_font(DEFAULT_FONT_HEIGHT)
#endif

/************************************************
* common color
************************************************/
#ifndef USE_CUSTOM_WIDGET_COLOR
//common widget
#define DEFAULT_BACKGROUND_COLOR UI_BLACK
#define DEFAULT_TEXT_COLOR UI_WHITE
#define DEFAULT_CURSOR_BLINK_PERIOD_MS 530  //same as window(HKEY_CURRENT_USER\Control Panel\Desktop\CursorBlinkRate)
//list
#define DEFAULT_LIST_BACKGROUND_COLOR UI_BLACK
#define DEFAULT_LIST_ITEM_BACKGROUND_COLOR UI_BLACK
#define DEFAULT_LIST_ITEM_BACKGROUND_COLOR_WHEN_SELECTED UI_BLACK
#define DEFAULT_LIST_ITEM_BACKGROUND_COLOR_WHEN_DISABLE UI_BLACK
#define DEFAULT_LIST_ITEM_TEXT_COLOR UI_WHITE
#define DEFAULT_LIST_ITEM_TEXT_COLOR_WHEN_SELECTED UI_WHITE
#define DEFAULT_LIST_ITEM_TEXT_COLOR_WHEN_DISABLE UI_WHITE
//edit
#define DEFAULT_EDIT_TEXT_COLOR  UI_WHITE
#define DEFAULT_EDIT_TEXT_COLOR_WHEN_FOCUS UI_WHITE
#define DEFAULT_EDIT_TEXT_COLOR_WHEN_DISABLE UI_WHITE
#define DEFAULT_EDIT_BACKGROUND_COLOR UI_BLACK
#define DEFAULT_EDIT_BACKGROUND_COLOR_WHEN_FOCUS UI_BLACK
#define DEFAULT_EDIT_BACKGROUND_COLOR_WHEN_DISABLE UI_BLACK
//page control
#define DEFAULT_PAGE_CONTROL_ITEM_WIDTH 16
#define DEFAULT_PAGE_CONTROL_SELECT_POINTER_IMG get_image(kImgSelectedPointer)
#define DEFAULT_PAGE_CONTROL_UNSELECT_POINTER_IMG get_image(kImgUnselectedPointer)
//progress bar 
#define DEFAULT_PROGRESS_BAR_BACKGROUND_COLOR UI_BLACK
#define DEFAULT_PROGRESS_BAR_COLOR UI_WHITE
#endif