#include "version.h"

/************************************************
* Declaration
************************************************/
#define SOFTWARE_NAME "Chemo-PumpV1_0_master"
#define SUPPORT_BOOTLOADER 0

#define MAJOR_VERSION   1
#define MINOR_VERSION   0
#define REVISION_VERSION   5
#define INTERNAL_VERSION 1382


#ifdef __ICCARM__
#define RESERVE_AT_LINK __root
#else
#define RESERVE_AT_LINK
#endif

#define M_NameToStr_(x) #x
#define M_NameToStr(x) M_NameToStr_(x)

/************************************************
* Variable 
************************************************/
static const Version g_version_info = {
	MAJOR_VERSION,MINOR_VERSION,REVISION_VERSION,INTERNAL_VERSION
};
static Version g_slaver_version = {8888,8888,8888,8888};

/************************************************
* Function 
************************************************/

/*lint --e{528}*/ 
RESERVE_AT_LINK const static char g_version_str[24] = "Ver: " M_NameToStr(MAJOR_VERSION) "." M_NameToStr(MINOR_VERSION) "." M_NameToStr(REVISION_VERSION) "." M_NameToStr(INTERNAL_VERSION);
RESERVE_AT_LINK const static char g_version_build_time[28] = __DATE__", "__TIME__;

Version* slaver_version_info(void) {
	return &g_slaver_version;
}
void slaver_version_info_set(Version *slaver_version) {
	g_slaver_version.major = slaver_version->major;
	g_slaver_version.minor = slaver_version->minor;
	g_slaver_version.revision = slaver_version->revision;
	g_slaver_version.internal_version = slaver_version->internal_version;
}

const Version* version_info(void) {
	return &g_version_info;
}
const char* version_software_name(void) {
	return SOFTWARE_NAME;
}
const char* version_build_time(void)
{
    return g_version_build_time;
}

