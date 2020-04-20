/************************************************
* DESCRIPTION:
*
************************************************/
#include "app_cmd.h"
#include "config.h"
#include "com.h"
#include "ssz_time.h"
#include "timer.h"
#include "version.h"
#include "record_log.h"
#include "ssz_time_utility.h"
#include "mid_rtc.h"
#include "string.h"
#include "stdlib.h"
#include "ssz_common.h"
#include "data.h"
#include "drv_sst25_flash.h"
#include "app_delivery.h"
#include "mid_common.h"
#include "drv_isd2360.h"
#include "common.h"
#include "drv_key.h"
#include "drv_stf_motor.h"
#include "pill_box_install_detect.h"
#include "pressure_bubble_sensor.h"
#include "vibrater_motor.h"
#include "infusion_motor.h"
#include "three_valve_motor.h"
#include "ssz_str_utility.h"
#include "drv_oled.h"
#include "pill_box_install_detect.h"
#include "ssz_config.h"
#include "ssz_def.h"
#include "ui_view.h"
#include "app_infusion_monitor.h"
#include "display.h"
#include "param.h"
#include "app_mcu_comm.h"
#include "drv_lowpower.h"
#include "screen.h"
#include "drv_infusion_motor.h"
#include "do_before_main_loop.h"
#include "app_mcu_monitor.h"
#include "scr_sleep.h"
#include "drv_rtc.h"
#include "app_system_monitor.h"


#ifdef CPU_USAGE_MODULE_ENABLE
#include "cpu_usage.h"
#endif
#if STOP_WATCH_MODULE_ENABLE
#include "stop_watch.h"
#endif
#if UI_MODULE_ENABLE
#include "ui.h"
#endif
#if UI_VIEW_MODULE_ENABLE
#include "ui_view.h"
#endif
#include "app_main_menu.h"
#include "record_log_text.h"
#include "app_common.h"
#include "sys_power.h"
#include "display.h"
#ifdef SSZ_TARGET_MACHINE
#include "tim.h"
#endif

/************************************************
* Declaration
************************************************/
#define CMD_MAX_SIZE_OF_RECEIVE_STR 60
#define CMD_ID kComPCUart
#define CMD_HANDLE_ORDER kComHandleSecond
#define CMD_ENABLE_PASSWORD "givemecmd"

typedef  int (*AppCmdHandler)(const char* cmd, char *params[], int param_size);

typedef struct  
{
    const char *command;
    AppCmdHandler handler;
    const char *description;
}AppCmdInfo;

static void app_cmd_parse_from_str(const char* cmd);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TEST
int run_test_main(int argc, const char **argv);
#endif

#ifdef __cplusplus
}
#endif


/************************************************
* Variable 
************************************************/
static char g_app_cmd_receive_str[CMD_MAX_SIZE_OF_RECEIVE_STR];
static int g_app_cmd_receive_index = 0;
static char g_app_cmd_last_receive_str[CMD_MAX_SIZE_OF_RECEIVE_STR];
static bool g_app_cmd_is_enable_cmd = false;
static bool g_app_cmd_is_enable_show_input = false;
static const AppCmdInfo* g_app_cmd_info_ptr = NULL;
static const uint8_t g_app_cmd_cmd_return_id[] = {0x0};
static bool g_app_cmd_wait_line_end; //wait \n when receive \r

/************************************************
* Function 
************************************************/

/***********************************************
* Description:
*   send the flag to PC, so PC can know that the cmd is exec complete.
* Argument:
*   ret_code:
*
* Return:
*
************************************************/
static void app_cmd_send_return_flag(int ret_code)
{
#ifdef SSZ_TARGET_SIMULATOR
	fwrite(g_app_cmd_cmd_return_id, 1, sizeof(g_app_cmd_cmd_return_id), stdout);
#else
	ssz_fwrite(com_to_file(CMD_ID), g_app_cmd_cmd_return_id, sizeof(g_app_cmd_cmd_return_id));
#endif
}

void app_cmd_repeat_exe_cmd(void)
{
	char tmp_receive_str[CMD_MAX_SIZE_OF_RECEIVE_STR];

	strcpy(tmp_receive_str, g_app_cmd_last_receive_str);
	app_cmd_parse_from_str(tmp_receive_str);
}

static void app_cmd_output_error_info(int error_code)
{
	switch (error_code) {
	case kInvalidParam:
		printf("invalid param\n");
		break;
	case kNotExist:
		printf("not exist\n");
		break;
	default:
		printf("error: %d\n", error_code);
		break;
	}
}
//all command function
static int app_cmd_help(const char* cmd, char* params[], int param_size)
{
    const AppCmdInfo* p;
    p = g_app_cmd_info_ptr;

    printf("all cmds:\n");
    while(p->command[0] != 0)
    {
        if (strlen(p->command) >= 9 ) {
            if(strcmp(p->command, "clear_data") == 0 || strcmp(p->command, "clear_log") == 0 )
                printf("%s\t\t", p->command);//%-24s
            else
                printf("%s\t", p->command);//%-24s

        } else {
            printf("%s\t\t", p->command);//%-24s
        }

        if (p->description != 0)
        {
            printf(" %s", p->description);
        }
        putchar('\n');
        p++;
    }
    
    return 0;
}

static int app_cmd_exit(const char* cmd, char* params[], int param_size)
{
    //do not receive data
    com_set_receive_handler(CMD_ID, CMD_HANDLE_ORDER, NULL);
    g_app_cmd_is_enable_cmd = false;
            
    return 0;
}

// static int app_cmd_disable(const char* cmd, char* params[], int param_size)
// {
//     g_app_cmd_is_enable_cmd = false;
            
//     return 0;
// }

static int app_cmd_repeat(const char* cmd, char* params[], int param_size)
{
	int time_ms = 0;

	if (param_size>0) {
		time_ms = atoi(params[0]);
	}
	if (time_ms==0) {
		time_ms = 1000;
	}
    timer_start_periodic_every(kTimerRepeatCmd, time_ms);
    app_cmd_repeat_exe_cmd();
    return 0;
}

#ifdef TEST
static int app_cmd_test(const char* cmd, char* params[], int param_size)
{
	run_test_main(param_size, (const char **)params);
	return 0;
}
#endif
#if STOP_WATCH_MODULE_ENABLE
static int app_cmd_stop_watch_set(const char* cmd, char* params[], int param_size) {
	int is_on = 1;
	if (param_size > 0) {
		is_on = atoi(params[0]);
	}
	if (is_on==0) {
		printf("disable stopwatch\n");
		stop_watch_disable();
	}
	else {
		printf("enable stopwatch\n");
		stop_watch_enable();
	}
	return 0;
}
#endif
static void app_cmd_output_all_module_info(void)
{
    int i;

    printf("\nmodule output status list:\n");

    for (i = 0; i < kModuleIDMax; i++)
    {
        if (is_module_enable_output((ModuleID)i))
        {
            printf("%s [yes]\n", get_module_name((ModuleID)i));
        }
        else
        {
            printf("%s [no]\n", get_module_name((ModuleID)i));
        }
    }
}

static bool app_cmd_set_module_output_by_name(const char* module_name, bool is_enable)
{
    int i;
    bool ret = false;

    for(i=0; i < kModuleIDMax; i++)
    {
        if(strcmp(module_name, get_module_name((ModuleID)i)) == 0 )
        {
            set_module_output((ModuleID)i, is_enable);
            ret = true;
            break;
        }
        else if(strcmp(module_name,"all") == 0)
        {
			set_module_output((ModuleID)i, is_enable);
            ret = true;
        }
            
    }

    return ret;
}

static int app_cmd_module(const char* cmd, char* params[], int param_size)
{
	app_cmd_output_all_module_info();

	return 0;
}

static int app_cmd_enable_output(const char* cmd, char* params[], int param_size)
{
    int i;

    for(i=0; i<param_size; i++)
    {
        if(app_cmd_set_module_output_by_name(params[i], true))
        {
            printf("%s module set enabled success\n", params[i]);
        }
		else {
			printf("unknow module: %s\n", params[i]);
		}
    }

    app_cmd_output_all_module_info();

    return 0;
}

static int app_cmd_disable_output(const char* cmd, char* params[], int param_size)
{
    int i;

    for(i=0; i<param_size; i++)
    {
        if(app_cmd_set_module_output_by_name(params[i], false))
        {
            printf("%s module set disabled success\n", params[i]);
        }
		else {
			printf("unkonw module: %s\n", params[i]);
		}
    }
    app_cmd_output_all_module_info();

    return 0;
}


static int app_cmd_test_watchdog(const char* cmd, char* params[], int param_size)
{
    ssz_check(0); //lint !e506
    return 0;
}

static int app_cmd_version(const char* cmd, char* params[], int param_size)
{
	app_mcu_send_to_slave(COMM_ASK_SLAVER_MCU_VERSION,0,0); 
	printf("%s\n", version_software_name());
	printf("Master Software Version: %d.%d.%d.%d\n", version_info()->major, version_info()->minor, version_info()->revision,
		version_info()->internal_version);
	if(app_mcu_receive_slaver_version_ture()){
		printf("Slaver Software Version: %d.%d.%d.%d\n", slaver_version_info()->major, slaver_version_info()->minor,
		slaver_version_info()->revision, slaver_version_info()->internal_version);
	}

	printf("Build Time: %s\n", version_build_time());
	int days;
	int hour;
	int minute;
	int second;
	int ms;
	ms = ssz_milliseconds_to_run_time_of_days(ssz_tick_time_now(), &days, &hour, &minute, &second);
	printf("Run Time: %d day, %02d:%02d:%02d.%03d\n", days, hour, minute, second, ms);
	return 0;
}

#if CPU_USAGE_MODULE_ENABLE
static int app_cmd_cpu_usage(const char* cmd, char* params[], int param_size)
{
	cpu_usage_print_if_large_than(0);
	return 0;
}
#endif
static int app_cmd_time(const char* cmd, char* params[], int param_size)
{
	printf("%s\n", ssz_time_now_str());
	return 0;
}
#if RTC_MODULE_ENABLE
//set time
static int app_cmd_set_time(const char* cmd, char* params[], int param_size)
{
	if (param_size == 1)
	{
		SszTime in_time;
		in_time = ssz_str_to_time(params[0]);
		rtc_set_time(&in_time);
	}
	else {
		app_cmd_output_error_info(kInvalidParam);
	}

	return 0;
}
//set date
static int app_cmd_set_date(const char* cmd, char* params[], int param_size)
{
	if (param_size == 1)
	{
		SszDate in_time;
		in_time = ssz_str_to_date(params[0],kSszDateFormat_YYYY_MM_DD);
		rtc_set_date(&in_time);
	}
	else {
		app_cmd_output_error_info(kInvalidParam);
	}

	return 0;
}
static int app_cmd_start_rtc(const char* cmd, char* params[], int param_size)
{
    drv_rtc_start();

	return 0;
}

