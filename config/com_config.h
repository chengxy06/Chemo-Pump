#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	kComPCUart, //communicate with PC by UART
	kComMCUUart, //Master-Slaver communicate
    kComBTUart, //communicate with BlueTooth by UART
	kComIDMax
}ComID;


//the handler order 
typedef enum
{
    kComHandleFirst, //it will handle first
    kComHandleSecond, //it will handle data second(after first handle finish)
    kComHandleThird, //it will handle data third(after first and second handle finish)

    kComHandleMax
}ComHandleOrder;



#ifdef __cplusplus
}
#endif



