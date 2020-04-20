/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-07 xqzhao
* Initial revision.
*
************************************************/
#include "ssz_def.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern bool g_interrupt_is_confirm_key_pressed;

void HAL_SYSTICK_Callback(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif


