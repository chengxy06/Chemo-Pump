#ifdef TEST
#include "greatest.h"
 
void run_all_test_suite();

int run_test_main(int argc, const char **argv) {
	greatest_init();
	if (argc >0)
	{
		int ret = greatest_parse_args(argc, argv);
		/* Handle command-line arguments, etc. */
		if (ret != 2) {
			return ret;
		}
	}
	else
	{
		//manual set the filter
		greatest_info.test_filter = NULL;
		greatest_info.suite_filter = NULL;
	}

	run_all_test_suite();

	return greatest_report();

}
#endif