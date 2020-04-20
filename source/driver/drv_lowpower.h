/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0  2017-6-26 jcwu
* Initial revision.
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

extern bool g_drv_lowpower_wake_up_by_time ;
extern bool g_drv_lowpower_is_slaver_wake_up;

void drv_lowpower_enable(  void ) ;
void drv_lowpower_disable(  void );
void drv_lowpower_pcb_peripheral_power_enable( void );

extern void drv_lowpower_sleep_slaver() ;
extern void drv_lowpower_wakeup_slaver() ;
// enter lowpower mode ,
// data = 0, only for key wake up;
// data = X, auto quit low power after X ms
// return: true->exit by time out, false->exit by other interrupt
extern bool drv_lowpower_enter_lpsleep(  int data, bool is_need_hold_led_and_voice ) ;
// exit  low power sleep mode
extern void drv_lowpower_exit_lpsleep(  void ) ;

#ifdef __cplusplus
}
#endif


