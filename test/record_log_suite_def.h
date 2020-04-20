#ifdef __cplusplus
extern "C" {
#endif
SUITE(record_log) {
	SET_SETUP(test_record_log_setup, NULL);
	SET_TEARDOWN(test_record_log_teardown, NULL);
	RUN_TEST(record_log_init);
	RUN_TEST(record_log_add);
	RUN_TEST(record_log_add_with_data);
	RUN_TEST(record_log_get_from_last);
	RUN_TEST(record_log_get_from_last_with_var);
	RUN_TEST(record_log_data_to_var);
	RUN_TEST(record_log_var_to_data);
	RUN_TEST(record_log_is_empty);
	RUN_TEST(record_log_size);
	RUN_TEST(record_log_clear);
}
#ifdef __cplusplus
}
#endif

