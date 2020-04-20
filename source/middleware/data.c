#include "data.h"
#include "ssz_record.h"
#include "drv_sst25_flash.h"
#include "ssz_common.h"
#include "ssz_block_file.h"
#include "timer.h"
#include "param.h"
#include "common.h"

/************************************************
* Declaration
************************************************/
#define DATA_ADDRESS_AUTO -1
#define DATA_TRY_COUNT 2
#define DATA_DEFINE_SIZE_AND_CACHE(cache) sizeof(cache),cache
#define DATA_DELAY_TO_FLUSH_MS 500

//used to check if need init nvram by default data
//if the version read from nvram is less than this define or flag is not this, need init
#define DATA_INIT_FLAG 0x55
#define DATA_FCT_VERSION 0
#define DATA_FACTORY_VERSION 3
#define DATA_USER_VERSION 1

//used to notify user when meet this error
#define DATA_NOTIFY_ERROR_NOT_EXIST 0x1
#define DATA_NOTIFY_ERROR_CHECKSUM_WRONG 0x2
#define DATA_NOTIFY_ERROR_OTHER 0x4


/************************************************
* Variable 
************************************************/
//flash erase unit cache
//uint8_t g_data_flash_unit_cache[W25Q_FLASH_ERASE_UNIT_SIZE];

//block cache for read and write
//the block size can contain all data at this block
uint8_t g_data_fct_block_cache[12];
uint8_t g_data_factory_block_cache[44];
uint8_t g_data_user_block_cache[28];
uint8_t g_data_infusion_info_block_cache[64];
uint8_t g_data_set_infusion_para_block_cache[32];

//the element define must by the order
//define all block info
const static DataBlockSaveInfo g_data_block_map[] = {
	//block id,		 type,			file type,		size, cache,
	//	address,		   save_area_size	
	{ kDataFCTInitBlock, kDataTypeFCT, kSszBlockFileNvram, 2,NULL,
		0, FLASH_ERASE_UNIT_SIZE  },
	{ kDataFactoryInitBlock, kDataTypeFactory, kSszBlockFileNvram, 2, NULL,
		DATA_ADDRESS_AUTO, FLASH_ERASE_UNIT_SIZE  },
	{ kDataUserInitBlock, kDataTypeUser, kSszBlockFileNvram, 2, NULL,
		DATA_ADDRESS_AUTO, FLASH_ERASE_UNIT_SIZE },

	{ kDataFCTBlock, kDataTypeFCT, kSszBlockFileRecord, DATA_DEFINE_SIZE_AND_CACHE(g_data_fct_block_cache),
		3*FLASH_ERASE_UNIT_SIZE, FLASH_ERASE_UNIT_SIZE*2},
	{ kDataFactoryBlock, kDataTypeFactory, kSszBlockFileRecord, DATA_DEFINE_SIZE_AND_CACHE(g_data_factory_block_cache),
		DATA_ADDRESS_AUTO, FLASH_ERASE_UNIT_SIZE * 2 },
	{ kDataUserBlock, kDataTypeUser, kSszBlockFileRecord, DATA_DEFINE_SIZE_AND_CACHE(g_data_user_block_cache), 
		DATA_ADDRESS_AUTO,FLASH_ERASE_UNIT_SIZE * 2},
	{ kDataInfusionInfoBlock, kDataTypeUser, kSszBlockFileRecord, DATA_DEFINE_SIZE_AND_CACHE(g_data_infusion_info_block_cache), 
		DATA_ADDRESS_AUTO, FLASH_ERASE_UNIT_SIZE * 2},
	{ kDataSetInfusionParaInfoBlock, kDataTypeUser, kSszBlockFileRecord, DATA_DEFINE_SIZE_AND_CACHE(g_data_set_infusion_para_block_cache), 
		DATA_ADDRESS_AUTO, FLASH_ERASE_UNIT_SIZE * 2},		
};

