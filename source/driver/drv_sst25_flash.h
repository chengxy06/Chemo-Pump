/************************************************
* DESCRIPTION:
*   sst25 is a norflash, it has below speciality:
*   More than 100,000 erase/program cycles
*   More than 20-year data retention
*
*   Program unit:
*   Page   - 256bytes
*
*   Erase unit:
*   Sector - 4K
*   Block  - 64K
*   Chip   - all   
*   
*   SST25VF010A :
*		Flash size: 1Mbyte 
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "config.h"
#include "ssz_file.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


#ifdef SST25_VF010

#define SST25_FLASH_SIZE (128*1024)
#define SST25_FLASH_ID 0xBF49 

#elif defined SST25_VF020

#define SST25_FLASH_SIZE (256*1024)
#define SST25_FLASH_ID 0xBF8C
#define SST25_FLASH_JEDEC_ID 0xBF258C

#elif defined SST25_VF016

#define SST25_FLASH_SIZE (16*1024*1024)
#define SST25_FLASH_JEDEC_ID 0xBF2541

#else
#error "not define flash type"

#endif

#define SST25_FLASH_SPI  FLASH_SPI

#define SST25_FLASH_AAI_PROGRAM_UNIT_SIZE  (4*1024)
#define SST25_FLASH_ERASE_UNIT_SIZE		   (4*1024)

#if FLASH_ERASE_UNIT_SIZE>0 && FLASH_ERASE_UNIT_SIZE!=SST25_FLASH_ERASE_UNIT_SIZE
#error "the defined flash erase unit size is not same as this device's erase unit size"
#endif

//#define SST25_FLASH_CS_PORT  FLASH_NSS_GPIO_Port	 	
//#define SST25_FLASH_CS_PIN   FLASH_NSS_Pin		 

 
bool drv_sst25_flash_init(void);

//read status register, param can be:SST25_FLASH_STATUS1,SST25_FLASH_STATUS2,SST25_FLASH_STATUS3
uint8_t drv_sst25_flash_read_status(uint8_t status_code);

//check is busy
bool drv_sst25_flash_is_busy();

bool drv_sst25_flash_read(int32_t address, void *buff, int need_read_size);

//Note: you must ensure the destion's data is already erased or will not success
bool drv_sst25_flash_write(int32_t address, const void *buff, int buffer_len);

//erase one unit, the address must be address%SST25_FLASH_ERASE_UNIT_SIZE==0
void drv_sst25_flash_erase_one_unit(int32_t address);

//Note: it will cost about 8(for W25Q32FV) seconds, 
//so after call it, please wait for xx seconds to  continue operation this flash
void drv_sst25_flash_erase_chip(void);

//read the JEDEC ID of the flash
uint32_t drv_sst25_flash_read_jedec(void);

//read the  ID of the flash
uint32_t drv_sst25_flash_read_id(void);

void drv_sst25_flash_wait_idle(void);

//enable sst25 power supply from system power supply 
void drv_sst25_pcb_sleep_enable(void);

//disable sst25 power supply from system power supply
void drv_sst25_pcb_sleep_disable(void);

//return ssz file
SszFile* drv_sst25_flash_file();

#ifdef __cplusplus
}
#endif


