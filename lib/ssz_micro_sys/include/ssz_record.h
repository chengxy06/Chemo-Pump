#pragma once
#include "ssz_def.h"
#include "ssz_nvram.h"
/************************************************
* Declaration
************************************************/
typedef enum {
	//the last head is broken, use the prev right head
	kSszRecordLastHeadBroken = kUserCustomErrorStart
}SszRecordError;

typedef struct{
	SszNvram* nvram;

	int32_t record_address; //record address at nvram
	int32_t record_area_size;

	int32_t record_max_count;
	int32_t record_user_max_count;//when at flash, the user_max_count=max_count-count_at_one_erase_unit

	int32_t record_head_address; //record head address at nvram
	int32_t record_head_area_size;
	int32_t record_head_max_count;


	int32_t record_last_index; //the last record index, when init from null area, it is -1
	//when no use head, the count will only be used for 0 or >0
	int32_t record_count; 
	//int32_t record_next_write_index;
	int32_t record_head_next_write_index; //when init from null area, it is 0
	/*the sequence number , it will be auto increased when write.*/
	//it is used to count the write total count, only valid when use head
	uint32_t curr_sequence_number;
	int one_record_size;
	bool is_need_checksum;//is the record need checksum

}SszRecord;

#ifdef __cplusplus
extern "C" {
#endif

//Note: one head is 16 bytes, so the max head is record_head_area_size/16
void ssz_record_struct_init(SszRecord* rc, SszNvram* nvram,
	int32_t record_area_address, int32_t record_area_size, int one_record_size, bool is_need_checksum,
	bool is_need_record_head, int32_t record_head_area_address, int record_head_area_size);

//read and init the record
// errors: kSuccess,kSszRecordLastHeadBroken
result_t ssz_record_load(SszRecord* rc);

//judge is empty
bool ssz_record_is_empty(SszRecord* rc);

int32_t ssz_record_size(SszRecord* rc);

//get the record at the index, index range is 0~size-1
//errors: kSuccess,kChecksumWrong,kInvalidParam,kError
result_t ssz_record_at(SszRecord* rc, int32_t index, void* one_record);

//return top record
//errors:	maybe kSuccess,kChecksumWrong,kNotExist,kError
result_t ssz_record_top(SszRecord* rc, void* one_record);

//push one record 
//errors: maybe kSuccess,kError
result_t ssz_record_push(SszRecord* rc, const void* one_record);	

//errors: maybe kSuccess,kError
result_t ssz_record_clear(SszRecord* rc);

// it will erase the all area,
// the difference from ssz_record_clear is that:
//	clear only update head or write some null record to let next read record size is 0
//	erase will erase all save area(include head and record area)
// errors:	maybe kSuccess,kError
result_t ssz_record_erase(SszRecord* rc);

#ifdef __cplusplus
}
#endif