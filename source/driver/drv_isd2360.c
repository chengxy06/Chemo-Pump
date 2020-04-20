/************************************************
*
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
#include "drv_isd2360.h"
#include "ssz_spi.h"
#include "ssz_common.h"
#include "ssz_utility.h"
//#include "stm32l1xx_hal_spi.h"

#include "dev_def.h"


#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#endif

static const uint8_t g_ISD2360_RegAdd_ChannelControl = 0x0c;
/*static const T_U8  ISD2360_ChannelControl_TDMOFF = 0x83;*/

#if 1
#define STOP_LP_CODE      0x2e
#define PLAY_VP_LP_CODE   0xa4
#define READ_STATUS_CODE  0x40
#define READ_INT_CODE     0x46
#define READ_ID_CODE      0x48
#define DIG_READ_CODE     0xa2
#define DIG_WRITE_CODE    0xa0
#define Chip_Erase_Code   0x26
#define CHECK_SUM_CODE    0xf2
#define RD_CFG_REG_CODE   0xba
#endif

#define STOP_CODE         0x2a
#define PLAY_VP_CODE      0xa6
#define EXE_VM_CODE       0xb0
#define PWR_UP_CODE       0x10
#define PWR_DN_CODE       0x12
#define WR_CFG_REG_CODE   0xb8
#define RESET_CODE        0x14
#define VOLUME_REG		  0x03
#define ISD2360_ID_CMD    0x48
 
static void drv_isd2360_select_slave(bool is_select) ;
static void drv_isd2360_voice_enable_cs(void) ;
static void drv_isd2360_voice_disable_cs(void) ;
static uint32_t drv_isd2360_read_id(void);

static SszSPI g_isd2360_spi;

static void drv_isd2360_select_slave(bool is_select) {
	if (is_select) {
		drv_isd2360_voice_enable_cs();
	}
	else {
		drv_isd2360_voice_disable_cs();
	}
}
static void drv_isd2360_voice_enable_cs(void)
{
    // ssz_gpio_clear(SST25_FLASH_CS_PORT, SST25_FLASH_CS_PIN);
}

static void drv_isd2360_voice_disable_cs(void)
{
    // ssz_gpio_set(SST25_FLASH_CS_PORT, SST25_FLASH_CS_PIN);
}

/*configure reg*/
static void drv_isd2360_writecfgreg(uint8_t RegAddr,uint8_t RegValue)
{
	uint8_t temp = WR_CFG_REG_CODE ;
    ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);      // enable the cs Low
    ssz_spi_write(&g_isd2360_spi, &temp, 1 );
    ssz_spi_write(&g_isd2360_spi, &RegAddr, 1 );
    ssz_spi_write(&g_isd2360_spi, &RegValue, 1 );
    ssz_spi_wait_write_finish(&g_isd2360_spi);
	ssz_delay_ms(2);
    ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS);        //disable the cs  High
}
uint8_t drv_isd2360_spi_read_write_one_byte(uint8_t ch){
	uint8_t temp ;

#ifdef SSZ_TARGET_MACHINE
	HAL_SPI_TransmitReceive(&ISD2360_VOICE_SPK, &ch, &temp, 1, 1000);
#else
    temp = 1;
#endif
	return temp;	
}
bool drv_isd2360_initial(void)
{
	uint32_t id;
	uint8_t temp = 0 ;
	uint8_t read_num = 0 ;	
	drv_isd2360_pcb_power_enable();	
	drv_isd2360_mcu_io_as_gpio();
#ifdef SSZ_TARGET_MACHINE
	MX_SPI1_Init();
#endif
	ssz_spi_init(&g_isd2360_spi, &ISD2360_VOICE_SPK, drv_isd2360_select_slave);	 
	do{
	  	drv_isd2360_cmd_powerup();
		ssz_delay_ms(20);
		temp = drv_isd2360_play_state();
		if(read_num){
			ssz_traceln("ISD2360 state 0x%x", temp);
		}		
		read_num++;
		if(read_num >= 4){
			temp = 0x40 ;
		}
	}
	while( (temp & 0x40) != 0x40 );
	id = drv_isd2360_read_id();
	
	if(read_num >= 4){
		ssz_traceln("ISD2360 init number[%d]  fail",read_num );
	}
	
	if (id == ISD2360_ID)
	{
	  	ssz_traceln("ISD2360 ID[0x%x]", id);
		return true;
	}
	else
	{
		ssz_traceln("ISD2360 ID[0x%x] is wrong", id);
		return false;
	}
}
void drv_isd2360_deinitial(void){
	drv_isd2360_pcb_power_disable();
#ifdef SSZ_TARGET_MACHINE
	HAL_SPI_DeInit(&ISD2360_VOICE_SPK);
#endif
	drv_isd2360_mcu_io_as_analog();
}


