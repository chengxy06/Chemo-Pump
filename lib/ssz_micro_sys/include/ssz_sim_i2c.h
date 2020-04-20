/************************************************
* DESCRIPTION:
*   I2C by use GPIO to simulate
*	e.g.
*	SszSimI2C eeprom_i2c = {SSZ_GPIOA, SSZ_PIN2,
*		SSZ_GPIOB, SSZ_PIN3, kSszGpioOutPP, kSszGpioInputPU};
*	ssz_sim_i2c_start(&eeprom_i2c);
* REVISION HISTORY:
*
* Initial revision.
************************************************/
#pragma once
#include "ssz_gpio.h"
#include "ssz_def.h"
#include "ssz_file.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define SSZ_SIM_IOCTL_NOT_SEND_STOP 1 //not_send_stop

typedef struct 
{
	SszFile file;
	SszGpio scl_port;
	SszPin scl_pin;
	SszGpio sda_port;
	SszPin sda_pin;
	SszGpioMode sda_pin_out_mode;
	SszGpioMode sda_pin_in_mode;
	//used when at master mode, ssz_fseek will set it by the param
	uint8_t slave_address;	
	//when start read or write, it will take effect, after read or write, it will clear to false
	//it is used to simulate restart
	bool not_send_stop;
}SszSimI2C;

//io dev
extern const SszIODevice kSszSimI2CMasterIODev;

//original interface, do not use them unless you know it
void ssz_sim_i2c_start(SszSimI2C* i2c);
void ssz_sim_i2c_stop(SszSimI2C* i2c);
void ssz_sim_i2c_send_ack(SszSimI2C* i2c);
void ssz_sim_i2c_send_nack(SszSimI2C* i2c);
uint8_t ssz_sim_i2c_read_ack(SszSimI2C* i2c);
uint8_t ssz_sim_i2c_read_byte(SszSimI2C* i2c);
void ssz_sim_i2c_write_byte(SszSimI2C* i2c, uint8_t ch);

//recommend use below interface  
void ssz_sim_i2c_master_init(SszSimI2C* i2c, SszGpio scl_port, SszPin scl_pin,
	SszGpio sda_port, SszPin sda_pin, SszGpioMode sda_pin_out_mode, SszGpioMode sda_pin_in_mode);
SszFile* ssz_sim_i2c_master_file(SszSimI2C* i2c);

bool ssz_sim_i2c_master_read(SszSimI2C* i2c, void *buff, int buff_size);
bool ssz_sim_i2c_master_write(SszSimI2C* i2c, const void *buff, int buff_size);
//set slave address
void ssz_sim_i2c_master_set_slave_address(SszSimI2C* i2c, uint8_t slave_address);
bool ssz_sim_i2c_master_is_slaver_busy(SszSimI2C* i2c);
//fore to let slaver release sda pin, so can let it not busy
void ssz_sim_i2c_master_force_slaver_release(SszSimI2C* i2c);


#ifdef __cplusplus
}
#endif
