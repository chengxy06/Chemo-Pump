#include "ssz_file.h"
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
//init file
void ssz_file_init(SszFile* file, const SszIODevice* dev, void* dev_data){
	ssz_assert(file);
	ssz_mem_zero(file, sizeof(SszFile));
	file->dev = dev;
	file->dev_data = dev_data;
}
result_t ssz_fwrite(SszFile* file, const void* buff, int buff_size){
	return ssz_fwrite_ex(file, buff, buff_size, NULL);
}
//real_write_buff_size: the real write bytes
result_t ssz_fwrite_ex(SszFile* file, const void* buff, int buff_size, int* real_write_buff_size)
{
	ssz_assert(file && file->dev && file->dev->write_fn);
	return file->dev->write_fn(file, buff, buff_size, real_write_buff_size);
}
result_t ssz_fread(SszFile* file, void* buff, int buff_size){
	return ssz_fread_ex(file, buff, buff_size, NULL);
}
//real_read_buff_size: the real read bytes
result_t ssz_fread_ex(SszFile* file, void* buff, int buff_size,int* real_read_buff_size)
{
	ssz_assert(file && file->dev && file->dev->read_fn);
	return file->dev->read_fn(file, buff, buff_size, real_read_buff_size);
}

//read a byte, return: SSZ_EOF->fail or meet end
int ssz_fgetc(SszFile* file){
	uint8_t sz;
	if(ssz_is_result_ok(ssz_fread(file, &sz, 1))){
		return sz;
	}else{
		return SSZ_EOF;		
	}
}
result_t ssz_fputc(SszFile* file, int one_byte){
	uint8_t sz = (uint8_t)one_byte;
	return ssz_fwrite(file, &sz, 1);
}

result_t ssz_fflush(SszFile* file) {
	return ssz_ioctl(file, SSZ_IOCTL_FLUSH, NULL);
}

//send command to dev
result_t ssz_ioctl(SszFile* file, int command, void* param){
	ssz_assert(file && file->dev &&file->dev->ctl_fn);
	return file->dev->ctl_fn(file, command, param);
}
//fromwhere: refer SSZ_SEEK_CUR, offset: can nagative
result_t ssz_fseek(SszFile* file, int32_t offset, int fromwhere){
	int32_t pos;
	int32_t f_size;

	ssz_ioctl(file, SSZ_IOCTL_SIZE, &f_size);
	switch(fromwhere){
		case SSZ_SEEK_CUR:
		pos = ssz_ftell(file);
		break;
		case SSZ_SEEK_END:
		pos = f_size-1; 
		break;		
		case SSZ_SEEK_SET:
		pos = 0;
		break;
		default:
		ssz_debug_break();
		return kInvalidParam;
	}

	pos += offset;
	if (pos >=f_size || pos<0) {
		return kInvalidParam;
	}
	return ssz_ioctl(file, SSZ_IOCTL_SEEK, &pos);

}
//return current pos, if dev not support, it will return 0
int32_t ssz_ftell(SszFile* file){
	int32_t ret;
	if(ssz_is_result_ok(ssz_ioctl(file, SSZ_IOCTL_CURR_POS, &ret))){
		return 0;
	}else{
		return ret;
	}
}

//return file size, if dev not support, it will return 0
//the size may be changed after each call, it will depend the dev
int32_t ssz_fsize(SszFile* file) {
	int32_t f_size = 0;
	if (ssz_is_result_ok(ssz_ioctl(file, SSZ_IOCTL_SIZE, &f_size))) {
		return f_size;
	}
	else {
		return 0;
	}

}
//return: kSuccess->success  other->have error, can Refer ErrorCode
//int ssz_ferror(SszFile* file){
//	return file->error_code;
//}

result_t ssz_fwrite_at(SszFile* file, int32_t address, const void* buff, int buff_size) {
	ssz_fseek(file, address, SSZ_SEEK_SET);
	return ssz_fwrite(file, buff, buff_size);
}
result_t ssz_fwrite_at_ex(SszFile* file, int32_t address, const void* buff, int buff_size, int* real_write_buff_size)
{
	ssz_fseek(file, address, SSZ_SEEK_SET);
	return ssz_fwrite_ex(file, buff, buff_size, real_write_buff_size);
}
result_t ssz_fread_at(SszFile* file, int32_t address, void* buff, int buff_size) {
	ssz_fseek(file, address, SSZ_SEEK_SET);
	return ssz_fread(file, buff, buff_size);
}
result_t ssz_fread_at_ex(SszFile* file, int32_t address, void* buff, int buff_size, int* real_read_buff_size)
{
	ssz_fseek(file, address, SSZ_SEEK_SET);
	return ssz_fread_ex(file, buff, buff_size, real_read_buff_size);
}