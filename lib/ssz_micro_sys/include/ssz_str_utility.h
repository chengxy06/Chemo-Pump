#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif
	
//find the first number at the str, return the find str
//if not find, return null
const char* ssz_str_find_number(const char* str);

//find the first not number at the str, return the find str
//if not find, return null
const char* ssz_str_find_not_number(const char* str);

//find the number and save to the numbers, return the valid number at str
int ssz_str_to_numbers(const char* str, int numbers[], int number_max_size);

/***********************************************
* Description:
*   split str by accept_delimiter(use any char at the delimiter str)
*   e.g.	char *params[10];
*			int param_size = 0;
*			param_size = ssz_str_split("hello", "eo", params, 10);
*			//it will return "h","ll"
* Argument:
*   str: the dest str, Note the dest str will be modified , set as 0 at the delimiter char
*
* Return:
*   the substr count
************************************************/
int ssz_str_split(char* str, const char* accept_delimiter, char* substrs[], int substr_max);

//convert to one utf16 char from utf8 str, return the utf16 ch
//byte_nums_to_one_utf16: how many byte to map a utf16 char
int ssz_str_utf8_to_one_utf16(const char* utf8_str, int* byte_nums_to_one_utf16);

//return how many lines at the str
int ssz_str_line_num(const char* str);

//convert hex char to int, if not hex char, return -1
int ssz_str_hex_char_to_int(char hex_ch);

//convert hex str to int
int ssz_str_hex_to_int(const char* hex_str);

//it will ignore the '\n' in the str, and print only one line
void ssz_str_print_only_one_line(const char* str);

#ifdef __cplusplus
}
#endif