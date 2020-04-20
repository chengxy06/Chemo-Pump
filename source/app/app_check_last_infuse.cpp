/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-22 xqzhao
* Initial revision.
*
************************************************/
#include "app_check_last_infuse.h"
#include "app_main_menu.h"
#include "app_alternative_popup.h"
#include "screen.h"
#include "data.h"
#include "app_delivery.h"
#include "ssz_common.h"
#include "scr_password_popup.h"
#include "string_data.h"
#include "pill_box_install_detect.h"
#include "three_valve_motor.h"
#include "alarm.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
static void app_check_last_infuse_on_cancel_infuse(PopupReturnCode ret_code)
{
    switch(ret_code){
        case kPopupReturnOK:
        {
			InfusionInfo info;
			ssz_mem_zero(&info, sizeof(info));
			data_write(kDataInfusionInfoBlock, &info, sizeof(info));
            three_valve_motor_move_to_position_b();
            app_main_menu_enter();
        }
            break;

        case kPopupReturnWrongPassword:
            break;
    }
}

static void app_check_last_infuse_on_select_if_resume_infuse(PopupReturnCode ret_code, int select_index) {
	switch (ret_code) {
	case kPopupReturnOK:
	{
		if (0 == select_index) {
			InfusionInfo info;
			data_read(kDataInfusionInfoBlock, &info, sizeof(info));

			InfusionParam param;
			param.total_dose = info.total_dose;
			param.meds_b_total_dose = info.meds_b_total_dose;
			//param.first_infusion_dose = info.first_infusion_dose;
			param.infusion_speed = info.infusion_speed;
			param.is_peek_infusion_mode = info.is_peek_infusion_mode;
			param.peek_infusion_start_time_point = info.peek_infusion_start_time_point;
			param.peek_infusion_end_time_point = info.peek_infusion_end_time_point;
			param.peek_infusion_speed = info.peek_infusion_speed;
			app_delivery_set_infusion_param(kNormalInfusion, &param, info.infused_dose, info.meds_b_infused_dose);
			app_delivery_start_to_pause(kNormalInfusion);
			screen_go_back_to_home();
		}
		else if (1 == select_index) {
            g_factory_password=data_read_int(kDataPassword);
            scr_password_popup_enter(&g_factory_password, NULL, app_check_last_infuse_on_cancel_infuse); 
		}
	}
	break;

	case kPopupReturnCancel:
        three_valve_motor_move_to_position_b();
		app_main_menu_enter();
		break;
	}
}

//check if need resume infuse or create new infuse
void app_check_last_infuse()
{
	InfusionInfo info;
	bool is_need_resume = false;
	result_t ret = data_read(kDataInfusionInfoBlock, &info, sizeof(info));

	if (ret== kSuccess) {
		if (info.infusion_state!=kInfusionStop &&
			!app_delivery_is_finish_expect_target_ex(&info)) {
			is_need_resume = true;
			app_alternative_popup_enter(get_string(kStrWhetherToContinueInfuse),
				get_string(kStrYes), get_string(kStrNo),
				0, app_check_last_infuse_on_select_if_resume_infuse);
		}
	}
	if (!is_need_resume) {
		if (!pill_box_install_left_detect() && !pill_box_install_right_detect()) {
			ssz_traceln("start move_to_position_b" );
			three_valve_motor_move_to_position_b();
			ssz_traceln("start move_to_position_b end" );
		}
		//if no delivery, enter main menu
		app_main_menu_enter();
	}
}