static int app_cmd_stop_rtc(const char* cmd, char* params[], int param_size)
{
    drv_rtc_stop();

	return 0;
}

#endif
//output io's pin value
int app_cmd_io(const char* cmd, char* params[], int param_size)
{

	//ssz_gpio_set(BT_PWR_EN_GPIO_Port,BT_PWR_EN_Pin);
	printf("PB3--motor encoder A: %d\n", ssz_gpio_is_high(SZ_HALL_A_GPIO_Port, SZ_HALL_A_Pin));
	printf("PD7--motor encoder B: %d\n", ssz_gpio_is_high(SZ_HALL_B_GPIO_Port, SZ_HALL_B_Pin));

	//需要辅MCU使能电源
	printf("optical coupler(need slaver enable power): %d\n", ssz_gpio_is_high(SZDJ_SUDU_GPIO_Port, SZDJ_SUDU_Pin));

	drv_three_valve_motor_pwr_enable(); 
	printf("PB8--three-way valve motor encoder A: %d\n", ssz_gpio_is_high(STF_HALL_A_GPIO_Port, STF_HALL_A_Pin));
	printf("PB9--three-way valve motor encoder B: %d\n", ssz_gpio_is_high(STF_HALL_B_GPIO_Port, STF_HALL_B_Pin));

//	 printf("PC13--power key: %d, is press:%d\n", drv_key_is_high(kKeyPower),
//	 	drv_key_is_pressed(kKeyPower));
	printf("PE4--left key: %d, is press:%d\n", drv_key_is_high(kKeyLeft), 
		drv_key_is_pressed(kKeyLeft));
	printf("PE5--right key: %d, is press:%d\n", drv_key_is_high(kKeyRight), 
		drv_key_is_pressed(kKeyRight));
	printf("PE3--confirm key: %d, is press:%d\n", drv_key_is_high(kKeyOK), 
		drv_key_is_pressed(kKeyOK));
	printf("PE6--back key: %d, is press:%d\n", drv_key_is_high(kKeyBack), 
		drv_key_is_pressed(kKeyBack));
	printf("PE2--start key: %d, is press:%d\n", drv_key_is_high(kKeyStart), 
		drv_key_is_pressed(kKeyStart));

	drv_three_valve_pos_detect_pwr_enable();
	ssz_delay_us(100);
	printf("PC2--three-way valve pos A, is at A:%d\n", drv_three_valve_is_pos_a_detected());
	printf("PC3--three-way valve pos B, is at B:%d\n", drv_three_valve_is_pos_b_detected());

	pill_box_install_sensor_left_pwr_enable();
	pill_box_install_sensor_right_pwr_enable();
	printf("PA8--pill box left install: %d, is install:%d\n", 
		ssz_gpio_is_high(PILL_BOX_INSTALL_SENSOR_PORT,PILL_BOX_INSTALL_SENSOR_LEFT_PIN), pill_box_install_left_detect());
	printf("PA11--pill box right install: %d, is install:%d\n", 
		ssz_gpio_is_high(PILL_BOX_INSTALL_SENSOR_PORT,PILL_BOX_INSTALL_SENSOR_RIGHT_PIN), pill_box_install_right_detect());
	
	printf("PD4--alarm from SMCU: %d\n", ssz_gpio_is_high(S_RST_STA_GPIO_Port, S_RST_STA_Pin));

	pressure_and_bubble_sensor_pwr_enable();
	printf("PB2--bubble: %d, is bubble exist:%d\n", ssz_gpio_is_high(BUBBLE_DETECT_GPIO_Port, BUBBLE_DETECT_Pin), pressure_bubble_sensor_is_generate_bubble());
	
	printf("PE9--AC connect: %d, is AC connect:%d\n", ssz_gpio_is_high(SYS_PWR_COM_PORT, SYS_PWR_ADAPTER_INSERT_PIN), sys_power_is_adapter_insert());

    return 0;
}

//output ad and ad's voltage value
int app_cmd_ad(const char* cmd, char* params[], int param_size)
{
	sys_power_ADC_enable();
	int ad = mid_adc_get_ADC_channel_value(kSysPwrADC);
    int voltage = sys_power_battery_voltage();
	printf("battery:%d=%dmv, offset:%d ad, real voltage:%dmv\n,", ad, M_get_battery_voltage_from_adc(ad), g_battery_offset_ad, voltage);

	ad = mid_adc_get_ADC_channel_value(kSlaverMCUPwrADC);
	printf("slaver mcu:%d=%dmv\n", ad, M_get_slaver_mcu_voltage_from_adc(ad));
	

	pressure_and_bubble_sensor_pwr_enable();
	ad = mid_adc_get_ADC_channel_value(kPressureADC);
	printf("pressure:%d=%dmv=%dkPa\n", ad, M_get_voltage_from_adc(ad), 
		data_read_int(kDataOcclusionSlope)*(ad - pressure_adc_when_installed())/1000);
	
	ad = mid_adc_get_ADC_channel_value(kSZDJCurrentADC);
	printf("motor current:%d=%dmv\n", ad, M_get_voltage_from_adc(ad));

	drv_three_valve_motor_pwr_enable();
	ad = mid_adc_get_ADC_channel_value(kSTFDJCurrentADC);
	printf("three-way valve current:%d=%dmv\n", ad, M_get_voltage_from_adc(ad));
    return 0;
}


//output saved data and other you need know data
int app_cmd_data(const char* cmd, char* params[], int param_size)
{
	printf("--FCT--\n");
	printf("BatteryRedress: %d\n", data_read_int(kDataBatteryRedress));

	printf("\n--Factory--\n");
	char sn[SERIAL_NUMBER_LEN + 1] = {0};
	data_read(kDataSerialNumber, sn, SERIAL_NUMBER_LEN);
	printf("S/N: %s\n", sn);
	printf("HW: V%03d\n", data_read_int(kDataHardwareVersion));
	printf("BatteryThreshold: %d<-%d<-%d<-%dmV\n", data_read_int(kDataBatteryTwoThreshold),
		data_read_int(kDataBatteryOneThreshold), data_read_int(kDataBatteryLowThreshold),
		data_read_int(kDataBatteryExhaustThreshold));
	//temp = data_read_int(kDataOcclusionThreshold);
	//printf("OcclusionThreshold: AD = %d  V = %d mv\n", temp ,(temp *3300 /4096) );
	printf("OcclusionSlope: %d/1000,  one drop increase adc:%d\n",
		data_read_int(kDataOcclusionSlope), data_read_int(kDataOcclusionIncreaseOfOneDrop));
	printf("EncoderCountEachML: %d\n", data_read_int(kDataEncoderCountEachML));
	printf("Password: %d\n", data_read_int(kDataPassword));
    printf("Bubble alarm: %d\n", data_read_int(kDataBubbleAlarmSwitch));
	printf("pressure threshold: %d\n", data_read_int(kDataPressureThreshold));
	printf("precision factor: %d\n", data_read_int(kDataPrecisionFactor));

	printf("\n--User--\n");
	printf("VoiceVolume: %d\n", data_read_int(KDataVoiceVolume));
	printf("Brightness: %d\n", data_read_int(kDataBrightness));
	printf("PeekModeSwitch: %d\n", data_read_int(kDataPeekModeSwitch));
	printf("RunLifetimeCount: %d\n", data_read_int(kDataRunLifetimeCount));
	printf("InfusionModeSelection: %d\n", data_read_int(kDataInfusionModeSelection));
	printf("MaxCleanTubeDose: %d\n", data_read_int(kDataMaxCleanTubeDose));

	InfusionInfo info;
	if (data_read(kDataInfusionInfoBlock, &info, sizeof(info)) == kSuccess) {
		printf("last Infusion Info:\nstate:%d\n", info.infusion_state);
		printf("meds-a total dose:%duL, speed:%duL/hr, infused dose:%duL\n",
			info.total_dose,
			info.infusion_speed,
			info.infused_dose);
		printf("meds-b total dose:%duL, infused dose:%duL\n",
			info.meds_b_total_dose,
			info.meds_b_infused_dose);
		if (info.is_peek_infusion_mode) {
			printf("peek_speed:%duL/hr, time:%dhr~%dhr\n",
				info.peek_infusion_speed,
				info.peek_infusion_start_time_point,
				info.peek_infusion_end_time_point
			);
		}
	}

	UserSetInfusionParaToSave param;
	if (data_read(kDataSetInfusionParaInfoBlock, &param, sizeof(param)) == kSuccess) {
		printf("last Infusion Param:\n");
		printf("meds-a total dose:%dmL, meds-b total dose:%dmL, speed:%d*0.1mL/hr, duration:%d*0.1hr\n",
			param.total_dose,
			param.meds_b_total_dose,
			param.infusion_speed,
			param.infusion_duration);
		printf("peek_speed:%d*0.1mL/hr, time:%dhr~%dhr\n",
			param.peek_infusion_speed,
			param.peek_infusion_start_time_point,
			param.peek_infusion_end_time_point
		);
	}
    
    return 0;
}
//clear user, factory, FCT data
static int app_cmd_change_data(const char* cmd, char* params[], int param_size)
{
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}
	int type = atoi(params[0]);
	int data = atoi(params[1]);
	switch(type)
	{
	case 1:
		data_write_int(kDataEncoderCountEachML, data);
        set_encoder_count_of_one_ml((int)(data * data_read_int(kDataPrecisionFactor) / 100.0));
		printf(" encoder per mL = %d   \n", data);
		break;
	case 2:
		data_write_int(kDataOcclusionSlope, data);
		printf("occlusion slope = %d   \n", data);
		break;
    case 3:
        data_write_int(kDataOcclusionIncreaseOfOneDrop, data);
        printf("occlusion one drop ADC = %d   \n", data);
        break;
    case 4:
        data_write_int(kDataBatteryRedress, data);
        set_battery_offset_ad(data);
        printf("battery ad offset = %d   \n", data);     
        break;
	default:
		app_cmd_output_error_info(kInvalidParam);
		break;
	}

	return 0;
}

