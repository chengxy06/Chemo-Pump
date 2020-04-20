#include "ssz_config.h"
#include "ssz_def.h"
#include "ssz_spi.h"
#include "init.h"
#include "config.h"
#include "version.h"
#include "event.h"
#include "ssz_common.h"
#include "timer.h"
#include "com.h"
#include "sys_power.h"
#include "app_home.h"
#include "ui.h"
#include "image_data.h"
#include "key.h"
#include "mid_rtc.h"
#include "mid_common.h"

#include "app_startup_and_post.h"
#include "app_main_menu.h"
#include "app_scr_alarm.h"
#include "app_common.h"
/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/


/************************************************
* Function 
************************************************/


extern "C" void run_test_after_main() {

	//test auto clear air
	//app_clear_air_create(AUTO_CLEAR_AIR_MEDS_A_TOTAL_DOSE_uL, AUTO_CLEAR_AIR_MEDS_B_TOTAL_DOSE_uL,
	//	false);
	//app_scr_auto_clear_air_enter();
	

	//test manual clear air
	//app_scr_manual_clear_air_enter();
	

	//test factory mode
	//enter_factory_mode();

	//app_scr_alarm_enter(kAlarmThreeValveMotorStuckID);
}

