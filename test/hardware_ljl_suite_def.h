#ifdef __cplusplus
extern "C" {
#endif
SUITE(hardware_ljl) {
	SET_SETUP(test_hardware_ljl_setup, NULL);
	SET_TEARDOWN(test_hardware_ljl_teardown, NULL);
	RUN_TEST(rtc);
	RUN_TEST(hardware_ljl);
}
#ifdef __cplusplus
}
#endif

