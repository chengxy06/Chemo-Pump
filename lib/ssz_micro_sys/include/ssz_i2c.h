#pragma once
/************************************************
* Declaration
************************************************/
#include "ssz_def.h"
#include "ssz_file.h"

typedef struct 
{
	SszFile file;
	void* obj;
	//used when at master mode, ssz_fseek will set it by the param
	uint8_t slave_address;
}SszI2C;

//io dev
extern const SszIODevice kSszI2CMasterIODev;

#ifdef __cplusplus
extern "C" {
#endif

void ssz_i2c_master_init(SszI2C* i2c, void *original_dev);
SszFile* ssz_i2c_master_file(SszI2C* i2c);

bool ssz_i2c_master_read(SszI2C* i2c, void *buff, int buff_size);
bool ssz_i2c_master_write(SszI2C* i2c, const void *buff, int buff_size);
//set slave address
void ssz_i2c_master_set_slave_address(SszI2C* i2c, uint8_t slave_address);

#ifdef __cplusplus
}
#endif