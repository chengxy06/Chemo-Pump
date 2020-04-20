#pragma once
#include "stdint.h"
/************************************************
* Declaration
************************************************/
#include "sim_interface.h"

typedef unsigned short BSP_PIN;
typedef int* BSP_GPIO;

#define BSP_GPIOA &g_sim_gpio[0]
#define BSP_GPIOB &g_sim_gpio[1]
#define BSP_GPIOC &g_sim_gpio[2]
#define BSP_GPIOD &g_sim_gpio[3]
#define BSP_GPIOE &g_sim_gpio[4]
#define BSP_GPIOF &g_sim_gpio[5]
#define BSP_GPIOG &g_sim_gpio[6]
#define BSP_GPIOH &g_sim_gpio[7]
#define BSP_GPIOI &g_sim_gpio[8]
#define BSP_GPIOJ &g_sim_gpio[9]
#define BSP_GPIOK &g_sim_gpio[10]

#define bsp_gpio_set(...) sim_gpio_setbits(__VA_ARGS__)
#define bsp_gpio_quick_set(...) sim_gpio_setbits(__VA_ARGS__)
#define bsp_gpio_clear(...) sim_gpio_resetbits(__VA_ARGS__)
#define bsp_gpio_quick_clear(...) sim_gpio_resetbits(__VA_ARGS__)
#define bsp_gpio_get(...) sim_gpio_is_pin_set(__VA_ARGS__)
#define bsp_gpio_quick_get(...) sim_gpio_is_pin_set(__VA_ARGS__)

