#include "drv_sst25_flash.h"
#include "ssz_spi.h"
#include "ssz_common.h"
#include "ssz_utility.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#endif
/************************************************
* Declaration
************************************************/
#ifndef SST25_FLASH_CS_PIN
#error "not define SST25_FLASH_CS_PIN"
#endif

#ifndef SST25_FLASH_CS_PORT
#error "not define SST25_FLASH_CS_PORT"
#endif

//command which flash support
#define SST25_FLASH_WRITE_ENABLE_CMD 0x06 

#define SST25_FLASH_WRITE_DISABLE_CMD 0x04

#define SST25_FLASH_READ_STATUS_CMD 0x05
#define SST25_FLASH_CHIP_ERASE_CMD		0xC7
#define SST25_FLASH_SECTOR_ERASE_CMD	0x20

#define SST25_FLASH_BYTE_PROGRAM_CMD	0x02
#define SST25_FLASH_AAI_PROGRAM_CMD		0xAF   
#define SST25_FLASH_AAI_WORD_PROGRAM_CMD		0xAD   

#define SST25_FLASH_EWSR_CMD	0x50
#define SST25_FLASH_WRSR_CMD	0x01

#define SST25_FLASH_READ_DATA_CMD	0x03	 
#define SST25_FLASH_High_READ_DATA_CMD	0x0B

#define SST25_FLASH_READ_JEDEC_ID_CMD 0x9F
#define SST25_FLASH_READ_ID_CMD 0x90

#define SST25_FLASH_DUMMY_BYTE 0x00 //used to send to device for read data

#define SST25_FLASH_RETRY_MAX_COUNT 10000

#define SST25_FLASH_STATUS1 0X05   //  busy
#define SST25_FLASH_STATUS2 0X35
#define SST25_FLASH_STATUS3 0X15

#define M_drv_sst25_flash_is_byte(status1) ( (((uint8_t)status1)&0x40)==0 )  // ok ?
#define M_drv_sst25_flash_is_AAI(status1) ( (((uint8_t)status1)&0x40)!=0 )  // ok ?
#define M_drv_sst25_flash_is_Protected(status1) ( (((uint8_t)status1)&0x0C)==0 )  // ok ?
#define M_drv_sst25_flash_is_enable_write(status1) ((((uint8_t)status1)&0x02)!=0) // ok ?
#define M_drv_sst25_flash_is_busy(status1) ( (((uint8_t)status1)&0x01)==0x01 )  // ok ?
///////////////////////////////////////////////////////////////////////////
typedef struct {
	int32_t address;
}DrvSST25FlashDev;
uint8_t drv_sst25_flash_read_status(uint8_t status_code);
static void drv_sst25_flash_enable_write(void);
static void drv_sst25_flash_disable_Protected(void);
static void drv_sst25_select_slave(bool is_select);
  bool drv_sst25_flash_write_at_one_page(int32_t address, uint8_t *buff, int buffer_len);
extern const SszIODevice kDrvSST25FlashIODev;
/************************************************
* Variable
************************************************/
//static bool g_sst25_flash_is_need_check_busy = true;

//spi define
static SszSPI g_sst25_flash_spi;
static SszFile g_sst25_flash_spi_file_inter = {
	&kSszSPIIODev, &g_sst25_flash_spi
};
static SszFile *g_sst25_flash_spi_file = &g_sst25_flash_spi_file_inter;

#ifdef SSZ_TARGET_SIMULATOR
static void* g_sst25_flash_sim;
#endif

//file define
static DrvSST25FlashDev g_sst25_flash_dev;
static SszFile g_sst25_flash_file;

/************************************************
* Function
************************************************/	
//enable sst25 power supply from system power supply 
void drv_sst25_pcb_sleep_enable(void)
{
    ssz_gpio_clear(FLASH_PWR_EN_GPIO_Port, FLASH_PWR_EN_Pin);      
    // disable the sst25 power
}

//disable sst25 power supply from system power supply
void drv_sst25_pcb_sleep_disable(void)
{
    ssz_gpio_set(FLASH_PWR_EN_GPIO_Port, FLASH_PWR_EN_Pin);
    // enable the sst25 power
}

static void drv_sst25_flash_enable_cs(void)
{
	ssz_gpio_clear(SST25_FLASH_CS_PORT, SST25_FLASH_CS_PIN);
}

