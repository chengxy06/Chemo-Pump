/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-13 jcwu
* Initial revision.
*
************************************************/
#include "app_mcu_comm.h"
#include "com.h"
#include "ssz_time.h"
#include "timer.h"
#include "version.h"
#include "ssz_time_utility.h"
#include "string.h"
#include "stdlib.h"
#include "ssz_common.h"
#include "timer_config.h"
#include "infusion_motor.h"
#include "app_delivery.h"
#include "alarm.h"
#include "drv_lowpower.h"
#include "app_mcu_monitor.h"
#include "app_common.h"
#include "record_log.h"

/************************************************
* Declaration
************************************************/
#define COMM_MAX_SIZE_OF_RECEIVE_STR 60
#define COMM_ID kComMCUUart
#define COMM_HANDLE_ORDER kComHandleFirst

/************************************************
* Variable 
************************************************/
static uint8_t g_app_comm_receive[COMM_MAX_SIZE_OF_RECEIVE_STR];
static int g_app_comm_receive_index = 0;
static Version g_slaver_version_temp = {0,0,0,0};
static int g_receive_slaver_version_flag = 0;
/************************************************
* Function 
************************************************/
// crc16 code calculation
uint16_t com_crc16( uint8_t* data_p, uint8_t length)
{
  uint8_t x;
  uint16_t crc = 0xFFFF;

  while(length--){
    x = (crc >> 8) ^ (*data_p++);
    x ^= x>>4;
    crc = (crc << 8) ^ ((uint16_t)(x << 12))
          ^ ((uint16_t)(x <<5)) ^ ((uint16_t)x);
  }
  return crc;
}
void app_mcu_respond_to_slave(ComID com_id ,TsUartCommand usart_data)
{
 	usart_data.crc_code = com_crc16((uint8_t*)&usart_data, 8);
	ssz_fwrite(com_to_file(com_id), (uint8_t*)&usart_data, 10 );
}

// A frame data timeout , Array index to 0
void app_mcu_comm_recv_from_slave_time_out( void )
{
    g_app_comm_receive_index = 0;   
}
void app_mcu_set_receive_slaver_version_flag(int data ){
	if(data == COMM_SLAVER_MCU_VERSION1){
		g_receive_slaver_version_flag |=0x0f;
	}else if(data == COMM_SLAVER_MCU_VERSION2){
		g_receive_slaver_version_flag |=0xf0;	
	}
}

bool app_mcu_receive_slaver_version_ture(void){
	if(g_receive_slaver_version_flag == 0xff){
		return true;
	}else{
		return false;
	}
}
void app_mcu_comm_analysis_from_slave(TsUartCommand usart_data)
{
	uint16_t head;
	
	head = usart_data.sop1;
	head = (head<<8) + usart_data.sop2;
	
	if( head != HEAD_DATA )  return;
	
	switch(usart_data.command)
	{
		case COMM_SLAVER_MOTOR_START_OK:			
			 motor_printfln("slaver motor start");
		break;
		
		case COMM_SLAVER_MOTOR_STOP_OK:			
			 motor_printfln("slaver motor stop");
		break;

		case COMM_MOTOR_UNEXPECT_STOP:	
			common_printfln("slaver stop motor by cause:%d", usart_data.data1);
			record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
				kLogEventMotorStopBySlaver,
				usart_data.data1,
				g_infusion_motor_encoder);
			 infusion_motor_stop(usart_data.data1);
			alarm_set(kAlarmInfuMotorErrorBySlaverID);
		break;	
		
		case COMM_SLAVER_MCU_SLEEP:			
			 //app_mcu_respond_to_slave(COMM_ID , usart_data);		
		break;
		
		case COMM_SLAVER_MCU_CLEAR_DOG:			
			 //app_mcu_respond_to_slave(COMM_ID , usart_data);
		break;		
		case COMM_MASTER_MCU_CLEAR_DOG:			
			//app_mcu_monitor_clear_by_slaver();
			  //ssz_traceln("slaver clear master watch dog\n");	
		break;

        case COMM_FINISHED_TARGET_ENCODER:
            printf("stop\n");
         //   infusion_motor_stop(MOTOR_STOP_BY_FINISH_EXPECT_TARGET);
            //app_delivery_on_motor_stop(0, MOTOR_STOP_BY_FINISH_EXPECT_TARGET);
		break;
        case COMM_SLAVER_POWER_ON_OK:
             common_printfln("slaver power on");
		break;
		case COMM_SLAVER_POWER_OFF_OK:
             common_printfln("slaver power off");
		break;
        case COMM_SLAVER_MCU_WAKE_UP_OK:
			 g_drv_lowpower_is_slaver_wake_up = true;
              common_printfln("slaver wake up");
		break;
//////////////////////////////////////////////////////
        case COMM_SLAVER_MCU_VERSION1:
			g_slaver_version_temp.major = usart_data.data1 ;
			g_slaver_version_temp.minor = usart_data.data2 ;
			app_mcu_set_receive_slaver_version_flag((int)COMM_SLAVER_MCU_VERSION1);
			if(app_mcu_receive_slaver_version_ture()){
				slaver_version_info_set(&g_slaver_version_temp);
			}			
            common_printfln("slaver ver1: %d.%d", usart_data.data1,usart_data.data2);
		break;

        case COMM_SLAVER_MCU_VERSION2:
			g_slaver_version_temp.revision = usart_data.data1 ;
			g_slaver_version_temp.internal_version = usart_data.data2 ;	
			app_mcu_set_receive_slaver_version_flag((int)COMM_SLAVER_MCU_VERSION2);
			if(app_mcu_receive_slaver_version_ture()){
				slaver_version_info_set(&g_slaver_version_temp);
			}
            common_printfln("slaver ver2: %d.%d", usart_data.data1,usart_data.data2);
		break;		
		case COMM_CLEAR_DOG_OK:
			app_mcu_monitor_stop_and_clear_reset_count();

			//检查slaver状态，有时候slaver可能会自动重启，然后状态会错误
			if(!(usart_data.data1&SYSTEM_STATE_WATCHDOG_ENABLE)){
				if(watchdog_is_enable()){
					//如果没有开启watchdog， 则进行开启
					app_mcu_send_to_slave(COMM_SLAVER_MCU_START_DOG,0,0);
				}
			}
			if(!(usart_data.data1&SYSTEM_STATE_POWER_ON)){
				//如果没有启动， 则启动（锁住电源）
				app_mcu_send_to_slave(COMM_POWER_ON,0,0);
			}
			break;
//////////////////////////////////////////////////////

		default:
		break;
	}
}


