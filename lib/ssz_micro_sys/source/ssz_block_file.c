#include "ssz_block_file.h"
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
		int32_t file_save_area_max_size)
{
	ssz_mem_zero(file, sizeof(*file));
	file->dev = dev;
	file->dev_type = dev_type;
	file->file_address = file_address;
	file->file_size = file_size;
	file->file_cache = file_cache;
	file->file_save_area_max_size = file_save_area_max_size;
	if (file->file_cache)
	{
		file->is_enable_read_cache = true;
		file->is_enable_write_cache = true;
	}
	if (dev_type == kSszBlockFileRecord) {
		ssz_tmp_buff_check_size(kTempAllocCommonBuff, ((SszRecord*)dev)->one_record_size);
	}

}
void ssz_block_file_check_file(SszBlockFile* file)
{
	ssz_assert(file->dev_type == kSszBlockFileNvram ||
		file->dev_type == kSszBlockFileRecord);
}
//must write file size
//errors: kSuccess,kError,kInvalidParam
result_t ssz_block_fwrite_orginal(SszBlockFile* file, const void* buff, int buff_size)
{
	ssz_assert(file->file_size == buff_size);
	ssz_block_file_check_file(file);
	if (file->dev_type == kSszBlockFileNvram) {
		return ssz_nvram_write((SszNvram*)file->dev, file->file_address,
			buff, buff_size);
	}
	else if (file->dev_type == kSszBlockFileRecord) {
		return ssz_record_push((SszRecord*)file->dev, buff);
	}
	else {
		ssz_debug_break();
		return kInvalidParam;
	}
}

//write the file all as the ch
//errors: kSuccess,kError,kInvalidParam
result_t ssz_block_fwrite_all_as_orginal(SszBlockFile* file, uint8_t ch)
{
	ssz_block_file_check_file(file);
	if (file->dev_type == kSszBlockFileNvram) {
		return ssz_nvram_fill_with((SszNvram*)file->dev, file->file_address,
			ch, file->file_size);
	}
	else if (file->dev_type == kSszBlockFileRecord) {
		uint8_t* p = ssz_tmp_buff_alloc(kTempAllocCommonBuff, ((SszRecord*)(file->dev))->one_record_size);
		memset(p, ch, ((SszRecord*)file->dev)->one_record_size);
		bool ret = ssz_record_push((SszRecord*)file->dev, p);
		ssz_tmp_buff_free(p);
		return ret;
	}
	else {
		ssz_debug_break();
		return kInvalidParam;
	}
}

//must read file size
//errors: kChecksumWrong,kNotExist,kError,kInvalidParam
result_t ssz_block_fread_orginal(SszBlockFile* file, void* buff, int buff_size)
{
	ssz_assert(file->file_size == buff_size);
	if (file->dev_type == kSszBlockFileNvram) {
		result_t ret = ssz_nvram_read((SszNvram*)file->dev, file->file_address,
			buff, buff_size);
		if (ret == kDataNotInit) {
			ret = kNotExist;
		}
		return ret;
	}
	else if (file->dev_type == kSszBlockFileRecord) {
//		int rc_size = ssz_record_size((SszRecord*)file->dev);
//		rc_size--;
//		result_t ret;
//		while (rc_size>=0) {
//			ret = ssz_record_at((SszRecord*)file->dev, rc_size, buff);
//			if (ret != kChecksumWrong) {
//				return ret;
//			}
//			else {
//				rc_size--;
//			}
//		}
		return ssz_record_top((SszRecord*)file->dev, buff);
	}
	else {
		ssz_debug_break();
		return kInvalidParam;
	}
}

