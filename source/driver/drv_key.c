#include "drv_key.h"
#include "ssz_gpio.h"
#include "dev_def.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
const static SszGpioPinEx g_key_gpios[kKeyIDMax] ={
    //{KEY_POWER_GPIO_Port, KEY_POWER_Pin, false},
    {KEY_LEFT_GPIO_Port, KEY_LEFT_Pin, false},
    {KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin, false},	
    {KEY_CONFIRM_GPIO_Port,KEY_CONFIRM_Pin, false},
    {KEY_BACK_GPIO_Port,KEY_BACK_Pin, false},
    {KEY_START_GPIO_Port, KEY_START_Pin, false},
};

/************************************************
* Function 
************************************************/
//it will scan all key state and put at the param
void drv_key_scan(int8_t key_state[kKeyIDMax]){
    for (int i = 0; i < kKeyIDMax; ++i) {
        key_state[i] = ssz_gpio_is_valid(&g_key_gpios[i]);
    }
}
bool drv_key_is_pressed(KeyID key){
    return ssz_gpio_is_valid(&g_key_gpios[key]);
}

bool drv_key_is_high(KeyID key){
	return ssz_gpio_is_high(g_key_gpios[key].gpio, g_key_gpios[key].pin_mask);
}

