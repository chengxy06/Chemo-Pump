/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ssz_ui_config.h"

/************************************************
* Declaration
************************************************/

/************************************************
* common widget's position and size
************************************************/
#define STATUSBAR_X 0
#define STATUSBAR_Y 0
#define STATUSBAR_WIDTH (DISPLAY_WIDTH)
#define STATUSBAR_HEIGHT 14

#define SCREEN_WINDOW_X 0
#define SCREEN_WINDOW_Y (STATUSBAR_HEIGHT)
#define SCREEN_WINDOW_WIDTH (DISPLAY_WIDTH)
#define SCREEN_WINDOW_HEIGHT (DISPLAY_HEIGHT-STATUSBAR_HEIGHT)

#define NAVIGATION_BAR_X 0
#define NAVIGATION_BAR_Y -16
#define NAVIGATION_BAR_WIDTH (DISPLAY_WIDTH)
#define NAVIGATION_BAR_HEIGHT 16

#define ONE_LINE_HEIGHT 16
#define FIRST_LINE_Y 2

#define LEFT_SOFT_KEY_X 0
#define LEFT_SOFT_KEY_Y -16
#define LEFT_SOFT_KEY_WIDTH 64
#define LEFT_SOFT_KEY_HEIGHT 16

#define RIGHT_SOFT_KEY_X -64
#define RIGHT_SOFT_KEY_Y -16
#define RIGHT_SOFT_KEY_WIDTH 64
#define RIGHT_SOFT_KEY_HEIGHT 16

#define PROGRESS_BAR_X 29
#define PROGRESS_BAR_Y ((DISPLAY_HEIGHT-PROGRESS_BAR_HEIGHT)/2)
#define PROGRESS_BAR_WIDTH (SCREEN_WINDOW_WIDTH-2*PROGRESS_BAR_X)
#define PROGRESS_BAR_HEIGHT (14)

#define RUN_INDICATOR_X (PROGRESS_BAR_X-RUN_INDICATOR_WIDTH-4)
#define RUN_INDICATOR_Y (PROGRESS_BAR_Y+3)
#define RUN_INDICATOR_WIDTH 8
#define RUN_INDICATOR_HEIGHT 8

/************************************************
* ui message id define
************************************************/
enum {
	//id need skip screen message
	//the msg send by popup, select popup or alternative popup, the msg take the select index
	kUIMsgOK = kUIMsgUserStart + 1000, 
	//the msg send by popup, select popup or alternative popup
	kUIMsgCancel,
	kUIMsgWrongPassword,
	kUIMsgDeliveryStateChanged,
};

/************************************************
* ui view id define
************************************************/
enum {
	//id need skip screen id
	kUI_IDStatusbarTitle = kUI_IDUserStart+1000,
	kUI_IDStatusbarTime,
	kUI_IDStatusbarBattery,
	kUI_IDStatusbarLock,
	kUI_IDStatusbarMute,
	kUI_IDLeftSoftKey,
	kUI_IDRightSoftKey,
};