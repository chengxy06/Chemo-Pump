#include "ssz_time_utility.h"
#include "ssz_common.h"
#include <stdlib.h>
#include <ctype.h>

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
//find the first number at the str, return the find str
//if not find, return null
const char* ssz_str_find_number(const char* str) {
	const char* p =str;
	ssz_assert(str);
	if (p) {
		while (*p != 0) {
			if (*p>='0' && *p<='9') {
				break;
			}
			else {
				p++;
			}
		}
		if (*p == 0) {
			p = NULL;
		}
	}

	return p;
}

//find the first not number at the str, return the find str
//if not find, return null
const char* ssz_str_find_not_number(const char* str) {
	const char* p = str;
	ssz_assert(str);
	if (p) {
		while (*p != 0) {
			if (*p < '0' || *p > '9') {
				break;
			}
			else {
				p++;
			}
		}
		if (*p == 0) {
			p = NULL;
		}
	}

	return p;
}

//find the number and save to the numbers, return the valid number at str
int ssz_str_to_numbers(const char* str, int numbers[], int number_max_size) {
	ssz_assert(str && numbers);
	const char* p = str;
	int number_size = 0;
	p = ssz_str_find_number(p);
	while (p!=NULL) {
		if (number_size < number_max_size) {
			numbers[number_size] = atoi(p);
			number_size++;
		}
		else {
			break;
		}
		p = ssz_str_find_not_number(p);
		if (p) {
			p = ssz_str_find_number(p);
		}
	}
	return number_size;
}

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
int ssz_str_split(char* str, const char* accept_delimiter, char* substrs[], int substr_max)
{
	if (!str) {
		return 0;
	}

	int substr_index = 0;
	bool is_in_substr = false;
	while (*str!=0 && substr_index < substr_max) {
		if (strchr(accept_delimiter, *str)!=NULL) {
			//if find the delimiter, set it as 0 and skip
			*str = 0;
			is_in_substr = false;//substr is end
		}
		else if (!is_in_substr) {
			is_in_substr = true;
			//get one substr start
			substrs[substr_index] = str;
			substr_index++;
		}
		str++;
	}

	return substr_index;
}

//convert to one utf16 char from utf8 str, return the utf16 ch
//byte_nums_to_one_utf16: how many byte to map a utf16 char
int ssz_str_utf8_to_one_utf16(const char* utf8_str, int* byte_nums_to_one_utf16)
{
	int str_len = strlen(utf8_str);
	char a;
	uint16_t ret=0;

	a = *utf8_str;
	if ((a & 0x80) == 0x0)              // one byte
	{
		ret += a;
		if (byte_nums_to_one_utf16) {
			*byte_nums_to_one_utf16 = 1;
		}
	}
	else if ((a & 0xE0) == 0xC0 ) // two byte
	{
		if (str_len < 2)
		{
			ssz_assert_fail();
			return 0; //lint !e527
		}
		if (byte_nums_to_one_utf16) {
			*byte_nums_to_one_utf16 = 2;
		}
		ret += (((int)(a & 0x1F)) << 6)
			| (utf8_str[1] & 0x3F);
	}
	else if ((a & 0xF0) == 0xE0) // three byte
	{
		if (str_len < 3)
		{
			ssz_assert_fail();
			return 0;//lint !e527
		}
		if (byte_nums_to_one_utf16) {
			*byte_nums_to_one_utf16 = 3;
		}
		ret += (((int)(a & 0x0F)) << 12)
			| (((int)(utf8_str[1] & 0x3F)) << 6)
			| (utf8_str[2] & 0x3F);
	}
	else {
		if (byte_nums_to_one_utf16) {
			*byte_nums_to_one_utf16 = 0;
		}
		ssz_assert_fail();
	}

	return ret;
}

//return how many lines at the str
int ssz_str_line_num(const char* str) {
	int ret = 1;
	while (*str!=0) {
		if (*str == '\n') {
			ret++;
		}
		str++;
	}

	return ret;
}

//convert hex char to int, if not hex char, return -1
int ssz_str_hex_char_to_int(char hex_ch) {
	if (isdigit(hex_ch)) {
		return hex_ch - '0';
	}
	else if (hex_ch >= 'A' && hex_ch <= 'F') {
		return hex_ch + 10 - 'A';
	}
	else if (hex_ch >= 'a' && hex_ch <= 'f') {
		return hex_ch + 10 - 'a';
	}
	else {
		return -1;
	}
}

//convert hex str to int
int ssz_str_hex_to_int(const char* hex_str) {
	uint32_t num = 0;
	const char* p = hex_str;
	if (strlen(hex_str)>2) {
		if ((hex_str[0]=='0') && ((hex_str[1]=='x')|| (hex_str[1] == 'X')) ) {
			p += 2;
		}
	}
	int tmp;
	while (*p!=0) {
		tmp = ssz_str_hex_char_to_int(*p);
		if (tmp==-1) {
			break;
		}
		else {
			num <<= 4;
			num |= tmp;
		}
		p++;
	}

	return num;
}

//it will ignore the '\n' in the str, and print only one line
void ssz_str_print_only_one_line(const char* str) {
	const char* p = str;
	while (*p != 0) {
		if (*p == '\n') {
			putchar(' ');
		}
		else {
			putchar(*p);
		}
		p++;
	}
}