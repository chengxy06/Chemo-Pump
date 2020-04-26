/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-13 jcwu
* Initial revision.
*
************************************************/
#pragma once
#include "ssz_def.h"

/************************************************
* Declaration
************************************************/

//系统状态
#define SYSTEM_STATE_WATCHDOG_ENABLE 	0x0001
#define SYSTEM_STATE_POWER_ON			0x0002
#define SYSTEM_STATE_MOTOR_RUN			0x0004
#define SYSTEM_STATE_HAVE_ALARM			0x0008

#define HEAD_DATA					0xa55a

/* Head master receive from slave */
#define COMM_MOTOR_START			0x4101  	//参数1：方向
#define COMM_MOTOR_STOP				0x4102  
#define COMM_MOTOR_UNEXPECT_STOP	0x4103  //参数1：马达停止原因

#define COMM_SLAVER_MCU_SLEEP		0x4104  
#define COMM_SLAVER_MCU_CLEAR_DOG	0x4105 
#define COMM_SLAVER_MCU_STOP_DOG	0x4115 
#define COMM_SLAVER_MCU_START_DOG	0x4116 
#define COMM_SLAVER_MCU_SET_DOG_EXPIRED_TIME	0x4117 

#define COMM_MASTER_MCU_CLEAR_DOG	0x4106 
#define COMM_FINISHED_TARGET_ENCODER	0x4107 
#define COMM_MOTOR_START_ENABLE_STOP_BY_SLAVE	0x4108
#define COMM_POWER_OFF	            0x4109
#define COMM_POWER_ON	            0x410a
#define COMM_BEEP_TEST              0x410b


#define COMM_SLAVER_MCU_WAKE_UP_OK  0x410C
#define COMM_ASK_SLAVER_MCU_VERSION 0x410D
#define COMM_SLAVER_MCU_VERSION1	0x410E	//参数1：主版本号，//参数2：次版本号
#define COMM_SLAVER_MCU_VERSION2	0x410F	//参数1：修正版本号，//参数2：内部版本号

#define COMM_SLAVER_MOTOR_START_OK  0x4131
#define COMM_SLAVER_MOTOR_STOP_OK  	0x4132
#define COMM_SLAVER_POWER_ON_OK 	0x4133
#define COMM_SLAVER_POWER_OFF_OK  	0x4134
#define COMM_CLEAR_DOG_OK			0x4135	//参数1：Slaver的系统状态，refer SYSTEM_STATE_WATCHDOG_ENABLE

#define ENABLE_STOP_BY_SLAVE	0xA0
#define DISABLE_STOP_BY_SLAVE	0xB0


#ifdef __cplusplus
extern "C" {
#endif

/* Define the structure of SPI data packet here. */
typedef struct
{
	uint8_t		sop1;
    uint8_t		sop2;
	uint16_t	command;
	uint16_t	data1;
    uint16_t	data2;
	uint16_t	crc_code;
} TsUartCommand;



void app_mcu_comm_init( void );

//enable comm receive
void app_mcu_comm_enable_receive(void);
//disable comm receive
void app_mcu_comm_disable_receive(void);

//enable comm respond
void app_mcu_comm_enable_respond(void);
//disable comm respond except the password
void app_mcu_comm_disable_respond(void);

void app_mcu_send_to_slave(uint16_t command, uint16_t data1, uint16_t data2);
bool app_mcu_receive_slaver_version_ture(void);

#ifdef __cplusplus
}
#endif








