#pragma once
#include "ssz_def.h"
#include "ssz_nvram.h"
#include "data_config.h"
#include "ssz_block_file.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	DataID data_id;
	DataType data_type;
	SszBlockFileType file_type;
	uint8_t data_size;
	uint8_t *data_cache;
	//when the address is auto, it's address is after previous define block at g_data_block_map
	int32_t data_address;//-1:auto , >=0:detail address
	int32_t save_area_size;
}DataBlockSaveInfo;
typedef struct {
	DataID data_id;
	uint8_t data_size;
	//when the offset is auto, it's address is after previous define block at g_data_child_map
	int32_t data_offset;//-1:auto , >=0:detail address
}DataChildSaveInfo;


//return the nvram object
SszNvram* data_nvram();

//valid errors: kSuccess,kChecksumWrong,kNotExist,kError,kInvalidParam
result_t data_read(DataID data_id, void* buff, int buff_size);
//valid errors: kSuccess,kError,kInvalidParam
result_t data_write(DataID data_id, const void* buff, int buff_size);

//return the data id's value and the id's value is signed 
//the id's type can be int8_t,int16_t,int32_t
//if fail, will return 0, the error refer ssz_last_error()
//valid errors: kSuccess,kChecksumWrong,kNotExist,kError,kInvalidParam
int32_t data_read_int(DataID data_id);
//return the data id's value  and the id's value is unsigned 
//the id's type can be uint8_t,uint16_t,uint32_t
uint32_t data_read_uint(DataID data_id);
//write the id's value
//the id's type can be int8_t,int16_t,int32_t
//valid errors: kSuccess,kError,kInvalidParam
result_t data_write_int(DataID data_id, int32_t value);

//erase the block, all the block's area will erase
//valid errors: kSuccess,kError,kInvalidParam
result_t data_erase_block(DataID block_id);

//flush the cache to nvram
//valid errors: kSuccess,kError
result_t data_flush();

//find the data info, return the block index and the data info
int data_find_save_info_by_id(DataID data_id, DataChildSaveInfo* data_info, DataBlockSaveInfo* block_info,
	SszBlockFile** data_file);

void data_init();


#ifdef __cplusplus
}
#endif


