#pragma once
/************************************************
* Declaration
************************************************/
#include "ssz_def.h"
#include "ssz_file.h"

typedef struct 
{
	SszFile file;
	void* obj;
	//USART need set is as true, UART need set it as false
	bool is_enable_synchronous;
}SszUART;

//io dev
extern const SszIODevice kSszUARTIODev;


#ifdef __cplusplus
extern "C" {
#endif

//is_enable_synchronous: USART need set is as true, UART need set it as false
void ssz_uart_init(SszUART* obj, void *original_dev, bool is_enable_synchronous);
SszFile* ssz_uart_file(SszUART* obj);

bool ssz_uart_read(SszUART* obj, void *buff, int buff_size);
bool ssz_uart_write(SszUART* obj, const void *buff, int buff_size);

#ifdef __cplusplus
}
#endif
