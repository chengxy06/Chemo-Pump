/************************************************
* DESCRIPTION:
*   I2C by use GPIO to simulate
*
* REVISION HISTORY:
*
************************************************/
#include "ssz_sim_i2c.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
#define SIM_I2C_SCL_SET ssz_gpio_set(i2c->scl_port, i2c->scl_pin)
#define SIM_I2C_SCL_CLR ssz_gpio_clear(i2c->scl_port, i2c->scl_pin)

#define SIM_I2C_SDA_SET_OUT ssz_gpio_set_mode(i2c->sda_port, i2c->sda_pin, i2c->sda_pin_out_mode)
#define SIM_I2C_SDA_SET_IN ssz_gpio_set_mode(i2c->sda_port, i2c->sda_pin, i2c->sda_pin_in_mode)
#define SIM_I2C_SDA_SET ssz_gpio_set(i2c->sda_port, i2c->sda_pin)
#define SIM_I2C_SDA_CLR ssz_gpio_clear(i2c->sda_port, i2c->sda_pin)
#define SIM_I2C_SDA_GET ssz_gpio_is_high(i2c->sda_port, i2c->sda_pin)
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
//start or restart
//when SCL=high, SDA=high->low, mean start
void ssz_sim_i2c_start(SszSimI2C* i2c){
    //config sda pin output
    SIM_I2C_SDA_SET_OUT;

    //here may be a stop
    SIM_I2C_SCL_SET;
    ssz_delay_us(1);//SCL setup time for STOP condition, 0.6uS
    SIM_I2C_SDA_SET;
    ssz_delay_us(2);//the bus must be free before a new transmission can start, 1.2uS
    
    //start
    SIM_I2C_SDA_CLR;
    ssz_delay_us(1);//SCL hold time for START condition, 0.6uS
    SIM_I2C_SCL_CLR;
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
}

//when SCL=high, SDA=low->high, mean stop
void ssz_sim_i2c_stop(SszSimI2C* i2c){
	//config sda pin output
    SIM_I2C_SDA_SET_OUT;
    
    //set SCL and SDA low first
    SIM_I2C_SCL_CLR;
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
    SIM_I2C_SDA_CLR;
    ssz_delay_us(1);//SDA low period * 0.5 = 0.65uS
    
    //stop
    SIM_I2C_SCL_SET;
    ssz_delay_us(1);//SCL setup time for STOP condition
    SIM_I2C_SDA_SET;
    ssz_delay_us(2);//Time the bus must be free before a new transmission can start, 1.2uS
}
void ssz_sim_i2c_send_ack(SszSimI2C* i2c){
	SIM_I2C_SCL_CLR;
	//when SCL=high->low, SDA=low
	//config sda pin output
    SIM_I2C_SDA_SET_OUT;
    
    //set sda=0
    //delay_us(1);//SCL low period * 0.5 = 0.65uS
    SIM_I2C_SDA_CLR;//SDA=0
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
    
    //scl pulse
    SIM_I2C_SCL_SET;
    ssz_delay_us(1);//SCL high period, 0.6uS
    SIM_I2C_SCL_CLR;
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
}
void ssz_sim_i2c_send_nack(SszSimI2C* i2c){
	SIM_I2C_SCL_CLR;
    //config sda pin output
    SIM_I2C_SDA_SET_OUT;
    
    //set sda=1
    //delay_us(1);//SCL low period * 0.5 = 0.65uS
    SIM_I2C_SDA_SET;//SDA=1
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
    
    //scl pulse
    SIM_I2C_SCL_SET;
    ssz_delay_us(1);//SCL high period, 0.6uS
    SIM_I2C_SCL_CLR;
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
}
// 0: ack, 1: nack
uint8_t ssz_sim_i2c_read_ack(SszSimI2C* i2c){
  	uint8_t ack;

  	//when SCL=high->low, read SDA
    //config sda pin input
    SIM_I2C_SDA_SET_IN;
    
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
    SIM_I2C_SCL_SET;
    ssz_delay_us(1);//SCL high period, 0.6uS
    ack = (uint8_t)SIM_I2C_SDA_GET;
    SIM_I2C_SCL_CLR;
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
    
    return ack;
}
uint8_t ssz_sim_i2c_read_byte(SszSimI2C* i2c){
 	int i;
    uint8_t  data;
    
    //config sda pin input
    SIM_I2C_SDA_SET_IN;
	SIM_I2C_SCL_CLR;

    data = 0;
    for(i=0; i<8; i++) {
            ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
            SIM_I2C_SCL_SET;
            ssz_delay_us(1);//SCL high period, 0.6uS
            //read data in
            data<<=1;
            if(1 == SIM_I2C_SDA_GET) data |= 0x01;
            SIM_I2C_SCL_CLR;
            ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
    }

    return data;
}
void ssz_sim_i2c_write_byte(SszSimI2C* i2c, uint8_t ch){
	int i;
    
    //config sda pin output
    SIM_I2C_SDA_SET_OUT;
	SIM_I2C_SCL_CLR;
    
    for(i=0; i<8; i++) {
        ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
        //sda bit output
        if(ch & 0x80){
            SIM_I2C_SDA_SET;
        }else{
            SIM_I2C_SDA_CLR;
        }
        ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
        //scl pulse
        SIM_I2C_SCL_SET;                
        ssz_delay_us(1);//SCL high period, 0.6uS
        SIM_I2C_SCL_CLR;
        //next bit
        ch <<= 1;
    }
    ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
}

