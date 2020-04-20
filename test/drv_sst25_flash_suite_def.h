#ifdef __cplusplus
extern "C" {
#endif
SUITE(drv_sst25_flash) {
	SET_SETUP(test_drv_sst25_flash_setup, NULL);
	SET_TEARDOWN(test_drv_sst25_flash_teardown, NULL);
	RUN_TEST(drv_sst25_select_slave);
	RUN_TEST(drv_sst25_flash_init);
	RUN_TEST(drv_sst25_flash_enable_cs);
	RUN_TEST(drv_sst25_flash_disable_cs);
	RUN_TEST(drv_sst25_flash_read_status);
	RUN_TEST(drv_sst25_flash_is_busy);
	RUN_TEST(drv_sst25_flash_wait_idle);
	RUN_TEST(drv_sst25_flash_enable_write);
	RUN_TEST(drv_sst25_flash_read);
	RUN_TEST(drv_sst25_flash_write_at_one_page);
	RUN_TEST(drv_sst25_flash_write);
	RUN_TEST(drv_sst25_flash_erase_one_unit);
	RUN_TEST(drv_sst25_flash_erase_chip);
	RUN_TEST(drv_sst25_flash_read_jedec);
	RUN_TEST(drv_sst25_flash_file_read);
	RUN_TEST(drv_sst25_flash_file_write);
	RUN_TEST(drv_sst25_flash_file_ctl);
	RUN_TEST(drv_sst25_flash_file);
}
#ifdef __cplusplus
}
#endif

