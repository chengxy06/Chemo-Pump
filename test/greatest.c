#ifdef TEST
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "greatest.h"

#define GREEN_COLOR "\033[32m"
#define RED_COLOR "\033[31m"
#define YELLOW_COLOR "\033[33m"
#define RESET_COLOR "\033[0m"

#ifdef GREATEST_OUTPUT_WITH_NO_COLOR
#define FAIL_STR "FAIL"
#define PASS_STR "PASS"
#define SKIP_STR "SKIP"
#else
#define FAIL_STR RED_COLOR "FAIL" RESET_COLOR
#define PASS_STR GREEN_COLOR "PASS" RESET_COLOR
#define SKIP_STR YELLOW_COLOR "SKIP" RESET_COLOR
#endif

/* Is FILTER a subset of NAME? */                                       
static int greatest_name_match(const char *name,                        
    const char *filter) {                                               
    size_t offset = 0;                                                  
    size_t filter_len = strlen(filter);     
	if (filter_len == 0) {
		return 1;
	}
    while (name[offset] != '\0') {                                      
        if (name[offset] == filter[0]) {                                
            if (0 == strncmp(&name[offset], filter, filter_len)) {      
                return 1;                                               
            }                                                           
        }                                                               
        offset++;                                                       
    }                                                                   
                                                                        
    return 0;                                                           
}                                                                       
                                                                        
int greatest_pre_test(const char *name) {                               
    if (!GREATEST_LIST_ONLY()                                           
        && (!GREATEST_FIRST_FAIL() || greatest_info.suite.failed == 0)  
        && (greatest_info.test_filter == NULL ||                        
            greatest_name_match(name, greatest_info.test_filter))) {    
        GREATEST_SET_TIME(greatest_info.suite.pre_test);                
        if (greatest_info.setup) {                                      
            greatest_info.setup(greatest_info.setup_udata);             
        }                                                               
        return 1;               /* test should be run */                
    } else {                                                            
        return 0;               /* skipped */                           
    }                                                                   
}                                                                       
                                                                        
void greatest_post_test(const char *name, int res) {                    
    GREATEST_SET_TIME(greatest_info.suite.post_test);                   
    if (greatest_info.teardown) {                                       
        void *udata = greatest_info.teardown_udata;                     
        greatest_info.teardown(udata);                                  
    }                                                                   
                                                                        
    if (res <= GREATEST_TEST_RES_FAIL) {                                
        greatest_do_fail(name);                                         
    } else if (res >= GREATEST_TEST_RES_SKIP) {                         
        greatest_do_skip(name);
		if (!GREATEST_IS_VERBOSE())
		{
			greatest_info.col++;
		}
    } else if (res == GREATEST_TEST_RES_PASS) {                         
        greatest_do_pass(name);
		if (!GREATEST_IS_VERBOSE())
		{
			greatest_info.col++;
		}
    }                                                                   
    greatest_info.suite.tests_run++;                                    
    if (GREATEST_IS_VERBOSE()) {                                        
        GREATEST_CLOCK_DIFF(greatest_info.suite.pre_test,               
            greatest_info.suite.post_test);                             
        fprintf(GREATEST_STDOUT, "\n");                                 
    } else if (greatest_info.col!=0 && greatest_info.col % greatest_info.width == 0) {
        fprintf(GREATEST_STDOUT, "\n");                                 
        greatest_info.col = 0;                                          
    }                                                                   
    if (GREATEST_STDOUT == stdout) fflush(stdout);                      
}                                                                       
                                                                        
static void report_suite(void) {                                        
    if (greatest_info.suite.tests_run > 0) {  
        /* add linebreak if in line of '.'s */                          
        if (greatest_info.col != 0) {                                   
            fprintf(GREATEST_STDOUT, "\n");                             
            greatest_info.col = 0;                                      
        }   
        fprintf(GREATEST_STDOUT,                                        
            "= %u test%s - %u passed, %u failed, %u skipped",           
            greatest_info.suite.tests_run,                              
            greatest_info.suite.tests_run == 1 ? "" : "s",              
            greatest_info.suite.passed,                                 
            greatest_info.suite.failed,                                 
            greatest_info.suite.skipped);                               
        GREATEST_CLOCK_DIFF(greatest_info.suite.pre_suite,              
            greatest_info.suite.post_suite);                            
        fprintf(GREATEST_STDOUT, "\n");                                 
    }                                                                   
}                                                                       
                                                                        
