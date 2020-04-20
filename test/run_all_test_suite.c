#ifdef TEST
#include "greatest.h"

void run_all_test_suite() {
	RUN_SUITE(data);
	RUN_SUITE(drv_rtc);
	RUN_SUITE(drv_sst25_flash);
	RUN_SUITE(hardware_ljl);
	RUN_SUITE(record_log);
	RUN_SUITE(ssz_algorithm);
	RUN_SUITE(ssz_str_utility);
	RUN_SUITE(ssz_time);
	RUN_SUITE(ssz_time_utility);
	RUN_SUITE(ui);
	RUN_SUITE(app_sleep);
	RUN_SUITE(drv_lowpower);
	RUN_SUITE(ui_view);
}
#endif
