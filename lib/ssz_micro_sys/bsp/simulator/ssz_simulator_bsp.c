#include "ssz_config.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "ssz_common.h"
#include "sim_interface.h"
#include "ssz_i2c.h"
#include "ssz_gpio.h"
#include "ssz_spi.h"
#include "ssz_uart.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
int ssz_sim_spi1 = 1;
int ssz_sim_spi2 = 2;
int ssz_sim_spi3 = 3;
int ssz_sim_spi4 = 4;
int ssz_sim_uart1 = 1;
int ssz_sim_uart2 = 2;
int ssz_sim_uart3 = 3;
int ssz_sim_uart4 = 4;
int ssz_sim_i2c1 = 1;
int ssz_sim_i2c2 = 2;
int ssz_sim_i2c3 = 3;
int ssz_sim_i2c4 = 4;
int ssz_sim_adc = 1;
/************************************************
* Function 
************************************************/



//CPU run hz
int32_t ssz_system_clock() {
	return 24000000;
}
//system reset
void ssz_system_reset() {
	
}
//delay microsecond
void ssz_delay_us(int us)
{

}
//delay millisecond
void ssz_delay_ms(int ms)
{
	sim_delay(ms);
}
//delay millisecond and clear watchdog to let the watchdog not effect
void ssz_delay_ms_with_clear_watchdog(int ms) {
	ssz_delay_ms(ms);
}

//sleep
void ssz_sleep() {
	sim_system_sleep();
}
void ssz_gpio_set_mode(SszGpio gpio, SszPin pin_mask, SszGpioMode mode) {
	(void)gpio;
	(void)pin_mask;
	(void)mode;
}
//enable clock
void ssz_gpio_enable_clock(SszGpio gpio){
}
//return the read byte count
bool ssz_i2c_master_read(SszI2C* i2c, void* buff, int buff_size){
	(void)i2c;
	(void)buff;
	ssz_assert_fail();
	return true;
}
//return the write byte count
bool ssz_i2c_master_write(SszI2C* i2c, const void* buff, int buff_size){
	(void)i2c;
	(void)buff;
	//ssz_assert_fail();
	return true;
}

bool ssz_spi_read(SszSPI* obj, void* buff, int buff_size){
	(void)obj;
	(void)buff;
	ssz_assert_fail();
	return true;
}
bool ssz_spi_write(SszSPI* obj, const void* buff, int buff_size){
	(void)obj;
	(void)buff;
	//ssz_assert_fail();
	return true;
}
void ssz_spi_wait_write_finish(SszSPI* obj) {
	(void)obj;
}
#if SSZ_USE_DEFAULT_UART_DEFINITION_AT_SIMULATOR
//return the read byte count
bool ssz_uart_read(SszUART* obj, void* buff, int buff_size) {
	(void)obj;
	(void)buff;
	ssz_assert_fail();
	return true;
}
//return the write byte count
bool ssz_uart_write(SszUART* obj, const void* buff, int buff_size) {
	(void)obj;
	(void)buff;
	//ssz_assert_fail();
	return true;
}
#endif

#endif