static void drv_sst25_flash_disable_cs(void)
{
	ssz_gpio_set(SST25_FLASH_CS_PORT, SST25_FLASH_CS_PIN);
}
static void drv_sst25_select_slave(bool is_select) {
	if (is_select) {
		drv_sst25_flash_enable_cs();
	}
	else {
		drv_sst25_flash_disable_cs();
	}
}

//read status register, param can be:SST25_FLASH_STATUS1,SST25_FLASH_STATUS2,SST25_FLASH_STATUS3
uint8_t drv_sst25_flash_read_status(uint8_t status_code)
{
	uint8_t status;

	drv_sst25_flash_enable_cs();

	ssz_fwrite(g_sst25_flash_spi_file, &status_code, 1);
	ssz_fread(g_sst25_flash_spi_file, &status, 1);

	drv_sst25_flash_disable_cs();
	//printf("0x%X status: 0x%X\n", status_code, status);
	return status;
}

//read status register, param can be:SST25_FLASH_STATUS1,SST25_FLASH_STATUS2,SST25_FLASH_STATUS3
static void drv_sst25_flash_disable_Protected(void)
{
	//int retry ;
	drv_sst25_flash_wait_idle();
	drv_sst25_flash_enable_cs();// retry=0;while (retry <3000) {retry++;};
	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_EWSR_CMD);
	//retry=0;while(retry <3000) {retry++;};
	drv_sst25_flash_disable_cs();
	
 	//retry=0;while (retry <3000) {retry++;};
	drv_sst25_flash_enable_cs();
	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_WRSR_CMD);
	ssz_fputc(g_sst25_flash_spi_file, 00);
 	//retry=0;while (retry <3000) {retry++;};
	drv_sst25_flash_disable_cs();
 	drv_sst25_flash_wait_idle();
#ifdef SSZ_TARGET_MACHINE
	ssz_assert(M_drv_sst25_flash_is_Protected(drv_sst25_flash_read_status(SST25_FLASH_STATUS1)));		
#endif
}

//check is busy
bool drv_sst25_flash_is_busy() {
#ifndef SSZ_TARGET_SIMULATOR
	return M_drv_sst25_flash_is_busy(drv_sst25_flash_read_status(SST25_FLASH_STATUS1));
#else
	return false;
#endif
}

//wait flash can accept command
void drv_sst25_flash_wait_idle(void)
{
	int retry = 0;
	uint8_t status1;
	do
	{
#ifdef SSZ_TARGET_MACHINE
		status1 = drv_sst25_flash_read_status(SST25_FLASH_STATUS1);
#else
		break;
#endif
		retry++;
		if (retry >= 30000) { // 30000
			ssz_trace("warn: flash busy long, status1[0x%x]\n", status1);
		}
	} while (M_drv_sst25_flash_is_busy(status1));
}

static void drv_sst25_flash_enable_write(void)
{
	drv_sst25_flash_wait_idle();

	drv_sst25_flash_enable_cs();
	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_WRITE_ENABLE_CMD);
	//wait spi send finish
	ssz_fflush(g_sst25_flash_spi_file);	
 	drv_sst25_flash_disable_cs();

}

/***********************************************
* Description:
*   read some data
*
* Argument:
*   address:
*   buff:
*   need_read_size:
*
* Return:
*   is ok
************************************************/
bool drv_sst25_flash_read(int32_t address, void *buff, int need_read_size)
{
	bool is_ok = true;
	int i ;
	ssz_assert(address >= 0 && address + need_read_size <= SST25_FLASH_SIZE);
#ifdef SSZ_TARGET_SIMULATOR
	sim_memory_read_by_address(g_sst25_flash_sim, address, buff, need_read_size);
	return true;
#endif
 	drv_sst25_flash_wait_idle();
	drv_sst25_flash_enable_cs();

	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_READ_DATA_CMD);
	//write 24bit address
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address) >> 16));
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address) >> 8));
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address)));
	//read data
	//is_ok = ssz_is_result_ok(ssz_fread(g_sst25_flash_spi_file, buff, need_read_size));
	for (i = 0; i < need_read_size; i++)
		{
	  	 	*((uint8_t*)buff + i) = ssz_fgetc(g_sst25_flash_spi_file); 		
		}	

	drv_sst25_flash_disable_cs();

	return is_ok;
}

