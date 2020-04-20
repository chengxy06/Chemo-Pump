#include "font_data.h"
#include "string.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/

typedef struct
{
    const char* font_name;
    const char* font_style;
    const Font* font;
}FontData;

typedef struct
{
    SszLang lang;
    const FontData* font_data;
}LangFontData;

/************************************************
* Variable 
************************************************/
#include "generated_font_data.h"

static const FontData* g_font_data_ptr = NULL;

/************************************************
* Function 
************************************************/


//select font data by lang
bool font_data_select_by_lang(SszLang lang)
{
    int i;

    for (i = 0; i < sizeof(g_lang_font_data) / sizeof(g_lang_font_data[0]); i++)
    {
        if (g_lang_font_data[i].lang == lang)
        {
            g_font_data_ptr = g_lang_font_data[i].font_data;
            return true;
        }
    }

    return false;
}

//get the font, it will find the first font which's heigh is same as the parameter
const Font* get_font(int font_height)
{
    const FontData* work = g_font_data_ptr;
    
    ssz_assert(g_font_data_ptr);

    //find the height font
    while (work && work->font_name)
    {
        if (work->font->height== font_height)
        {
            return work->font;
        }
        work++;
    }

    return NULL;
}

//get the font by name, style and height at the select lang
//font_style: can be regular,regular_italic,bold,bold_italic
const Font* get_font_ex(const char* font_name, const char* font_style, int font_height)
{
    const FontData* work = g_font_data_ptr;

    ssz_assert(g_font_data_ptr);

    //find the font
    while (work && work->font_name)
    {
        if (work->font->height == font_height && strcmp(work->font_name,font_name)==0
            && strcmp(work->font_style, font_style)==0 )
        {
            return work->font;
        }
        work++;
    }

    return NULL;
}

//get the font by lang, name, style and height at all lang
const Font* get_font_by_lang(SszLang lang, const char* font_name, const char* font_style, int font_height)
{
    const FontData* work = NULL;
    int i;

    //find the lang font
    for (i = 0; i < sizeof(g_lang_font_data) / sizeof(g_lang_font_data[0]); i++)
    {
        if (g_lang_font_data[i].lang == lang)
        {
            work = g_lang_font_data[i].font_data;
            break;
        }
    }

    //find the font
    while (work && work->font_name)
    {
        if (work->font->height == font_height && strcmp(work->font_name, font_name) == 0
            && strcmp(work->font_style, font_style)==0 )
        {
            return work->font;
        }
        work++;
    }

    return NULL;
}

//ch: UTF16 char
const CharInfo* get_char_info(const Font* font, uint16_t ch) {
	for (int i = 0; i < font->char_count; i++) {
		if (font->char_info[i].char_code == ch) {
			return &font->char_info[i];
		}
	}

	return NULL;
}