//the element define must by the order
//define all child data at block
const static DataChildSaveInfo g_data_child_map[] = {
	//id,			size,				offset
	//FCT data
	{ kDataBatteryRedress, 2, DATA_ADDRESS_AUTO },
	{ kDataPressureRedress, 2, DATA_ADDRESS_AUTO },
	//Factory data
	{ kDataSerialNumber, SERIAL_NUMBER_LEN, DATA_ADDRESS_AUTO },
	{ kDataHardwareVersion, 2, DATA_ADDRESS_AUTO },
	{ kDataBatteryTwoThreshold, 2, DATA_ADDRESS_AUTO },
	{ kDataBatteryOneThreshold, 2, DATA_ADDRESS_AUTO },
	{ kDataBatteryLowThreshold, 2, DATA_ADDRESS_AUTO },
	{ kDataBatteryExhaustThreshold, 2, DATA_ADDRESS_AUTO },
	{ kDataOcclusionThreshold, 2, DATA_ADDRESS_AUTO },
	{ kDataEncoderCountEachML, 4, DATA_ADDRESS_AUTO },
	{ kDataPassword, 4, DATA_ADDRESS_AUTO },
	{ kDataBubbleAlarmSwitch, 1, DATA_ADDRESS_AUTO },
	{ kDataOcclusionSlope, 4, DATA_ADDRESS_AUTO },
	{ kDataOcclusionIncreaseOfOneDrop, 2, DATA_ADDRESS_AUTO },
	{ kDataPressureThreshold, 2, DATA_ADDRESS_AUTO },
    { kDataPrecisionFactor, 2, DATA_ADDRESS_AUTO },

//	{ kDataOledBrightness, 1, DATA_ADDRESS_AUTO },  
	//User data
	{ kDataLastShutdownIsAbnormal, 1, DATA_ADDRESS_AUTO },
	{ KDataVoiceVolume, 1, DATA_ADDRESS_AUTO },
	{ kDataBrightness, 1, DATA_ADDRESS_AUTO },
	{ kDataPeekModeSwitch, 1, DATA_ADDRESS_AUTO },
	{ kDataInfusionModeSelection, 1, DATA_ADDRESS_AUTO },
	{ kDataRunLifetimeCount, 4, DATA_ADDRESS_AUTO },
    { kDataMaxCleanTubeDose, 2, DATA_ADDRESS_AUTO },
};
static SszBlockFile g_data_block_files[sizeof(g_data_block_map) / sizeof(g_data_block_map[0])];

static SszNvram g_data_file_nvram;
static SszRecord g_data_block_record_inter[sizeof(g_data_block_map) / sizeof(g_data_block_map[0])];


/************************************************
* Function 
************************************************/

//find the bock id index which have the data id
int data_id_to_block_map_index(DataID data_id, int32_t* block_address)
{
	int index;
	int32_t next_block_address = 0;
	int32_t address = 0;
	for (index = 0; index < ssz_array_size(g_data_block_map); index++) {
		if (data_id < g_data_block_map[index].data_id) {
			break;
		}
		if (block_address) {
			if (g_data_block_map[index].data_address != DATA_ADDRESS_AUTO) {
				address = g_data_block_map[index].data_address;
			}
			else {
				address = next_block_address;
			}
			next_block_address = address + g_data_block_map[index].save_area_size;
		}
	}

	if (index > 0) {
		index -= 1;
	}
	if (block_address) {
		*block_address = address;
	}

	return index;
}

//find the data info at the block
bool data_find_save_info_at_block(DataID block_id, DataID child_id, DataChildSaveInfo* data_info)
{
	int index;
	bool is_find_block_start = false;
	DataID work_id;
	bool is_find = false;

	data_info->data_id = child_id;
	data_info->data_offset = 0;
	for (index = 0; index < ssz_array_size(g_data_child_map) && g_data_child_map[index].data_id<=child_id ; index++) {
		work_id = g_data_child_map[index].data_id;
		if (!is_find_block_start) {
			if (work_id >= block_id) {
				is_find_block_start = true;
			}
		}

		if (is_find_block_start) {
			//find the block, need calc the address
			if (work_id == child_id) {
				//if find it, break
				is_find = true;
				data_info->data_size = g_data_child_map[index].data_size;
				if (g_data_child_map[index].data_offset != DATA_ADDRESS_AUTO) {
					data_info->data_offset = g_data_child_map[index].data_offset;
				}
				break;
			}
			else {
				//calc the address
				if (g_data_child_map[index].data_offset != DATA_ADDRESS_AUTO) {
					data_info->data_offset = g_data_child_map[index].data_offset;
				}
				else {
					data_info->data_offset += g_data_child_map[index].data_size;
				}

			}
		}
	}

	return is_find;
}