//output data map
int app_cmd_data_map(const char* cmd, char* params[], int param_size)
{
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	int block_sec = 0;

	for (int i = 0; i < kDataIDMax; i++) {
		if(data_find_save_info_by_id((DataID)i, &data_info, &block_info, NULL)!=-1){
			if (i == block_info.data_id) {
				block_sec++;
				printf("[%d] block id[%d]: address[0x%x],size[%d], area_size[%d]\n", block_sec, i, 
					block_info.data_address,
					data_info.data_size, block_info.save_area_size);
			}
			else {
				printf("-id[%d]: address[0x%x],size[%d]\n", i, block_info.data_address + data_info.data_offset,
					data_info.data_size);
			}
		}
	}

#if RECORD_LOG_MODULE_ENABLE
	const SszRecord* rc_info;
	printf("\n");
	for (int i = 0; i < kLogTypeMax; i++) {
		rc_info = record_log_info((LogType)i);
		printf("Log id[%d]: head_address[0x%x],size[%d], head_max_count[%d]\n",
			i, rc_info->record_head_address, rc_info->record_head_area_size,rc_info->record_head_max_count);
		printf("	address[0x%x],size[%d], one_size[%d],max_count[%d]\n",
			rc_info->record_address, rc_info->record_area_size, 
			rc_info->one_record_size, rc_info->record_max_count);
	}
#endif

	return 0;
}



//clear user, factory, FCT data
static int app_cmd_clear_data(const char* cmd, char* params[], int param_size)
{
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}
	int type = atoi(params[0]);
	switch(type)
	{
	case 1:
		data_erase_block(kDataUserInitBlock);
		printf("finish clear %s data, please reset\n", "user");
		break;
	case 2:
		data_erase_block(kDataFactoryInitBlock);
		printf("finish clear %s data, please reset\n", "factory");
		break;
	case 3:
		data_erase_block(kDataFCTInitBlock);
		printf("finish clear %s data, please reset\n", "FCT");
		break;
	default:
		app_cmd_output_error_info(kInvalidParam);
		break;
	}

	return 0;
}


//get log type
LogType app_cmd_print_and_get_log_type(int param)
{
    LogType log_type = kOperationLog;
    switch(param)
    {
		case 0:
		case 1:
			log_type = kOperationLog;
			printf("operation log");
			break;
        case 2:
            log_type = kHistoryLog;
			printf("history log");
            break;  
        default:
			//ssz_assert(0);
			log_type = kLogTypeMax;
        	break;
    }	

	return log_type;
}

static void app_cmd_print_log(LogType log_type, uint32_t log_event_type,
	int query_log_size, bool is_print_converted_text) {
	int log_size = record_log_size(log_type);

	LogOneRecord one_record;
	SszDateTime occur_time;
	int j;
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_start();
	int stop_watch_index;
#endif
	result_t ret;
	LogVar vars[4];
	int var_size;
	char time_str[SSZ_TIME_STR_SIZE];
	char buff[LOG_TEXT_MAX_SIZE];
	for (int i = log_size - query_log_size; i < log_size; ++i) {
		ssz_clear_watchdog();
		j = i-(log_size - query_log_size) + 1;
#if STOP_WATCH_MODULE_ENABLE
		stop_watch_index = stop_watch_child_start("read_one_record");
#endif
		ret = record_log_get(log_type, i, &one_record);
#if STOP_WATCH_MODULE_ENABLE
		stop_watch_child_stop(stop_watch_index);
#endif
		if (ret == kChecksumWrong) {
			printf("%04d: crc wrong\n", j);
		}
		else if (one_record.event_type&log_event_type) {
			ssz_seconds_to_time(one_record.occur_time, &occur_time);

			var_size = record_log_data_to_var(one_record.event_data, vars, 4);
			ssz_time_str(&occur_time, time_str);
			if (is_print_converted_text) {
				printf("%04d: %s, ",
					j, time_str);
				ssz_str_print_only_one_line(record_log_to_str(&one_record, buff, sizeof(buff)));
				putchar('\n');
			}else{
				printf("%04d: %s, id[%d], param[",
					j, time_str, one_record.event_id);
				for (j = 0; j < var_size; j++) {
					if (j > 0) {
						printf(", ");
					}
					printf("%s", record_log_var_to_str(&vars[j], buff, sizeof(buff)));
				}
				printf("]\n");
			}
		}
		ssz_clear_watchdog();
	}
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_stop();
	stop_watch_print();
#endif
}
//read log
static int app_cmd_log(const char* cmd, char* params[], int param_size)
{
	int log_type_i= 0;
	int query_log_size = -1;
	bool is_print_converted_text = true;

	for (int i = 0; i < param_size; i++) {
		if (i == 0)
		{
			log_type_i = atoi(params[i]);
		}
		else if (i==1) {
			query_log_size = atoi(params[i]);
		}
		else if (i == 2 && atoi(params[i])==0) {
			is_print_converted_text = false;
		}
	}

	LogType log_type = app_cmd_print_and_get_log_type(log_type_i);
	if (log_type == kLogTypeMax) {
		app_cmd_output_error_info(kInvalidParam);
		return -1;
	}
	if (query_log_size < 0 || query_log_size > record_log_size(log_type)) {
		query_log_size = record_log_size(log_type);
	}

    printf(" total have [%d], read [%d] from last\n", record_log_size(log_type), query_log_size);
	app_cmd_print_log(log_type, LOG_EVENT_TYPE_ALL, query_log_size, is_print_converted_text);

	return 0;
}

//read log by log type
static int app_cmd_filter_log(const char* cmd, char* params[], int param_size)
{
	int log_type_i = 0;
	uint32_t event_type = LOG_EVENT_TYPE_ALL;
	int query_log_size = -1;
	bool is_print_converted_text = true;

	for (int i = 0; i < param_size; i++) {
		if (i == 0)
		{
			log_type_i = atoi(params[i]);
		}
		else if (i == 1) {
			event_type = ssz_str_hex_to_int(params[i]);
		}
		else if (i == 2) {
			query_log_size = atoi(params[i]);
		}
		else if (i == 3 && atoi(params[i]) == 0) {
			is_print_converted_text = false;
		}
	}

	LogType log_type = app_cmd_print_and_get_log_type(log_type_i);
	if (log_type == kLogTypeMax) {
		app_cmd_output_error_info(kInvalidParam);
		return -1;
	}
	if (query_log_size < 0 || query_log_size > record_log_size(log_type)) {
		query_log_size = record_log_size(log_type);
	}
	printf(" total have [%d], read [%d] from last, use filter type[0x%x]\n", 
		record_log_size(log_type), query_log_size, event_type);

	app_cmd_print_log(log_type, event_type, query_log_size, is_print_converted_text);

	return 0;
}

//clear log
static int app_cmd_clear_log(const char* cmd, char* params[], int param_size)
{
    int i = 0;

	if (param_size == 1)
	{
		i = atoi(params[0]);
	}
	
	LogType log_type = app_cmd_print_and_get_log_type(i);    
	if(log_type==kLogTypeMax) {
		app_cmd_output_error_info(kInvalidParam);
		return 0;
	}

	printf(" clear\n");
	record_log_clear(log_type);

	return 0;
}

//test write more than max log 
static int app_cmd_test_log(const char* cmd, char* params[], int param_size)
{
	int log_type_i = 0;
	int add_log_total = 1;

	for (int i = 0; i < param_size; i++) {
		if (i == 0)
		{
			log_type_i = atoi(params[i]);
		}
		else if (i == 1) {
			//user can input the size as OPERATION_LOG_MAX_SIZE * 2 + 1
			add_log_total = atoi(params[i]);
		}
	}

	LogType log_type = app_cmd_print_and_get_log_type(log_type_i);
	if (log_type == kLogTypeMax) {
		app_cmd_output_error_info(kInvalidParam);
		return 0;
	}

	printf(" add %d for test\n", add_log_total);
	#if STOP_WATCH_MODULE_ENABLE
	int stop_watch_index;
	stop_watch_start();
	#endif
	for (int i = 0; i < add_log_total; i++) {
		ssz_clear_watchdog();
		printf("write %d\n", i+1);
		#if STOP_WATCH_MODULE_ENABLE
		stop_watch_index = stop_watch_child_start("add_one_record");
		#endif
		record_log_add_info_with_data(log_type, kLogEventPowerOn, NULL, 0);
		#if STOP_WATCH_MODULE_ENABLE
		stop_watch_child_stop(stop_watch_index);
		#endif
		ssz_clear_watchdog();
	}
	#if STOP_WATCH_MODULE_ENABLE
	stop_watch_stop();
	stop_watch_print();
	#endif
	return 0;
}

int app_cmd_erase(const char* cmd, char* params[], int param_size)
{
	#if STOP_WATCH_MODULE_ENABLE
	stop_watch_start();
	#endif
	drv_sst25_flash_erase_chip();
	while (drv_sst25_flash_is_busy()) {
		ssz_clear_watchdog();
	}
	#if STOP_WATCH_MODULE_ENABLE
	stop_watch_stop();
	stop_watch_print();
	#endif 
	printf("erase finish\n");
	return 0;
}
int app_cmd_read_nvram(const char* cmd, char* params[], int param_size)
{
	int nvram_size = data_nvram()->size;
	int start_address = 0;
	int end_address = nvram_size-1;

	for (int i = 0; i < param_size; i++) {
		if (i == 0)
		{
			start_address = ssz_str_hex_to_int(params[i]);
		}
		else if (i == 1) {
			end_address = start_address + atoi(params[i])-1;
		}
	}

	if (start_address<0||start_address>=nvram_size||
		start_address>end_address ||
		end_address>=nvram_size){
		app_cmd_output_error_info(kInvalidParam);
	}

	int once_read_size = 0;
	uint8_t buff[16];
	for (int i = start_address; i <= end_address; i += 16) {
		once_read_size = end_address - i + 1;
		if (once_read_size>sizeof(buff)) {
			once_read_size = sizeof(buff);
		}
		if (ssz_nvram_read_ex(data_nvram(), i, buff, once_read_size,0)==kSuccess) {
			printf("0x%06X:", i);
			for (int j = 0; j < once_read_size; j++) {
				printf(" %02X", buff[j]);
			}
			printf("\n");
		}
		else {
			printf("read fail!\n");
			break;
		}
		ssz_clear_watchdog();
	}
	printf("read finish\n");
	return 0;
}
int app_cmd_write_nvram(const char* cmd, char* params[], int param_size) {
	int start_address = 0;
	const char *write_content = NULL;
	for (int i = 0; i < param_size; i++) {
		if (i == 0)
		{
			start_address = ssz_str_hex_to_int(params[i]);
		}
		else if (i == 1) {
			write_content = params[i];
		}
	}

	if (start_address<0 || start_address >= data_nvram()->size||
		write_content ==NULL) {
		app_cmd_output_error_info(kInvalidParam);
	}
	if (ssz_nvram_write_ex(data_nvram(), start_address, write_content, strlen(write_content), 0) == kSuccess) {
		printf("write success\n");
		return 0;
	}
	else {
		printf("write fail\n");
		return -1;
	}
}

