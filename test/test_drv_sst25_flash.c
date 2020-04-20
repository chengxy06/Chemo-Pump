#ifdef TEST
#include "greatest.h"
#include "drv_sst25_flash.h"
#include "ssz_spi.h"
#include "ssz_common.h"
#include "ssz_utility.h"
#include <stdio.h>
#include <stdlib.h>
#include "ssz_nvram.h"

static uint8_t *g_test_sst25_flash_one_unit_buff_for_backup;
/***********************************************/
//this will call before every case at this suite
static void test_drv_sst25_flash_setup(void *arg) {
	(void)arg;
	g_test_sst25_flash_one_unit_buff_for_backup = ssz_tmp_buff_alloc(kTempAllocBackupBuffWhenWriteFlash, SSZ_BACKUP_TMP_BUFF_WHEN_WRITE_FLASH);
}
//this will call after every case at this suite
static void test_drv_sst25_flash_teardown(void *arg) {
	(void)arg;
	ssz_tmp_buff_free(g_test_sst25_flash_one_unit_buff_for_backup);
}

/***********************************************/
static void test_drv_sst25_select_slave() {
	SKIP();
}

static void test_drv_sst25_flash_init() {
  SKIP();
	ssz_traceln( "test_drv_sst25_flash_init ");
	ssz_traceln( "sst25 write start 1 ");
	ssz_traceln( "drv_sst25_flash_erase_one_unit(0) start");
	drv_sst25_flash_erase_one_unit(0);

	ssz_traceln( "drv_sst25_flash_erase_one_unit(0) end");
}

static void test_drv_sst25_flash_enable_cs() {
}

static void test_drv_sst25_flash_disable_cs() {
}
 
static void test_drv_sst25_flash_read_status() {
}

static void test_drv_sst25_flash_is_busy() {
}
	
static void test_drv_sst25_flash_wait_idle() {
	SKIP();
	unsigned int  i = 0 ;
	unsigned int  j = 0 ;
	ssz_traceln( "  g_nvram_one_unit_buff_for_backup made start ");

	for (j = 0; j < 16; j++) // 256*16
	{
		for (i = 0; i <= 0xff; i++)
		{
			g_test_sst25_flash_one_unit_buff_for_backup[j*256 + i ] = i;
		}
	}

//	for (i = 0; i < 4096; i++)
//	{
//		printf(" %02X", g_nvram_one_unit_buff_for_backup[i]);
//
//		if ((i & 31) == 31)
//		{
//			printf("\r\n");	// 每行显示16字节数据  
//		}
//		else if ((i & 31) == 15)
//		{
//			printf(" - ");
//		}
//	}
		
	ssz_traceln( "drv_sst25_flash_write(0,g_nvram_one_unit_buff_for_backup,4096) start ");	
  	drv_sst25_flash_write (0 ,(uint8_t*)g_test_sst25_flash_one_unit_buff_for_backup, 4096);
	// drv_sst25_flash_write_at_one_page drv_sst25_flash_write
	ssz_traceln( "drv_sst25_flash_write(0,g_nvram_one_unit_buff_for_backup,4096) end ");
			
	ssz_traceln( "test_drv_sst25_flash_wait_idle ");
}

static void test_drv_sst25_flash_enable_write() {
	SKIP();
	unsigned int  i = 0 ;

	for (i = 0; i < 4096; i++)
	{
		g_test_sst25_flash_one_unit_buff_for_backup[ i ] = 0;
	}
//	for (i = 0; i < 4096; i++)
//	{
//		printf(" %02X", g_test_sst25_flash_one_unit_buff_for_backup[i]);
//
//		if ((i & 31) == 31)
//		{
//			printf("\r\n");	// 每行显示16字节数据 
//		}
//		else if ((i & 31) == 15)
//		{
//			printf(" - ");
//		}
//	}

    ssz_traceln( " test_drv_sst25_flash_enable_write");
	ssz_traceln( "drv_sst25_flash_read(0 ,g_test_sst25_flash_one_unit_buff_for_backup , 4096) start");	

	drv_sst25_flash_read(0 ,(uint8_t*)g_test_sst25_flash_one_unit_buff_for_backup , 4096);
	ssz_traceln( "drv_sst25_flash_read(0 ,g_test_sst25_flash_one_unit_buff_for_backup , 4096) end");
	
	for (i = 0; i < 4096; i++)
	{
		printf(" %02X", g_test_sst25_flash_one_unit_buff_for_backup[i]);

		if ((i & 31) == 31)
		{
			printf("\r\n");	/* 每行显示16字节数据 */
		}
		else if ((i & 31) == 15)
		{
			printf(" - ");
		}
	}	
}