void drv_sst25_flash_write_one_byte(int32_t address, uint8_t one_byte){
	//drv_sst25_flash_disable_Protected( );
	drv_sst25_flash_enable_write();
	
	drv_sst25_flash_wait_idle();
#ifdef SSZ_TARGET_MACHINE
		ssz_assert(M_drv_sst25_flash_is_enable_write(drv_sst25_flash_read_status(SST25_FLASH_STATUS1)));
#endif
	drv_sst25_flash_enable_cs();
	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_BYTE_PROGRAM_CMD);
	//write 24bit address
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address) >> 16));
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address) >> 8));
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address)));
	//write data
	ssz_fputc(g_sst25_flash_spi_file, one_byte);// ssz_fputc(g_sst25_flash_spi_file, *buff);
	drv_sst25_flash_disable_cs();
}

void drv_sst25_flash_write_buff(int32_t address, uint8_t *buff, int buffer_len){

	ssz_assert(buffer_len>0&&buffer_len%2==0);

	drv_sst25_flash_enable_write();

	drv_sst25_flash_wait_idle();
#ifdef SSZ_TARGET_MACHINE
	ssz_assert(M_drv_sst25_flash_is_enable_write(drv_sst25_flash_read_status(SST25_FLASH_STATUS1)));
#endif	
	drv_sst25_flash_enable_cs();
	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_AAI_WORD_PROGRAM_CMD);
	//write 24bit address
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address) >> 16));
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address) >> 8));
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address)));
	//write data
	ssz_fputc(g_sst25_flash_spi_file, buff[0]);
	ssz_fputc(g_sst25_flash_spi_file, buff[1]);
	drv_sst25_flash_disable_cs();
	
	for (int i = 2; i < buffer_len; i+=2){
		drv_sst25_flash_wait_idle();
		drv_sst25_flash_enable_cs();
		ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_AAI_WORD_PROGRAM_CMD);
		ssz_fputc(g_sst25_flash_spi_file, buff[i]);
		ssz_fputc(g_sst25_flash_spi_file, buff[i+1]);

		drv_sst25_flash_disable_cs();
	}			
//	ssz_assert(M_drv_sst25_flash_is_AAI(drv_sst25_flash_read_status(SST25_FLASH_STATUS1))); 

	drv_sst25_flash_wait_idle();
	drv_sst25_flash_enable_cs();
	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_WRITE_DISABLE_CMD);	
	drv_sst25_flash_disable_cs();
	

}

bool drv_sst25_flash_write_at_one_page(int32_t address, uint8_t *buff, int buffer_len)
{
	bool is_ok = true;
    //int i ;
	ssz_assert(address >= 0 && address + buffer_len <= SST25_FLASH_SIZE
		&& address%SST25_FLASH_AAI_PROGRAM_UNIT_SIZE + buffer_len <= SST25_FLASH_AAI_PROGRAM_UNIT_SIZE);

#ifdef SSZ_TARGET_SIMULATOR
	sim_memory_write_flash_by_address(g_sst25_flash_sim, address, buff, buffer_len);
	return true;
#endif
	if(buffer_len>0){
		if(buffer_len%2==0){
			drv_sst25_flash_write_buff(address, buff, buffer_len);
		}else if(buffer_len == 1){
			drv_sst25_flash_write_one_byte(address, *buff);
		}else{
			drv_sst25_flash_write_buff(address, buff, buffer_len-1);
			drv_sst25_flash_write_one_byte(address+buffer_len-1, buff[buffer_len-1]);
		}
	}
	return is_ok;
}
 
/***********************************************
* Description:
*   write data, Note: you must ensure the destion's data is already erased or will not success
* Argument:
*   address:
*   buff:
*   buffer_len:
*
* Return:
************************************************/
bool drv_sst25_flash_write(int32_t address, const void *buff, int buffer_len)
{
	int write_bytes_count = 0;	//	record the write bytes count
	int once_write_count;		//	once write bytes count at one page

	ssz_assert(address >= 0 && address + buffer_len <= SST25_FLASH_SIZE);

	while (write_bytes_count < buffer_len)
	{
		//get once write bytes count
		once_write_count = buffer_len - write_bytes_count;
		//write finish, break
		if (once_write_count <= 0)
		{
			break;
		}
		//if once write count overstep one page, reset it
		if ((address + write_bytes_count) % SST25_FLASH_AAI_PROGRAM_UNIT_SIZE + once_write_count>SST25_FLASH_AAI_PROGRAM_UNIT_SIZE)
		{                     
			once_write_count = SST25_FLASH_AAI_PROGRAM_UNIT_SIZE - (address + write_bytes_count) % SST25_FLASH_AAI_PROGRAM_UNIT_SIZE;
		}
		//write at one page
		drv_sst25_flash_write_at_one_page(address + write_bytes_count, (uint8_t*)buff + write_bytes_count, once_write_count);
		write_bytes_count += once_write_count;
	}

	return true;
}