static void update_counts_and_reset_suite(void) {                       
    greatest_info.setup = NULL;                                         
    greatest_info.setup_udata = NULL;                                   
    greatest_info.teardown = NULL;                                      
    greatest_info.teardown_udata = NULL;                                
    greatest_info.passed += greatest_info.suite.passed;                 
    greatest_info.failed += greatest_info.suite.failed;                 
    greatest_info.skipped += greatest_info.suite.skipped;               
    greatest_info.tests_run += greatest_info.suite.tests_run;           
    memset(&greatest_info.suite, 0, sizeof(greatest_info.suite));       
    greatest_info.col = 0;                                              
}                                                                       
                                                                        
void greatest_run_suite(greatest_suite_cb *suite_cb,             
                               const char *suite_name) {                
    if (greatest_info.suite_filter &&                                   
        !greatest_name_match(suite_name, greatest_info.suite_filter)) { 
        return;                                                         
    }                                                                   
    update_counts_and_reset_suite();                                    
    if (GREATEST_FIRST_FAIL() && greatest_info.failed > 0) { return; }  
    fprintf(GREATEST_STDOUT, "\n* Suite %s:\n", suite_name);            
    GREATEST_SET_TIME(greatest_info.suite.pre_suite);                   
    suite_cb();                                                         
    GREATEST_SET_TIME(greatest_info.suite.post_suite);                  
    report_suite();                                                     
}                                                                       
                                                                        
void greatest_do_pass(const char *name) {                               
    if (GREATEST_IS_VERBOSE()) {                                        
        fprintf(GREATEST_STDOUT, PASS_STR " %s: %s",                         
            name, greatest_info.msg ? greatest_info.msg : "");          
    } else {                                                            
        fprintf(GREATEST_STDOUT, ".");                                  
    }                                                                   
    greatest_info.suite.passed++;                                       
}                                                                       
                                                                        
void greatest_do_fail(const char *name) {                               
    if (GREATEST_IS_VERBOSE()) {                                        
//	        fprintf(GREATEST_STDOUT,                                        
//	            "FAIL %s: %s (%s:%u)",                                      
//	            name, greatest_info.msg ? greatest_info.msg : "",           
//	            greatest_info.fail_file, greatest_info.fail_line);      
        fprintf(GREATEST_STDOUT,                                        
            FAIL_STR " %s: %s",                                      
            name, greatest_info.msg ? greatest_info.msg : "");   
    } else {                                                            
        //fprintf(GREATEST_STDOUT, "F");                                  
        //greatest_info.col++;                                            
        /* add linebreak if in line of '.'s */                          
        if (greatest_info.col != 0) {                                   
            fprintf(GREATEST_STDOUT, "\n");                             
            greatest_info.col = 0;                                      
        }                                                               
//	        fprintf(GREATEST_STDOUT, "FAIL %s: %s (%s:%u)\n",               
//	            name,                                                       
//	            greatest_info.msg ? greatest_info.msg : "",                 
//	            greatest_info.fail_file, greatest_info.fail_line);          
        fprintf(GREATEST_STDOUT, FAIL_STR " %s\n",               
            name);      
    }                                                                   
    greatest_info.suite.failed++;                                       
}                                                                       
                                                                        
void greatest_do_skip(const char *name) {                               
    if (GREATEST_IS_VERBOSE()) {                                        
        fprintf(GREATEST_STDOUT, SKIP_STR " %s: %s",                         
            name,                                                       
            greatest_info.msg ?                                         
            greatest_info.msg : "" );                                   
    } else {                                                            
        fprintf(GREATEST_STDOUT, "s");                                  
    }                                                                   
    greatest_info.suite.skipped++;                                      
}                                                                       
                                                                        
int greatest_do_assert_equal_t(const void *exp, const void *got,        
        greatest_type_info *type_info, void *udata) {                   
    int eq = 0;                                                         
    if (type_info == NULL || type_info->equal == NULL) {                
        return 0;                                                       
    }                                                                   
    eq = type_info->equal(exp, got, udata);                             
    if (!eq) {                                                          
        if (type_info->print != NULL) {                                 
            fprintf(GREATEST_STDOUT, "\nExpected: ");                   
            (void)type_info->print(exp, udata);                         
            fprintf(GREATEST_STDOUT, "\n     Got: ");                   
            (void)type_info->print(got, udata);                         
            fprintf(GREATEST_STDOUT, "\n");                             
        } else {                                                        
            fprintf(GREATEST_STDOUT,                                    
                "GREATEST_ASSERT_EQUAL_T failure at %s:%u\n",           
                greatest_info.fail_file,                                
                greatest_info.fail_line);                               
        }                                                               
    }                                                                   
    return eq;                                                          
}                                                                       
                                                                        
