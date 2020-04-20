#include "ssz_spi.h"
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

//enable slave
void ssz_spi_enable_slave(SszSPI* obj)
{
	if (obj->slave_select_fun){
		obj->slave_select_fun(true);
	}

}
//disable slave
void ssz_spi_disable_slave(SszSPI* obj)
{
	if (obj->slave_select_fun){
		obj->slave_select_fun(false);
	}	
}
static int ssz_spi_file_read(struct ssz_file_t* file, void* buff, int buff_size, int* real_read_buff_size){
    SszSPI* dev = file->dev_data;

	if (ssz_spi_read(dev, buff, buff_size)) {
		if (real_read_buff_size)*real_read_buff_size = buff_size;
		return kSuccess;
	}
	else
	{
		if (real_read_buff_size)*real_read_buff_size = 0;
		return kError;
	}
}
static int ssz_spi_file_write(struct ssz_file_t* file, const void* buff, int buff_size, int* real_write_buff_size){
    SszSPI* dev = file->dev_data;

	if (ssz_spi_write(dev, buff, buff_size)) {
		if (real_write_buff_size)*real_write_buff_size = buff_size;
		return kSuccess;
	}
	else {
		if (real_write_buff_size)*real_write_buff_size = 0;
		return kError;
	}
}
static int ssz_spi_file_ctl(struct ssz_file_t* file, int command, void* param){
	int ret = kSuccess;
	(void)param;
	SszSPI* dev = file->dev_data;
	switch(command){
		case SSZ_IOCTL_ENABLE:
		if (dev->slave_select_fun) {
			dev->slave_select_fun(true);
		}
		break;
		case SSZ_IOCTL_DISABLE:
		if (dev->slave_select_fun) {
			dev->slave_select_fun(false);
		}
		break;
		case SSZ_IOCTL_FLUSH:
			ssz_spi_wait_write_finish(dev);
			break;
		default:
			ret = kNotSupport;
			break;
	}
	return ret;
}

const SszIODevice kSszSPIIODev = {
	"spi",
	ssz_spi_file_read,
	ssz_spi_file_write,
	ssz_spi_file_ctl,
};

void ssz_spi_init(SszSPI* obj, void *original_dev, SszSPISlaveSelectFun slave_select_fun){
	ssz_file_init(&obj->file, &kSszSPIIODev, obj);
	obj->obj = original_dev;
	obj->slave_select_fun = slave_select_fun;
}

SszFile * ssz_spi_file(SszSPI * obj)
{
	return &obj->file;
}