//find the data info, return the block index and the data info
int data_find_save_info_by_id(DataID data_id, DataChildSaveInfo* data_info, DataBlockSaveInfo* block_info,
	SszBlockFile** data_file)
{
	int block_index;
	int32_t block_address;
	block_index = data_id_to_block_map_index(data_id, &block_address);
	//if not find the block, return
	if (block_index < 0) {
		return -1;
	}

	if (block_info) {
		*block_info = g_data_block_map[block_index];
		block_info->data_address = block_address;
	}
	if (data_file) {
		*data_file = &g_data_block_files[block_index];
	}
	//the id is the block
	if (data_id == g_data_block_map[block_index].data_id) {
		data_info->data_size = g_data_block_map[block_index].data_size;
		data_info->data_offset = 0;
	}
	//the id is at the block
	else if (data_find_save_info_at_block(g_data_block_map[block_index].data_id,
		data_id, data_info) == true) {

	}
	else {
		//not find the id
		block_index = -1;
	}

	return block_index;
}

//return the nvram object
SszNvram* data_nvram() {
	return &g_data_file_nvram;
}

void data_on_read_error(DataID read_data_id, int error_code, uint8_t notify_error_flags) {
	bool is_need_notify = false;

	if (DATA_NOTIFY_ERROR_NOT_EXIST&notify_error_flags &&
		error_code == kNotExist){
		is_need_notify = true;
	}
	else if (DATA_NOTIFY_ERROR_CHECKSUM_WRONG&notify_error_flags &&
		error_code == kChecksumWrong) {
		is_need_notify = true;
	}
	else if (DATA_NOTIFY_ERROR_OTHER&notify_error_flags &&
		(error_code != kNotExist && error_code != kChecksumWrong) ) {
		is_need_notify = true;
	}

	if (is_need_notify)
	{
		common_printfln("read %d error:%d\n", read_data_id, error_code);
		if(record_log_is_inited()){
			record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
				kLogEventDataReadError,
				read_data_id, error_code);		
		}
		int block_index = data_id_to_block_map_index(read_data_id, NULL);
		if (g_data_block_map[block_index].data_id == kDataUserBlock || g_data_block_map[block_index].data_id == kDataInfusionInfoBlock
            || g_data_block_map[block_index].data_id == kDataSetInfusionParaInfoBlock) {
			alarm_set(kAlarmUserParamReadErrorID);
		}
		else {
			alarm_set(kAlarmDataAccessErrorID);
		}
	}
}
void data_on_write_error(DataID write_data_id, int error_code, uint8_t notify_error_flags) {
	if (DATA_NOTIFY_ERROR_OTHER&notify_error_flags) {
		common_printfln("write %d error:%d\n", write_data_id, error_code);
		if(record_log_is_inited()){
			record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
				kLogEventDataWriteError,
				write_data_id, error_code);
		}
		alarm_set(kAlarmDataAccessErrorID);
	}
}

result_t data_read_ex(DataID data_id, void* buff, int buff_size, uint8_t notify_error_flags)
{
	int block_index;
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	SszBlockFile* data_file;
	result_t ret;

	block_index = data_find_save_info_by_id(data_id, &data_info, &block_info, &data_file);
	ssz_assert(block_index >= 0);
	//if not find the block, return
	if (block_index < 0) {
		return kInvalidParam;
	}
	//check the buff size
	ssz_assert(buff_size <= data_info.data_size);
	if (buff_size > data_info.data_size ||
		data_info.data_offset + buff_size > block_info.data_size) {
		return kInvalidParam;
	}

	//read the block
	ret = ssz_block_fread_at(data_file, data_info.data_offset, buff, buff_size);


	if (ret != kSuccess) {
		//error may be: kError, kChecksumWrong, kDataNotInit
		data_on_read_error(data_id, ret, notify_error_flags);
	}

	return ret;
}

//valid errors: kSuccess,kChecksumWrong,kNotExist,kError,kInvalidParam
result_t data_read(DataID data_id, void* buff, int buff_size)
{
	return data_read_ex(data_id, buff, buff_size, 0xFF);
}

