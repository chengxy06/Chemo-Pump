#include "ui_view.h"
#include "ssz_common.h"
#include "font_data.h"
#include "event.h"
#include "screen_config.h"
#include "new"

#include "ui_window.h"
#include "ui_lable.h"
#include "ui_image.h"
#include "ui_text_list.h"
#include "ui_image_list.h"
#include "ui_page_control.h"
#include "ui_number_edit.h"
#include "ui_statusbar.h"
#include "ui_progress_bar.h"
#include "ui_list_pos_indicator.h"

/************************************************
* Declaration
************************************************/
#define UI_VIEW_DEFINE_SIZE_AND_MEMORY(type, class_name,cache) cache,sizeof(class_name),sizeof(cache)/sizeof(class_name),type
#define M_init_obj(obj, class_name, obj_size)  (new (obj) class_name(),ssz_assert(sizeof(class_name)==obj_size))

typedef struct {
	const void* buff;
	int one_view_size;
	int view_num;
	UIViewTypeID type;
}UIViewMemoryInfo;

/************************************************
* Variable 
************************************************/
//all defined widget's count must be smaller than UI_VIEW_COUNT_ALLOC_MAX or will ui_view_regist fail
static uint8_t g_ui_windows[sizeof(UIWindow)*3];
static uint8_t g_ui_lables[sizeof(UILable)*15];
static uint8_t g_ui_images[sizeof(UIImage)*12];
static uint8_t g_ui_text_list[sizeof(UITextList)*1];
static uint8_t g_ui_img_list[sizeof(UIImageList)*1];
static uint8_t g_ui_page_control[sizeof(UIPageControl)*1];
static uint8_t g_ui_number_edit[sizeof(UINumberEdit)*3];
static uint8_t g_ui_status_bar[sizeof(UIStatusbar) * 2];
static uint8_t g_ui_progress_bar[sizeof(UIProgressBar)];
static uint8_t g_ui_list_pos_indicator[sizeof(UIListPosIndicator)];

const static UIViewMemoryInfo g_ui_memory[]={
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUIWindow, UIWindow, g_ui_windows) },
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUILable, UILable, g_ui_lables) },
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUIImage,UIImage, g_ui_images) },
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUITextList,UITextList, g_ui_text_list) },
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUIImageList,UIImageList, g_ui_img_list) },
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUIPageControl, UIPageControl,g_ui_page_control) },
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUINumberEdit, UINumberEdit,g_ui_number_edit) },
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUIStatusBar, UIStatusbar,g_ui_status_bar) },
	{ UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUIProgressBar, UIProgressBar,g_ui_progress_bar) },
	{UI_VIEW_DEFINE_SIZE_AND_MEMORY(kUIListPosIndicator, UIListPosIndicator,g_ui_list_pos_indicator)},
};

/************************************************
* Function 
************************************************/
//user must implement it, user should call the class's constructed function
void ui_view_init_obj(UIViewTypeID view_type, UIView* view, int one_view_size) {
	switch (view_type) {
	case kUIWindow:
		M_init_obj(view, UIWindow, one_view_size);
		break;
	case kUILable:
		M_init_obj(view, UILable, one_view_size);
		break;
	case kUIImage:
		M_init_obj(view, UIImage, one_view_size);
		break;
	case kUINumberEdit:
		M_init_obj(view, UINumberEdit, one_view_size);
		break;
	case kUITextList:
		M_init_obj(view, UITextList, one_view_size);
		break;
	case kUIImageList:
		M_init_obj(view, UIImageList, one_view_size);
		break;
	case kUIPageControl:
		M_init_obj(view, UIPageControl, one_view_size);
		break;
	case kUIStatusBar:
		M_init_obj(view, UIStatusbar, one_view_size);
		break;
	case kUIProgressBar:
		M_init_obj(view, UIProgressBar, one_view_size);
		break;
	case kUIListPosIndicator:
		M_init_obj(view, UIListPosIndicator, one_view_size);
		break;
	default:
		ssz_assert_fail();
		break;
	}
}

void ui_view_config()
{
	int i;
	for (i = 0; i < ssz_array_size(g_ui_memory); ++i) {
		for (int j = 0; j < g_ui_memory[i].view_num; ++j) {
			ui_view_regist(g_ui_memory[i].type, g_ui_memory[i].one_view_size,
				(uint8_t*)(g_ui_memory[i].buff)+j*g_ui_memory[i].one_view_size );
		}
	}

}