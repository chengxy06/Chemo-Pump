#ifdef __cplusplus
extern "C" {
#endif
SUITE(ssz_algorithm) {
	SET_SETUP(test_ssz_algorithm_setup, NULL);
	SET_TEARDOWN(test_ssz_algorithm_teardown, NULL);
	RUN_TEST(ssz_crc32);
	RUN_TEST(ssz_crc32_ex);
}
#ifdef __cplusplus
}
#endif

