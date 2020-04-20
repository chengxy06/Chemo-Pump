#ifdef __cplusplus
extern "C" {
#endif
SUITE(drv_lowpower) {
	SET_SETUP(test_drv_lowpower_setup, NULL);
	SET_TEARDOWN(test_drv_lowpower_teardown, NULL);
	RUN_TEST(drv_lowpower_enable);
	RUN_TEST(drv_lowpower_disable);
	RUN_TEST(drv_lowpower_enter_lpsleep);
	RUN_TEST(drv_lowpower_exit_lpsleep);
	RUN_TEST(drv_lowpower_run_at_lpsleep);
}
#ifdef __cplusplus
}
#endif

