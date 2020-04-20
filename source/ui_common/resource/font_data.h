#pragma once

#include "ssz_locale.h"
#include "ui.h"

/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif


//select font data by lang
bool font_data_select_by_lang(SszLang lang);

//get the font, it will find the first font which's heigh is same as the parameter
const Font* get_font(int font_height);

//get the font by name, style and height at the select lang
//font_style: can be regular,regular_italic,bold,bold_italic
const Font* get_font_ex(const char* font_name, const char* font_style, int font_height);

//get the font by lang, name, style and height at all lang
const Font* get_font_by_lang(SszLang lang, const char* font_name, const char* font_style, int font_height);

//ch: UTF16 char
const CharInfo* get_char_info(const Font* font, uint16_t ch);




#ifdef __cplusplus
}
#endif