#if UI_MODULE_ENABLE
static int app_cmd_test_ui(const char* cmd, char* params[], int param_size)
{
	ui_set_canvas_rect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	printf("LCD all white\n");
	ui_set_fill_color(UI_WHITE);
#if STOP_WATCH_MODULE_ENABLE
	int stop_watch_index;
	stop_watch_start();
#endif
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_index = stop_watch_child_start("fill");
#endif
	ui_fill_rect_at(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_child_stop(stop_watch_index);
#endif
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_index = stop_watch_child_start("flush");
#endif
	display_flush_dirty();
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_child_stop(stop_watch_index);
#endif
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_stop();
	stop_watch_print();
#endif
	ssz_delay_ms_with_clear_watchdog(3000);

	printf("LCD all black\n");
	ui_set_fill_color(UI_BLACK);
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_start();
#endif
	ui_fill_rect_at(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	display_flush_dirty();
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_stop();
	stop_watch_print();
#endif
	ssz_delay_ms_with_clear_watchdog(3000);

	return 0;
}
#endif
#if UI_VIEW_MODULE_ENABLE
//show view info 
static int app_cmd_view_info(const char* cmd, char* params[], int param_size)
{
	ui_view_output_all_view_info();

    return 0;
}
//trace view
static int app_cmd_trace_view(const char* cmd, char* params[], int param_size)
{
	if (param_size>0 && atoi(params[0])==0) {
		ui_view_disable_trace();
	}
	else {
		ui_view_enable_trace();
	}
	ui_view_output_all_view_info();

	return 0;
}

//trace view draw
static int app_cmd_trace_view_draw(const char* cmd, char* params[], int param_size)
{
	if (param_size > 0 && atoi(params[0]) == 0) {
		ui_view_disable_trace_draw();
	}
	else {
		ui_view_enable_trace_draw();
	}
	ui_view_output_all_view_info();

	return 0;
}
#endif



//infusion amount of dose of specific meds
static int app_cmd_infusion_start(const char* cmd, char* params[], int param_size)
{   
	InfusionBagMeds bag_type = kBagAMeds;
	bool is_need_find_bag_dose = false;
	InfusionParam param;

	ssz_mem_zero(&param, sizeof(param));

	for (int i = 0; i < param_size; i++) {
		if ((strcmp(params[i], "bag-a") == 0)) {
			bag_type = kBagAMeds;
			is_need_find_bag_dose = true;
		}
		else if ((strcmp(params[i], "bag-b") == 0)) {
			bag_type = kBagBMeds;
			is_need_find_bag_dose = true;
		}
		else if (is_need_find_bag_dose) {
			if (bag_type == kBagAMeds) {
				param.total_dose = atoi(params[i]) * 1000;
			}
			else {
				param.meds_b_total_dose = atoi(params[i]) *1000;
			}
			is_need_find_bag_dose = false;
		}
		else {
			param.infusion_speed = atoi(params[i]) * 1000;
		}
	}

	if ((param.total_dose!=0 && param.infusion_speed>0)||(param.meds_b_total_dose!=0)) {
		app_delivery_start_with_param(kNormalInfusion,&param);
	}

    return 0;
}

static int app_cmd_infusion_pause(const char* cmd, char* params[], int param_size)
{
	InfusionType infuse_type = app_delivery_curr_running_infusion();
	if (infuse_type != kInfusionTypeMax) {
		app_delivery_pause(infuse_type);
	}

	return 0;
}

static int app_cmd_infusion_resume(const char* cmd, char* params[], int param_size)
{   
	InfusionType infuse_type = app_delivery_curr_running_infusion();
	if(infuse_type!=kInfusionTypeMax){
		app_delivery_resume(infuse_type);
	}

    return 0;
}
static int app_cmd_infusion_stop(const char* cmd, char* params[], int param_size)
{   
	InfusionType infuse_type = app_delivery_curr_running_infusion();
	if (infuse_type != kInfusionTypeMax) {
		app_delivery_stop(infuse_type);
	}

    return 0;
}

//infusion info 
static int app_cmd_infusion_info(const char* cmd, char* params[], int param_size)
{
	InfusionType infuse_type = app_delivery_curr_running_infusion();
	if (infuse_type == kInfusionTypeMax) {
		printf("not infusion\n");
		return 0;
	}
	printf("Infusion type:%d, state:%d\n", infuse_type, app_delivery_state(infuse_type));
	printf("meds-a total dose:%duL, speed:%duL/hr, infused dose:%duL, rest seconds:%d\n",
		app_delivery_info(infuse_type)->total_dose,
		app_delivery_info(infuse_type)->infusion_speed,
		app_delivery_infused_dose(infuse_type), 
		app_delivery_a_bag_rest_time_secs(infuse_type)
		);
	printf("meds-b total dose:%duL, infused dose:%duL, rest seconds:%d\n",
		app_delivery_info(infuse_type)->meds_b_total_dose,
		app_delivery_meds_b_infused_dose(infuse_type),
		app_delivery_b_bag_rest_time_secs(infuse_type));
	if (app_delivery_info(infuse_type)->is_peek_infusion_mode) {
		printf("peek_speed:%duL/hr, time:%dhr~%dhr\n",
			app_delivery_info(infuse_type)->peek_infusion_speed,
			app_delivery_info(infuse_type)->peek_infusion_start_time_point,
			app_delivery_info(infuse_type)->peek_infusion_end_time_point
		);
	}
	if (app_delivery_curr_bag(infuse_type) == kBagAMeds) {
		printf("curr meds type:meds-a\n");
	}
	else {
		printf("curr meds type:meds-b\n");
	}
	printf("curr speed: %duL/hr\n", app_delivery_curr_speed(infuse_type));
	if (!app_delivery_is_infusing_one_point(infuse_type)) {
		InfusionTime infuse_time = app_delivery_next_infuse_point_time();
		if (infuse_time.is_use_rtc) {
			SszDateTime tmp;
			ssz_seconds_to_time(infuse_time.rtc_seconds, &tmp);
			printf("next infusion point time: %02d:%02d:%02d.%03d\n", 
				tmp.hour, tmp.minute, tmp.second, infuse_time.rtc_millisecond);
		}
		else {
			printf("next infusion point tick time: %dms\n",
				infuse_time.tick_time);
		}
	}

	return 0;
}


static int app_cmd_motor_run(const char* cmd, char* params[], int param_size)
{
	bool is_infusion_motor = true;
	int encoder = -1;
	for (int i = 0; i < param_size; i++) {
		if ((strcmp(params[i], "three") == 0)) {
			//is_infusion_motor = false;
			printf("not support\n");
			return -1;
		}
		else {
			encoder = atoi(params[i]);
		}
	}

	if (is_infusion_motor) {
		infusion_motor_start(kForward, encoder, NULL);
	}
	else {
		 three_valve_motor_start(kForward, encoder, NULL);
	}



	return 0;
}
static int app_cmd_motor_reverse(const char* cmd, char* params[], int param_size)
{
	bool is_infusion_motor = true;
	int encoder = -1;
	for (int i = 0; i < param_size; i++) {
		if ((strcmp(params[i], "three") == 0)) {
			is_infusion_motor = false;
			printf("not support\n");
			return -1;			
		}
		else {
			encoder = atoi(params[i]);
		}
	}

	if (is_infusion_motor) {
		infusion_motor_start(kBackward, encoder, NULL);
	}
	else {
		 three_valve_motor_start(kBackward, encoder, NULL);
	}


	return 0;
}

static int app_cmd_motor_run_ex(const char* cmd, char* params[], int param_size)
{
	int encoder = -1;
	encoder = atoi(params[0]);

	infusion_motor_start_internal(kForward,-1,encoder,NULL);

	return 0;
}
static int app_cmd_motor_reverse_ex(const char* cmd, char* params[], int param_size)
{
	int encoder = -1;
	encoder = atoi(params[0]);
	
	infusion_motor_start_internal(kBackward,-1,encoder,NULL);

	return 0;
}

static int app_cmd_motor_stop(const char* cmd, char* params[], int param_size)
{
	bool is_infusion_motor = true;
	for (int i = 0; i < param_size; i++) {
		if ((strcmp(params[i], "three") == 0)) {
			is_infusion_motor = false;
		}
	}

	if (is_infusion_motor) {
		infusion_motor_stop(0);
	}
	else {
		three_valve_motor_stop(0);
	}

	return 0;
}

static int app_cmd_move_to(const char* cmd, char* params[], int param_size)
{
	if ((param_size > 0 && strcmp(params[0], "a")==0) ||
		param_size ==0 ){
		printf("move to position a, big bag\n");
		three_valve_motor_move_to_position_a();
	}
	else if (param_size>0 && strcmp(params[0],"b")==0) {
		printf("move to position b, small bag\n");
		three_valve_motor_move_to_position_b();
	}

	return 0;
}

void app_cmd_test_play_printf(void)
{	
	//ad = drv_isd2360_play_state();
	//ssz_traceln("test_play:%x", ad);
  ;
}

static int app_drv_isd2360_test_play(const char* cmd, char* params[], int param_size)
{   
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int type = atoi(params[0]);
	int data = atoi(params[1]);
	if (type == 1) {
		drv_isd2360_setvolume(20);
		ssz_delay_ms(300);
		drv_isd2360_play_voice_prompts(  VOPROMPT_HIGH_ALARM,ISD2360_CHANNELCONTROL_SPICMDCH0);
		printf("play_voice_prompts\n");
	}
	else if (type == 2) {
		drv_isd2360_cmd_powerup();
		printf("isd2360_cmd_powerup\n");
	}
	 else if (type == 3) {
	 	drv_isd2360_cmd_powerdown();
	 	printf("drv_isd2360_cmd_powerdown\n");
	 }
	 else if (type == 4) {
	 	drv_isd2360_exe_voice_macros(data, ISD2360_CHANNELCONTROL_SPICMDCH0);
	 	printf("play_VOMACRO %d\n", data);
	 }
	 else if (type == 5) {
	 	drv_isd2360_pcb_power_disable();
	 	printf("drv_isd2360_pcb_power_disable\n" );
	 }	 
	 else if (type == 6) {
	 	drv_isd2360_pcb_power_enable();
	 	printf("drv_isd2360_pcb_power_enable\n" );
	 }		 
	else if (type == 7) {
		drv_isd2360_cmd_powerup();
		printf("isd2360_cmd_powerup\n");		
		timer_set_handler(kTimerTestPlay, app_cmd_test_play_printf);
		timer_start_periodic_every(kTimerTestPlay,4);
		ssz_traceln("start test play");
	}
	return 0;
}
 

static int app_drv_isd2360_set_volume(const char* cmd, char* params[], int param_size)
{   
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int data = atoi(params[0]);
	drv_isd2360_setvolume(data);

	return 0;
}
 

static int app_cmd_io_on(const char* cmd, char* params[], int param_size)
{   
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int type = atoi(params[0]);

	if (type == 1){
		drv_all_motor_pwr_enable();
	 	printf("3.3V_MOTOR OPEN \n");
	}
	else if (type == 2){
		drv_oled_pwr_en();
		printf("16V0_OLED OPEN \n");
	}
	else if (type == 3){
		drv_oled_rest_dis();
		drv_oled_init();
		printf("OLED Reset disable\n");
		printf("app_main_menu_enter\n");	
		app_main_menu_enter();				
	}	
	else if (type == 4){
		drv_all_5v_pwr_enable();
		printf("5.0V OPEN \n");
	}	
	else if (type == 5){
		drv_oled_sleep_off();
		printf("oled sleep off\n");
	}		
	else if (type == 6){
		drv_three_valve_pos_detect_pwr_enable();
		printf("three valve pos detect pwr enable\n");
	}
	else if (type == 7){
		drv_three_valve_motor_pwr_enable(); 	
		printf("drv_three_valve_motor_pwr_enable\n");
	}	
	else if (type == 8){
		pill_box_install_sensor_right_pwr_enable(); 
		printf("pill_box_install_sensor_right_pwr_enable\n");
	}
	else if (type == 9){
		pill_box_install_sensor_left_pwr_enable();	
		printf("pill_box_install_sensor_left_pwr_enable\n");
	}	
	else if (type == 10){
		pressure_and_bubble_sensor_pwr_enable();	
		printf("pressure_and_bubble_sensor_pwr_enable\n");
	}
	else if (type == 11){
		drv_infusion_motor_sleep_disable();
		printf("drv_infusion_motor_sleep_disable\n");
	}	
	else if (type == 12){
		sys_power_wake_up_slaver_pin_high();	
		printf("wakeup high\n");
	}
	else if (type == 13){
		sys_master_mcu_power_enable();	
		printf("master mcu up power\n");
	}	
	else if (type == 14){
		drv_isd2360_pcb_power_enable();	
		printf("isd2360 pcb power on\n");
	}

	
	return 0;
}

static int app_cmd_io_off(const char* cmd, char* params[], int param_size)
{   
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int type = atoi(params[0]);

	if (type == 1){
		drv_all_motor_pwr_disable();
	 	printf("3.3V_MOTOR CLOSE \n");
	}
	else if (type == 2){
		drv_oled_pwr_dis();
		printf("16V0_OLED CLOSE \n");
	}
	else if (type == 3){
		drv_oled_rest_en();
		printf("OLED Reset enable\n");		
	}	
	else if (type == 4){
		drv_all_5v_pwr_disable();
		printf("5.0V CLOSE \n");
	}	
	else if (type == 5){
		drv_oled_sleep_on( );
		printf("oled_sleep_on \n");
	}	
	else if (type == 6){
		drv_three_valve_pos_detect_pwr_disable(); 
		printf("three valve pos detect pwr disable\n");	
	}	
	else if (type == 7){
		drv_three_valve_motor_pwr_disable();	
		printf("drv_three_valve_motor_pwr_disable\n");
	}	
	else if (type == 8){
		pill_box_install_sensor_right_pwr_disable();	
		printf("pill_box_install_sensor_right_pwr_disable\n");
	}
	else if (type == 9){
		pill_box_install_sensor_left_pwr_disable();		
		printf("pill_box_install_sensor_left_pwr_disable\n");
	}	
	else if (type == 10){
		pressure_and_bubble_sensor_pwr_disable();
		printf("pressure_and_bubble_sensor_pwr_disable\n");
	}	
	else if (type == 11){
		drv_infusion_motor_sleep_enable();
		printf("drv_infusion_motor_sleep_enable\n");
	}		
	else if (type == 12){
		sys_power_wake_up_slaver_pin_low();	
		printf("wakeup low\n");
	}
	else if (type == 13){
		sys_master_mcu_power_disable();	
		printf("master mcu no up power\n");
	}
	else if (type == 14){
		drv_isd2360_pcb_power_disable();	
		printf("isd2360 pcb power off\n");
	}	
	return 0;
}

static int app_cmd_led_on(const char* cmd, char* params[], int param_size)
{   
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int type = atoi(params[0]);


	if (type == 1){
		ssz_gpio_set(LED_R_GPIO_Port,LED_R_Pin);

		printf("LED_R OPEN \n");
	}
	else if (type == 2){
		ssz_gpio_set(LED_G_GPIO_Port,LED_G_Pin);
		printf("LED_G OPEN \n");
	}
	else if (type == 3){
		ssz_gpio_set(LED_Y_GPIO_Port,LED_Y_Pin);
		printf("LED_Y OPEN \n");
	}
	else if (type == 4){
		ssz_gpio_set(LED_R_GPIO_Port,LED_R_Pin);
		ssz_gpio_set(LED_G_GPIO_Port,LED_G_Pin);
		ssz_gpio_set(LED_Y_GPIO_Port,LED_Y_Pin);
		printf("ALL OPEN \n");
	}

	return 0;
}

//static int  app_cmd_set_150ul_bubble(const char* cmd, char* params[], int param_size)
//{
//	if (param_size < 1) {
//		app_cmd_output_error_info(kInvalidParam);
//		return kInvalidParam;
//	}
//
//	int type = atoi(params[0]);
//	//app_infusion_monitor_set_150ul_check_bubble_time(type);
//
//	printf("150ul bubble set times %d \n", type);
//
//	return 0;
//}
//static int  app_cmd_set_alarm_bubble(const char* cmd, char* params[], int param_size)
//{
//	if (param_size < 1) {
//		app_cmd_output_error_info(kInvalidParam);
//		return kInvalidParam;
//	}
//
//	int type = atoi(params[0]);
//	//app_infusion_monitor_set_alarm_check_bubble_time(type);
//
//	printf("bubble alarm set times %d \n", type);
//
//	return 0;
//}

static int app_cmd_led_off(const char* cmd, char* params[], int param_size)
{   
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int type = atoi(params[0]);

	if (type == 1){
		ssz_gpio_clear(LED_R_GPIO_Port,LED_R_Pin);
	 	printf("LED_R CLOSE \n");
	}
	else if (type == 2){
		ssz_gpio_clear(LED_G_GPIO_Port,LED_G_Pin);
		printf("LED_G CLOSE \n");
	}
	else if (type == 3){
		ssz_gpio_clear(LED_Y_GPIO_Port,LED_Y_Pin);
		printf("LED_Y CLOSE \n");
	}
	else if (type == 4){
		ssz_gpio_clear(LED_R_GPIO_Port,LED_R_Pin);
		ssz_gpio_clear(LED_G_GPIO_Port,LED_G_Pin);
		ssz_gpio_clear(LED_Y_GPIO_Port,LED_Y_Pin);
		printf("ALL CLOSE \n");
	}	

	return 0;
}

//turn vibrator on for 500ms then off
static int app_cmd_vibrator_play(const char* cmd, char* params[], int param_size)
{        
	vibrator_motor_start();
	ssz_delay_ms(300);
	vibrator_motor_stop();
	return 0;
}
//all pixels on
static int app_cmd_oled_on(const char* cmd, char* params[], int param_size)
{        
	drv_oled_display_all_screen();
	//display_open();
	return 0;
}

//all pixels off
static int app_cmd_oled_off(const char* cmd, char* params[], int param_size)
{        
	drv_oled_clear_all_screen();
	//display_close();
	return 0;
}

// app_cmd_oled_cmd
static int app_cmd_oled_cmd(const char* cmd, char* params[], int param_size)
{        
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int type = atoi(params[0]);

	drv_oled_set_contrast_current((uint8_t)type); // 
	printf("oled_set_contrast_current %d \n", type);

	return 0;
}

static int app_sst25_test(const char* cmd, char* params[], int param_size)
{  
	const uint8_t app_temp[ ] = "Flash test is OK";
	uint8_t data[sizeof(app_temp)] = { 0 };
	uint8_t temp = 0 ;	

	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int type = atoi(params[0]);

	if (type == 1){
		//id = drv_sst25_flash_read_jedec();
		printf("Flash ID[0x%x], JEDEC ID[0x%x]\n", drv_sst25_flash_read_id(), 
			drv_sst25_flash_read_jedec());		
	}
	else if (type == 2){
		drv_sst25_flash_erase_chip();
		drv_sst25_flash_write (100 ,(uint8_t*)app_temp , sizeof(app_temp) );	
		drv_sst25_flash_read(100 ,(uint8_t*)data , sizeof(app_temp));	
		for (temp = 0; temp < (sizeof(app_temp)-1); temp++){			
			printf("%c", data[temp]);
		}
		printf("\r\n");	// 每行显示16字节数据 
	}	
	return 0;
}

static int app_cmd_enter_factory_mode(const char* cmd, char* params[], int param_size) {
	enter_factory_mode();
	return 0;
}
static int app_cmd_enter_low_mode(const char* cmd, char* params[], int param_size) {
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	//stop monitors and scan
	stop_system_scan();
	app_mcu_monitor_stop();

	int type = atoi(params[0]);
 	printf("enter lowpower \n");	
	drv_lowpower_sleep_slaver();
	drv_lowpower_enter_lpsleep(type , false);
	
	drv_lowpower_exit_lpsleep();
	drv_lowpower_wakeup_slaver();
	printf("exit lowpower \n");	
	ssz_delay_ms(20);
	if(!watchdog_is_enable()){
		app_mcu_send_to_slave(COMM_SLAVER_MCU_STOP_DOG,0,0);
	}

	//wake up
	//start monitor and scan
	start_system_scan();
	//temp disable sleep
	set_sleep_allow(false);
	//enter sleep screen
	scr_sleep_enter();
	
	return 0;
}  

//static int app_cmd_exit_low_mode(const char* cmd, char* params[], int param_size) {
//	if (param_size < 1) {
//		app_cmd_output_error_info(kInvalidParam);
//		return kInvalidParam;
//	}
//
//	int type = atoi(params[0]);
//
//	drv_lowpower_exit_lpsleep( );
//	printf("exit lowpower \n");
//
//	return 0;
//} // app_cmd_exit_low_mode


#if MOTOR_ENABLE_CHECK_ENCODER_AFTER_STOP
static int app_cmd_encoder_after_stop(const char* cmd, char* params[], int param_size) {
	printf("encoder after stop: forward[%d],backward[%d]\n", 
		g_infusion_motor_forward_encoder_after_stop,
		g_infusion_motor_backward_encoder_after_stop);
	return 0;
}
#endif

static int app_cmd_test_com(const char* cmd, char* params[], int param_size) {
	 app_mcu_send_to_slave(COMM_BEEP_TEST,0,0);
 	return 0;
}
static int app_cmd_wake_up_slaver(const char* cmd, char* params[], int param_size) {
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

	int type = atoi(params[0]);

	if (type == 1){
		sys_power_wake_up_slaver();
	 	printf("wake up slaver \n");
	}
	else if (type == 2){
		sys_power_wake_up_slaver_pin_high();
	 	printf("sys_power_wake_up_slaver_pin_high \n");
	}
	else if (type == 3){
		sys_power_wake_up_slaver_pin_low();
	 	printf("sys_power_wake_up_slaver_pin_low \n");
	}	
		
 	return 0;
}

static int app_cmd_installed_pressure(const char* cmd, char* params[], int param_size) {
	printf("is ready:%d, installed pressure adc:%d\n", is_pressure_adc_ready_after_installed(), 
		pressure_adc_when_installed());
	return 0;
}

static int app_cmd_test_lifetime(const char* cmd, char* params[], int param_size) {
#if TEST_LIFETIME_ENABLE
	printf("it will run cycle to test lifetime\n");
	InfusionParam param = {300000,20000,15000};
	for (int i = 0; i < param_size; i++) {
		if (i == 0)
		{
			param.total_dose = atoi(params[i])*1000;
		}
		else if (i == 1) {
			param.meds_b_total_dose = atoi(params[i])*1000;
		}
		else if (i == 2) {
			param.infusion_speed = atoi(params[i]) * 1000;
		}
	}
    //data_write_int(kDataRunLifetimeCount, 0);
	g_is_testing_lifetime = true;
	app_delivery_start_with_param(kNormalInfusion, &param);
	g_test_lifetime_infusion_param = param;
	screen_go_back_to_home();
#else
	printf("not support\n");
#endif

	return 0;
}
// 
static int app_cmd_test_motor(const char* cmd, char* params[], int param_size)
{        
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}
#ifdef SSZ_TARGET_MACHINE
	int type = atoi(params[0]);
	if(type == 1){
		MX_TIM3_Init_ex(1);
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	}
	if(type!= 1){
		MX_TIM3_Init_ex(1);
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		TIM3->CCR1	= type ;
	}
#endif
	return 0;
}

static int app_cmd_test_motor_cmd(const char* cmd, char* params[], int param_size)
{        
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}
#ifdef SSZ_TARGET_MACHINE
	int type = atoi(params[0]);
	int data = atoi(params[1]);
	switch(type)
	{
		case 1:
			g_infusion_motor_run_start_pwm_level = data ;			
			printf("infusion motor start pwm = %d \n", data);
			break;
		case 2:
			g_infusion_motor_pwm_step_num = data;
			printf("infusion motor pwm step num = %d \n", data);
			break;
		case 3:
			g_three_valve_motor_run_start_pwm_level = data;
			printf("three valve motor run start pwm = %d \n", data);
			break;
		case 4:
			g_three_valve_motor_pwm_step_num = data ;
			printf("three valve motor pwm step num = %d \n", data);
			break;	
		case 5:
			MX_TIM3_Init_ex(data);
			HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
			TIM3->CCR1  = 0 ;			
			printf("infusion motor pwm init \n");
			break;	
		case 6:
			TIM3->CCR1  = data ;			
			printf("infusion motor pwm duty cycle = %d \n", data);
			break;			
		default:			
			break;
	}
#endif
	return 0;
}
#if LOW_POWER_TEST_ENABLE
static int app_cmd_low_power_vioce_test(const char* cmd, char* params[], int param_size)
{        
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}
#ifdef SSZ_TARGET_MACHINE
	int type = atoi(params[0]);
	int data = atoi(params[1]);
	int data1 = atoi(params[2]);

 	printf("enter lowpower \n");	
 	drv_lowpower_sleep_slaver();
 	drv_lowpower_enter_lpsleep(99999, false);
	
 	drv_lowpower_exit_lpsleep();
  	drv_lowpower_wakeup_slaver();
	printf("exit lowpower \n");		

	drv_all_motor_pwr_disable();
	drv_all_5v_pwr_disable();
	drv_oled_pwr_dis();
	drv_three_valve_pos_detect_pwr_disable(); 
	drv_three_valve_motor_pwr_disable();
	pill_box_install_sensor_right_pwr_disable();	
	pill_box_install_sensor_left_pwr_disable(); 
	drv_infusion_motor_sleep_enable();
	drv_isd2360_pcb_power_enable();

	if (data1 == 1) {
	   drv_isd2360_pcb_power_disable();
	   printf("drv_isd2360_pcb_power_disable\n" );
	}	
	else if (data1 == 2) {
	   drv_isd2360_pcb_power_enable();
	   printf("drv_isd2360_pcb_power_enable\n" );
	}
	else if (data1 == 3) {
		drv_isd2360_pcb_power_disable();
		printf("drv_isd2360_pcb_power_disable\n" );		
		drv_isd2360_cmd_powerdown();
		printf("drv_isd2360_cmd_powerdown\n" );
	}
	else if (data1 == 4) {
		drv_isd2360_pcb_power_enable();
		printf("drv_isd2360_pcb_power_enable\n" );	
		drv_isd2360_cmd_powerdown();
		printf("drv_isd2360_cmd_powerdown\n" );
	}

	GPIO_InitTypeDef GPIO_InitStruct;
	if( 1== type ){ // GPIO_MODE_OUTPUT_PP
	    GPIO_InitStruct.Pin = VOICE_SPI1_SSB_Pin|VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		

		GPIO_InitStruct.Pin =  VOICE_SPI1_RDY_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin =  VOICE_INTB_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		printf("isd2360 all io for GPIO_MODE_OUTPUT_PP\n");		
	} 
	else if( 2 == type ){ // GPIO_MODE_OUTPUT_OD
	    GPIO_InitStruct.Pin = VOICE_SPI1_SSB_Pin|VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;				
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		

		GPIO_InitStruct.Pin =  VOICE_SPI1_RDY_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin =  VOICE_INTB_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		printf("isd2360 all io for GPIO_MODE_OUTPUT_OD\n");		
	} 
	else if( 3 == type ){ // GPIO_MODE_AF_PP
		GPIO_InitStruct.Pin = VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;				
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		
		
		GPIO_InitStruct.Pin =  VOICE_SPI1_SSB_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin =  VOICE_SPI1_RDY_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin =  VOICE_INTB_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		printf("isd2360 spi pin for GPIO_MODE_AF_PP,other GPIO_MODE_OUTPUT_PP\n");	
	} 
	else if( 4 == type ){ // GPIO_MODE_AF_OD
		GPIO_InitStruct.Pin = VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;				
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
		
		GPIO_InitStruct.Pin =  VOICE_SPI1_SSB_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin =  VOICE_SPI1_RDY_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin =  VOICE_INTB_Pin;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		printf("isd2360 spi pin for GPIO_MODE_AF_OD,other GPIO_MODE_OUTPUT_PP\n");		
	} 
	else if( 5 == type ){ // GPIO_MODE_INPUT
	    GPIO_InitStruct.Pin = VOICE_SPI1_SSB_Pin|VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;	
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		

		GPIO_InitStruct.Pin =  VOICE_SPI1_RDY_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;	
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin =  VOICE_INTB_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;	
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		printf("isd2360 all io for GPIO_MODE_INPUT\n");	  
	} 
	else if( 6 == type ){ // GPIO_MODE_ANALOG
		GPIO_InitStruct.Pin = VOICE_SPI1_SSB_Pin|VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 	
		
		GPIO_InitStruct.Pin =  VOICE_SPI1_RDY_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		
		GPIO_InitStruct.Pin =  VOICE_INTB_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		printf("isd2360 all io for GPIO_MODE_ANALOG\n"); 
	}

	if( 1== data ){ // high
		ssz_gpio_set(GPIOE, VOICE_INTB_Pin);
		ssz_gpio_set(GPIOC, VOICE_SPI1_RDY_Pin);
		ssz_gpio_set(GPIOA,VOICE_SPI1_SSB_Pin|VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin);
		printf("isd2360 all io for high\n"); 
	}
	else if( 2 == data ){ // low
		ssz_gpio_clear(GPIOE, VOICE_INTB_Pin);
		ssz_gpio_clear(GPIOC, VOICE_SPI1_RDY_Pin);
		ssz_gpio_clear(GPIOA,VOICE_SPI1_SSB_Pin|VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin);
		printf("isd2360 all io for low\n"); 
	}
	
