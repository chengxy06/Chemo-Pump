#include "ssz_record.h"
#include "ssz_common.h"
#include "ssz_utility.h"
/************************************************
* Declaration
************************************************/
#define SSZ_ONE_RECORD_HEAD_SIZE sizeof(SszOneRecordHead)
#define SSZ_RECORD_CHECKSUM_SIZE SSZ_NVRAM_CHECKSUM_SIZE
#define SSZ_RECORD_WRITE_WITH_CHECK_MODE (SSZ_NVRAM_RW_MODE_USE_CHECKSUM|SSZ_NVRAM_RW_MODE_CHECK_IF_CAN_WRITE| \
			SSZ_NVRAM_RW_MODE_CHECK_IF_WRITE_SUCCESS)
#define SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE (SSZ_NVRAM_RW_MODE_CHECK_IF_CAN_WRITE| \
			SSZ_NVRAM_RW_MODE_CHECK_IF_WRITE_SUCCESS)

#define SSZ_RECORD_TEST_READ_EN 0

#define M_record_count_at_one_erase_unit(rc) (rc->nvram->one_erase_unit_size/rc->one_record_size)
#define M_record_head_count_at_one_erase_unit(rc) (rc->nvram->one_erase_unit_size/SSZ_ONE_RECORD_HEAD_SIZE)

typedef struct {
	int32_t record_count;
	int32_t record_curr_index;
	//it is used to count the write total count
	uint32_t curr_sequence_number;
	uint32_t crc32;
}SszOneRecordHead;

static result_t ssz_record_at_address(SszRecord* rc, int32_t address, void* one_record);
/************************************************
* Variable 
************************************************/


