#include "ssz_nvram.h"
#include "ssz_common.h"
#include "ssz_algorithm.h"
#include "ssz_utility.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

//the unit_cache's size must be equal with the erase unit size
void ssz_nvram_init(SszNvram* dev, SszFile* f, bool is_need_backup, int try_count, uint8_t rw_mode) {
	int tmp;

	ssz_mem_zero(dev, sizeof(SszNvram));
	dev->file = f;

	if (ssz_is_result_ok(ssz_ioctl(f, SSZ_IOCTL_IS_SUPPORT_ERASE, &tmp))) {
		dev->is_support_erase = tmp == 1 ? true : false;
	}
	if (ssz_is_result_ok(ssz_ioctl(f, SSZ_IOCTL_IS_NEED_ERASE_BEFORE_WRITE, &tmp))) {
		dev->is_need_erase_before_write = tmp == 1 ? true : false;
	}
	if (ssz_is_result_ok(ssz_ioctl(f, SSZ_IOCTL_ERASE_UNIT_SIZE, &tmp))) {
		dev->one_erase_unit_size = tmp;
	}
	ssz_ioctl(f, SSZ_IOCTL_SIZE, &dev->size);
	dev->try_count = try_count;
	dev->is_need_backup = is_need_backup;
	if (is_need_backup) {
		ssz_tmp_buff_check_size(kTempAllocBackupBuffWhenWriteFlash, dev->one_erase_unit_size);
	}
	if (dev->try_count <1) {
		dev->try_count = 1;
	}
	dev->rw_mode = rw_mode;
}

//write according the param:write_mode
// valid error codes:kError,kSuccess
// write_mode: 
//		SSZ_NVRAM_WR_MODE_USE_CHECKSUM: will write the checksum(4 bytes) which after the data
//		SSZ_NVRAM_WR_MODE_CHECK_IF_CAN_WRITE: will check if the destination area can write, 
//			if can not write, it will read back, erase,then write, 
//			so it will not effect other data(is_need_backup must be true)
//		SSZ_NVRAM_WR_MODE_CHECK_IF_WRITE_SUCCESS: after write, will read some bytes to check if write success
result_t ssz_nvram_write_ex(SszNvram* dev, int32_t address, const void* buff, int buff_size, uint8_t write_mode)
{
	int try_count = 0;
	int error_code = kError;
	uint32_t crc_32 = 0;
	int need_write_size = buff_size;
	bool is_need_check_if_write_success = false;
	bool is_need_checksum = false;
	bool is_need_check_if_can_write =false;

	if (write_mode&SSZ_NVRAM_RW_MODE_USE_CHECKSUM) {
		is_need_checksum = true;
	}

	if (write_mode&SSZ_NVRAM_RW_MODE_CHECK_IF_WRITE_SUCCESS) {
		is_need_check_if_write_success= true;
	}

	if (write_mode&SSZ_NVRAM_RW_MODE_CHECK_IF_CAN_WRITE) {
		is_need_check_if_can_write= true;
	}


	//used to read some bytes to check if write ok
	uint8_t read_to_check_buff[4];
	int32_t read_to_check_address;
	int read_to_check_size = 4;

	if (is_need_checksum) {
		crc_32 = ssz_crc32(buff, buff_size);
		need_write_size = buff_size + SSZ_NVRAM_CHECKSUM_SIZE;
		read_to_check_size = 4;
		read_to_check_address = address + buff_size;
	}
	else {
		if (buff_size < 4) {
			read_to_check_size = buff_size;
		}
		else {
			read_to_check_size = 4;
		}
		read_to_check_address = address + buff_size - read_to_check_size;
	}

	//try to write
	for (try_count = 0; try_count < dev->try_count; try_count++) {
		if (is_need_check_if_can_write && dev->is_need_erase_before_write) {
			//erase first
			ssz_nvram_erase(dev, address, need_write_size);
		}

		//write
		if (ssz_fwrite_at(dev->file, address, buff, buff_size)!=kSuccess) {
			continue;
		}

		//if need checksum, write the checksum
		if (is_need_checksum &&
			ssz_fwrite_at(dev->file, address + buff_size,
				&crc_32, SSZ_NVRAM_CHECKSUM_SIZE) != kSuccess) {
			continue;
		}

		//read some to check if write ok
		if (is_need_check_if_write_success&&
			ssz_fread_at(dev->file, read_to_check_address, read_to_check_buff, read_to_check_size)==kSuccess) {
			if (is_need_checksum) {
				if (memcmp(&crc_32, read_to_check_buff, read_to_check_size) == 0) {
					error_code = kSuccess;
					break;
				}
			}
			else {
				if (memcmp((uint8_t*)buff + buff_size - read_to_check_size, read_to_check_buff, read_to_check_size) == 0) {
					error_code = kSuccess;
					break;
				}
			}
		}
		else {
			error_code = kSuccess;
			break;
		}


	}

	return error_code;
}

