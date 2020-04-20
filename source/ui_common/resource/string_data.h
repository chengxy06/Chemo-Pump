#pragma once
#include "ssz_locale.h"

/************************************************
* Declaration
************************************************/

#define  DYNAMIC_STRING_MAX_SIZE 64
#define DYNAMIC_STRING_ID_DEFINE 	\
	kStrDynamicIDStart, \
	kStrDynamic1 = kStrDynamicIDStart, \
	kStrDynamic2, \
	kStrDynamic3, \
	kStrDynamic4, \
	kStrDynamic5, \
	kStrDynamic6, \
	kStrDynamicIDEnd = kStrDynamic6

#include "generated_string_id.h"


#ifdef __cplusplus
extern "C" {
#endif

//select string data by the language
bool string_data_select_by_lang(SszLang lang);

//get string from the string id
//Note: the string is encode as UTF8
const char* get_string(StrID str_id);

//get string from the string id and lang
//Note: the string is encode as UTF8
const char* get_string_by_lang(SszLang lang, StrID str_id);

//get dynamic string from the string id
char* get_dynamic_string(StrID str_id);

//set dynamic string
void set_dynamic_string(StrID str_id, const char* str);


#ifdef __cplusplus
}
#endif

