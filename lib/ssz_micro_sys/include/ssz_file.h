/************************************************
* DESCRIPTION:
*   it same as clib's file.
*   and it changed to easy use.
*   e.g. 
*       add write_at, read_at
*
************************************************/
#pragma once
#include "ssz_io.h"
#include "ssz_errorcode.h"
/************************************************
* Declaration
************************************************/
#define SSZ_EOF 	-1
#define SSZ_SEEK_CUR 1	//from current pos
#define SSZ_SEEK_END    2	//from file end
#define SSZ_SEEK_SET    0 //from file head
//below is common defined command
//the common defined command is <0
//user defined command need >0
#define SSZ_IOCTL_ENABLE -1
#define SSZ_IOCTL_DISABLE -2

#define SSZ_IOCTL_FLUSH	-3 //flush the data
#define SSZ_IOCTL_SIZE	-4 //return the file size, param[out]: &int32_t
#define SSZ_IOCTL_CURR_POS	-5 //return the curr pos, param[out]: &int32_t
#define SSZ_IOCTL_SEEK	-6 //seek to the address, param[in] is address: &int32_t

#define SSZ_IOCTL_IS_SUPPORT_ERASE	-10 //param[out]: int&
#define SSZ_IOCTL_ERASE	-11 //erase one unit at the address,  param[in] is address: &int32_t
#define SSZ_IOCTL_IS_NEED_ERASE_BEFORE_WRITE	-12 //param[out]: int&
#define SSZ_IOCTL_ERASE_UNIT_SIZE	-13 //return the erase unit size, param[out]: int&

//lint -e{39}
typedef struct ssz_file_t
{
    const SszIODevice* dev;
    void* dev_data;
    //int error_code; //Refer ErrorCode
}SszFile;

#ifdef __cplusplus
extern "C" {
#endif

//init file
void ssz_file_init(SszFile* file, const SszIODevice* dev, void* dev_data);

result_t ssz_fwrite(SszFile* file, const void* buff, int buff_size);
//real_write_buff_size: the real write bytes
result_t ssz_fwrite_ex(SszFile* file, const void* buff, int buff_size, int* real_write_buff_size);
result_t ssz_fread(SszFile* file, void* buff, int buff_size);
//real_read_buff_size: the real read bytes
result_t ssz_fread_ex(SszFile* file, void* buff, int buff_size,int* real_read_buff_size);
//read a byte(unsigned), return: SSZ_EOF->fail or meet end
int ssz_fgetc(SszFile* file);
result_t ssz_fputc(SszFile* file, int one_byte);
result_t ssz_fflush(SszFile* file);

//fromwhere: refer SSZ_SEEK_CUR, offset: can negative
result_t ssz_fseek(SszFile* file, int32_t offset, int fromwhere);
//return current pos, if dev not support, it will return 0
int32_t ssz_ftell(SszFile* file);
//return file size, if dev not support, it will return 0
//the size may be changed after each call, it will depend the dev
int32_t ssz_fsize(SszFile* file);

//send command to dev
result_t ssz_ioctl(SszFile* file, int command, void* param);

//return: kSuccess->success  other->have error, can Refer ErrorCode
//int ssz_ferror(SszFile* file);

result_t ssz_fwrite_at(SszFile* file, int32_t address, const void* buff, int buff_size);
result_t ssz_fwrite_at_ex(SszFile* file, int32_t address, const void* buff, int buff_size, int* real_write_buff_size);
result_t ssz_fread_at(SszFile* file, int32_t address, void* buff, int buff_size);
result_t ssz_fread_at_ex(SszFile* file, int32_t address, void* buff, int buff_size, int* real_read_buff_size);

#ifdef __cplusplus
}
#endif