/***********************************************
* Description:
*   erase one unit, the address must be address%SST25_FLASH_ERASE_UNIT_SIZE==0
* Argument:
*   address:
*
* Return:
*
************************************************/
void drv_sst25_flash_erase_one_unit(int32_t address)
{
	ssz_assert(address >= 0 && address<SST25_FLASH_SIZE
		&& address%SST25_FLASH_ERASE_UNIT_SIZE == 0);

#ifdef SSZ_TARGET_SIMULATOR
	sim_memory_write_ff_by_address(g_sst25_flash_sim, address, SST25_FLASH_ERASE_UNIT_SIZE);
	return;
#endif
	drv_sst25_flash_enable_write();
	drv_sst25_flash_wait_idle();
	ssz_assert(M_drv_sst25_flash_is_enable_write(drv_sst25_flash_read_status(SST25_FLASH_STATUS1)));
	drv_sst25_flash_enable_cs();

	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_SECTOR_ERASE_CMD);
	//write 24bit address
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address) >> 16));
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address) >> 8));
	ssz_fputc(g_sst25_flash_spi_file, (uint8_t)(((uint32_t)address)));

	ssz_fflush(g_sst25_flash_spi_file);
	drv_sst25_flash_disable_cs();
	drv_sst25_flash_wait_idle();
}

/***********************************************
* Description:
*   erase chip, that mean all data at flash will be erased
* Argument:
*
* Return:
*
************************************************/
void drv_sst25_flash_erase_chip(void)
{
#ifdef SSZ_TARGET_SIMULATOR
	sim_memory_write_ff_by_address(g_sst25_flash_sim, 0, SST25_FLASH_SIZE);
	return;
#endif
	drv_sst25_flash_disable_Protected();

	drv_sst25_flash_enable_write();
	ssz_assert(M_drv_sst25_flash_is_enable_write(drv_sst25_flash_read_status(SST25_FLASH_STATUS1)));
	drv_sst25_flash_enable_cs();

	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_CHIP_ERASE_CMD);

	ssz_fflush(g_sst25_flash_spi_file);
	drv_sst25_flash_disable_cs();
	drv_sst25_flash_wait_idle();

}

/***********************************************
* Description:
*   read the JEDEC ID of the flash
*   the four byte is: dummy,manufacturer ID, Memory Type, Memory Capacity
* Argument:
*
* Return:
*
************************************************/
uint32_t drv_sst25_flash_read_jedec(void)
{
	uint32_t id = 0;

#ifdef SSZ_TARGET_SIMULATOR
	return SST25_FLASH_ID;
#endif
	drv_sst25_flash_wait_idle();
	drv_sst25_flash_enable_cs();

	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_READ_JEDEC_ID_CMD);
	id += ssz_fgetc(g_sst25_flash_spi_file);//manufacturer ID
	id <<= 8;
	id += ssz_fgetc(g_sst25_flash_spi_file);//Memory Type
	id <<= 8;
	id += ssz_fgetc(g_sst25_flash_spi_file);//Memory Capacity

	drv_sst25_flash_disable_cs();

	return id;
}
/***********************************************
* Description:
*   read the ID of the flash
*   the two byte is: manufacturer ID,Device ID(Memory Capacity)
* Argument:
*
* Return:
*
************************************************/
uint32_t drv_sst25_flash_read_id(void)
{
	uint32_t id = 0;

#ifdef SSZ_TARGET_SIMULATOR
	return SST25_FLASH_ID;
#endif
	drv_sst25_flash_wait_idle();
	drv_sst25_flash_enable_cs();

	ssz_fputc(g_sst25_flash_spi_file, SST25_FLASH_READ_ID_CMD);
	ssz_fputc(g_sst25_flash_spi_file, 0);
	ssz_fputc(g_sst25_flash_spi_file, 0);
	ssz_fputc(g_sst25_flash_spi_file, 0); //	
	id += ssz_fgetc(g_sst25_flash_spi_file);//manufacturer ID
	id <<= 8;
	id += ssz_fgetc(g_sst25_flash_spi_file);//Memory Capacity

	drv_sst25_flash_disable_cs();

	return id;
}

bool drv_sst25_flash_init(void)
{
	uint32_t id;

#ifdef SSZ_TARGET_SIMULATOR
	g_sst25_flash_sim = sim_memory_create("SST25_norflash", SST25_FLASH_SIZE, 0xFF);
#endif
	drv_sst25_pcb_sleep_disable();

	ssz_spi_init(&g_sst25_flash_spi, &SST25_FLASH_SPI, drv_sst25_select_slave);
	ssz_file_init(&g_sst25_flash_file, &kDrvSST25FlashIODev, &g_sst25_flash_dev);

	ssz_gpio_set(FLASH_HOLD_GPIO_Port, FLASH_HOLD_Pin);
	ssz_gpio_set(FLASH_WP_GPIO_Port, FLASH_WP_Pin);

	drv_sst25_flash_disable_Protected();	
	id = drv_sst25_flash_read_id();

	if (id == SST25_FLASH_ID)
	{
		ssz_traceln("Flash ID[0x%x]", id);
		return true;
	}
	else
	{
		ssz_traceln("Flash ID[0x%x] is wrong", id);
		return false;
	}
}

//IO define
static int drv_sst25_flash_file_read(struct ssz_file_t* file, void* buff, int buff_size, int* real_read_buff_size) {
	DrvSST25FlashDev* dev = file->dev_data;

	if (drv_sst25_flash_read(dev->address, buff, buff_size)) {
		if (real_read_buff_size)*real_read_buff_size = buff_size;
		return kSuccess;
	}
	else
	{
		if (real_read_buff_size)*real_read_buff_size = 0;
		return kError;
	}
}
static int drv_sst25_flash_file_write(struct ssz_file_t* file, const void* buff, int buff_size, int* real_write_buff_size) {
	DrvSST25FlashDev* dev = file->dev_data;

	if (drv_sst25_flash_write(dev->address, buff, buff_size)) {
		if (real_write_buff_size)*real_write_buff_size = buff_size;
		return kSuccess;
	}
	else {
		if (real_write_buff_size)*real_write_buff_size = 0;
		return kError;
	}
}
static int drv_sst25_flash_file_ctl(struct ssz_file_t* file, int command, void* param) {
	int ret = kSuccess;
	DrvSST25FlashDev* dev = file->dev_data;
	switch (command) {
	case SSZ_IOCTL_ERASE:
		ssz_assert(param);
		drv_sst25_flash_erase_one_unit(*(int32_t*)param);
		break;
	case SSZ_IOCTL_SEEK:
		ssz_assert(param);
		dev->address = *(int32_t*)param;
		break;
	case SSZ_IOCTL_IS_SUPPORT_ERASE:
		ssz_assert(param);
		*(int*)param = 1;
		break;
	case SSZ_IOCTL_IS_NEED_ERASE_BEFORE_WRITE:
		ssz_assert(param);
		*(int*)param = 1;
		break;
	case SSZ_IOCTL_ERASE_UNIT_SIZE:
		ssz_assert(param);
		*(int*)param = SST25_FLASH_ERASE_UNIT_SIZE;
		break;
	case SSZ_IOCTL_SIZE:
		ssz_assert(param);
		(*(int32_t*)param) = SST25_FLASH_SIZE;
		break;
	case SSZ_IOCTL_CURR_POS:
		ssz_assert(param);
		(*(int32_t*)param) = dev->address;
		break;
	default:
		ret = kNotSupport;
		break;
	}
	return ret;
}
//io dev
const SszIODevice kDrvSST25FlashIODev = {
	"flash",
	drv_sst25_flash_file_read,
	drv_sst25_flash_file_write,
	drv_sst25_flash_file_ctl,
};

//return ssz file
SszFile* drv_sst25_flash_file() {
	return &g_sst25_flash_file;
}