result_t data_write_ex(DataID data_id, const void* buff, int buff_size, uint8_t notify_error_flags)
{
	int block_index;
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	SszBlockFile* data_file;
	result_t ret;

	block_index = data_find_save_info_by_id(data_id, &data_info, &block_info, &data_file);
	ssz_assert(block_index >= 0);
	//if not find the block, return
	if (block_index < 0) {
		return kInvalidParam;
	}
	//check the buff size
	ssz_assert(buff_size <= data_info.data_size);
	if (buff_size > data_info.data_size ||
		data_info.data_offset + buff_size > block_info.data_size) {
		return kInvalidParam;
	}

	//write the block
	ret =ssz_block_fwrite_at(data_file, data_info.data_offset, buff, buff_size);
	if (ssz_block_fdirty(data_file)) {
		timer_start_oneshot_after(kTimerFlushToNvram, DATA_DELAY_TO_FLUSH_MS);
	}

	if (ret != kSuccess) {
		//write error, do something
		data_on_write_error(data_id, ret, notify_error_flags);
	}

	return ret;
}
//valid errors: kSuccess,kError,kInvalidParam
result_t data_write(DataID data_id, const void* buff, int buff_size)
{
	return data_write_ex(data_id, buff, buff_size, 0xFF);
}

//return the data id's value
//the id's type can be int8_t,int16_t,int32_t
//if fail, will return 0, the error refer ssz_last_error()
//valid errors: kSuccess,kChecksumWrong,kNotExist,kError,kInvalidParam
int32_t data_read_int(DataID data_id)
{
	int block_index;
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	SszBlockFile* data_file;

	block_index = data_find_save_info_by_id(data_id, &data_info, &block_info, &data_file);
	ssz_assert(block_index >= 0);
	ssz_set_last_error(kSuccess);
	//if not find the block or the id's size is great than 4 byte, return
	if (block_index < 0||data_info.data_size>4) {
		ssz_set_last_error(kInvalidParam);
		return 0;
	}
	ssz_assert(data_info.data_size == 4 || data_info.data_size == 2 ||
		data_info.data_size == 1);
	int32_t ret = 0;
	result_t read_ret = data_read(data_id, &ret, data_info.data_size);
	ssz_set_last_error(read_ret);

	if (read_ret==kSuccess) {
		switch(data_info.data_size) {
		case 1:
			ret = (int8_t)ret;
			break;
		case 2:
			ret = (int16_t)ret;
			break;
		}
		return ret;
	}
	else {
		return 0;
	}
}
//return the data id's value  and the id's value is unsigned 
//the id's type can be uint8_t,uint16_t,uint32_t
uint32_t data_read_uint(DataID data_id)
{
	int block_index;
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	SszBlockFile* data_file;

	block_index = data_find_save_info_by_id(data_id, &data_info, &block_info, &data_file);
	ssz_assert(block_index >= 0);
	ssz_set_last_error(kSuccess);
	//if not find the block or the id's size is great than 4 byte, return
	if (block_index < 0 || data_info.data_size>4) {
		ssz_set_last_error(kInvalidParam);
		return 0;
	}
	ssz_assert(data_info.data_size == 4 || data_info.data_size == 2 ||
		data_info.data_size == 1);
	uint32_t ret = 0;
	result_t read_ret = data_read(data_id, &ret, data_info.data_size);
	ssz_set_last_error(read_ret);

	if (read_ret == kSuccess) {
		return ret;
	}
	else {
		return 0;
	}
}
//write the id's value
//the id's type can be int8_t,int16_t,int32_t
//valid errors: kSuccess,kError,kInvalidParam
result_t data_write_int(DataID data_id, int32_t value)
{
	int block_index;
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	SszBlockFile* data_file;

	block_index = data_find_save_info_by_id(data_id, &data_info, &block_info, &data_file);
	ssz_assert(block_index >= 0);
	//if not find the block or the id's size is great than 4 byte, return
	if (block_index < 0 || data_info.data_size>4) {
		return kInvalidParam;
	}
	ssz_assert(data_info.data_size == 4 || data_info.data_size == 2 ||
		data_info.data_size == 1);
	return data_write(data_id, &value, data_info.data_size);
}

//erase the block, all the block's area will erase
//valid errors: kSuccess,kError,kInvalidParam
result_t data_erase_block(DataID block_id)
{
	int block_index;
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	SszBlockFile* data_file;
	result_t ret;

	block_index = data_find_save_info_by_id(block_id, &data_info, &block_info, &data_file);
	ssz_assert(block_index >= 0);
	//if not find the block or the id is not block, return
	if (block_index < 0 || block_id != block_info.data_id) {
		return kInvalidParam;
	}

	//clear the block
	ret = ssz_block_fdelete(data_file);

	if (ret != kSuccess) {
		//write error, do something
		data_on_write_error(block_id, ret, 0xFF);
	}
	return ret;

}

