#ifdef __cplusplus
extern "C" {
#endif
SUITE(ssz_str_utility) {
	SET_SETUP(test_ssz_str_utility_setup, NULL);
	SET_TEARDOWN(test_ssz_str_utility_teardown, NULL);
	RUN_TEST(ssz_str_find_number);
	RUN_TEST(ssz_str_find_not_number);
	RUN_TEST(ssz_str_to_numbers);
	RUN_TEST(ssz_str_split);
	RUN_TEST(ssz_hex_to_int);
}
#ifdef __cplusplus
}
#endif

