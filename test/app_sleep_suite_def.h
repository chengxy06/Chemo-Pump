#ifdef __cplusplus
extern "C" {
#endif
SUITE(app_sleep) {
	SET_SETUP(test_app_sleep_setup, NULL);
	SET_TEARDOWN(test_app_sleep_teardown, NULL);
	RUN_TEST(app_sleep_enter);
	RUN_TEST(app_sleep_enter_and_exit_at);
	RUN_TEST(app_sleep_enter_for_ms);
	RUN_TEST(app_sleep_exit);
}
#ifdef __cplusplus
}
#endif