static uint32_t drv_isd2360_read_id(void)
{

	uint32_t id = 0;
	uint8_t temp = ISD2360_ID_CMD ;	

#ifdef SSZ_TARGET_SIMULATOR
	return ISD2360_ID;
#endif
	ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);	   // enable the cs Low	
	
	ssz_spi_write(&g_isd2360_spi, &temp, 1 );
	
	ssz_spi_read(&g_isd2360_spi, &temp, 1 );
	id += temp ; id <<= 8;
	ssz_spi_read(&g_isd2360_spi, &temp, 1 );
	id += temp ; id <<= 8;
	ssz_spi_read(&g_isd2360_spi, &temp, 1 );
	id += temp ; id <<= 8;
	ssz_spi_read(&g_isd2360_spi, &temp, 1 );
	id += temp ;

	ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS); 	   //disable the cs  High

	return id;
}

// ISD2360  play or stop 
// return bit0 is set Channel 0 is playing
// return bit1 is set Channel 1 is playing
// return bit2 is set Channel 2 is playing
// bit3 -- bit7 is zero
uint8_t drv_isd2360_play_state(void)
{
	uint8_t temp ;
	//HAL_SPI_Init(&ISD2360_VOICE_SPK);
    ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);      // enable the cs Low
#ifdef SSZ_TARGET_MACHINE
	temp = drv_isd2360_spi_read_write_one_byte(READ_STATUS_CODE);
#else
    temp = 0;
#endif
    ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS);        //disable the cs  High

	return temp ;
}

/*power on ISD2360*/
void drv_isd2360_cmd_powerup(void)
{
	uint8_t temp = PWR_UP_CODE ;

    ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);      // enable the cs Low
	ssz_spi_write(&g_isd2360_spi, &temp, 1 );
    ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS);        //disable the cs  High
	
}

/*power off ISD2360*/
void drv_isd2360_cmd_powerdown(void)
{
	uint8_t temp = PWR_DN_CODE ;
	
    ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);      // enable the cs Low
	ssz_spi_write(&g_isd2360_spi, &temp, 1 );
    ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS);        //disable the cs  High
	
}

/*reset ISD2360*/
void drv_isd2360_resetdevice(void)
{
	uint8_t temp = RESET_CODE ;
	
    ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);      // enable the cs Low
	ssz_spi_write(&g_isd2360_spi, &temp, 1 );
    //DrvSPIxWaitFinish(ISD2360_VOICE_SPK);
    ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS);        //disable the cs  High
	
}
#ifdef SSZ_TARGET_SIMULATOR
#define REPEAT_PLAY_5S "repeat play 5s"
#define REPEAT_PLAY_16S "repeat play 16s"
static void play_after_5s(const char* name, void* event_param, void* user_param)
{
	sim_voice_play(0, "high_alarm.wav", 0);
}
static void play_after_16s(const char* name, void* event_param, void* user_param)
{
	sim_voice_play(0, "low_alarm.wav", 0);
}
#endif

/*play voice prompt with index address*/
void drv_isd2360_play_voice_prompts(uint8_t index, uint8_t channel)
{
	uint8_t temp = PLAY_VP_CODE ;
	drv_isd2360_writecfgreg(g_ISD2360_RegAdd_ChannelControl, channel);
	
    ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);      // enable the cs Low
	ssz_spi_write(&g_isd2360_spi, &temp, 1 );
	temp = 0;
	ssz_spi_write(&g_isd2360_spi, &temp, 1 );
	ssz_spi_write(&g_isd2360_spi, &index, 1 );
    ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS);        //disable the cs  High
	
