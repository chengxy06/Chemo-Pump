#include "ssz_uart.h"
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

static int ssz_uart_file_read(struct ssz_file_t* file, void* buff, int buff_size, int* real_read_buff_size){
    SszUART* dev = file->dev_data;

	if (ssz_uart_read(dev, buff, buff_size)) {
		if(real_read_buff_size)*real_read_buff_size = buff_size;
		return kSuccess;
	}
	else
	{
		if (real_read_buff_size)*real_read_buff_size = 0;
		return kError;
	}

}
static int ssz_uart_file_write(struct ssz_file_t* file, const void* buff, int buff_size, int* real_write_buff_size){
    SszUART* dev = file->dev_data;

	if (ssz_uart_write(dev, buff, buff_size)) {
		if (real_write_buff_size)*real_write_buff_size = buff_size;
		return kSuccess;
	}
	else {
		if (real_write_buff_size)*real_write_buff_size = 0;
		return kError;
	}
}
static int ssz_uart_file_ctl(struct ssz_file_t* file, int command, void* param){
	int ret = kSuccess;
	(void)param;
	//SszUART* dev = file->dev_data;
	switch(command){
		default:
			ret = kNotSupport;
			break;
	}
	return ret;
}

const SszIODevice kSszUARTIODev= {
	"uart",
	ssz_uart_file_read,
	ssz_uart_file_write,
	ssz_uart_file_ctl,
};

//is_enable_synchronous: USART need set is as true, UART need set it as false
void ssz_uart_init(SszUART* obj, void *original_dev, bool is_enable_synchronous) {
	ssz_file_init(&obj->file, &kSszUARTIODev, obj);
	obj->obj = original_dev;
	obj->is_enable_synchronous = is_enable_synchronous;
}
SszFile* ssz_uart_file(SszUART* obj)
{
	return &obj->file;
}