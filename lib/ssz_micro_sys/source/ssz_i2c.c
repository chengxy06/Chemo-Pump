#include "ssz_i2c.h"
#include "ssz_common.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

//set slave address
void ssz_i2c_master_set_slave_address(SszI2C* i2c, uint8_t slave_address)
{
	i2c->slave_address = slave_address;
}
static int ssz_i2c_master_file_read(struct ssz_file_t* file, void* buff, int buff_size, int* real_read_buff_size){
    SszI2C* dev = file->dev_data;

	if (ssz_i2c_master_read(dev, buff, buff_size)) {
		if (real_read_buff_size)*real_read_buff_size = buff_size;
		return kSuccess;
	}
	else
	{
		if (real_read_buff_size)*real_read_buff_size = 0;
		return kError;
	}
}
static int ssz_i2c_master_file_write(struct ssz_file_t* file, const void* buff, int buff_size, int* real_write_buff_size){
    SszI2C* dev = file->dev_data;

	if (ssz_i2c_master_write(dev, buff, buff_size)) {
		if (real_write_buff_size)*real_write_buff_size = buff_size;
		return kSuccess;
	}
	else {
		if (real_write_buff_size)*real_write_buff_size = 0;
		return kError;
	}
}
static int ssz_i2c_master_file_ctl(struct ssz_file_t* file, int command, void* param){
	(void)param;
	(void)command;
	(void)file;

	int ret = kSuccess;
	SszI2C* dev = file->dev_data;

	switch (command) {
	case SSZ_IOCTL_SEEK:
		ssz_assert(param);
		dev->slave_address = *(int32_t*)param;
		break;
	default:
		ret = kNotSupport;
		break;
	}
	return ret;
}

const SszIODevice kSszI2CMasterIODev = {
	"i2c_master",
	ssz_i2c_master_file_read,
	ssz_i2c_master_file_write,
	ssz_i2c_master_file_ctl,
};

void ssz_i2c_master_init(SszI2C* i2c, void *original_dev) {
	ssz_file_init(&i2c->file, &kSszI2CMasterIODev, i2c);
	i2c->obj = original_dev;
	i2c->slave_address = 0;
}
SszFile* ssz_i2c_master_file(SszI2C* i2c) {
	return &i2c->file;
}