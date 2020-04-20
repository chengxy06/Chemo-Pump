#pragma once

/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int major;
    int minor;
    int revision;
    int internal_version;
}Version;

const Version* version_info(void);
const char* version_software_name(void);
const char* version_build_time(void);
void slaver_version_info_set(Version *slaver_version);
Version* slaver_version_info(void);


#ifdef __cplusplus
}
#endif