//write according the read/write mode which define at init
// valid error codes:kError,kSuccess
result_t ssz_nvram_write(SszNvram* dev, int32_t address, const void* buff, int buff_size) {
	return ssz_nvram_write_ex(dev, address, buff, buff_size, dev->rw_mode);
}
//write the address with the size bytes which all is the onebyte,
// valid error codes:kError,kSuccess
result_t ssz_nvram_fill_with(SszNvram* dev, int32_t address, uint8_t onebyte, int size)
{
	return ssz_nvram_fill_with_ex(dev, address, onebyte, size, dev->rw_mode);
}
//write the address with the size bytes which all is the onebyte,
// valid error codes:kError,kSuccess
// write_mode: 
//		SSZ_NVRAM_WR_MODE_USE_CHECKSUM: will write the checksum(4 bytes) which after the data
//		SSZ_NVRAM_WR_MODE_CHECK_IF_CAN_WRITE: will check if the destination area can write, 
//			if can not write, it will read back, erase,then write,
//			so it will not effect other data(is_need_backup must be true)
//		SSZ_NVRAM_WR_MODE_CHECK_IF_WRITE_SUCCESS: after write, will read some bytes to check if write success
result_t ssz_nvram_fill_with_ex(SszNvram* dev, int32_t address, uint8_t onebyte, int size, uint8_t write_mode) {
	uint32_t crc32 = 0;
	bool is_need_checksum = false;
	int real_size  = size;

	ssz_assert(size > 0);
	if (write_mode&SSZ_NVRAM_RW_MODE_USE_CHECKSUM) {
		is_need_checksum = true;
		real_size += SSZ_NVRAM_CHECKSUM_SIZE;
	}
	if ((write_mode&SSZ_NVRAM_RW_MODE_CHECK_IF_CAN_WRITE) && dev->is_need_erase_before_write) {
		//erase first
		ssz_nvram_erase(dev, address, real_size);
	}
	
	if (is_need_checksum)
	{
		crc32 = SSZ_CRC32_SEED;
	}
	uint8_t write_buff[16];
	memset(write_buff, onebyte, sizeof(write_buff));
	int real_write_size = 0;
	int once_write_size;
	//read data
	while (real_write_size < size)
	{
		once_write_size = size - real_write_size;
		if (once_write_size > ssz_array_size(write_buff)) {
			once_write_size = ssz_array_size(write_buff);
		}
		
		if (ssz_fwrite_at(dev->file, address + real_write_size, write_buff, once_write_size) != kSuccess) {
			return kError;
		}
		if (is_need_checksum)
		{
			crc32 = ssz_crc32_ex(&write_buff, once_write_size, crc32);
		}
		real_write_size += once_write_size;
	}

	if (is_need_checksum)
	{
		crc32 = crc32^SSZ_CRC32_SEED;
		if (ssz_fwrite_at(dev->file, address + size, &crc32, 4) != kSuccess) {
			return kError;
		}
	}

	if (write_mode & SSZ_NVRAM_RW_MODE_CHECK_IF_WRITE_SUCCESS)
	{
		uint8_t read_check;
		if (ssz_fread_at(dev->file, address, &read_check, 1) != kSuccess||
			read_check != onebyte) {
			return kError;
		}

	}
	return kSuccess;
}