void greatest_usage(const char *name) {                                 
    fprintf(GREATEST_STDOUT,                                            
        "Usage: %s [-hlfv] [-s SUITE] [-t TEST]\n"                      
        "  -h, --help  print this Help\n"                               
        "  -l          List suites and their tests, then exit\n"        
        "  -f          Stop runner after first failure\n"               
		"  -v          Verbose output\n"
		"  -a          assert when meet test fail\n"
        "  -s SUITE    only run suites containing string SUITE\n"       
        "  -t TEST     only run tests containing string TEST\n",        
        name);                                                          
}                                                                       
                                                                        
int greatest_parse_args(int argc, const char **argv) {                
    int i = 0;                                                          
    for (i = 0; i < argc; i++) {      
		if (argv[i][0] == 0) {
			continue;
		}
        if (0 == strncmp("-t", argv[i], 2)) {                           
            if (argc <= i + 1) {                                        
                greatest_usage(argv[0]);                                
                return EXIT_FAILURE;                                     
            }                                                           
            greatest_info.test_filter = argv[i+1];                      
            i++;                                                        
        } else if (0 == strncmp("-s", argv[i], 2)) {                    
            if (argc <= i + 1) {                                        
                greatest_usage(argv[0]);                                
                return EXIT_FAILURE;                                     
            }                                                           
            greatest_info.suite_filter = argv[i+1];                     
            i++;                                                        
        } else if (0 == strncmp("-f", argv[i], 2)) {                    
            greatest_info.flags |= GREATEST_FLAG_FIRST_FAIL;        
		}else if (0 == strncmp("-a", argv[i], 2)) {
			greatest_info.flags |= GREATEST_FLAG_ASSERT;
        } else if (0 == strncmp("-v", argv[i], 2)) {                    
            greatest_info.verbosity++;                                  
        } else if (0 == strncmp("-l", argv[i], 2)) {                    
            greatest_info.flags |= GREATEST_FLAG_LIST_ONLY;             
        } else if (0 == strncmp("-h", argv[i], 2) ||                    
                   0 == strncmp("--help", argv[i], 6)) {                
            greatest_usage(argv[0]);                                    
            return EXIT_SUCCESS;                                         
        } else if (0 == strncmp("--", argv[i], 2)) {                    
            break;                                                      
        } else {                                                        
            fprintf(GREATEST_STDOUT,                                    
                "Unknown argument '%s'\n", argv[i]);                    
            greatest_usage(argv[0]);                                    
            return EXIT_FAILURE;                                         
        }                                                               
    }   

	return 2;
}                                                                       
                                                                        
int greatest_all_passed(void) { return (greatest_info.failed == 0); }   
                                                                        
void greatest_set_test_filter(const char *name) {                       
    greatest_info.test_filter = name;                                   
}                                                                       
                                                                        
void greatest_set_suite_filter(const char *name) {                      
    greatest_info.suite_filter = name;                                  
}                                                                       
                                                                        
void greatest_get_report(struct greatest_report_t *report) {            
    if (report) {                                                       
        report->passed = greatest_info.passed;                          
        report->failed = greatest_info.failed;                          
        report->skipped = greatest_info.skipped;                        
        report->assertions = greatest_info.assertions;                  
    }                                                                   
}                                                                       
                                                                        
unsigned int greatest_get_verbosity(void) {                             
    return greatest_info.verbosity;                                     
}                                                                       
                                                                        
void greatest_set_verbosity(unsigned int verbosity) {                   
    greatest_info.verbosity = (unsigned char)verbosity;                 
}                                                                       
                                                                        
void greatest_set_flag(greatest_flag_t flag) {                          
    greatest_info.flags |= flag;                                        
}                                                                       
                                                                        
void GREATEST_SET_SETUP_CB(greatest_setup_cb *cb, void *udata) {        
    greatest_info.setup = cb;                                           
    greatest_info.setup_udata = udata;                                  
}                                                                       
                                                                        
