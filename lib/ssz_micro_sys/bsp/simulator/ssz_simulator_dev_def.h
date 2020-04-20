#pragma once
#include "ssz_gpio.h"
#ifdef SSZ_TARGET_SIMULATOR
extern int ssz_sim_spi1;
extern int ssz_sim_spi2;
extern int ssz_sim_spi3;
extern int ssz_sim_spi4;
extern int ssz_sim_uart1;
extern int ssz_sim_uart2;
extern int ssz_sim_uart3;
extern int ssz_sim_uart4;
extern int ssz_sim_i2c1;
extern int ssz_sim_i2c2;
extern int ssz_sim_i2c3;
extern int ssz_sim_i2c4;

#endif