#endif

ssz_delay_ms(6000); 	  // 空闲

	printf("----test end-----\n" );

	return 0;
}
static int app_cmd_low_power_test(const char* cmd, char* params[], int param_size)
{    
	uint32_t PIN_NUM = 0;
	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}
#ifdef SSZ_TARGET_MACHINE
	int type = atoi(params[0]);
	int data1 = atoi(params[1]);
	int data2 = atoi(params[2]);
	int data3 = atoi(params[3]);

 	printf("enter lowpower \n");	
 	drv_lowpower_sleep_slaver();
 	drv_lowpower_enter_lpsleep(99999, true);
	
 	drv_lowpower_exit_lpsleep();
	printf("exit lowpower \n");	
	
	drv_all_motor_pwr_disable();
	drv_all_5v_pwr_disable();
	drv_oled_pwr_dis();
	drv_three_valve_pos_detect_pwr_disable(); 
	drv_three_valve_motor_pwr_disable();
	pill_box_install_sensor_right_pwr_disable();	
	pill_box_install_sensor_left_pwr_disable(); 
	drv_infusion_motor_sleep_enable();

	GPIO_InitTypeDef GPIO_InitStruct;
	if(1 == type ){ // pin
		PIN_NUM = VOICE_SPI1_SSB_Pin ;
	}else if(2 == type ){
		PIN_NUM = VOICE_SPI1_SCK_Pin ;
	}else if(3 == type ){
		PIN_NUM = VOICE_SPI1_MISO_Pin ;
	}else if(4 == type ){
		PIN_NUM = VOICE_SPI1_MOSI_Pin ;
	}else if(5 == type ){
		PIN_NUM = VOICE_SPI1_RDY_Pin ;
	}else if(6 == type ){
		PIN_NUM = VOICE_INTB_Pin ;
	}
	
	if(1 == data1 ){ // Mode
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP ;
	}else if(2 == data1 ){
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD ;
	}else if(3 == data1 ){
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP ;
	}else if(4 == data1 ){
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD ;
	}else if(5 == data1 ){
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT ;
	}else if(6 == data1 ){
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG ;
	}

	if(1 == data2 ){ // Pull Speed
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	}else if(2 == data2 ){
		GPIO_InitStruct.Pull = GPIO_PULLUP; 
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	}else if(3 == data2 ){
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	}else if(4 == data2 ){
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;	
	}else if(5 == data2 ){
	    GPIO_InitStruct.Pull = GPIO_PULLUP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;	
	}else if(6 == data2 ){
	    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;	
	}else if(7 == data2 ){
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL ;
	}else if(8 == data2 ){
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}else if(9 == data2 ){
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}else if(10 == data2 ){
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	}else if(11 == data2 ){
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}else if(12 == data2 ){
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}else if(13 == data2 ){
		GPIO_InitStruct.Mode = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
	}else if(14 == data2 ){
		GPIO_InitStruct.Mode = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
	}else if(15 == data2 ){
		GPIO_InitStruct.Mode = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	}

	if(type<=4){
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		if(1 == data3 ){ // pin
			HAL_GPIO_WritePin(GPIOA, PIN_NUM, GPIO_PIN_SET);
		}else if(2 == data3 ){
			HAL_GPIO_WritePin(GPIOA, PIN_NUM, GPIO_PIN_RESET);
		}
	}else if(type==5){
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		if(1 == data3 ){ // pin
			HAL_GPIO_WritePin(GPIOC, PIN_NUM, GPIO_PIN_SET);
		}else if(2 == data3 ){
			HAL_GPIO_WritePin(GPIOC, PIN_NUM, GPIO_PIN_RESET);
		}		
	}else if(type==6){
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		if(1 == data3 ){ // pin
			HAL_GPIO_WritePin(GPIOE, PIN_NUM, GPIO_PIN_SET);
		}else if(2 == data3 ){
			HAL_GPIO_WritePin(GPIOE, PIN_NUM, GPIO_PIN_RESET);
		}		
	}	
#endif
	ssz_delay_ms(9000);	  
	return 0;
}

#endif
int app_cmd_reset_slaver(const char* cmd, char* params[], int param_size)
{
	ssz_gpio_clear(SLAVER_NRST_GPIO_Port,SLAVER_NRST_Pin);
	ssz_delay_ms(2);
	ssz_gpio_set(SLAVER_NRST_GPIO_Port,SLAVER_NRST_Pin);
 	return 0;
}

int app_cmd_write_sn(const char* cmd, char* params[], int param_size)
{
	if (param_size<0) {
		app_cmd_output_error_info(kInvalidParam);
		return -1;
	}
	if (data_write(kDataSerialNumber, params[0], strlen(params[0])) == kSuccess) {
		printf("write success\n");
		return 0;
	}
	else {
		printf("write fail\n");
		return -1;
	}
}

int app_cmd_clear_alarm(const char* cmd, char* params[], int param_size)
{
	int i;
	for (i = kRemindAlarmStart; i < kHighestAlarmEnd; i++){
		alarm_clear((AlarmID)(i));
	}
 	return 0;
}

int app_cmd_set_no_operate_time(const char* cmd, char* params[], int param_size)
{
	int time_ms = 30000;

	if (param_size < 1) {
		app_cmd_output_error_info(kInvalidParam);
		return kInvalidParam;
	}

    time_ms = atoi(params[0]) * 1000;
    app_system_monitor_set_no_operate_time(time_ms);

    return 0;
}

int app_cmd_disable_sleep(const char* cmd, char* params[], int param_size)
{
    g_is_sleep_allowed = false;
    printf("sleep disabled\n");
    return 0;
}

int app_cmd_enable_sleep(const char* cmd, char* params[], int param_size)
{
    g_is_sleep_allowed = true;
    printf("sleep enabled\n");
    return 0;
}

//all cmd handlers
const static AppCmdInfo g_app_cmd_info[] =
{
	{"help", app_cmd_help,"show cmd info"},
	{"exit", app_cmd_exit,"exit cmd mode"},
	//{"disable", app_cmd_disable,"disable cmd"},
	{"repeat", app_cmd_repeat, "repeat last cmd each time, e.g. repeat [time_ms], repeat 2000" },
#ifdef TEST
	{"test", app_cmd_test, "run test case, inputs \'test -h\' to get help "},
#endif
	{"enable_output", app_cmd_enable_output, "enable module's output. e.g. enable_output [module_name], enable_output all"
	 "\n\t\t module_name: common, infusion_monitor, motor\n\t\t motor_monitor, screen"},
	{"disable_output", app_cmd_disable_output, "disable module's output. e.g. disable_output [module_id], disable_output all" },
	{"module", app_cmd_module, "show all module output state" },
	{"test_watchdog", app_cmd_test_watchdog},
#if STOP_WATCH_MODULE_ENABLE
	{"stop_watch", app_cmd_stop_watch_set,"enable/disable stopwatch, param:0->disable,other->enable"},
#endif
	//output version
	{"version", app_cmd_version, "show system version" },
#if CPU_USAGE_MODULE_ENABLE
	{"cpu_usage", app_cmd_cpu_usage, "output cpu usage" },
#endif
	{"io", app_cmd_io, "show all input GPIO value"},
	{"ad", app_cmd_ad, "show all ad value" },
#if DATA_MODULE_ENABLE
	{"data", app_cmd_data, "show saved data" },
	{"data_change", app_cmd_change_data, "data_change x y , if x=1,encoder per mL,\n\t\t if x=2, occlusion slope,\n\t\t if x=3, occlusion one drop ADC,\n\t\t if x=4, battery offset"},
	{"data_map", app_cmd_data_map, "show data map" },
	{"clear_data", app_cmd_clear_data,
	 "clear data, e.g. clear_data [data_type] \n\t\t data_type: 1->user data, 2->factory data, 3->FCT data" },
#endif
#if RECORD_LOG_MODULE_ENABLE
	{"log", app_cmd_log,
	 "read log, e.g. log [log_type] [log_size], log_type:1->operation, 2->history" },
	{"filter_log_read", app_cmd_filter_log,
	 "e.g. filter_log [log_type] [event_type] [log_size]" },
#endif
#if SCREEN_MODULE_ENABLE
	{"screen_id", app_cmd_screen_id, "show current screen_id" },
#endif

	//operate
#if RTC_MODULE_ENABLE
	{"time", app_cmd_time, "show current time" },
	{"set_time", app_cmd_set_time,"e.g. set_time 1:15:30" },
	{"set_date", app_cmd_set_date,"e.g. set_date 2017-10-20" },
	{"rtc_run", app_cmd_start_rtc,"start rtc" },
	{"rtc_stop", app_cmd_stop_rtc,"stop rtc" },
#endif
#if RECORD_LOG_MODULE_ENABLE
	{"clear_log", app_cmd_clear_log,
	 "e.g. clear_log [log_type], 1->operation, 2->history" },
	{"test_log", app_cmd_test_log, "e.g. test_log [log_type] [log_size], test write some log "},
#endif
#if DATA_MODULE_ENABLE
	{"erase", app_cmd_erase, "erase nvram" },
	{"read_nvram", app_cmd_read_nvram, "Usage: [hex_address] [read_size]"},
	{"write_nvram", app_cmd_write_nvram, "Usage: [hex_address] [str]" },
#endif
#if UI_MODULE_ENABLE
	{"test_ui", app_cmd_test_ui, "test ui screen" },
#endif
#if SCREEN_MODULE_ENABLE
	{"show_screen", app_cmd_show_screen, "e.g. show_screen screen_id" },
#endif
#if UI_VIEW_MODULE_ENABLE
	{"view_info", app_cmd_view_info, "show ui view info" },
	{"trace_view", app_cmd_trace_view, "trace ui view " },
	{"trace_view_draw", app_cmd_trace_view_draw, "trace ui view draw"},
#endif

	{"infusion_start", app_cmd_infusion_start, "start infuse, Usage: infusion_start speed [bag-a|bag-b] [volume],\n\t\t e.g.: infusion_start 10 bag-a 20"},
	{"infusion_pause", app_cmd_infusion_pause, "pause infuse"},
	{"infusion_resume", app_cmd_infusion_resume, "resume infuse"},
	{"infusion_stop", app_cmd_infusion_stop, "stop infuse"},
	{"infusion_info", app_cmd_infusion_info, "print current infusion info(status)"},

	{"motor_run", app_cmd_motor_run, "Usage: [encoder], e.g.: motor_run, motor_run 50" },
	{"motor_reverse", app_cmd_motor_reverse, "Usage: [encoder]" },
	{"motor_run_ex", app_cmd_motor_run_ex, "run by optical coupler encoder, Usage: [optical_coupler_encoder]" },
	{"motor_reverse_ex", app_cmd_motor_reverse_ex, "motor reverse by optical coupler encoder" },	
	{"motor_stop", app_cmd_motor_stop, "Usage:motor_stop [three]" },
	{"move_to", app_cmd_move_to, "move valve to position a or b, Usage: move_to [a|b]"},	
	
    {"play", app_drv_isd2360_test_play, "play control,play x. \n\t\t x:1->play alarm high once, 2->isd2360 powerup, 3->isd2360 powerdown"
     "\n\t\t 4->drv_isd2360_pcb_power_disable, 5->drv_isd2360_pcb_power_enable"},
    {"set_volume", app_drv_isd2360_set_volume, "set_volume 0-255"},
    {"flash", app_sst25_test, "flash control,flash x. \n\t\t x:1->read flash id, 2->2 output Flash test is OK"},
	{"vibrator", app_cmd_vibrator_play, "turn vibrator on for 300ms then off" },

    {"oled_on", app_cmd_oled_on, "turn all pixels on"},
    {"oled_off", app_cmd_oled_off, "turn all pixels off"},
    {"oled_cmd", app_cmd_oled_cmd, "oled_cmd x. \n\t\t x:1-> x 0--255 oled_set_contrast_current ,"},
    //{"set_150ul_bubble", app_cmd_set_150ul_bubble, "set_150ul_bubble x. \n\tx:150ul bubble set times x , x size 0-255"},
    //{"set_alarm_bubble", app_cmd_set_alarm_bubble, "set_alarm_bubble x. \n\tx:bubble alarm set times x , x size 0-255"},
    {"led_on", app_cmd_led_on, "led_on x. \n\t\t x:1->open LED_R, 2->open LED_G,\n\t\t 3->open LED_Y, 4->open ALL"},
	{"led_off", app_cmd_led_off,"led_off x. \n\t\t x:1->close LED_R, 2->close LED_G,\n\t\t 3->close LED_Y, 4->close ALL" },
	{"enter_factory_mode", app_cmd_enter_factory_mode,"enter factory mode, not sleep"},
	{"enter_low_mode", app_cmd_enter_low_mode,"enter_low_mode x. \n\t\t x:1000-> enter lowpower,auto quit after 1 second"},
	{"no_operate_time", app_cmd_set_no_operate_time,"no operate time before sleep in second"},
	{"disable_sleep", app_cmd_disable_sleep,"sleep disabled"},
	{"enable_sleep", app_cmd_enable_sleep,"sleep enabled"},
    {"io_on", app_cmd_io_on,"io_on x. \n\t\t x:1->open 3.3V_MOTOR, 2->open 16V0_OLED, \n\t\t 3->OLED init to main menu, 4->5.0V OPEN,"
	 "\n\t\t 5->oled sleep off, 6->open three valve pos detect pwr,\n\t\t 7->open three valve motor pwr," 
	 "\n\t\t 8->open pill box install sensor right pwr,\n\t\t 9->open pill box install sensor lift pwr,"
	 "\n\t\t 10->open pressure and bubble sensor pwr,\n\t\t 11->infusion_motor_sleep_disable,"
	 "\n\t\t 12->wakeup_slaver_pin high,\n\t\t 13->master mcu up power,\n\t\t 14->isd2360 pcb power on"},
	{"io_off", app_cmd_io_off,"io_off x. \n\t\t x:1->close 3.3V_MOTOR , 2->close 16V0_OLED,\n\t\t 3->OLED Reset enable, 4->5.0V close,"
	 "\n\t\t 5->oled sleep, 6->close three valve pos detect pwr,\n\t\t 7->close three valve motor pwr,"
	 "\n\t\t 8->close pill box install sensor right pwr,\n\t\t 9->close pill box install sensor lift pwr,"
	 "\n\t\t 10->close pressure and bubble sensor,\n\t\t 11->infusion_motor_sleep_enable,\n\t\t 12->wakeup_slaver_pin low,"
	 "\n\t\t 13->master mcu no up power, 14->isd2360 pcb power off"},	
	{"test_com", app_cmd_test_com, "test communication with slave MCU" },
#if MOTOR_ENABLE_CHECK_ENCODER_AFTER_STOP
	{"encoder_after_stop", app_cmd_encoder_after_stop,"output encoder after motor stop"},
#endif
	{"installed_pressure", app_cmd_installed_pressure,"installed pressure" },
	{"test_lifetime", app_cmd_test_lifetime,"test_lifetime [big_bag_volume] [small_bag_volume] [speed]"},
	{"wake_up_slaver", app_cmd_wake_up_slaver,"wake_up_slaver x. \n\t\t x:1->wake up slaver once, 2->wake up pin high, 3->wake up pin low" },
    {"test_motor", app_cmd_test_motor, "test_motor x. \n\t\t x:1-> TIM3 INIT, x->2--500 PWM,"},
    {"test_motor_cmd", app_cmd_test_motor_cmd, "test_motor_cmd x y; x-->for test item, y-->for param,"
     "\n\t\t x->1 infusion start pwm, x->2 infusion pwm step num, \n\t\t x->3 three valve motor start pwm,"
     "\n\t\t x->4 three valve motor pwm step num, \n\t\t x->5 infusion motor fre, \n\t\t x->6 infusion motor pwm duty cycle"},
	{"reset_slaver", app_cmd_reset_slaver,"" },
	{"set_sn", app_cmd_write_sn,"set Serial Number, Usage: <SerialNumber>" },
	{"clear_alarm", app_cmd_clear_alarm,"clear all alarms" },
#if LOW_POWER_TEST_ENABLE
	{"low_power_vioce_test", app_cmd_low_power_vioce_test,
	 "low_power_vioce_test x y z; x->io mode, y->io high or low,z-> voice power on or off"
     "\n\t\t\t x->1 gpio_mode_output_pp, x->2 gpio_mode_output_od, x->3 gpio_mode_af_pp,"
     "\n\t\t\t x->4 gpio_mode_af_od, x->5 gpio_mode_input, x->6 gpio_mode_analog"
     "\n\t\t\t y->1 out high, x->2 out low,"
     "\n\t\t\t z->1 power off, z->2 power on,"},

	{"low_power_test", app_cmd_low_power_test,"low_power_test x y z m; x->io pin, y->Mode, z->Pull Speed,"
     "m->high low"
     "\n\t\t\t x->1 SSB_Pin, x->2 SCK_Pin, x->3 MISO_Pin,\n\t\t\t x->4 MOSI_Pin,x->5 RDY_Pin, x->6 INTB_Pin,"
     "\n\t\t\t y->1 GPIO_MODE_OUTPUT_PP, y->2 GPIO_MODE_OUTPUT_OD, y->3 GPIO_MODE_AF_PP,"
     "\n\t\t\t y->4 GPIO_MODE_AF_OD, y->5 GPIO_MODE_INPUT, y->6 GPIO_MODE_ANALOG,"
     "\n\t\t\t z->1 SPEED_LOW Null, z->2 SPEED_LOW up, z->3 SPEED_LOW down,"
     "\n\t\t\t z->4 SPI1 Null, z->5 SPI1 up, z->6 SPI1 down"
	 "\n\t\t\t z->7 INPUT Null, z->8 INPUT up, z->9 INPUT down"
	 "\n\t\t\t z->10 ANALOG Null, z->11 ANALOG up, z->12 ANALOG down"
     "\n\t\t\t m->1 out high, m->2 out low,"},
#endif	
	//below must at end
    {"", NULL}   
};

