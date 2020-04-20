#pragma once
/************************************************
* Declaration
************************************************/
#include "ssz_def.h"
#include "ssz_file.h"

typedef void (*SszSPISlaveSelectFun)(bool is_select);
typedef struct 
{
	SszFile file;
	void* obj;
	//slave select fun, can be null, useage: ssz_ioctl(file, SSZ_IOCTL_ENABLE, NULL);
	SszSPISlaveSelectFun slave_select_fun;
}SszSPI;

//io dev
extern const SszIODevice kSszSPIIODev;

#ifdef __cplusplus
extern "C" {
#endif
	
void ssz_spi_init(SszSPI* obj, void *original_dev, SszSPISlaveSelectFun slave_select_fun);
SszFile* ssz_spi_file(SszSPI* obj);

bool ssz_spi_read(SszSPI* obj, void *buff, int buff_size);
bool ssz_spi_write(SszSPI* obj, const void *buff, int buff_size);
//enable slave
void ssz_spi_enable_slave(SszSPI* obj);
//disable slave
void ssz_spi_disable_slave(SszSPI* obj);
//wait spi send finish
void ssz_spi_wait_write_finish(SszSPI* obj);

#ifdef __cplusplus
}
#endif