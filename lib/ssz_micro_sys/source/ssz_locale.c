#include "ssz_locale.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static SszLang g_locale_lang;
static SszCountryRegion g_locale_country_region;


/************************************************
* Function 
************************************************/

//get current language
SszLang ssz_locale_lang(void)
{
    return g_locale_lang;
}

//get current country region
SszCountryRegion ssz_locale_country_region(void)
{
    return g_locale_country_region;
}

//set current language
void  ssz_locale_set_lang(SszLang lang)
{
    g_locale_lang = lang;
}

//set current country region
void ssz_locale_set_country_region(SszCountryRegion country_region)
{
    g_locale_country_region = country_region;
}


