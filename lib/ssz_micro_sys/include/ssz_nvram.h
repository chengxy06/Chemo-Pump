/************************************************
* DESCRIPTION:
*   nvram include eeprom, norflash, nandflash, fram...
*   
*
************************************************/
#pragma once
#include "ssz_def.h"
#include "ssz_errorcode.h"
#include "ssz_file.h"

/************************************************
* Declaration
************************************************/
#define SSZ_NVRAM_CHECKSUM_SIZE 4

//read/write mode
//will write/read the checksum(4 bytes) which after the data
#define SSZ_NVRAM_RW_MODE_USE_CHECKSUM 0x1
//will check if the destination area can write, if can not, it will read back, erase,then write.
#define SSZ_NVRAM_RW_MODE_CHECK_IF_CAN_WRITE 0x2
//after write, will read some bytes to check if write success
#define SSZ_NVRAM_RW_MODE_CHECK_IF_WRITE_SUCCESS 0x4


typedef struct {
	SszFile* file;
	//used for flash which need erase before write
	//uint8_t* one_unit_buff;
	int32_t size; //the nvram's size
	int one_erase_unit_size;
	//int error_code; //values: kSuccess, kError,kChecksumWrong,kDataNotInit
	int try_count; //if read or write fail, try again count
	bool is_need_erase_before_write;
	bool is_support_erase;
	//is need backup when need erase before write, if need, the one_unit_buff should valid
	bool is_need_backup; 
	uint8_t rw_mode;//read/write mode, refer SSZ_NVRAM_WR_MODE_USE_CHECKSUM...
}SszNvram;

#ifdef __cplusplus
extern "C" {
#endif

void ssz_nvram_init(SszNvram* dev, SszFile* f, bool is_need_backup, int try_count, uint8_t rw_mode);

//write according the read/write mode which define at init
// valid error codes:kError,kSuccess
result_t ssz_nvram_write(SszNvram* dev, int32_t address, const void* buff, int buff_size);

//write according the param:write_mode
// valid error codes:kError,kSuccess
// write_mode: 
//		SSZ_NVRAM_WR_MODE_USE_CHECKSUM: will write the checksum(4 bytes) which after the data
//		SSZ_NVRAM_WR_MODE_CHECK_IF_CAN_WRITE: will check if the destination area can write, 
//			if can not write, it will read back, erase,then write,
//			so it will not effect other data(is_need_backup must be true)
//		SSZ_NVRAM_WR_MODE_CHECK_IF_WRITE_SUCCESS: after write, will read some bytes to check if write success
result_t ssz_nvram_write_ex(SszNvram* dev, int32_t address, const void* buff, int buff_size, uint8_t write_mode);

//write the address with the size bytes which all is the onebyte,
// valid error codes:kError,kSuccess
result_t ssz_nvram_fill_with(SszNvram* dev, int32_t address, uint8_t onebyte, int size);

//write the address with the size bytes which all is the onebyte,
// valid error codes:kError,kSuccess
// write_mode: 
//		SSZ_NVRAM_WR_MODE_USE_CHECKSUM: will write the checksum(4 bytes) which after the data
//		SSZ_NVRAM_WR_MODE_CHECK_IF_CAN_WRITE: will check if the destination area can write, 
//			if can not write, it will read back, erase,then write,
//			so it will not effect other data(is_need_backup must be true)
//		SSZ_NVRAM_WR_MODE_CHECK_IF_WRITE_SUCCESS: after write, will read some bytes to check if write success
result_t ssz_nvram_fill_with_ex(SszNvram* dev, int32_t address, uint8_t onebyte, int size, uint8_t write_mode);

//read according the read/write mode which define at init
// valid error codes:kSuccess,kError,kChecksumWrong(only at checksum mode),kDataNotInit(only at checksum mode)
result_t ssz_nvram_read(SszNvram* dev, int32_t address, void* buff, int buff_size);

//return according the param:read_mode
// valid error codes:kSuccess,kError,kChecksumWrong(only at checksum mode),kDataNotInit(only at checksum mode)
// read_mode:
//	SSZ_NVRAM_WR_MODE_USE_CHECKSUM: will read the checksum(4 bytes) which after the data
result_t ssz_nvram_read_ex(SszNvram* dev, int32_t address, void* buff, int buff_size, uint8_t read_mode);

//check if the buff is all the byte
bool ssz_nvram_check_if_is_byte(SszNvram* dev, int32_t address, int need_check_size, uint8_t dest_byte);

//check the specify area is erased
bool ssz_nvram_is_erased(SszNvram* dev, int32_t address, int need_check_size);
//erase one unit
result_t ssz_nvram_erase_one_unit(SszNvram* dev, int32_t address);
//it only erase the specify area
result_t ssz_nvram_erase(SszNvram* dev, int32_t address, int need_erase_size);

//it will erase the unit if write to new unit or write at unit begin
//Note: the write range must at 1~2 unit
result_t ssz_nvram_write_and_erase_new_unit_if_need(SszNvram* dev, int32_t address, const void* buff, int buff_size);

#ifdef __cplusplus
}
#endif