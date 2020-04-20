/************************************************
* DESCRIPTION:
*
*
************************************************/
#pragma once
#include "ssz_def.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void app_cmd_init(void);

//enable cmd receive
void app_cmd_enable_receive(void);
//disable cmd receive
void app_cmd_disable_receive(void);

//enable cmd respond
void app_cmd_enable_respond(void);
//disable cmd respond except the password
void app_cmd_disable_respond(void);

#ifdef __cplusplus
}
#endif

