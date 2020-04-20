/************************************************
* DESCRIPTION:
*	define some common param
*
* REVISION HISTORY:
*   Rev 1.0 2017-11-10 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "sys_power.h"
#include "alarm.h"
#include "record_log.h"
/************************************************
* Declaration
************************************************/
//自动排气需要排空的两种药的量, 单位: uL
#define AUTO_CLEAR_AIR_MEDS_A_TOTAL_DOSE_uL 3000 
#define AUTO_CLEAR_AIR_MEDS_B_TOTAL_DOSE_uL 500 

//首次量的速度
//#define FIRST_INFUSION_SPEED        50000   //  unit: uL/h

//输液最大速度
#define MAX_INFUSION_SPEED   200000 //  unit: uL/h
#define MIN_INFUSION_SPEED   1000 //  unit: uL/h

#define FLUSH_SPEED   300 //  unit: mL/h

//max infusion duration
#define MAX_INFUSION_DURATION   5000 //  unit: 0.1h

//B袋输液速度
#define BAG_B_MEDS_INFUSION_SPEED (MAX_INFUSION_SPEED*2)

//总量设置限制
#define MAX_TOTAL_DOSE_SET 300
#define MIN_TOTAL_DOSE_SET 0

#define MAX_CLEAN_TUBE_DOSE_SET 200
#define MIN_CLEAN_TUBE_DOSE_SET 25
#define DEFAULT_CLEAN_TUBE_DOSE_MAX_SET 25

#define MAX_PRESSURE_THRESHOLD 150
#define MIN_PRESSURE_THRESHOLD 60
#define DEFAULT_PRESSURE_THRESHOLD 60

#define MAX_PRECISION_FACTOR 150   // UNIT 0.01
#define MIN_PRECISION_FACTOR 50
#define DEFAULT_PRECISION_FACTOR 100


//输注速度码盘和输注电机码盘减速比是1:62.37,可以圧两次, 
//我们只需要压一次,挤出一滴即可, 输注电机码盘一圈1个码盘,速度码盘一圈12个码盘,
//所以一滴需要输注电机码盘31.185个, 速度码盘6个
#define SPEED_ENCODER_TO_INFUSION_ENCODER_REDUCTION_RATIO 31.185 
#define ONE_DROP_OPTICAL_COUPLER_ENCODER	6 //it will run about 400ms, and it just press once and ouput one drop water
#define INFUSION_MOTOR_SPEED_ENCODER	    81 //infusion motor speed: 81 encoder per second (calculate according to hall-a)

//输液管路容量,最后输注B药袋时需要多输这么多,因为最后输液管路中的无法输到人体, unit:ul
#define INFUSION_TUBE_VOLUME_UL 0

//关机开机按键时间
#define KEY_HOLD_TIME_MS_TO_POWER_UP 1000
#define KEY_HOLD_TIME_MS_TO_POWER_OFF 2000

//按键粘连时间
#define KEY_STUCK_TIME_MS 30000

#define REMIND_SHOW_TIME_MS 5000

//无操作时间
#define NO_OPERATE_TIME_MS 30000
#define NO_OPERATE_TOO_LONG_MS_WHEN_INFUSION_PAUSE (3*60*1000)

//报警暂停时间
#define ALARM_PAUSE_TIME_MS (2 * 60 * 1000)

//等待另一个MCU启动
#define WAIT_OPPOSITE_MCU_POWER_ON_TIME_MS 2000

//耗尽报警后自动关机时间
#define AUTO_POWER_OFF_TIME_MS_AFTER_BATTERY_EMPTY (4*60*1000) // 30*1000

#define DEFAULT_PASSWORD 120

#define DEFAULT_OLED_BRIGHTNESS 32

#define DEFAULT_VOICE_VOLUME 50

#define DELAY_BEFORE_BATTERY_LOW_ALARM_MS (3*1000)

#ifdef __cplusplus
extern "C" {
#endif

extern int g_battery_offset_ad;

//dose unit: 0.001mL
int dose_to_encoder(int dose);
//dose unit: 0.001mL
int encoder_to_dose(int encoder);
float encoder_to_dose_float(int encoder);

void set_encoder_count_of_one_ml(int encoder_count);

void set_battery_offset_ad(int offset);

void set_bubble_alarm_status(bool status);
bool is_bubble_alarm_enable(void);

void set_pressure_threshold(int threshold);
int  get_pressure_threshold(void);

//note: all str is get from string data, if need format, you need format it
const char* alarm_id_to_const_str(AlarmID alarm_id);

//note: all str is get from string data, if need format, you need format it
const char* log_event_type_to_const_str(uint32_t log_event_type);

//note: all str is get from string data, if need format, you need format it
const char* log_event_id_to_const_str(LogEvent log_event_id);
#ifdef __cplusplus
}
#endif