void GREATEST_SET_TEARDOWN_CB(greatest_teardown_cb *cb,                 
                                    void *udata) {                      
    greatest_info.teardown = cb;                                        
    greatest_info.teardown_udata = udata;                               
}                                                                       
                                                                        
static int greatest_string_equal_cb(const void *exp, const void *got,   
    void *udata) {                                                      
    size_t *size = (size_t *)udata;                                     
    return (size != NULL                                                
        ? (0 == strncmp((const char *)exp, (const char *)got, *size))   
        : (0 == strcmp((const char *)exp, (const char *)got)));         
}                                                                       
                                                                        
static int greatest_string_printf_cb(const void *t, void *udata) {      
    (void)udata; /* note: does not check \0 termination. */             
    return fprintf(GREATEST_STDOUT, "%s", (const char *)t);             
}                                                                       
                                                                        
greatest_type_info greatest_type_info_string = {                        
    greatest_string_equal_cb,                                           
    greatest_string_printf_cb,                                          
};                                                                      
                                                                        
static int greatest_memory_equal_cb(const void *exp, const void *got,   
    void *udata) {                                                      
    greatest_memory_cmp_env *env = (greatest_memory_cmp_env *)udata;    
    return (0 == memcmp(exp, got, env->size));                          
}                                                                       
                                                                        
static int greatest_memory_printf_cb(const void *t, void *udata) {      
    greatest_memory_cmp_env *env = (greatest_memory_cmp_env *)udata;    
    unsigned char *buf = (unsigned char *)t, diff_mark = ' ';           
    FILE *out = GREATEST_STDOUT;           
    (void)out;
    size_t i, line_i, line_len = 0;                                     
    int len = 0;   /* format hexdump with differences highlighted */    
    for (i = 0; i < env->size; i+= line_len) {                          
        diff_mark = ' ';                                                
        line_len = env->size - i;                                       
        if (line_len > 16) { line_len = 16; }                           
        for (line_i = i; line_i < i + line_len; line_i++) {             
            if (env->exp[line_i] != env->got[line_i]) diff_mark = 'X';  
        }                                                               
        len += fprintf(out, "\n%04x %c ", (unsigned int)i, diff_mark);  
        for (line_i = i; line_i < i + line_len; line_i++) {             
            int m = env->exp[line_i] == env->got[line_i]; /* match? */  
            len += fprintf(out, "%02x%c", buf[line_i], m ? ' ' : '<');  
        }                                                               
        for (line_i = 0; line_i < 16 - line_len; line_i++) {            
            len += fprintf(out, "   ");                                 
        }                                                               
        fprintf(out, " ");                                              
        for (line_i = i; line_i < i + line_len; line_i++) {             
            unsigned char c = buf[line_i];                              
            len += fprintf(out, "%c", isprint(c) ? c : '.');            
        }                                                               
    }                                                                   
    len += fprintf(out, "\n");                                          
    return len;                                                         
}                                                                       
                                                                        
greatest_type_info greatest_type_info_memory = {                        
    greatest_memory_equal_cb,                                           
    greatest_memory_printf_cb,                                          
};                                                                      

void greatest_init()
{
	/* initialization. */
	/* Suppress unused function warning if features aren't used */
	(void)greatest_run_suite;
	(void)greatest_parse_args;

	memset(&greatest_info, 0, sizeof(greatest_info));
	greatest_info.width = GREATEST_DEFAULT_WIDTH;
	GREATEST_SET_TIME(greatest_info.begin);
}
int greatest_report()
{
	/* Report passes, failures, skipped tests, the number of
	* assertions, and the overall run time. */
	if (!GREATEST_LIST_ONLY()) {
		update_counts_and_reset_suite();
		GREATEST_SET_TIME(greatest_info.end);
		fprintf(GREATEST_STDOUT,
			"\nTotal: %u test%s",
			greatest_info.tests_run,
			greatest_info.tests_run == 1 ? "" : "s");
		GREATEST_CLOCK_DIFF(greatest_info.begin,
			greatest_info.end);
		fprintf(GREATEST_STDOUT, ", %u assertion%s\n",
			greatest_info.assertions,
			greatest_info.assertions == 1 ? "" : "s");
		fprintf(GREATEST_STDOUT,
			"Pass: %u, fail: %u, skip: %u.\n",
			greatest_info.passed,
			greatest_info.failed, greatest_info.skipped);
	}

	return (greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE);
}

greatest_run_info greatest_info;
enum greatest_test_res greatest_last_test_result;
#endif