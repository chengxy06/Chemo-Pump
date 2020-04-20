/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-09 xqzhao
* Initial revision.
*
************************************************/
#include "app_scr_event_handle.h"
#include "screen.h"
#include "timer.h"
#include "key.h"
#include "ssz_common.h"
#include "app_common.h"
#include "ui_common.h"
#include "app_delivery.h"
#include "app_scr_task_confirm.h"
#include "param.h"
#include "app_popup.h"
#include "scr_tip_popup.h"

/************************************************
* Declaration
************************************************/
#define ENTER_FACTORY_MODE_TOTAL_DOSE_VALUE 88
#define ENTER_FACTORY_MODE_SPEED_VALUE 11

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
void app_scr_on_press_or_release_key(KeyID key, int repeat_count, bool is_press) {
	UIView* p;
	UIMsg msg;

	ssz_mem_zero(&msg, sizeof(msg));
	if (is_press) {
		msg.msg_id = kUIMsgKeyPress;
	}
	else {
		msg.msg_id = kUIMsgKeyRelease;
	}
	switch (key) {
	case kKeyLeft:
		p = ui_view_focused();
		if (p) {
			msg.key_info_param.key = UI_KEY_DECREASE;
			msg.key_info_param.repeat_count = repeat_count;
			ui_view_send_msg(p, msg);
		}
		break;
	case kKeyRight:
		p = ui_view_focused();
		if (p) {
			msg.key_info_param.key = UI_KEY_INCREASE;
			msg.key_info_param.repeat_count = repeat_count;
			ui_view_send_msg(p, msg);
		}
		break;
	case kKeyOK:
		p = ui_view_focused();
		if (p) {
			msg.key_info_param.key = UI_KEY_ENTER;
			msg.key_info_param.repeat_count = repeat_count;
			ui_view_send_msg(p, msg);
		}
		break;
	}
}
void app_scr_on_press_key(KeyID key, int repeat_count) {
	app_scr_on_press_or_release_key(key, repeat_count, true);
}
void app_scr_on_release_key(KeyID key, int repeat_count) {
	app_scr_on_press_or_release_key(key, repeat_count, false);
}

void app_scr_power_off_confirm_popup_callback(PopupReturnCode ret_code)
{
	if (ret_code == kPopupReturnOK) {
		power_off();
	}
}
//hold for 2 seconds
void app_scr_on_hold_power_key_2s() {
	if (!app_delivery_is_exit_running_infusion()) {
		app_popup_enter(kPopupAsk, get_string(kStrPowerOffConfirm),
			get_string(kStrIsConfirm), app_scr_power_off_confirm_popup_callback);
	}
}
void app_scr_on_release_power_key(KeyID key, int repeat_count) {
	timer_stop(kTimerHoldPowerOffKey);
}
void app_scr_on_hold_power_key(KeyID key, int repeat_count) {
	timer_set_handler(kTimerHoldPowerOffKey, app_scr_on_hold_power_key_2s);
	timer_start_oneshot_after(kTimerHoldPowerOffKey, KEY_HOLD_TIME_MS_TO_POWER_OFF - KEY_HOLD_TIME_MS);

}
void app_scr_on_press_start_key(KeyID key, int repeat_count) {
	if (screen_current_id()==kScreenScrSetTotalDose &&
		g_user_set_infusion_para.total_dose == ENTER_FACTORY_MODE_TOTAL_DOSE_VALUE&&
		g_user_set_infusion_para.infusion_speed == ENTER_FACTORY_MODE_SPEED_VALUE) {
		enter_factory_mode();
		return;
	}

	if (app_delivery_state(kNormalInfusion) == kInfusionRunning) {
		app_delivery_pause(kNormalInfusion);
	}
	else if (screen_current_id() == kScreenScrSetTotalDose ||
		screen_current_id() == kScreenScrSetFisrtInfusionDose ||
		screen_current_id() == kScreenScrSetLimitInfusionSpeed ||
		screen_current_id() == kScreenScrSetPeekModeDuration ||
		screen_current_id() == kScreenScrSetInfusionSpeed ||
		screen_current_id() == kScreenScrSetPeekModeSpeed
		) {
		app_scr_task_confirm_enter();
	}
}

/***********************************************
* Description:
*   handle before screen activate
* Argument:
*   scr: the new screen
* Return:
*
************************************************/
void app_scr_on_before_activate(Screen* scr) {
	//reset all key handles
	key_clear_all_handler();

	//set block all keys when pressed
	//key_set_all_blocked_when_key_pressed();//TODO:

	//register some key to map to UI key msg
	const KeyID ui_keys[] = { kKeyLeft,kKeyRight,kKeyOK };
	for (int i = 0; i < ssz_array_size(ui_keys); i++) {
		if (ui_keys[i] == kKeyLeft || ui_keys[i] == kKeyRight) {
			//enable left and right key's repeat
			key_set_handler_with_repeat(ui_keys[i], kKeyEventPressed, app_scr_on_press_key);
		}
		else {
			key_set_handler(ui_keys[i], kKeyEventPressed, app_scr_on_press_key);
		}
		key_set_handler(ui_keys[i], kKeyEventReleased, app_scr_on_release_key);
	}

	//register common key handler
	key_set_handler(kKeyBack, kKeyEventHold, app_scr_on_hold_power_key);
	key_set_handler(kKeyStart, kKeyEventPressed, app_scr_on_press_start_key);

	//show global statusbar
	UIStatusbar* p_statusbar = M_ui_statusbar(ui_view_desktop()->get_child(kUI_IDStatusBar));
	p_statusbar->show();
	//clear global statusbar's title
	p_statusbar->set_title(NULL);
}
void app_scr_on_after_activate(Screen* scr) {
	if (scr->view) {
		Rect rc;
		scr->view->area(&rc);
		//if the screen is full screen, hide statusbar
		if (rc.x0==0&& rc.y0 == 0&& rc.x1 == SCREEN_WIDTH-1&&rc.y1==SCREEN_HEIGHT-1) {
			UIStatusbar* p = M_ui_statusbar(ui_view_desktop()->get_child(kUI_IDStatusBar));
			p->hide();
		}
	}

	//init global or screen's statusbar, like time, battery icon
	statusbar_init();
}
void app_scr_on_after_deactivate(Screen* scr) {
	//stop all screen timer
	timer_stop_all(TIMER_GROUP_SCREEN);

}

/***********************************************
* Description:
*   register screen monitor event
* Argument:
*
* Return:
*
************************************************/
void app_scr_event_handle_init()
{
	//register the event before active screen
	screen_set_event_Handler(kScreenEventBeforeActivate, app_scr_on_before_activate);
	//register the event after active screen
	screen_set_event_Handler(kScreenEventAfterActivate, app_scr_on_after_activate);
	//register the event after deactive screen
	screen_set_event_Handler(kScreenEventAfterDeactivate, app_scr_on_after_deactivate);
}