//is_delete_first: first delete the file, then open
//when last error is kChecksumWrong,kError,  it open fail
//when last error is kSszRecordLastHeadBroken,kNotExist, it open success
// the error code can use ssz_last_error to view
bool ssz_block_fopen(SszBlockFile* file, bool is_delete_first)
{
	result_t ret = kSuccess;

	ssz_set_last_error(ret);
	if (is_delete_first)
	{
		ssz_block_fdelete(file);
	}
	if (file->is_opened)
	{
		ssz_set_last_error(kSuccess);
		return true;
	}

	if (file->dev_type == kSszBlockFileNvram) {
		if (file->file_cache)
		{
			ret = ssz_nvram_read((SszNvram*)file->dev, file->file_address,
				file->file_cache, file->file_size);
			if (ret == kSuccess ||
				ret==kDataNotInit) {
				file->is_opened = true;
				if (ret == kDataNotInit) {
					ret = kNotExist;
				}
				ssz_set_last_error(ret);
			}
		}
		else {
			file->is_opened = true;
			ssz_set_last_error(kSuccess);
			//if (ssz_nvram_check_if_is_byte((SszNvram*)file->dev, file->file_address, 
			//	file->file_size+ SSZ_NVRAM_CHECKSUM_SIZE, 0xFF)
			//{
			//	ssz_set_last_error(kNotExist);
			//}
		}
	}
	else if (file->dev_type == kSszBlockFileRecord) {
		ret = ssz_record_load((SszRecord*)file->dev);

		if (ret == kSszRecordLastHeadBroken ||
			ret == kSuccess) {
			file->is_opened = true;
			ssz_set_last_error(ret);
		}
		if (ssz_record_size((SszRecord*)file->dev) == 0)
		{
			//if not exist, set error code
			ssz_set_last_error(kNotExist);
		}else if (file->file_cache)
		{
//			int rc_size = ssz_record_size((SszRecord*)file->dev);
//			rc_size--;
//			result_t ret;
//			while (rc_size>=0) {
//				ret = ssz_record_at((SszRecord*)file->dev, rc_size, file->file_cache);
//				if (ret != kChecksumWrong) {
//					break;
//				}
//				else {
//					rc_size--;
//				}
//			}
			ret = ssz_record_top((SszRecord*)file->dev, file->file_cache);
			if (ret != kSuccess) {
				//if read to cache fail, set open fail
				file->is_opened = false;
				ssz_set_last_error(ret);
			}
		}
	}
	else {
		ssz_set_last_error(kInvalidParam);
		ssz_assert_fail();
	}

	return file->is_opened;
}
//if no cache, the buff_size must be equal with the file size, the address must be zero
//if have cache, buff_size<=file_size, address<=file_size
//errors: kSuccess,kError,kInvalidParam,KNotOpen
result_t ssz_block_fwrite_at(SszBlockFile* file, int16_t address, const void* buff, int buff_size)
{
	//check the buff size
	ssz_assert(buff_size <= file->file_size);
	if (buff_size > file->file_size ||
		address + buff_size > file->file_size) {
		return kInvalidParam;
	}
	if (!file->is_opened)
	{
		return KNotOpen;
	}

	//write the block
	int block_size = file->file_size;
	const void* block_buff = file->file_cache;
	if (block_buff == NULL) {
		//if not use cache, the id must be block, the size must equal with block size
		block_buff = buff;
		block_size = buff_size;
		ssz_assert(buff_size == block_size);
	}
	else {
		memcpy((uint8_t*)block_buff + address, buff, buff_size);
	}
	//if have block cache , enable write cache,  only write to cache
	if (file->file_cache != NULL &&
		file->is_enable_write_cache) {
		file->is_cache_dirty = true;
		return kSuccess;
	}
	else {//write to nvram
		file->is_cache_dirty = false;
		return ssz_block_fwrite_orginal(file,
			block_buff, block_size);
	}
}