//static int g_receive_version = 0;
//static Version g_slaver_version_temp = 0;

void app_mcu_send_to_slave(uint16_t command, uint16_t data1, uint16_t data2)
{
	TsUartCommand send_command;

	send_command.sop1 = HEAD_DATA >> 8;
	send_command.sop2 = HEAD_DATA & 0xFF;
	send_command.command= command;
	send_command.data1 = data1;
	send_command.data2 = data2; 

	send_command.crc_code = com_crc16((uint8_t*)&send_command, 8);
	ssz_fwrite(com_to_file(COMM_ID), (uint8_t*)&send_command, 10 );  
}

// 
static bool app_mcu_comm_on_receive_byte(uint8_t ch, bool is_handled_ok_at_pre_handler)
{
	timer_start_oneshot_after(kTimerCommTimeTooLong, 1000);
	// Received the correct data , handle it
	if (g_app_comm_receive_index == 0){
		 //control command from master 
		if (ch != 0xA5) {
			g_app_comm_receive_index = 0;
		}
		else {
			g_app_comm_receive[g_app_comm_receive_index] = ch;
			g_app_comm_receive_index++;
		}
	}
	else if (g_app_comm_receive_index == 1) {
		if (ch != 0x5A) {
			g_app_comm_receive_index = 0;
			if (ch == 0xA5) {
				g_app_comm_receive[g_app_comm_receive_index] = ch;
				g_app_comm_receive_index++;
			}
		}
		else {
			g_app_comm_receive[g_app_comm_receive_index] = ch;
			g_app_comm_receive_index++;
		}
	}
	else{
		 g_app_comm_receive[g_app_comm_receive_index] = ch;
		 g_app_comm_receive_index++;
		if (g_app_comm_receive_index==10){
			TsUartCommand command;
			uint16_t crc_result;

			//receive finish, reset pos
			g_app_comm_receive_index = 0;
			 
			 //check the checksum is right
			 crc_result = com_crc16(g_app_comm_receive, 8);
			 if(memcmp(&crc_result, g_app_comm_receive+8, 2)==0)	{
				 //the checksum is right
				 memcpy(&command, g_app_comm_receive, 10);
			//	 printf("recv type[%d] cmd[%c%c],data[%d]\n", command.sop2, command.command>>8, command.command&0xFF, command.data1);
				 app_mcu_comm_analysis_from_slave(command);
			 }
			 else	{
			 	printf("receive comm checksum fail\n");	
			 }
			 timer_stop(kTimerCommTimeTooLong);			 
		}
	}
    return true;
}

//enable comm receive
void app_mcu_comm_enable_receive(void)
{
	//handle the receive char
    com_set_receive_handler(COMM_ID, COMM_HANDLE_ORDER, app_mcu_comm_on_receive_byte);
}

//disable comm receive
void app_mcu_comm_disable_receive(void)
{
	com_set_receive_handler(COMM_ID, COMM_HANDLE_ORDER, NULL);	
}

//
void app_mcu_comm_init( void )
{ 
	timer_set_handler(kTimerCommTimeTooLong, app_mcu_comm_recv_from_slave_time_out);
	//handle the receive char
    com_set_receive_handler(COMM_ID, COMM_HANDLE_ORDER, app_mcu_comm_on_receive_byte);
}