//read according the read/write mode which define at init
// valid error codes:kSuccess,kError,kChecksumWrong(only at checksum mode),kDataNotInit(only at checksum mode)
result_t ssz_nvram_read(SszNvram* dev, int32_t address, void* buff, int buff_size) {
	return ssz_nvram_read_ex(dev, address, buff, buff_size, dev->rw_mode);
}
//return according the param:read_mode
// valid error codes:kSuccess,kError,kChecksumWrong(only at checksum mode),kDataNotInit(only at checksum mode)
// read_mode:
//	SSZ_NVRAM_WR_MODE_USE_CHECKSUM: will read the checksum(4 bytes) which after the data
result_t ssz_nvram_read_ex(SszNvram* dev, int32_t address, void* buff, int buff_size, uint8_t read_mode) {
	int try_count = 0;
	uint32_t crc_32;
	int error_code = kError;
	bool is_need_checksum = false;

	if (read_mode&SSZ_NVRAM_RW_MODE_USE_CHECKSUM) {
		is_need_checksum = true;
	}
	for (try_count = 0; try_count < dev->try_count; try_count++) {
		if (ssz_fread_at(dev->file, address, buff, buff_size)==kSuccess){
			//check if need checksum
			if (is_need_checksum) {
				//read the checksum
				if (ssz_fread_at(dev->file, address + buff_size,
					&crc_32, SSZ_NVRAM_CHECKSUM_SIZE)==kSuccess) {
					//check if crc32 is right
					if (ssz_crc32(buff, buff_size) == crc_32) {
						error_code = kSuccess;
						break;
					}
					//or it is not init
					else if (ssz_is_all_byte_as(buff, buff_size, 0xFF) &&
						ssz_is_all_byte_as(&crc_32, SSZ_NVRAM_CHECKSUM_SIZE, 0xFF)) {
						error_code = kDataNotInit;
						break;
					}
					else {
						error_code = kChecksumWrong;
					}
				}
			}
			else {
				error_code = kSuccess;
				break;
			}

		}
	}

	return error_code;
}

