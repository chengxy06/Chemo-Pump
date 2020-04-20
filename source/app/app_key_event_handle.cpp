/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-09 xqzhao
* Initial revision.
*
************************************************/
#include "app_key_event_handle.h"
#include "key.h"
#include "voice.h"
#include "msg.h"
#include "alarm.h"
#include "app_common.h"
#include "app_scr_event_handle.h"
#include "screen.h"
#include "scr_tip_popup.h"
#include "scr_unlock_tip.h"
#include "param.h"

/************************************************
* Declaration
************************************************/
extern int g_monitor_no_operate_time_ms;

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

//static void app_key_on_press_unlock_combination_key(KeyID key1, KeyID key2)
//{
//	unlock_keypad();
//	if (screen_is_exist(kScreenKeypadLockTipPopup)) {
//		screen_delete_by_id(kScreenKeypadLockTipPopup);
//	}
//	scr_tip_popup_enter(kTipPopupInformation,
//		NULL, get_string(kStrKeyIsUnlocked), TIP_POPUP_SHOW_TIME_MS, NULL);
//
//}

/***********************************************
* Description:
*   handle before key handle
* Argument:
*   key: the pressed key
*   is_key_handled: is the key handled
*
* Return:
*
************************************************/
static void app_key_on_before_any_key_handle(KeyID key, KeyEvent key_event, bool is_key_handled, int repeat_count)
{
	if (key_event == kKeyEventPressed) {
#ifdef SSZ_TARGET_MACHINE
		//play beep while the key pressed
		//voice_play_beep_once();
#endif 
		//continue check no operate
		if (timer_is_actived(kTimerNoOperateWhenPause)) {
			timer_start_oneshot_after(kTimerNoOperateWhenPause, NO_OPERATE_TOO_LONG_MS_WHEN_INFUSION_PAUSE);
		}

		if (timer_is_actived(kTimerNoOperate)) {
			timer_start_oneshot_after(kTimerNoOperate, g_monitor_no_operate_time_ms);
		}
	}

	//release power key timer to avoid power off
	if (key_event == kKeyEventReleased && key == kKeyBack) {
		app_scr_on_release_power_key(kKeyBack, repeat_count);
	}

	//check if keypad lock, 
	if (is_keypad_locked()) {
		//if (key_event == kKeyEventPressed) {
		//	key_set_combination_handler(kKeyRight, kKeyLeft, kKeyEventPressed, app_key_on_press_unlock_combination_key);
		//}
		//if locked and not at lock tip screen or sleep , notify user
		if (screen_current_id() != kScreenKeypadLockTipPopup &&
			screen_current_id() != kScreenKeypadUnlockTip &&
			screen_current_id() != kScreenPressWrongToUnlockKeypad &&
			screen_current_id() != kScreenSleep) {
			if (key != kKeyStart) {
				key_disable_handle_once();
				if (key_event == kKeyEventPressed ) {
					if (key == kKeyLeft) {
						scr_unlock_tip_enter();
					}
					else {
						scr_tip_popup_enter_ex(kScreenKeypadLockTipPopup, kTipPopupInformation,
							NULL, get_string(kStrKeyIsLocked), TIP_POPUP_SHOW_TIME_MS, NULL);
					}
					return;
				}
			}
		}
		else if (screen_current_id() == kScreenKeypadLockTipPopup) {
			if (key_event == kKeyEventPressed && key == kKeyLeft) {
				key_disable_handle_once();
				scr_unlock_tip_enter();
			}
		}
	}
}

void app_key_on_before_any_combination_key_handle(KeyID key1, KeyID key2, KeyEvent key_event, bool is_key_handled) {
	//if keypad lock and is not unlock key, disable handle
	if (is_keypad_locked()) {
		//if ((key1 == kKeyLeft && key2 == kKeyRight) ||
		//	(key2 == kKeyLeft && key1 == kKeyRight)) {
		//}
		//else {
			key_disable_handle_once();
		//}
	}
}

//register key monitor event
void app_key_event_handle_init()
{
	//register the event before handle key
	key_set_monitor_event_handler(kKeyMonitorEventBefore, app_key_on_before_any_key_handle);
	key_set_combination_monitor_event_handler(kKeyMonitorEventBefore, app_key_on_before_any_combination_key_handle);
}
