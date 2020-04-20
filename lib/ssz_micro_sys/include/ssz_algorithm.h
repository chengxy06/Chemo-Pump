#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
/* crc32 seed */
#define SSZ_CRC32_SEED        0xFFFFFFFF
/* crc32 data length */
#define SSZ_CRC32_DATA_LEN    4

#ifdef __cplusplus
extern "C" {
#endif

//calc the buffer's CRC32, the seed is SSZ_CRC32_SEED
uint32_t ssz_crc32(const void *p_buffer, int buffer_len);

//calc the buffer's CRC32, 
//e.g. crc32 = ssz_crc32_ex(buff, buff_size, SSZ_CRC32_SEED)^SSZ_CRC32_SEED;
//also you can calc many part replace to calc the whole, the crc32 iresult is same:
//e.g.
//	   uint32_t crc32 = SSZ_CRC32_SEED;
//     crc32 = ssz_crc32_ex(buff, 10, crc32);
//     crc32 = ssz_crc32_ex(buff+10, 10, crc32);
//     crc32 = crc32^SSZ_CRC32_SEED;
uint32_t ssz_crc32_ex(const void *p_buffer, int buffer_len, uint32_t seed);

#ifdef __cplusplus
}
#endif