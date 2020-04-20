#pragma once
#include "string.h"
#include "assert.h"
#include "stdio.h"
#include "ssz_tick_time.h"
#include "ssz_config.h"
/************************************************
* Declaration
************************************************/

//trace message, valid ony when debug
#ifdef DEBUG
//simple trace
#define ssz_trace printf  
//trace with current time
#define ssz_tracet printf("%s: ", ssz_now_time_str_for_print());printf
//trace with current time and auto add line break
#define ssz_traceln(...) printf("%s: ", ssz_now_time_str_for_print());printf(__VA_ARGS__);printf("\n");
#else
#define ssz_trace(...)
#define ssz_tracet(...)
#define ssz_traceln(...)
#endif //_DEBUG

//simple output
#define ssz_printf(is_module_enable_output, ...) if(is_module_enable_output){printf(__VA_ARGS__);}
//output with current time
#define ssz_printft(is_module_enable_output, ...) if(is_module_enable_output){printf("%s: ", ssz_now_time_str_for_print());printf(__VA_ARGS__);}
//output with current time and auto add line break
#define ssz_printfln(is_module_enable_output, ...) if(is_module_enable_output){printf("%s: ", ssz_now_time_str_for_print());printf(__VA_ARGS__);printf("\n");}
#define ssz_printfln_ex(is_module_enable_output, extra_str, ...) if(is_module_enable_output){printf("%s:[%s]: ", ssz_now_time_str_for_print(), extra_str);printf(__VA_ARGS__);printf("\n");}

//assert if it is true, valid ony when debug
#ifdef _lint
#ifdef __cplusplus
extern "C"
#endif
void exit(int);
#define ssz_assert assert
#define ssz_assert_fail() exit(-1)
#elif defined SSZ_TARGET_SIMULATOR
#define ssz_assert assert
#define ssz_assert_fail() assert(0)
#define ssz_check assert
#elif defined DEBUG
#define ssz_assert(expression) (void)(                                                       \
            (!!(expression)) ||                                                              \
            (ssz_on_assert_fail(__FILE__, __LINE__, #expression), 0) \
        )
#define ssz_assert_fail() ssz_on_assert_fail(__FILE__, __LINE__, "")
//it's difference from ssz_assert is that, it will run at release mode
#define ssz_check(expression) if (!(expression))ssz_on_check_fail(__FILE__, __LINE__, #expression) 
#else
#define ssz_assert(expression) ((void)0)
#define ssz_assert_fail() ((void)0)
//it's difference from ssz_assert is that, it will run at release mode
#define ssz_check(expression) if (!(expression))ssz_on_check_fail() 
#endif




#define ssz_is_result_ok(result) ((result) == kSuccess)
#define ssz_is_result_fail(result) ((result) != kSuccess)
#define ssz_min(a,b)            (((a) < (b)) ? (a) : (b))
#define ssz_max(a,b)            (((a) > (b)) ? (a) : (b))

typedef enum {
	kTempAllocBackupBuffWhenWriteFlash,
	kTempAllocCommonBuff,
	kTempAllocBuffTypeMax,
}TempAllocBuffType;

#ifdef __cplusplus
extern "C" {
#endif

//zero memory
void ssz_mem_zero(void* buff, int buff_size);
//CPU run hz
int32_t ssz_system_clock();
//system reset
void ssz_system_reset();
//delay microsecond
void ssz_delay_us(int us);
//delay millisecond
void ssz_delay_ms(int ms);
//delay millisecond and clear watchdog to let the watchdog not effect
void ssz_delay_ms_with_clear_watchdog(int ms);
//sleep
void ssz_sleep();
void ssz_clear_watchdog();
//read a char from terminal
char ssz_get_char();

//set the last error code
void ssz_set_last_error(int error_code);
//get the last error code
int ssz_last_error();

//check if the tmp buff is >=alloc_size
void ssz_tmp_buff_check_size(TempAllocBuffType alloc_type, int alloc_size);
//alloc tmp buff, once can only one use it
void* ssz_tmp_buff_alloc(TempAllocBuffType alloc_type, int alloc_size);
//free tmp buff
void ssz_tmp_buff_free(void* alloc_buff);

#ifdef DEBUG
//call it when assert fail by ssz_assert, need hold or terminate system
void ssz_on_assert_fail(const char* filename, int lineno, const char* expression);
//call it when check fail by ssz_check, need hold or terminate system
void ssz_on_check_fail(const char* filename, int lineno, const char* expression);
#else
//call it when check fail by ssz_check, need hold or terminate system
void ssz_on_check_fail();
#endif

//abort system
void ssz_abort();
#ifdef DEBUG
//break system when at debug mode
void ssz_debug_break();
#else 
#define ssz_debug_break()
#endif

//return the now time, used at ssz_print, ssz_traceln
const char* ssz_now_time_str_for_print();

#define ssz_array_size(arr)	((sizeof(arr))/(sizeof(arr[0])))
#define ssz_array_node_size(arr)	sizeof(arr[0])
#define ssz_array_at(arr, index) (ssz_assert((index)>=0&&(index)<ssz_array_size(arr)),arr[(index)])
#define ssz_array_set(arr, index, value) (ssz_assert((index)>=0&&(index)<ssz_array_size(arr)),arr[(index)]=value)
#define ssz_array_font(arr) arr[0]
#define ssz_array_back(arr) arr[ssz_array_size(arr)-1]

#ifdef __cplusplus
}
#endif