/************************************************
* DESCRIPTION:
*   used to handle communication
*
************************************************/
#pragma once
#include "com_config.h"
#include "ssz_def.h"
#include "ssz_file.h"

/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//ch is the received byte
typedef bool (*ComReceiveHandler)(uint8_t ch, bool is_handled_ok_at_pre_handler);

//handle new received data
void com_handle_new_received_data(ComID com_id);
//handle one new received byte, return true if someone handled it
bool com_handle_new_byte( ComID com_id, uint8_t ch);

//bind receive handler
void com_set_receive_handler(ComID com_id, ComHandleOrder order, ComReceiveHandler handler);

SszFile* com_to_file( ComID com_id);

//init
void com_init_by_file(ComID com_id, SszFile* file);


#ifdef __cplusplus
}
#endif