static void test_drv_sst25_flash_read() {
}

static void test_drv_sst25_flash_write_at_one_page() {
	SKIP();
	char str[] = "test write1";
	char str2[20]={0};
	drv_sst25_flash_erase_one_unit(0);
	drv_sst25_flash_write_at_one_page(0, str, strlen(str));
	drv_sst25_flash_read(0, str2, strlen(str));
	GASSERT(strcmp(str, str2)==0);

}

static  void test_drv_sst25_flash_write() {

}

static void test_drv_sst25_flash_erase_one_unit() {
}

static void test_drv_sst25_flash_erase_chip() {
	SKIP();
    ssz_traceln( " test_drv_sst25_flash_erase_chip");
	drv_sst25_flash_erase_chip();
	for (int i = 0; i < SST25_FLASH_SIZE; i+=4){
		drv_sst25_flash_write(i, &i, 4);
	}
	int j;
	for (int i = 0; i < SST25_FLASH_SIZE; i+=4){
		drv_sst25_flash_read(i, &j, 4);
		GASSERT(j==i);
	}
}

static void test_drv_sst25_flash_read_jedec() {
	SKIP();
	int temp = 0 ;
    ssz_traceln( "test_drv_sst25_flash_read_jedec ");
	ssz_traceln( "drv_sst25_flash_write(0,g_test_sst25_flash_one_unit_buff_for_backup,4096) start ");	
for (temp = 0; temp < 128; ++temp)
	{
		ssz_traceln( "drv_sst25_flash_write( %d,g_test_sst25_flash_one_unit_buff_for_backup,4096) end ",temp);
		drv_sst25_flash_write (temp*4096  ,(uint8_t*)g_test_sst25_flash_one_unit_buff_for_backup , 4096);	
	}
  		
	// drv_sst25_flash_write_at_one_page drv_sst25_flash_write
	ssz_traceln( "drv_sst25_flash_write(0,g_test_sst25_flash_one_unit_buff_for_backup,4096) end ");

	
}

static void test_drv_sst25_flash_file_read() {
	SKIP();
int i = 0 ;
int temp = 0 ;
	
	for (temp = 0; temp < 128; ++temp){
		ssz_traceln( "drv_sst25_flash_read(%d ,g_test_sst25_flash_one_unit_buff_for_backup , 4096) start",temp);	
		drv_sst25_flash_read(temp*4096 ,(uint8_t*)g_test_sst25_flash_one_unit_buff_for_backup , 4096);
		ssz_traceln( "drv_sst25_flash_read(%d ,g_test_sst25_flash_one_unit_buff_for_backup , 4096) end",temp);
		for (i = 0; i < 4096; i++){
			printf(" %02X", g_test_sst25_flash_one_unit_buff_for_backup[i]);

			if ((i & 31) == 31)
			{
				printf("\r\n");	/* 每行显示16字节数据 */
			}
			else if ((i & 31) == 15)
			{
				printf(" - ");
			}
		}
	}


	


    ssz_traceln( " test_drv_sst25_flash_file_read");
}

static void test_drv_sst25_flash_file_write() {
}

static void test_drv_sst25_flash_file_ctl() {
}

static void test_drv_sst25_flash_file() {
}


/***********************************************/

#include "drv_sst25_flash_suite_def.h"

#endif

//3.105: drv_sst25_flash_erase_one_unit(0) start
//3.129: drv_sst25_flash_erase_one_unit(0) end
//擦除一个扇区   4K 24ms

//3.167: drv_sst25_flash_write(0,g_test_sst25_flash_one_unit_buff_for_backup,4096) start 
//3.798: drv_sst25_flash_write(0,g_test_sst25_flash_one_unit_buff_for_backup,4096) end 
// 写一个扇区 4K 631ms

//3.820: drv_sst25_flash_read(0 ,g_test_sst25_flash_one_unit_buff_for_backup , 4096) start
//4.004: drv_sst25_flash_read(0 ,g_test_sst25_flash_one_unit_buff_for_backup , 4096) end
//读一个扇区 4K 184ms

//10.134:  test_drv_sst25_flash_erase_chip
//10.211: test_drv_sst25_flash_read_jedec 
//芯片擦除 77ms