/************************************************
* Function 
************************************************/
int32_t ssz_record_head_address(SszRecord* rc, int32_t index) {
	ssz_assert(index >= 0);
	if (rc->nvram->is_need_erase_before_write) {
		return rc->record_head_address +
			index / M_record_head_count_at_one_erase_unit(rc)*rc->nvram->one_erase_unit_size +
			index%M_record_head_count_at_one_erase_unit(rc)*SSZ_ONE_RECORD_HEAD_SIZE;
	}
	else {
		return rc->record_head_address + index*SSZ_ONE_RECORD_HEAD_SIZE;
	}
}
int32_t ssz_record_address(SszRecord* rc, int32_t index) {
	ssz_assert(index >= 0);
	if (rc->nvram->is_need_erase_before_write) {
		return rc->record_address +
			index / M_record_count_at_one_erase_unit(rc)*rc->nvram->one_erase_unit_size +
			index%M_record_count_at_one_erase_unit(rc)*rc->one_record_size;
	}
	else {
		return rc->record_address + index*rc->one_record_size;
	}
}
static result_t ssz_record_init_from_nvram_by_head(SszRecord* rc) {
	int32_t i;
	int32_t address;
	result_t ret;
	//int read_bytes;
	bool is_last_head_ok = true;
	//use record head
	SszOneRecordHead head;
	/*find the null head */
	for (i = 0; i < rc->record_head_max_count; i++)
	{
		address = ssz_record_head_address(rc, i);
		ret = ssz_nvram_read_ex(rc->nvram, address, &head,
			SSZ_ONE_RECORD_HEAD_SIZE - SSZ_RECORD_CHECKSUM_SIZE, SSZ_NVRAM_RW_MODE_USE_CHECKSUM);
		if (ret==kSuccess) {
			rc->record_last_index = head.record_curr_index;
			rc->record_count = head.record_count;
			rc->curr_sequence_number = head.curr_sequence_number;
			is_last_head_ok = true;
		}
		else if (ret == kDataNotInit) {
			//if the data is null, stop read
			break;
		}
		else {
			is_last_head_ok = false;
		}
	}

	//if first is null, check tail if ok
	if (i == 0) {
		ssz_assert(rc->record_head_max_count > 2);
		for (int j = rc->record_head_max_count - 1; j >0; j--)
		{
			//check the pre two 			
			address = ssz_record_head_address(rc, j);
			ret = ssz_nvram_read_ex(rc->nvram, address, &head,
				SSZ_ONE_RECORD_HEAD_SIZE - SSZ_RECORD_CHECKSUM_SIZE, SSZ_NVRAM_RW_MODE_USE_CHECKSUM);
			if (ret==kSuccess) {
				rc->record_last_index = head.record_curr_index;
				rc->record_count = head.record_count;
				rc->curr_sequence_number = head.curr_sequence_number;
				break;
			}
			else if (ret == kDataNotInit) {
				//if the node is null and not the end, stop search
				if(j!=rc->record_head_max_count - 1){
					break;
				}
			}
			else {
				is_last_head_ok = false;
			}
		}
	}

	rc->record_head_next_write_index = i;
	if (rc->record_head_next_write_index >= rc->record_head_max_count) {
		rc->record_head_next_write_index = 0;
	}
	if (rc->record_last_index == -1 && rc->record_count==0) {

	}
	else if (rc->record_last_index < 0 || rc->record_last_index >= rc->record_max_count ||
		rc->record_count<0 || rc->record_count>rc->record_user_max_count) {
		rc->record_last_index = -1;
		rc->record_count = 0;
		ssz_debug_break();//the value is not right
	}

	if (is_last_head_ok) {
		return kSuccess;
	}
	else {
		return kSszRecordLastHeadBroken;
	}
}
static bool ssz_record_init_from_nvram_by_record(SszRecord* rc) {
	int32_t i;
	int32_t address;
#if SSZ_RECORD_TEST_READ_EN
	uint8_t* data = ssz_tmp_buff_alloc(kTempAllocCommonBuff, rc->one_record_size);
	result_t read_ret;
#endif
	//not use record head

	//find the null record
	for (i = 0; i < rc->record_max_count; i++) {
		address = ssz_record_address(rc, i);
		if (ssz_nvram_check_if_is_byte(rc->nvram, address, rc->one_record_size, 0xFF) == true) {
			//if meet null, stop read
			break;
		}
		else {
#if SSZ_RECORD_TEST_READ_EN
			//check it when at debug mode
			read_ret = ssz_record_at_address(rc, address, data);
			if (read_ret != kSuccess) {
				if (read_ret==kChecksumWrong) {
					printf("init record[0x%x] checksum wrong\n", address);
				}
				else {
					printf("init record[0x%x] not right", address);
				}
			}
#endif
		}
	}
	if (i > 0) {
		rc->record_last_index = i - 1;
	}
	rc->record_count = i;
	if (rc->record_count > rc->record_user_max_count) {
		rc->record_count = rc->record_user_max_count;
	}
	//if first is null, check tail if ok
	if (i == 0) {
		ssz_assert(rc->record_max_count > 2);
		for (int j = rc->record_max_count - 1; j >= rc->record_max_count - 2; j--)
		{
			address = ssz_record_address(rc, j);
			if (ssz_nvram_check_if_is_byte(rc->nvram, address, rc->one_record_size, 0xFF) == false) {
				rc->record_last_index = j;
				rc->record_count++;
				break;
			}
		}
#if SSZ_RECORD_TEST_READ_EN
		for (int j = rc->record_max_count - 1; j >= 0; j--)
		{
			address = ssz_record_address(rc, j);
			if (ssz_nvram_check_if_is_byte(rc->nvram, address, rc->one_record_size, 0xFF)) {
				break;
			}
			else {
				//check it when at debug mode
				read_ret = ssz_record_at_address(rc, address, data);
				if (read_ret != kSuccess) {
					if (read_ret == kChecksumWrong) {
						printf("init record[0x%x] checksum wrong\n", address);
					}
					else {
						printf("init record[0x%x] not right", address);
					}
				}
			}
		}
#endif
	}

	//rc->record_next_write_index = i;
	//if (rc->record_next_write_index >= rc->record_max_count) {
	//	rc->record_next_write_index = 0;
	//}
#if SSZ_RECORD_TEST_READ_EN
	if (data) {
		ssz_tmp_buff_free(data);
	}
#endif
	return kSuccess;
}
static result_t ssz_record_init_from_nvram(SszRecord* rc) {

	//use record head
	if (rc->record_head_max_count > 0) {
		return ssz_record_init_from_nvram_by_head(rc);
	}
	//not use record head
	else {
		return ssz_record_init_from_nvram_by_record(rc);
	}
}

