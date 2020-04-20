#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
struct ssz_file_t;
//typedef int (* SszIOOpenFun)( SszFile* file, const char *file_name, const void *param);
//typedef int (* SszIOCloseFun)( SszFile* file);
// return error code
typedef int (* SszIOReadFun)(struct ssz_file_t* file, void* buff, int buff_size, int* real_read_buff_size);
// return error code
typedef int (* SszIOWriteFun)(struct ssz_file_t* file, const void* buff, int buff_size, int* real_write_buff_size);
// return error code
typedef int (* SszIOCtlFun)(struct ssz_file_t* file, int command, void* param);
//typedef int(*SszIOFlushFun)(struct ssz_file_t* file);
typedef struct 
{
	//base fun
	const char*     dev_type;
	SszIOReadFun     read_fn;
	SszIOWriteFun	write_fn;
	SszIOCtlFun		ctl_fn;

	//extend fun
	//SszIOFlushFun	flush_fn;
	//SszIOOpenFun     open_fn;
	//SszIOCloseFun    close_fn;
}SszIODevice;
