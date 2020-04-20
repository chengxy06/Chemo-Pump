#include "ssz_common.h"
//#include "string.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static int g_last_error_code = 0;
static uint8_t g_common_tmp_buff[SSZ_COMMON_TMP_BUFF_MAX_SIZE];
#if !defined(SSZ_BACKUP_TMP_BUFF_WHEN_WRITE_FLASH) || SSZ_BACKUP_TMP_BUFF_WHEN_WRITE_FLASH==0 
static uint8_t g_backup_tmp_buff_when_write_flash[1];
#else
static uint8_t g_backup_tmp_buff_when_write_flash[SSZ_BACKUP_TMP_BUFF_WHEN_WRITE_FLASH];
#endif
static bool g_tmp_buff_is_used[kTempAllocBuffTypeMax] = {0};
/************************************************
* Function 
************************************************/

 //zero memory
void ssz_mem_zero(void* buff, int buff_size){
	memset(buff, 0, buff_size);
}

//set the last error code
void ssz_set_last_error(int error_code)
{
	g_last_error_code = error_code;
}
//get the last error code
int ssz_last_error()
{
	return g_last_error_code;
}


//check if the tmp buff is >=alloc_size
void ssz_tmp_buff_check_size(TempAllocBuffType alloc_type, int alloc_size)
{
	switch (alloc_type) {
	case kTempAllocBackupBuffWhenWriteFlash:
		ssz_check(ssz_array_size(g_backup_tmp_buff_when_write_flash) >= alloc_size);
		break;
	case kTempAllocCommonBuff:
		ssz_check(ssz_array_size(g_common_tmp_buff) >= alloc_size);
		break;
	default:
		ssz_assert_fail();
		break;
	}
}
//alloc tmp buff, once can only one use it
void* ssz_tmp_buff_alloc(TempAllocBuffType alloc_type, int alloc_size)
{
	ssz_check(g_tmp_buff_is_used[alloc_type] == false);
	g_tmp_buff_is_used[alloc_type] = true;
	switch (alloc_type) {
	case kTempAllocBackupBuffWhenWriteFlash:
		ssz_check(ssz_array_size(g_backup_tmp_buff_when_write_flash) >= alloc_size);
		return g_backup_tmp_buff_when_write_flash;
		break;
	case kTempAllocCommonBuff:
		ssz_check(ssz_array_size(g_common_tmp_buff) >= alloc_size);
		return g_common_tmp_buff;
		break;
	}
	ssz_assert_fail();
	return NULL;
}
//free tmp buff
void ssz_tmp_buff_free(void* alloc_buff) {
	TempAllocBuffType alloc_type = kTempAllocCommonBuff;

	if (alloc_buff == g_backup_tmp_buff_when_write_flash) {
		alloc_type = kTempAllocBackupBuffWhenWriteFlash;
	}
	else if (alloc_buff == g_common_tmp_buff) {
		alloc_type = kTempAllocCommonBuff;
	}else{
		ssz_check(0);
	}
	g_tmp_buff_is_used[alloc_type] = false;
}

#ifdef DEBUG
//break system when at debug mode
void ssz_debug_break()
{
	ssz_assert(0);//lint !e506
}
#endif