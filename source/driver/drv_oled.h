/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-18 dczhang
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"


#define OLED_CS_PORT    OLED_SPI2_CS_GPIO_Port
#define OLED_CS_PIN     OLED_SPI2_CS_Pin
#define OLED_RES_PORT   OLED_RES_GPIO_Port
#define OLED_RES_PIN    OLED_RES_Pin
#define OLED_PWR_EN_PORT OLED_PWR_EN_GPIO_Port
#define OLED_PWR_EN_PIN  OLED_PWR_EN_Pin
#define OLED_DC_PORT     OLED_DC_GPIO_Port
#define OLED_DC_PIN      OLED_DC_Pin

#define COLUMN_MAX 0XFF
#define ROW_MAX     0X3F
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void drv_oled_pwr_en();

void drv_oled_pwr_dis();

void drv_oled_cs_en();

void drv_oled_cs_dis();

void drv_oled_rest_en();

void drv_oled_rest_dis();

void drv_oled_write_command_mode();

void drv_oled_write_data_mode();

void drv_oled_write_command(uint8_t command, uint8_t spi_mode);

void drv_oled_write_data(uint8_t data, uint8_t spi_mode);

// buff = 0x12 unlock; buff = 0x16 lock
void drv_oled_command_lock_mode(uint8_t buff);

//解锁命令
void drv_oled_conmmand_unlock();

//锁定之后只能接收解锁命令
void drv_oled_command_lock();

//buff = 0xae sleep on; buff = 0xaf sleep off
void drv_oled_sleep_mode(uint8_t buff);

void drv_oled_sleep_on();

void drv_oled_sleep_off();

//start = 0; end = 0xff
void drv_oled_set_column_start_end_addr(uint8_t start_addr, uint8_t end_addr);

void drv_oled_set_row_start_end_addr(uint8_t start_addr, uint8_t end_addr);

void drv_oled_set_Re_map_and_Dual_COM_line_mode(uint8_t re_map, uint8_t dual_com);

void drv_oled_set_display_start_line(uint8_t start_line);

void drv_oled_set_display_offset(uint8_t offset);

//function=0b, Select external VDD
//function=1b, Enable internal VDD regulator [reset]
void drv_oled_function_selection(uint8_t function);

void drv_oled_set_phase_length(uint8_t phase_length);

void drv_oled_set_front_clock_divider_or_oscillator_frequency(uint8_t divset);

void drv_oled_set_display_enhancement_A(uint8_t enhanc_1, uint8_t enhanc_2);

void drv_oled_set_gpio(uint8_t gpio);

void drv_oled_set_second_precharge_period(uint8_t period);

void drv_oled_select_default_linear_gray_scale_table();

void drv_oled_set_pre_charge_voltage(uint8_t vol);

void drv_oled_set_Vcomh(uint8_t vol_level);

void drv_oled_set_contrast_current(uint8_t cur);

void drv_oled_set_master_contrast_current_control(uint8_t master_cur);

void drv_oled_set_MUX_ratio(uint8_t ratio);

void drv_oled_set_display_enhancement_B(uint8_t enhanc_1, uint8_t enhanc_2);

/****************************************************
mode = A4h = Entire Display OFF, all pixels turns OFF in GS level 0
mode = A5h = Entire Display ON, all pixels turns ON in GS level 15
mode = A6h = Normal Display [reset]
mode = A7h = Inverse Display 
****************************************************/
void drv_oled_set_display_mode(uint8_t mode);


void drv_oled_init();
void drv_oled_deinit();

void drv_oled_clear_all_screen();

void drv_oled_display_all_screen();

void drv_oled_display_text_at_pos(int x, int y, uint8_t* str, int size);

//set start and end addr
void drv_oled_set_pos(int col, int row);

//enable write RAM 
void drv_oled_write_RAM_en();

//write the data
void drv_oled_write_byte(uint8_t data);
void drv_oled_write_bytes(uint8_t *data, int data_size);
void drv_oled_all_pixels_on();
void drv_oled_all_pixels_off();
void drv_oled_mcu_io_as_gpio(void);
void drv_oled_mcu_io_as_analog(void);


#ifdef __cplusplus
}
#endif