/***********************************************
* Description:
*   parse cmd str
* Argument:
*   cmd:
*
* Return:
*
************************************************/
static void app_cmd_parse_from_str(const char* cmd)
{      
   
    if (strcmp(cmd, CMD_ENABLE_PASSWORD)==0)
    {
        printf("cmd enable, input help to get avalid cmd\n");
        g_app_cmd_is_enable_cmd = true;
        g_app_cmd_is_enable_show_input = false;
        return;
    }
    else if (strcmp(cmd, CMD_ENABLE_PASSWORD "_for_AI")==0)
    {
        printf("cmd with format enable, input help to get avalid cmd\n");
        g_app_cmd_is_enable_cmd = true;
        g_app_cmd_is_enable_show_input = false;
        app_cmd_send_return_flag(0);
        return;
    }
    else if(g_app_cmd_is_enable_cmd)
    {
        int ret = -1;
        char *params[10];
        int param_size = 0;
        const AppCmdInfo* p;
        char *work_str;

        //find the ' '
        work_str = (char*)strchr(cmd, ' ');
        if(work_str != NULL)
        {
            *work_str = 0;
            work_str++;
        }
        param_size = ssz_str_split(work_str, " ",params, 10);

        p = g_app_cmd_info_ptr;

        //find the cmd handler
        while(p->command[0] != 0)
        {
            if (strcmp(p->command, cmd)==0)
            {
                ret = p->handler(cmd, params, param_size);
                break;
            }
            p++;
        }


        //if not find cmd handler, return -1
        if (p->command[0] == 0)
        {
            ret = -1;
            if(g_app_cmd_is_enable_cmd && g_app_cmd_is_enable_show_input)
            {
                puts("invalid cmd");
            }
        }

        if (!g_app_cmd_is_enable_show_input)
        {
            //if have format, send the end flag
            app_cmd_send_return_flag(ret);
        }
    }
}



/***********************************************
* Description:
*   handle special char like backspace,delete
* Argument:
*   ch:
*
* Return:
*   true:it is handled,  false:it is not handled
************************************************/
static bool app_cmd_handle_special_char(uint8_t ch)
{
    bool ret = false;

    if (ch == 0x8)//backspace key
    {        
        ret = true;
        if (g_app_cmd_receive_index>0)
        {
            g_app_cmd_receive_index--;
        }
        if(g_app_cmd_is_enable_cmd && g_app_cmd_is_enable_show_input)
        {
            putchar(0x8);
        }
    }
    else if(ch == 0x1B)//ESC key
    {
        ret = true;
        g_app_cmd_receive_index = 0;
        if(g_app_cmd_is_enable_cmd && g_app_cmd_is_enable_show_input)
        {
            putchar('\n');
        }
    }
    else if(ch == '\r')//return key
    {
        ret = true;
    }
	else if (ch == '\n')//return key
	{
		ret = true;
	}
    return ret;
}


/***********************************************
* Description:
*   handle char received from one com
* Argument:
*   ch:
*   is_handled_ok_at_pre_handler:
*
* Return:
*
************************************************/
static bool app_cmd_on_receive_byte(uint8_t ch, bool is_handled_ok_at_pre_handler)
{
    //if previous handler handle it OK, reset the index
    if (is_handled_ok_at_pre_handler)
    {
        g_app_cmd_receive_index = 0;
    }
	else {
		if (g_app_cmd_wait_line_end && ch == '\n') {
			g_app_cmd_wait_line_end = false;
			return true;
		}

		g_app_cmd_wait_line_end = false;
		//stop last cmd repeat run
		timer_stop(kTimerRepeatCmd);

		if (ch == '\n' || ch == '\r') //cmd end char
		{
			if (ch == '\r') {
				g_app_cmd_wait_line_end = true;
			}
			if (g_app_cmd_is_enable_cmd && g_app_cmd_is_enable_show_input)
			{
				printf("\n");
			}
			g_app_cmd_receive_str[g_app_cmd_receive_index] = 0;
			if (strncmp(g_app_cmd_receive_str, "repeat", 6) != 0) {
				strcpy(g_app_cmd_last_receive_str, g_app_cmd_receive_str);
			}
			//parse the cmd
			if (strlen(g_app_cmd_receive_str) > 0)
			{
				app_cmd_parse_from_str(g_app_cmd_receive_str);
			}
			//reset the index
			g_app_cmd_receive_index = 0;
		}
		else if (app_cmd_handle_special_char(ch))//is special char
		{
			//handle special char ok
		}
		else
		{
			if (g_app_cmd_is_enable_cmd && g_app_cmd_is_enable_show_input)
			{
				putchar(ch);
			}

			//receive cmd srt
			g_app_cmd_receive_str[g_app_cmd_receive_index] = ch;
			g_app_cmd_receive_index++;
			if (g_app_cmd_receive_index >= CMD_MAX_SIZE_OF_RECEIVE_STR - 1)
			{
				if (g_app_cmd_is_enable_cmd && g_app_cmd_is_enable_show_input)
				{
					printf("cmd too long\n");
				}
				g_app_cmd_receive_index = 0;
			}
		}
	}
    return true;
}


//enable cmd receive
void app_cmd_enable_receive(void)
{
	//handle the receive char
    com_set_receive_handler(CMD_ID, CMD_HANDLE_ORDER, app_cmd_on_receive_byte);
}
//disable cmd receive
void app_cmd_disable_receive(void)
{
	com_set_receive_handler(CMD_ID, CMD_HANDLE_ORDER, NULL);	
}

//enable cmd respond
void app_cmd_enable_respond(void)
{
	g_app_cmd_is_enable_cmd = true;
}
//disable cmd respond except the password
void app_cmd_disable_respond(void)
{
	g_app_cmd_is_enable_cmd = false;
}


/***********************************************
* Description:
*
* Argument:
*
* Return:
*
************************************************/
void app_cmd_init( void )
{
    g_app_cmd_info_ptr = g_app_cmd_info;
    timer_set_handler(kTimerRepeatCmd, app_cmd_repeat_exe_cmd);

	app_cmd_enable_receive();

#ifdef DEBUG	
    g_app_cmd_is_enable_cmd = true;
    g_app_cmd_is_enable_show_input = true;
#endif
}
