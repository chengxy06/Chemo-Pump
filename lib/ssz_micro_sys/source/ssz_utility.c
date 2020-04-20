#include "ssz_utility.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/


/************************************************
* Function 
************************************************/


//check if all buff's byte is dest_byte
bool ssz_is_all_byte_as(const void* buff, int buff_size, uint8_t dest_byte) {
	for (int i = 0; i < buff_size; i++) {
		if (((const uint8_t*)buff)[i] != dest_byte) {
			return false;
		}
	}

	return true;
}

// Returns the number of characters required to display the signed Number.
//e.g. 10->2, -12->3
int ssz_get_char_num_to_display(int number) {
	int chars_num;

	if (number < 0) {
		chars_num = 2;
		number *= -1;
	}
	else {
		chars_num = 1;
	}
	while (number >= 10) {
		chars_num++;
		number /= 10;
	}
	return chars_num;
}

//e.g. 10 -> 0x10
uint8_t ssz_decimal_to_bcd(uint8_t dec) {
	uint8_t bcd;
	bcd = (dec / 10) * 16 + dec % 10;
	return bcd;
}
//e.g.  0x99->99
uint8_t ssz_bcd_to_decimal(uint8_t bcd) {
	int dec;
	dec = (bcd / 16) * 10 + bcd % 16;
	return dec;
}
