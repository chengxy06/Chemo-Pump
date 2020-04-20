#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
//all language can refer ISO 639 Standard
//refer: http://www.iso.org/iso/language_codes
//http://www.loc.gov/standards/iso639-2/php/code_list.php
//http://baike.baidu.com/link?url=lgY_HTkEXNjjxskeIUQ94AN77ePFOPVHSJt3vONcuE3IyxQi1I8nW23jwn96JKNwQcbsnnYkzpS-rxSYTed58a
//https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
typedef enum
{
    kSszLangEnglish,
    kSszLangSimplifiedChinese,
    kSszLangTraditionalChinese,
    kSszLangJapanese,
    
    //add language before this line
    kSszLangMax
}SszLang;

//all country region can refer ISO 3166 Standard
//refer http://baike.baidu.com/link?url=Y3yBvZuyggTPyshXL0fpMLbPPY14_NFFLn3xCAUXGlaXgIFRLSq8TTPduyNSEUW-5Y84HfeW6wULCeWk0Kjitq  
//https://www.iso.org/obp/ui/#search
typedef enum
{
    kSszCountryRegionUS,
    kSszCountryRegionChina,
    kSszCountryRegionTaiwan,
    kSszCountryRegionJapan,

    //add before this line
    kSszCountryRegionMax
}SszCountryRegion;

#ifdef __cplusplus
extern "C" {
#endif

//get current language
SszLang ssz_locale_lang(void);

//get current country region
SszCountryRegion ssz_locale_country_region(void);

//set current language
void  ssz_locale_set_lang(SszLang lang);

//set current country region
void ssz_locale_set_country_region(SszCountryRegion country_region);

#ifdef __cplusplus
}
#endif