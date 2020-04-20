#ifdef __cplusplus
extern "C" {
#endif
SUITE(drv_rtc) {
	SET_SETUP(test_drv_rtc_setup, NULL);
	SET_TEARDOWN(test_drv_rtc_teardown, NULL);
	RUN_TEST(drv_rtc_init);
	RUN_TEST(drv_rtc_start);
	RUN_TEST(drv_rtc_stop);
	RUN_TEST(drv_rtc_set_time_individually);
	RUN_TEST(drv_rtc_get_time_individually);
	RUN_TEST(drv_rtc_set_time_all);
	RUN_TEST(drv_rtc_get_time_all);
}
#ifdef __cplusplus
}
#endif

