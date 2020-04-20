#ifdef __cplusplus
extern "C" {
#endif
SUITE(ssz_time_utility) {
	SET_SETUP(test_ssz_time_utility_setup, NULL);
	SET_TEARDOWN(test_ssz_time_utility_teardown, NULL);
	RUN_TEST(ssz_is_leap_year);
	RUN_TEST(ssz_max_days_in_month);
	RUN_TEST(ssz_week_day);
	RUN_TEST(ssz_time_to_seconds);
	RUN_TEST(ssz_time_to_seconds_ex);
	RUN_TEST(ssz_seconds_to_time);
	RUN_TEST(ssz_seconds_to_time_ex);
	RUN_TEST(ssz_str_to_date);
	RUN_TEST(ssz_str_to_time);
}
#ifdef __cplusplus
}
#endif