//write the file all as the ch
//errors: kSuccess,kError,kInvalidParam,KNotOpen
result_t ssz_block_fwrite_all_as(SszBlockFile* file, uint8_t ch)
{
	if (!file->is_opened)
	{
		return KNotOpen;
	}
	result_t ret;
	int block_size = file->file_size;
	void* block_buff = file->file_cache;
	if (block_buff) {
		//set block all as ch
		memset(block_buff, ch, file->file_size);

		//if have block cache , enable write cache,  only write to cache
		if (file->file_cache != NULL &&
			file->is_enable_write_cache) {
			ret = kSuccess;
			file->is_cache_dirty = true;
		}
		else {//write to nvram
			ret = ssz_block_fwrite_orginal(file,
				block_buff, block_size);
			file->is_cache_dirty = false;
		}
	}else{
		ret = ssz_block_fwrite_all_as_orginal(file,
			ch);
		file->is_cache_dirty = false;
	}

	return ret;
}
//if no cache, the buff_size must be equal with the file size, the address must be zero
//if have cache, buff_size<=file_size, address<=file_size
//errors: kChecksumWrong,kNotExist,kError,kInvalidParam,KNotOpen
result_t ssz_block_fread_at(SszBlockFile* file, int16_t address, void* buff, int buff_size)
{
	//check the buff size
	ssz_assert(buff_size <= file->file_size);
	if (buff_size > file->file_size ||
		address + buff_size > file->file_size) {
		return kInvalidParam;
	}
	if (!file->is_opened)
	{
		return KNotOpen;
	}

	result_t ret;
	//write the block
	int block_size = file->file_size;
	void* block_buff = file->file_cache;
	if (block_buff == NULL) {
		//if not use cache, the id must be block, the size must equal with block size
		block_buff = buff;
		block_size = buff_size;
		ssz_assert(buff_size == block_size);
	}

	//if have block cache , enable read cache,  and last read success, only read from cache
	if (file->file_cache != NULL &&
		file->is_enable_read_cache &&
		file->is_last_read_success) {
		ret = kSuccess;
	}
	else {//read from nvram
		ret = ssz_block_fread_orginal(file,
			block_buff, block_size);
	}

	if (ret == kSuccess) {//read success
		file->is_last_read_success = true;
		memcpy(buff, (uint8_t*)block_buff + address, buff_size);
		return ret;
	}
	else {
		//error may be: kError, kChecksumWrong, kNotExist
		file->is_last_read_success = false;
		return ret;
	}
}
//return is cache dirty, if dirty need, the cache is not same as nvram, need flush
bool ssz_block_fdirty(SszBlockFile* file)
{
	return file->is_cache_dirty;
}
result_t ssz_block_fflush(SszBlockFile* file)
{
	result_t ret = kSuccess;
	if (file->is_cache_dirty)
	{
		ssz_assert(file->file_cache);
		ret = ssz_block_fwrite_orginal(file, file->file_cache, file->file_size);
		if (ret ==kSuccess) {
			file->is_cache_dirty = false;
		}

	}

	return ret;
}
//it will erase file_save_area_max_size and set the file opened
result_t ssz_block_fdelete(SszBlockFile* file)
{
	result_t ret = kSuccess;

	ssz_block_file_check_file(file);
	if (file->dev_type == kSszBlockFileNvram) {
		if (((SszNvram*)(file->dev))->is_support_erase)
		{
			ret = ssz_nvram_erase(file->dev, file->file_address, file->file_save_area_max_size);
		}
		else {
			ret = ssz_nvram_fill_with_ex(file->dev, file->file_address,
				0xFF, file->file_save_area_max_size, 
				SSZ_NVRAM_RW_MODE_CHECK_IF_CAN_WRITE | SSZ_NVRAM_RW_MODE_CHECK_IF_WRITE_SUCCESS);
		}
	}
	else if (file->dev_type == kSszBlockFileRecord) {
		ret = ssz_record_erase((SszRecord*)file->dev);
	}
	else {
		ssz_debug_break();
		return kInvalidParam;
	}
	file->is_opened = true;
	file->is_cache_dirty = false;
	file->is_last_read_success = true;
	if (file->file_cache)
	{
		memset(file->file_cache, 0xFF, file->file_size);
	}

	return ret;
}

//return file size
int16_t ssz_block_fsize(SszBlockFile* file)
{
	return file->file_size;
}
void ssz_block_file_enable_read_cache(SszBlockFile* file, bool is_enable)
{
	file->is_enable_read_cache = is_enable;
}
void ssz_block_file_enable_write_cache(SszBlockFile* file, bool is_enable)
{
	file->is_enable_write_cache = is_enable;
}