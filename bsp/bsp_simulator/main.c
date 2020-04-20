/************************************************
* DESCRIPTION:
*   
************************************************/
#include "main_run.h"
#include "init.h"
#include "sim_interface.h"
#include "ssz_tick_time.h"
#include "config.h"
#include "drv_interrupt.h"
#include "drv_com.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
/*!< Interrupt Number Definition */
typedef enum
{
	/******  Cortex-M3 Processor Exceptions Numbers ******************************************************/
	SVC_IRQn = -5,     /*!< 11 Cortex-M3 SV Call Interrupt                          */
	PendSV_IRQn = -2,     /*!< 14 Cortex-M3 Pend SV Interrupt                          */
	SysTick_IRQn = -1,     /*!< 15 Cortex-M3 System Tick Interrupt                      */
	PC_UART_IRQn,
	MCU_UART_IRQn,
	GPIO_SZ_HALL_A_IRQn,
	SZDJ_SUDU_Pin_IRQn,

} IRQn_Type;
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
static void interrupt_event_callback(const char* event_name, void* event_param, void* user_param)
{
	sim_interrupt_trigger((int)user_param);
}

void pc_uart_receive_callback()
{
	drv_com_on_receive_one_byte(kComPCUart, sim_rs232_get_char());
}
void mcu_uart_receive_callback()
{
	drv_com_on_receive_one_byte(kComMCUUart, sim_com_get_byte(SIM_COM1));
}
static void gpio_hall_a_change_callback() {
	HAL_GPIO_EXTI_Callback(SZ_HALL_A_Pin);
}
static void gpio_motor_optical_change_callback() {
	HAL_GPIO_EXTI_Callback(SZDJ_SUDU_Pin);
}
static void interrupt_init()
{
	//systick init
	sim_interrupt_bind(SysTick_IRQn, HAL_SYSTICK_Callback);
	sim_interrupt_set_systick_num(SysTick_IRQn, 1);
	sim_timer_bind("systick", interrupt_event_callback, (void*)SysTick_IRQn);
	sim_timer_set("systick", MSECS_PER_TICK, 1);

	//PC uart init
	sim_interrupt_bind(PC_UART_IRQn, pc_uart_receive_callback);
	sim_rs232_bind_on_recv(interrupt_event_callback, (void*)PC_UART_IRQn);

	//MCU uart init
	sim_com_create(SIM_COM1);
	sim_interrupt_bind(MCU_UART_IRQn, mcu_uart_receive_callback);
	sim_com_bind_on_recv(SIM_COM1, interrupt_event_callback, (void*)MCU_UART_IRQn);

	//motor timer init
	sim_interrupt_bind(GPIO_SZ_HALL_A_IRQn, gpio_hall_a_change_callback);
	sim_interrupt_bind(SZDJ_SUDU_Pin_IRQn, gpio_motor_optical_change_callback);
	sim_timer_bind("infusion_motor", interrupt_event_callback, (void*)GPIO_SZ_HALL_A_IRQn);
	sim_timer_bind("infusion_motor_optical", interrupt_event_callback, (void*)SZDJ_SUDU_Pin_IRQn);
}
static void device_init()
{
	sim_LCD_display_on("main");
}

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
	for (int i = 0; i < buff_size; i++) {
		if (obj->obj == &PC_UART) {
			sim_rs232_put_char(((char*)buff)[i]);
		}
		else if (obj->obj == &MCU_UART) {
			sim_com_put_byte(SIM_COM1, ((char*)buff)[i]);
		}
	}

	return true;
}
void sim_main(int param)
{

	interrupt_init();
	device_init();
	while (1)
	{
		main_run();
	}

}