#ifdef SSZ_TARGET_SIMULATOR
	switch (index) {
	case VOPROMPT_BEEP_ALARM:
		sim_voice_beep();
		break;
	case VOPROMPT_LOW_ALARM:
		sim_voice_play(channel, "low_alarm.wav", 0);
		break;
	case VOPROMPT_HIGH_ALARM:
		sim_voice_play(channel, "high_alarm.wav", 0);
		break;
	default:
		break;
	}
#endif
}


/*play voice macro with index address*/
void drv_isd2360_exe_voice_macros(uint8_t index, uint8_t channel)
{
	uint8_t temp = EXE_VM_CODE ;
	drv_isd2360_writecfgreg(g_ISD2360_RegAdd_ChannelControl, channel);
	
    ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);      // enable the cs Low
	ssz_spi_write(&g_isd2360_spi, &temp, 1 ); temp = 0 ;
	ssz_spi_write(&g_isd2360_spi, &temp, 1 );
	ssz_spi_write(&g_isd2360_spi, &index, 1 );
    ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS);        //disable the cs  High
	
#ifdef SSZ_TARGET_SIMULATOR
	switch (index) {
	case VOPROMPT_BEEP_ALARM:
		ssz_assert_fail();
		break;
	case VOPROMPT_LOW_ALARM:
		//sim_timer_bind(REPEAT_PLAY_5S, play_after_5s, NULL);
		sim_voice_play(channel, "low_alarm.wav", 0);
		//sim_timer_set(REPEAT_PLAY_5S, 5000, 1);
		break;
	case VOPROMPT_HIGH_ALARM:
		//sim_timer_bind(REPEAT_PLAY_16S, play_after_16s, NULL);
		sim_voice_play(channel, "high_alarm.wav", 0);
		//sim_timer_set(REPEAT_PLAY_16S, 16000, 1);
		break;
	default:
		break;
	}
#endif
}

void drv_isd2360_stop(uint8_t channel)
{
	uint8_t temp = STOP_CODE ;
	drv_isd2360_writecfgreg(g_ISD2360_RegAdd_ChannelControl, channel);
		
    ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);      // enable the cs Low
	ssz_spi_write(&g_isd2360_spi, &temp, 1 );
    ssz_gpio_set(PORTX_ISD2360_CS,PINX_ISD2360_CS);        //disable the cs  High
	
#ifdef SSZ_TARGET_SIMULATOR
	sim_voice_stop(channel);
#endif
}

void drv_isd2360_setvolume(uint8_t volume)
{
	drv_isd2360_writecfgreg(VOLUME_REG, volume);
}

//enable isd2360 power supply from system power supply 
void drv_isd2360_pcb_power_disable(void)
{
    ssz_gpio_clear(PORTX_ISD2360_POWER,PINX_ISD2360_POWER);      
    // disable the isd2360 power
}

//disable isd2360 power supply from system power supply
void drv_isd2360_pcb_power_enable(void)
{
    ssz_gpio_set(PORTX_ISD2360_POWER,PINX_ISD2360_POWER);
    // enable the isd2360 power
}

void drv_isd2360_mcu_io_as_gpio(void){
#ifdef SSZ_TARGET_MACHINE
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = VOICE_SPI1_SSB_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	ssz_gpio_clear(PORTX_ISD2360_CS,PINX_ISD2360_CS);
#endif
}
void drv_isd2360_mcu_io_as_analog(void){
#ifdef SSZ_TARGET_MACHINE
 	GPIO_InitTypeDef GPIO_InitStruct;		
 	GPIO_InitStruct.Pin = VOICE_SPI1_SSB_Pin|VOICE_SPI1_SCK_Pin|VOICE_SPI1_MISO_Pin|VOICE_SPI1_MOSI_Pin;
 	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
 	GPIO_InitStruct.Pull = GPIO_NOPULL;
 	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
#endif
}