//check if the buff is the byte
bool ssz_nvram_check_if_is_byte(SszNvram* dev, int32_t address, int need_check_size, uint8_t dest_byte){
	int real_read_size = 0;
	uint8_t read_buff[16];
	int once_read_size;
	bool is_find_avalid_byte = false;//valid byte is not 0xFF
	
	//read data
	while (real_read_size < need_check_size)
	{
		once_read_size = need_check_size - real_read_size;
		if (once_read_size > ssz_array_size(read_buff)) {
			once_read_size = ssz_array_size(read_buff);
		}
		ssz_fread_at(dev->file, address+real_read_size, read_buff, once_read_size);
		if (!ssz_is_all_byte_as(read_buff, once_read_size, dest_byte)) {
			is_find_avalid_byte = true;
			break;
		}
		real_read_size += once_read_size;
	}

	if (is_find_avalid_byte)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//check the specify area is erased
bool ssz_nvram_is_erased(SszNvram* dev, int32_t address, int need_check_size) {
	return ssz_nvram_check_if_is_byte(dev, address, need_check_size, 0xFF);
}
//erase one unit, if the dev not support erase, will return false
result_t ssz_nvram_erase_one_unit(SszNvram* dev, int32_t address) {
	if (!dev->is_support_erase) {
		return kNotSupport;
	}	
	return ssz_ioctl(dev->file, SSZ_IOCTL_ERASE, &address);
}

//it erase the specify area at one unit
static void ssz_nvram_erase_at_one_unit(SszNvram* dev, int32_t address, int buffer_len)
{
	int32_t address_of_unit_begin;//address of one unit begin
	int front_size_of_erase;//the size at the front of the erase area
	int behind_size_of_erase;//the size behind the erase area

	ssz_assert(address >= 0 && address + buffer_len <= dev->size&&
		address%dev->one_erase_unit_size + buffer_len <= dev->one_erase_unit_size);

	//check if the destination area is erased.
	if (ssz_nvram_is_erased(dev, address, buffer_len))
	{
		return;
	}

	//calc the address of the unit begin
	address_of_unit_begin = address / dev->one_erase_unit_size*dev->one_erase_unit_size;
	//if need erase one unit, erase it and return
	if (address_of_unit_begin == address && buffer_len == dev->one_erase_unit_size)
	{
		ssz_nvram_erase_one_unit(dev, address_of_unit_begin);
		return;
	}

	//calc the front and behind size of the erase area
	front_size_of_erase = address - address_of_unit_begin;
	behind_size_of_erase = dev->one_erase_unit_size - front_size_of_erase - buffer_len;

	uint8_t *one_unit_buff = ssz_tmp_buff_alloc(kTempAllocBackupBuffWhenWriteFlash,dev->one_erase_unit_size);
	if (front_size_of_erase)
	{
		//read front area
		ssz_fread_at(dev->file, address_of_unit_begin, one_unit_buff, front_size_of_erase);
	}
	if (behind_size_of_erase)
	{
		//read behind area
		ssz_fread_at(dev->file, address + buffer_len, one_unit_buff + front_size_of_erase + buffer_len, behind_size_of_erase);
	}

	//erase the unit
	ssz_nvram_erase_one_unit(dev, address_of_unit_begin);

	if (front_size_of_erase)
	{
		//write front area
		ssz_fwrite_at(dev->file, address_of_unit_begin, one_unit_buff, front_size_of_erase);
	}

	if (behind_size_of_erase)
	{
		//write behind area
		ssz_fwrite_at(dev->file, address + buffer_len, one_unit_buff + front_size_of_erase + buffer_len, behind_size_of_erase);
	}
	ssz_tmp_buff_free(one_unit_buff);
}

//it only erase the specify area
result_t ssz_nvram_erase(SszNvram* dev, int32_t address, int need_erase_size) {
	if (!dev->is_support_erase) {
		return kNotSupport;
	}
	int need_erase_size_once; //once erase size
	int curr_erase_size = 0; //current erase size total
	int32_t need_erase_flash_address;

	//check if erase finish, if not erase continue.
	while (curr_erase_size < need_erase_size)
	{
		//calc erase flash address
		need_erase_flash_address = address + curr_erase_size;
		//calc once erase write size
		need_erase_size_once = need_erase_size - curr_erase_size;
		//check if it exceed one sector
		if (need_erase_size_once > 
			dev->one_erase_unit_size - need_erase_flash_address%dev->one_erase_unit_size)
		{
			//it is exceed, reduce the once erase size to at one sector
			need_erase_size_once = dev->one_erase_unit_size - 
				need_erase_flash_address%dev->one_erase_unit_size;
		}
		if (dev->is_need_backup) {
			ssz_nvram_erase_at_one_unit(dev, need_erase_flash_address, need_erase_size_once);
		}
		else {
			int32_t address_of_unit_begin = need_erase_flash_address / dev->one_erase_unit_size*
				dev->one_erase_unit_size;
			ssz_nvram_erase_one_unit(dev, address_of_unit_begin);
		}		

		curr_erase_size += need_erase_size_once;
	}

	return kSuccess;
}

//it will erase the unit if write to new unit or write at unit begin
//Note: the write range must at 1~2 unit
result_t ssz_nvram_write_and_erase_new_unit_if_need(SszNvram* dev, int32_t address, const void* buff, int buff_size){
	if (dev->is_support_erase) {
		int curr_unit = address / dev->one_erase_unit_size;
		int next_unit = (address + buff_size - 1) / dev->one_erase_unit_size;

		ssz_assert(buff_size <= dev->one_erase_unit_size * 2);
		//check if at begin of sector
		if (address%dev->one_erase_unit_size == 0)
		{
			//if at begin, erase it first
			ssz_nvram_erase_one_unit(dev, address);
		}
		//check if write at two sector
		else if (curr_unit != next_unit)
		{
			//if write to new sector, need erase it
			ssz_nvram_erase_one_unit(dev, next_unit*dev->one_erase_unit_size);
		}
	}
	return ssz_nvram_write(dev, address, buff, buff_size);
}