//default the block
//the block is auto reset to zero,
// so user only need write some no zero default data to nvram
void data_default_block(DataID block_id)
{
	//the block data is auto reset to zero,
	// so user only need write some no zero default data to nvram

	switch (block_id) {
	case kDataFactoryBlock:
	{
		data_write_int(kDataOcclusionThreshold, 2500);
		data_write_int(kDataEncoderCountEachML, 2000);
        data_write_int(kDataPassword, DEFAULT_PASSWORD);  
		data_write_int(kDataBubbleAlarmSwitch, 1);
        data_write_int(kDataPressureThreshold, DEFAULT_PRESSURE_THRESHOLD);
        data_write_int(kDataPrecisionFactor, DEFAULT_PRECISION_FACTOR);
		break;
	}
	case kDataUserBlock:
	{
		data_write_int(KDataVoiceVolume, DEFAULT_VOICE_VOLUME);
		data_write_int(kDataBrightness,DEFAULT_OLED_BRIGHTNESS);
		data_write_int(kDataMaxCleanTubeDose, DEFAULT_CLEAN_TUBE_DOSE_MAX_SET);
		break;
	}
	}
}
//set the block's content all as ch
//valid errors: kSuccess,kChecksumWrong,kNotExist,kError,kInvalidParam
result_t data_fill_block_with(DataID block_id, uint8_t ch)
{
	int block_index;
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	SszBlockFile* data_file;
	result_t ret;

	block_index = data_find_save_info_by_id(block_id, &data_info, &block_info, &data_file);
	ssz_assert(block_index >= 0);
	//if not find the block or the id is not block, return
	if (block_index < 0 || block_id != block_info.data_id) {
		return kInvalidParam;
	}

	ret = ssz_block_fwrite_all_as(data_file, ch);
	if (ssz_block_fdirty(data_file)) {
		timer_start_oneshot_after(kTimerFlushToNvram, DATA_DELAY_TO_FLUSH_MS);
	}

	if (ret != kSuccess) {
		//write error, do something
		data_on_write_error(block_id, ret, 0xFF);
	}

	return ret;

}

//flush the cache to nvram
//valid errors: kSuccess,kError
result_t data_flush()
{
	result_t ret = kSuccess;
	result_t tmp_ret ;

	for (int i = 0; i < ssz_array_size(g_data_block_map); i++) {
		if (ssz_block_fdirty(&g_data_block_files[i])) {
			tmp_ret = ssz_block_fflush(&g_data_block_files[i]);
			if(tmp_ret != kSuccess){
				ret = tmp_ret;
				data_on_write_error(g_data_block_map[i].data_id, tmp_ret, 0xFF);
			}
		}
	}
	timer_stop(kTimerFlushToNvram);

	return ret;
}
void data_clear_by_data_type(DataType data_type)
{
	for (int i = 0; i < ssz_array_size(g_data_block_map); i++) {
		if (g_data_block_map[i].data_type == data_type) {
			data_erase_block(g_data_block_map[i].data_id);
		}
	}
}
void data_zero_by_data_type(DataType data_type)
{
	for (int i = 0; i < ssz_array_size(g_data_block_map); i++) {
		if (g_data_block_map[i].data_type == data_type) {
			data_fill_block_with(g_data_block_map[i].data_id, 0);
		}
	}
}
void data_default_by_data_type(DataType data_type)
{
	for (int i = 0; i < ssz_array_size(g_data_block_map); i++) {
		if (g_data_block_map[i].data_type == data_type) {
			data_default_block(g_data_block_map[i].data_id);
		}
	}

}