bool ssz_sim_i2c_master_read(SszSimI2C* i2c, void *buff, int buff_size)
{
	ssz_sim_i2c_start(i2c);
	ssz_sim_i2c_write_byte(i2c, i2c->slave_address);
	if(ssz_sim_i2c_read_ack(i2c)!=0){
		ssz_sim_i2c_stop(i2c);
		return false;
	}
	int i;
    for (i = 0; i < buff_size; ++i){
        ((uint8_t*)buff)[i] = ssz_sim_i2c_read_byte(i2c);
		if(i==buff_size-1){
			ssz_sim_i2c_send_nack(i2c);
		}else{
			ssz_sim_i2c_send_ack(i2c);
		}
    }
    if (!i2c->not_send_stop) {
        ssz_sim_i2c_stop(i2c);
    }else{
        i2c->not_send_stop = false;
    }
    return true;	
}
bool ssz_sim_i2c_master_write(SszSimI2C* i2c, const void *buff, int buff_size)
{
	ssz_sim_i2c_start(i2c);
	ssz_sim_i2c_write_byte(i2c, i2c->slave_address);
	if(ssz_sim_i2c_read_ack(i2c)!=0){
		ssz_sim_i2c_stop(i2c);
		return false;
	}
	int i;
    for (i = 0; i < buff_size; ++i){
        ssz_sim_i2c_write_byte(i2c, ((uint8_t*)buff)[i]);
		if(ssz_sim_i2c_read_ack(i2c)!=0){
			ssz_sim_i2c_stop(i2c);
			return false;
		}
    }
    if (!i2c->not_send_stop) {
        ssz_sim_i2c_stop(i2c);
    }else{
        i2c->not_send_stop = false;
    }

    return true;	
}
//set slave address
void ssz_sim_i2c_master_set_slave_address(SszSimI2C* i2c, uint8_t slave_address)
{
	i2c->slave_address = slave_address;
}

bool ssz_sim_i2c_master_is_slaver_busy(SszSimI2C * i2c)
{    
	//config sda pin input
	SIM_I2C_SDA_SET_IN;
	if (SIM_I2C_SDA_GET == 0) {
		return true;
	}
	return false;
}

//fore to let slaver release sda pin, so can let it not busy
void ssz_sim_i2c_master_force_slaver_release(SszSimI2C * i2c)
{
	//bool is_not_busy;
	//config sda pin input
	SIM_I2C_SDA_SET_IN;
	SIM_I2C_SCL_CLR;

	for (int i = 0; i < 9; i++) {
		ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS
		SIM_I2C_SCL_SET;
		ssz_delay_us(1);//SCL high period, 0.6uS
		//read data in, if sda==1, it is released
		//is_not_busy = SIM_I2C_SDA_GET;
		SIM_I2C_SCL_CLR;
		ssz_delay_us(1);//SCL low period * 0.5 = 0.65uS

//		if (is_not_busy) {
//			break;
//		}
	}
	
	ssz_sim_i2c_stop(i2c);
}

static int ssz_sim_i2c_master_file_read(struct ssz_file_t* file, void* buff, int buff_size, int* real_read_buff_size){
    SszSimI2C* dev = file->dev_data;
	if (ssz_sim_i2c_master_read(dev, buff, buff_size)) {
		if (real_read_buff_size)*real_read_buff_size = buff_size;
		return kSuccess;
	}
	else
	{
		if (real_read_buff_size)*real_read_buff_size = 0;
		return kError;
	}
}
static int ssz_sim_i2c_master_file_write(struct ssz_file_t* file, const void* buff, int buff_size, int* real_write_buff_size){
    SszSimI2C* dev = file->dev_data;

	if (ssz_sim_i2c_master_write(dev, buff, buff_size)) {
		if (real_write_buff_size)*real_write_buff_size = buff_size;
		return kSuccess;
	}
	else {
		if (real_write_buff_size)*real_write_buff_size = 0;
		return kError;
	}
}
static int ssz_sim_i2c_master_file_ctl(struct ssz_file_t* file, int command, void* param){
	(void)param;
	(void)command;
	(void)file;

	int ret = kSuccess;
	SszSimI2C* dev = file->dev_data;

	switch (command) {
	case SSZ_IOCTL_SEEK:
		ssz_assert(param);
		dev->slave_address = *(int32_t*)param;
		break;
	case SSZ_SIM_IOCTL_NOT_SEND_STOP:
		dev->not_send_stop = true;
		break;
	default:
		ret = kNotSupport;
		break;
	}
	return ret;
}

//io dev
const SszIODevice kSszSimI2CMasterIODev = {
	"sim_i2c",
	ssz_sim_i2c_master_file_read,
	ssz_sim_i2c_master_file_write,
	ssz_sim_i2c_master_file_ctl,
};

void ssz_sim_i2c_master_init(SszSimI2C* i2c, SszGpio scl_port, SszPin scl_pin,
	SszGpio sda_port, SszPin sda_pin, SszGpioMode sda_pin_out_mode, SszGpioMode sda_pin_in_mode) {
	ssz_file_init(&i2c->file, &kSszSimI2CMasterIODev, i2c);
	i2c->scl_port = scl_port;
	i2c->scl_pin = scl_pin;
	i2c->sda_port = sda_port;
	i2c->sda_pin = sda_pin;
	i2c->sda_pin_out_mode = sda_pin_out_mode;
	i2c->sda_pin_in_mode = sda_pin_in_mode;
	i2c->slave_address = 0;
	i2c->not_send_stop = false;
}
SszFile* ssz_sim_i2c_master_file(SszSimI2C* i2c) {
	return &i2c->file;
}