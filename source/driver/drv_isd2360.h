/************************************************
*
* NAME: drv_isd2360.h
*
* DESCRIPTION:
*	driver file for isd2360
*
* REVISION HISTORY:
*	Rev 1.0  
* Initial revision.
*
* END OF FILE HEADER
************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "ssz_def.h"
#include "dev_def.h"
#include "config.h"
#include "ssz_file.h"  
#ifdef SSZ_TARGET_MACHINE
#include "spi.h"
#endif
//#include "stm32l1xx_hal_spi.h" 
#if 0
#define	VOPROMPT_BEEP_0		9
#define	VOMACRO_HIALARM_0	3    
#define	VOMACRO_LOWALARM_0	4    

#define	VOPROMPT_BEEP_1		0XA
#define	VOMACRO_HIALARM_1	5    
#define	VOMACRO_LOWALARM_1	6    

#define	VOPROMPT_BEEP_2		0XA
#define	VOMACRO_HIALARM_2   7
#define	VOMACRO_LOWALARM_2	8   
#endif

#define ISD2360_ID 0x05EF2060  // 0xBF2549

//音元参数根据工厂提供更改
#define VOPROMPT_BEEP_ALARM 0X05
#define VOPROMPT_LOW_ALARM  0X07
#define VOPROMPT_HIGH_ALARM 0X06

#define VOMACRO_BEEP_ALARM 0X02
#define VOMACRO_LOW_ALARM  0X04
#define VOMACRO_HIGH_ALARM 0X03

//通道
#define	ISD2360_CHANNELCONTROL_SPICMDCH0	0x00
#define	ISD2360_CHANNELCONTROL_SPICMDCH1	0x01
#define	ISD2360_CHANNELCONTROL_SPICMDCH2	0x02

//音量
#define VOICE_VOLUME_LOWEST  0XFF
#define VOICE_VOLUME_LOW     0XC0
#define VOICE_VOLUME_NORMAL  0X80
#define VOICE_VOLUME_HIGH    0X40
#define VOICE_VOLUME_HIGHEST 0X00

#define PORTX_ISD2360_CS  VOICE_SPI1_SSB_GPIO_Port	// control cs	
#define PINX_ISD2360_CS   VOICE_SPI1_SSB_Pin		// control cs

#define PORTX_ISD2360_POWER  VOICE_PWR_EN_GPIO_Port	// control POWER	
#define PINX_ISD2360_POWER   VOICE_PWR_EN_Pin		// control POWER



extern bool drv_isd2360_initial(void);
extern void drv_isd2360_deinitial(void);


extern void drv_isd2360_cmd_powerup(void);

extern void drv_isd2360_cmd_powerdown(void);

extern void drv_isd2360_resetdevice(void);

extern void drv_isd2360_stop(uint8_t channel);
/***************************************************************
函数名称 :  drv_isd2360_play_voice_prompts(uint8_t index, uint8_t channel)
功能描述 : 播放一次编号为 index 音元
输入参数 :
			index: 音元索引
			channel: ISD2360_CHANNELCONTROL_SPICMDCH0
					 ISD2360_CHANNELCONTROL_SPICMDCH1
					 ISD2360_CHANNELCONTROL_SPICMDCH2
输出参数 : 无
返回值   : 无
***************************************************************/
extern void drv_isd2360_play_voice_prompts(uint8_t index, uint8_t channel);
/***************************************************************
函数名称 :  drv_isd2360_exe_voice_macros(uint8_t index, uint8_t channel)
功能描述 : 执行一次编号为 index 宏
输入参数 :
			index:  宏索引
			channel: ISD2360_CHANNELCONTROL_SPICMDCH0
					 ISD2360_CHANNELCONTROL_SPICMDCH1
					 ISD2360_CHANNELCONTROL_SPICMDCH2
输出参数 : 无
返回值   : 无
***************************************************************/
extern void drv_isd2360_exe_voice_macros(uint8_t index, uint8_t channel);
/***************************************************************
函数名称 :  drv_isd2360_setvolume(uint8_t volume)
功能描述 :  音量设置为 volume
输入参数 :   volume       音量
			 0000 0000 = 0dB   Maximum volume
			 0000 0001 = -0.25dB
			 0000 0010 = -.50 dB
			 1111 1111 = -63.75dB  Minimum volume.
输出参数 : 无
返回值   : 无
***************************************************************/
extern void drv_isd2360_setvolume(uint8_t volume);

//enable isd2360 power supply from system power supply 
extern void drv_isd2360_pcb_power_disable(void);

//disable isd2360 power supply from system power supply
extern void drv_isd2360_pcb_power_enable(void) ;

// ISD2360  play or stop 
// return bit0  set : Channel 0 is playing
// return bit1  set : Channel 1 is playing
// return bit2  set : Channel 2 is playing
// bit3 -- bit7 is zero
uint8_t drv_isd2360_play_state(void) ;
extern void drv_isd2360_mcu_io_as_gpio(void);
extern void drv_isd2360_mcu_io_as_analog(void);
extern void drv_isd2360_mcu_io_set_as_gpio(void);

//#endif

#ifdef __cplusplus
}
#endif

