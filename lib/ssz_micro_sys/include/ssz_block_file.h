/************************************************
* DESCRIPTION:
*   the block file need read/write with a block.
*	because it will read/write the block checksum to check if the block is read right.
*	but when block use cache, you can read/write any bytes and it real read/write with a block background
************************************************/
#pragma once
#include "ssz_nvram.h"
#include "ssz_record.h"
/************************************************
* Declaration
************************************************/

typedef enum {
	kSszBlockFileNvram, //ssz_nvram
	kSszBlockFileRecord, //ssz_record
}SszBlockFileType;
typedef struct
{
	void* dev;
	int32_t file_save_area_max_size; //must >= file_size
	int32_t file_address;
	void *file_cache;//cache's size must be equal with the file_size
	int16_t file_size;	
	SszBlockFileType dev_type;
	bool is_enable_read_cache;
	bool is_enable_write_cache;


	bool is_opened; //true: the file is opened, false: the file is not opened
	bool is_last_read_success;
	//true: the cache is not same as nvram, need flush to nvram
	//false: the cache is same as nvram, no need flush to nvram
	bool is_cache_dirty;


}SszBlockFile;


#ifdef __cplusplus
extern "C" {
#endif
	
//init
//Param:
//	dev_type: 
//	dev:  when type is kSszBlockFileNvram, the dev is ssz_nvram
//			when type is kSszBlockFileRecord, the dev is ssz_record
//	file_address: address at nvram	
//	file_size: the file size
//	file_cache: if have, cache's size must be equal with the file_size
//	file_save_area_max_size: must >= file_size
void ssz_block_file_init(SszBlockFile* file, SszBlockFileType dev_type, void* dev, int32_t file_address, int16_t file_size, void *file_cache,
		int32_t file_save_area_max_size);

//is_delete_first: first delete the file, then open
// return if open success
//when last error is kChecksumWrong,kError,  it open fail
//when last error is kSszRecordLastHeadBroken,kNotExist, it open success
// the error code can use ssz_last_error to view
bool ssz_block_fopen(SszBlockFile* file, bool is_delete_first);
//if no cache, the buff_size must be equal with the file size, the address must be zero
//if have cache, buff_size<=file_size, address<=file_size
//errors: kSuccess,kError,kInvalidParam,KNotOpen
result_t ssz_block_fwrite_at(SszBlockFile* file, int16_t address, const void* buff, int buff_size);

//write the file all as the ch
//errors: kSuccess,kError,kInvalidParam,KNotOpen
result_t ssz_block_fwrite_all_as(SszBlockFile* file, uint8_t ch);
//if no cache, the buff_size must be equal with the file size, the address must be zero
//if have cache, buff_size<=file_size, address<=file_size
//errors: kChecksumWrong,kNotExist,kError,kInvalidParam,KNotOpen
result_t ssz_block_fread_at(SszBlockFile* file, int16_t address, void* buff, int buff_size);
//return is cache dirty, if dirty need, the cache is not same as nvram, need flush
bool ssz_block_fdirty(SszBlockFile* file);
//errors: kSuccess,kError
result_t ssz_block_fflush(SszBlockFile* file);
//it will erase file_save_area_max_size and set the file opened
result_t ssz_block_fdelete(SszBlockFile* file);

//return file size
int16_t ssz_block_fsize(SszBlockFile* file);
void ssz_block_file_enable_read_cache(SszBlockFile* file, bool is_enable);
void ssz_block_file_enable_write_cache(SszBlockFile* file, bool is_enable);

#ifdef __cplusplus
}
#endif