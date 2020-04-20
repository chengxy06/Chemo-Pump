#ifdef __cplusplus
extern "C" {
#endif
SUITE(ssz_time) {
	SET_SETUP(test_ssz_time_setup, NULL);
	SET_TEARDOWN(test_ssz_time_teardown, NULL);
	RUN_TEST(ssz_time_now);
	RUN_TEST(ssz_time_now_seconds);
	RUN_TEST(ssz_time_str);
	RUN_TEST(ssz_time_format);
}
#ifdef __cplusplus
}
#endif

