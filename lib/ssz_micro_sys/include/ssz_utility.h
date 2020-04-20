#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//check if all buff's byte is dest_byte
bool ssz_is_all_byte_as(const void* buff, int buff_size, uint8_t dest_byte);

// Returns the number of characters required to display the signed Number.
//e.g. 10->2, -12->3
int ssz_get_char_num_to_display(int number);

//e.g. 10 -> 0x10
uint8_t ssz_decimal_to_bcd(uint8_t dec);
//e.g.  0x99->99
uint8_t ssz_bcd_to_decimal(uint8_t bcd);

#ifdef __cplusplus
}
#endif