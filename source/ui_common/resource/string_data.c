#include "string_data.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
typedef struct
{
    StrID string_id;
    const char *str;
}StringData;

typedef struct
{
    SszLang lang;
    const StringData* string_data;
}LangStringData;


/************************************************
* Variable 
************************************************/

//string data define
#include "generated_string_data.h"

static const StringData* g_string_data_ptr = NULL;
static char g_dynamic_strs[kStrDynamicIDEnd- kStrDynamicIDStart+1][DYNAMIC_STRING_MAX_SIZE];

/************************************************
* Function 
************************************************/

//select string data by the language
bool string_data_select_by_lang(SszLang lang)
{
    int i;

    for(i=0; i<sizeof(g_lang_string_data)/sizeof(g_lang_string_data[0]); i++)
    {
        if(g_lang_string_data[i].lang == lang)
        {
            g_string_data_ptr = g_lang_string_data[i].string_data;
            return true;
        }
    }

    return false;
}

//get string from the string id
//Note: the string is encode as UTF8
const char* get_string(StrID str_id)
{
	if (str_id>= kStrDynamicIDStart) {
		return get_dynamic_string(str_id);
	}

	ssz_assert(str_id == g_string_data_ptr[str_id].string_id);
    return g_string_data_ptr[str_id].str;
}

//get string from the string id and lang
//Note: the string is encode as UTF8
const char* get_string_by_lang(SszLang lang, StrID str_id)
{
    int i;
    const StringData *work = NULL;

	if (str_id >= kStrDynamicIDStart) {
		return get_dynamic_string(str_id);
	}
    for (i = 0; i < sizeof(g_lang_string_data) / sizeof(g_lang_string_data[0]); i++)
    {
        if (g_lang_string_data[i].lang == lang)
        {
            work = g_lang_string_data[i].string_data;
            break;
        }
    }


    ssz_assert(work);
    if (work)
    {
        return work[str_id].str;
    }
    return NULL;
}

//get dynamic string from the string id
char * get_dynamic_string(StrID str_id)
{
	ssz_assert(str_id>= kStrDynamicIDStart && str_id <= kStrDynamicIDEnd);
	if (str_id >= kStrDynamicIDStart && str_id <= kStrDynamicIDEnd) {
		return g_dynamic_strs[str_id- kStrDynamicIDStart];
	}else{
		return NULL;
	}
}


//set dynamic string
void set_dynamic_string(StrID str_id, const char* str) {
	ssz_assert(str_id >= kStrDynamicIDStart && str_id <= kStrDynamicIDEnd);
	if (str_id >= kStrDynamicIDStart && str_id <= kStrDynamicIDEnd) {
		g_dynamic_strs[str_id - kStrDynamicIDStart][0] = 0;
		strncat(g_dynamic_strs[str_id - kStrDynamicIDStart], str, DYNAMIC_STRING_MAX_SIZE-1);
	}
}