void ssz_record_struct_init(SszRecord* rc, SszNvram* nvram,
	int32_t record_area_address, int32_t record_area_size, int one_record_size, bool is_need_checksum,
	bool is_need_record_head, int32_t record_head_area_address, int record_head_area_size) {
	
	ssz_mem_zero(rc, sizeof(SszRecord));
	//add 4 byte to one record if need checksum
	if (is_need_checksum) {
		one_record_size += SSZ_RECORD_CHECKSUM_SIZE;
	}
	rc->is_need_checksum = is_need_checksum;
	rc->nvram = nvram;
	rc->one_record_size = one_record_size;
	rc->record_address = record_area_address;
	rc->record_area_size = record_area_size;
	//if need save record_head, check the record_head area is not conflict with record area
	if (is_need_record_head) {
		ssz_assert(record_head_area_address <= record_area_address ||
			record_head_area_address >= record_area_address + record_area_size);
		ssz_assert(record_head_area_address + record_head_area_size <=record_area_address ||
			record_head_area_address + record_head_area_size >= record_area_address+record_area_size);
		rc->record_head_address = record_head_area_address;
		rc->record_head_area_size = record_head_area_size;
	}

	if (rc->nvram->is_need_erase_before_write) {
		ssz_assert(record_area_address%rc->nvram->one_erase_unit_size == 0);
		ssz_assert(record_area_size%rc->nvram->one_erase_unit_size == 0);
		//the record area must have a cache unit
		ssz_assert(record_area_size>=rc->nvram->one_erase_unit_size*2);
		ssz_assert(one_record_size <= rc->nvram->one_erase_unit_size);

		//the last erase unit used to avoid lost data when it is full and need add new
		rc->record_max_count = (record_area_size/rc->nvram->one_erase_unit_size) *
			M_record_count_at_one_erase_unit(rc);
		rc->record_user_max_count = rc->record_max_count - M_record_count_at_one_erase_unit(rc);
		if (is_need_record_head) {
			ssz_assert(record_head_area_address%rc->nvram->one_erase_unit_size == 0);
			ssz_assert(record_head_area_size%rc->nvram->one_erase_unit_size == 0);
			//suggest the record_head area have more than one unit
			//so when erase new unit and then crashed, the data will not lost
			rc->record_head_max_count = (record_head_area_size / rc->nvram->one_erase_unit_size) *
				M_record_head_count_at_one_erase_unit(rc);
		}
		else {
		    //because the write node is null, 
			//and when before write, need set next node as null
			//so there are one node is null and one erase unit is null
			rc->record_user_max_count--;
		}

	}
	else {
		rc->record_max_count = record_area_size / one_record_size;
		rc->record_user_max_count = rc->record_max_count;
		if (!is_need_record_head) {
		    //because the write node is null, 
			//and when before write, need set next node as null
			//so there are two is null
			rc->record_user_max_count -= 2;
		}
		if (record_area_address %one_record_size != 0) {
			ssz_traceln("record area not all used");
		}
		if (is_need_record_head) {
			rc->record_head_max_count = record_head_area_size / SSZ_ONE_RECORD_HEAD_SIZE;
			if (record_head_area_size %SSZ_ONE_RECORD_HEAD_SIZE != 0) {
				ssz_traceln("index area not all used");
			}
		}
	}

	rc->record_last_index = -1;

	ssz_assert(rc->record_max_count >= 3);
	if (rc->record_head_max_count>0) {
		ssz_assert(rc->record_head_max_count >= 3);
	}
	
}


//read and init the record
//errors: kSuccess,kSszRecordLastHeadBroken
result_t ssz_record_load(SszRecord* rc)
{
	rc->record_last_index = -1;
	rc->record_count = 0;
	rc->record_head_next_write_index = 0;
	rc->curr_sequence_number = 0;
	return ssz_record_init_from_nvram(rc);
}
//judge is empty
bool ssz_record_is_empty(SszRecord* rc) {
	if (rc->record_count==0) {
		return true;
	}
	else {
		return false;
	}
}

int32_t ssz_record_size(SszRecord* rc) {
	return rc->record_count;
}

//get record at address
//errors:	maybe kSuccess,kChecksumWrong,kInvalidParam,kError
static result_t ssz_record_at_address(SszRecord* rc, int32_t address, void* one_record) {
	if (rc->is_need_checksum) {
		return ssz_nvram_read_ex(rc->nvram, address, one_record,
			rc->one_record_size - SSZ_RECORD_CHECKSUM_SIZE, SSZ_NVRAM_RW_MODE_USE_CHECKSUM);
	}
	else {
		return ssz_nvram_read_ex(rc->nvram, address, one_record,
			rc->one_record_size, 0);
	}
}