//write default data to nvram if not init or version is upgrade
void data_first_init_if_need()
{
	DataInitInfo initinfo;
	bool is_need_init;
	const DataID init_ids[] = { kDataFCTInitBlock, kDataFactoryInitBlock, kDataUserInitBlock };
	const int init_id_versions[] = { DATA_FCT_VERSION, DATA_FACTORY_VERSION, DATA_USER_VERSION};
	const DataType init_data_type[] = { kDataTypeFCT, kDataTypeFactory, kDataTypeUser };
	result_t ret;
	int block_index;

	for (int i = 0; i < ssz_array_size(init_ids); i++) {
		is_need_init = false;
		//open and read init info
		block_index = data_id_to_block_map_index(init_ids[i], NULL);
		ssz_assert(block_index >= 0);
		ssz_block_fopen(&g_data_block_files[i], false);
		ret = data_read_ex(init_ids[i], &initinfo, sizeof(initinfo), DATA_NOTIFY_ERROR_OTHER | DATA_NOTIFY_ERROR_CHECKSUM_WRONG);
		if (ret == kSuccess) {
			//if flag is wrong or version is low, need init
			if (initinfo.init_flag != DATA_INIT_FLAG || initinfo.version != init_id_versions[i]) {
				is_need_init = true;
			}
		}
		else if (ret == kNotExist) {
			//if not exist, need init
			is_need_init = true;
		}

		if (is_need_init) {
			//clear block
			data_clear_by_data_type(init_data_type[i]);
			//write zero data to nvram
			data_zero_by_data_type(init_data_type[i]);

			//write default to nvram
			data_default_by_data_type(init_data_type[i]);

			//write the init info
			initinfo.init_flag = DATA_INIT_FLAG;
			initinfo.version = init_id_versions[i];
			data_write(init_ids[i], &initinfo, sizeof(initinfo));

			//flush
			data_flush();
		}

	}

}

#ifdef DEBUG
void data_check_id(DataID data_id)
{
	int block_index;
	DataChildSaveInfo data_info;
	DataBlockSaveInfo block_info;
	SszBlockFile* data_file;

	block_index = data_find_save_info_by_id(data_id, &data_info, &block_info, &data_file);
	ssz_assert(block_index >= 0);

	//check the buff size
	ssz_assert(data_info.data_offset + data_info.data_size <= block_info.data_size);

}
void data_check_define()
{
	for (int i = 0; i < ssz_array_size(g_data_child_map); i++) {
		data_check_id(g_data_child_map[i].data_id);
	}
}
#endif

void data_delay_to_flush()
{
	data_flush();
}
void data_init() {
	ssz_traceln("data init begin");

	//init data nvram file
	ssz_nvram_init(&g_data_file_nvram, drv_sst25_flash_file(), true, DATA_TRY_COUNT,
		SSZ_NVRAM_RW_MODE_USE_CHECKSUM | SSZ_NVRAM_RW_MODE_CHECK_IF_CAN_WRITE | SSZ_NVRAM_RW_MODE_CHECK_IF_WRITE_SUCCESS);

	//init all block struct
	int32_t next_block_address = 0;
	int32_t address = 0;
	void* dev;
	for (int i = 0; i < ssz_array_size(g_data_block_map); i++) {
		if (g_data_block_map[i].data_address!=DATA_ADDRESS_AUTO) {
			address = g_data_block_map[i].data_address;
		}
		else {
			address = next_block_address;
		}
		next_block_address = address + g_data_block_map[i].save_area_size;
		//set block's relate file
		if (g_data_block_map[i].file_type == kSszBlockFileNvram) {
			dev = &g_data_file_nvram;
		}
		else if (g_data_block_map[i].file_type == kSszBlockFileRecord) {
			dev = &g_data_block_record_inter[i];
			ssz_record_struct_init(&g_data_block_record_inter[i], data_nvram(), address,
				g_data_block_map[i].save_area_size, g_data_block_map[i].data_size, true,
				false, 0, 0);

		}
		else{
			ssz_assert_fail();
		}
		ssz_block_file_init(&g_data_block_files[i], g_data_block_map[i].file_type,
			dev, address, g_data_block_map[i].data_size,
			g_data_block_map[i].data_cache, g_data_block_map[i].save_area_size);
	}


#ifdef DEBUG
	data_check_define();
#endif
	//check if need init default data
	data_first_init_if_need();

	//open each block
	result_t ret;
	for (int i = 0; i < ssz_array_size(g_data_block_files); i++) {
		ssz_block_fopen(&g_data_block_files[i], false);
		ret = ssz_last_error();
		if (ret != kSuccess) {
			data_on_read_error(g_data_block_map[i].data_id, ret, 0xFF);
		}
	}

	timer_set_handler(kTimerFlushToNvram, data_delay_to_flush);
	ssz_traceln("data init end");
}
