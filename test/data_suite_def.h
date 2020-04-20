#ifdef __cplusplus
extern "C" {
#endif
SUITE(data) {
	SET_SETUP(test_data_setup, NULL);
	SET_TEARDOWN(test_data_teardown, NULL);
	RUN_TEST(data_nvram);
	RUN_TEST(data_read);
	RUN_TEST(data_write);
	RUN_TEST(data_read_int);
	RUN_TEST(data_read_uint);
	RUN_TEST(data_write_int);
	RUN_TEST(data_erase_block);
	RUN_TEST(data_flush);
	RUN_TEST(data_find_save_info_by_id);
	RUN_TEST(data_init);
}
#ifdef __cplusplus
}
#endif