//get the record at the index, index range is 0~size-1
//errors:	maybe kSuccess,kChecksumWrong,kInvalidParam,kError
result_t ssz_record_at(SszRecord* rc, int32_t index, void* one_record) {
	if (rc->record_count<=0  || index >= rc->record_count || index<0) {
		return kInvalidParam;
	}
	else {
		int32_t real_index = rc->record_last_index+1 - rc->record_count + index;

		if (real_index<0) {
			real_index += rc->record_max_count;
		}

		int32_t address;

		address = ssz_record_address(rc, real_index);
		return ssz_record_at_address(rc, address, one_record);
	}
}

//return top record
//errors:	maybe kSuccess,kChecksumWrong,kNotExist,kError
result_t ssz_record_top(SszRecord* rc, void* one_record) {
	if (rc->record_count==0) {
		return kNotExist;
	}
	else
	{
		return ssz_record_at(rc, rc->record_count - 1, one_record);
	}
}

static result_t ssz_record_update_head(SszRecord* rc, SszOneRecordHead* head) {
	int32_t address;
	int32_t next_next_write_index = rc->record_head_next_write_index+1;
	result_t ret;

	if (next_next_write_index >= rc->record_head_max_count) {
		next_next_write_index = 0;
	}

	if (rc->nvram->is_need_erase_before_write) {
		//if write at the end of one unit, need erase the next unit first
		if (next_next_write_index%M_record_head_count_at_one_erase_unit(rc) ==0) {
			address = ssz_record_head_address(rc, next_next_write_index);
			ssz_nvram_erase_one_unit(rc->nvram, address);
		}
	}
	else
	{
		//set next as null first
		SszOneRecordHead tmp;
		memset(&tmp, 0xFF, sizeof(tmp));
		address = ssz_record_head_address(rc, next_next_write_index);
		ret = ssz_nvram_write_ex(rc->nvram, address, &tmp, SSZ_ONE_RECORD_HEAD_SIZE, 
			SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE);

		//if write fail, return
		if (ret != kSuccess)
		{
			return ret;
		}
	}

	//write the head
	address = ssz_record_head_address(rc, rc->record_head_next_write_index);
	ret = ssz_nvram_write_ex(rc->nvram, address, head, SSZ_ONE_RECORD_HEAD_SIZE-SSZ_RECORD_CHECKSUM_SIZE, 
		SSZ_RECORD_WRITE_WITH_CHECK_MODE);

	//if write fail, return
	if (ret != kSuccess)
	{
		return ret;
	}
	//update record head next write index
	rc->record_head_next_write_index = next_next_write_index;

	return kSuccess;
}
//push one record 
//errors:	maybe kSuccess,kError
result_t ssz_record_push(SszRecord* rc, const void* one_record) {
	int32_t record_next_write_index;
	int32_t next_next_write_index;
	int32_t address;
	int32_t record_count;
	result_t ret;

	//calc the next write index
	record_next_write_index = rc->record_last_index + 1;
	if (record_next_write_index>=rc->record_max_count) {
		record_next_write_index = 0;
	}

	//calc the next next write index
	next_next_write_index = record_next_write_index + 1;
	if (next_next_write_index >= rc->record_max_count) {
		next_next_write_index = 0;
	}
	

	if(rc->record_head_max_count==0){ //no head
		if (rc->nvram->is_need_erase_before_write) {
			//if no head, when write at the end of one unit, need erase the next unit first
			if (next_next_write_index%M_record_count_at_one_erase_unit(rc) == 0) {
				address = ssz_record_address(rc, next_next_write_index);
				ssz_nvram_erase_one_unit(rc->nvram, address);
			}
		}
		else
		{
			//if no head, need set next as null first
			address = ssz_record_address(rc, next_next_write_index);
			ret = ssz_nvram_fill_with_ex(rc->nvram, address, 0xFF, rc->one_record_size, SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE);
			//if write fail, return
			if (ret != kSuccess)
			{
				return ret;
			}
		}
	}
	else if(rc->record_head_max_count>0 && rc->nvram->is_need_erase_before_write) //have head
	{
		//if at begin of one unit, need erase the unit first
		if (record_next_write_index%M_record_count_at_one_erase_unit(rc) == 0) {
			address = ssz_record_address(rc, record_next_write_index);
			ssz_nvram_erase_one_unit(rc->nvram, address);
		}
	}

	address = ssz_record_address(rc, record_next_write_index);
	//write the record
	if (rc->is_need_checksum) {
		ret = ssz_nvram_write_ex(rc->nvram, address, one_record,
			rc->one_record_size - SSZ_RECORD_CHECKSUM_SIZE, SSZ_RECORD_WRITE_WITH_CHECK_MODE);
	}
	else {
		ret = ssz_nvram_write_ex(rc->nvram, address, one_record,
			rc->one_record_size, SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE);
	}
	//if write fail, return
	if (ret != kSuccess)
	{
		return ret;
	}
	record_count = rc->record_count;
	if (record_count < rc->record_user_max_count) {
		record_count++;
	}

	//if have head, update head
	if (rc->record_head_max_count > 0) {
		SszOneRecordHead head;
		head.record_count = record_count;
		head.record_curr_index = record_next_write_index;
		head.curr_sequence_number = rc->curr_sequence_number + 1;
		ret = ssz_record_update_head(rc, &head);
		if(ret != kSuccess)
		{
			return ret;
		}
	}

	//update record last index and record count
	rc->record_last_index = record_next_write_index;
	rc->record_count = record_count;
	rc->curr_sequence_number++;

	return kSuccess;
}

//errors: maybe kSuccess,kError
result_t ssz_record_clear(SszRecord* rc) {
	if (rc->record_count<=0) {
		return kSuccess;
	}
	result_t ret;
	if (rc->record_head_max_count>0) {
		//if have head, write a new head to clear it
		SszOneRecordHead head;
		head.record_count = 0;
		head.record_curr_index = rc->record_last_index;
		head.curr_sequence_number = rc->curr_sequence_number + 1;
		ret = ssz_record_update_head(rc, &head);
		if (ret !=kSuccess) {
			return ret;
		}
	}
	else {
		if (rc->nvram->is_need_erase_before_write) {
			//erase first ,last and pre last
			ssz_nvram_erase_one_unit(rc->nvram, rc->record_address);
			int32_t address_last;
			int32_t address_pre_last;
			address_last = ssz_record_address(rc, rc->record_max_count-1)/rc->nvram->one_erase_unit_size*
				rc->nvram->one_erase_unit_size;
			if (address_last != rc->record_address) {
				ssz_nvram_erase_one_unit(rc->nvram, address_last);
			}
			address_pre_last = ssz_record_address(rc, rc->record_max_count - 2) / rc->nvram->one_erase_unit_size*
				rc->nvram->one_erase_unit_size;
			if (address_pre_last != address_last && address_pre_last!= rc->record_address) {
				ssz_nvram_erase_one_unit(rc->nvram, address_pre_last);
			}
		}
		else {
			//set first and last, pre last as null
			ret = ssz_nvram_fill_with_ex(rc->nvram, rc->record_address, 0xFF,
				rc->one_record_size, SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE);
			if (ret !=kSuccess)
			{
				return ret;
			}
			ret = ssz_nvram_fill_with_ex(rc->nvram, ssz_record_address(rc, rc->record_max_count - 1),
				0xFF, rc->one_record_size, SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE);
			if (ret != kSuccess) {
				return ret;
			}
			ret = ssz_nvram_fill_with_ex(rc->nvram, ssz_record_address(rc, rc->record_max_count - 2),
				0xFF, rc->one_record_size, SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE);
			if (ret != kSuccess) {
				return ret;
			}
		}

		rc->record_last_index = -1;
	}

	rc->record_count = 0;
	rc->curr_sequence_number++;
	return kSuccess;
}

// it will erase the all area,
// the difference from ssz_record_clear is that:
//	clear only update head or write some null record to let next read record size is 0
//	erase will erase all save area(include head and record area)
// errors:	maybe kSuccess,kError
result_t ssz_record_erase(SszRecord* rc)
{
	if (rc->record_head_max_count > 0) {
		//if have head, clear head area
		if (rc->nvram->is_support_erase) {
			//if support erase, erase it
			ssz_nvram_erase(rc->nvram, rc->record_head_address, rc->record_head_area_size);
		}
		else
		{
			//if not support erase, write all 0xFF
			ssz_nvram_fill_with_ex(rc->nvram, rc->record_head_address,
				0xFF, rc->record_head_area_size, SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE);
		}
	}

	if (rc->nvram->is_support_erase) {
		//if support erase, erase it
		ssz_nvram_erase(rc->nvram, rc->record_address, rc->record_area_size);
	}
	else {
		//if not support erase, write all 0xFF
		ssz_nvram_fill_with_ex(rc->nvram, rc->record_address,
			0xFF, rc->record_area_size, SSZ_RECORD_WRITE_WITH_NO_CHECK_MODE);
	}


	rc->record_last_index = -1;
	rc->record_count = 0;
	rc->record_head_next_write_index = 0;
	rc->curr_sequence_number = 0;
	return kSuccess;
}
