/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-18 dczhang
* Initial revision.
*
************************************************/
#include "drv_oled.h"
#include "ssz_spi.h"
#include "ssz_common.h"
#include "dev_def.h"
#include "ssz_gpio.h"
#ifdef SSZ_TARGET_SIMULATOR
#define __NOP()
#endif


/************************************************
* Declaration
************************************************/
static SszSPI g_oled_spi;
static bool g_oled_is_init = false;
static bool g_oled_is_first_init = true;


#define COMMAND_MODE  0x01
#define DATA_MODE     0x02

#define SPI_SIM_3_WIRE   0x0A
#define SPI_HARDWARE     0x0B

#define SIM_SPI_SCLK_SET ssz_gpio_quick_set(OLED_SPI2_SCLK_GPIO_Port, OLED_SPI2_SCLK_Pin)
#define SIM_SPI_SCLK_CLR ssz_gpio_quick_clear(OLED_SPI2_SCLK_GPIO_Port, OLED_SPI2_SCLK_Pin)

#define SIM_SPI_SDIN_SET ssz_gpio_quick_set(OLED_SPI2_SDIN_GPIO_Port, OLED_SPI2_SDIN_Pin)
#define SIM_SPI_SDIN_CLR ssz_gpio_quick_clear(OLED_SPI2_SDIN_GPIO_Port, OLED_SPI2_SDIN_Pin)

#define SIM_SPI_SCLK_OUT ssz_gpio_set_mode(OLED_SPI2_SCLK_GPIO_Port, OLED_SPI2_SCLK_Pin, kSszGpioOutPP_PU)
#define SIM_SPI_SDIN_OUT ssz_gpio_set_mode(OLED_SPI2_SDIN_GPIO_Port, OLED_SPI2_SDIN_Pin, kSszGpioOutPP_PU)

// set spi mode
#define SPI_MODE_SELECT SPI_HARDWARE //SPI_SIM_3_WIRE

bool drv_oled_spi_write(void* buff, int buff_size);
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
//3 wire spi mode write
void drv_oled_sim_spi_write(uint8_t *data, int data_size, uint8_t data_mode)
{
    int i;

    drv_oled_cs_en();
    //ssz_delay_us(1);
    //__NOP();


	uint8_t ch;
	for (int j = 0; j < data_size; j++) {
		ch = data[j];

		//send 1 bit control bit: D/C#
	    if(data_mode == COMMAND_MODE){
	        SIM_SPI_SCLK_CLR;
	        SIM_SPI_SDIN_CLR;
	        //ssz_delay_us(1);
	       // __NOP();
	        SIM_SPI_SCLK_SET;
	        //ssz_delay_us(1);
	        //__NOP();
	    }else if(data_mode == DATA_MODE){
	        SIM_SPI_SCLK_CLR;
	        SIM_SPI_SDIN_SET;
	        //ssz_delay_us(1);
	        //__NOP();
	        SIM_SPI_SCLK_SET;
	        //ssz_delay_us(1);
	        //__NOP();
	    }
	
		//send 1 byte data
		for (i = 0; i < 8; i++) {
			SIM_SPI_SCLK_CLR;
			if (ch & 0x80) {
				SIM_SPI_SDIN_SET;
			}
			else {
				SIM_SPI_SDIN_CLR;
			}
			//ssz_delay_us(1);
			//__NOP();
			SIM_SPI_SCLK_SET;
			//ssz_delay_us(1);
			//__NOP();
			ch <<= 1;
		}
	}

    drv_oled_cs_dis();
}


bool drv_oled_spi_write(void* buff, int buff_size)
{
    return ssz_spi_write(&g_oled_spi, buff, buff_size);
}

void drv_oled_pwr_en()
{
    ssz_gpio_set(OLED_PWR_EN_PORT,OLED_PWR_EN_PIN);
}

void drv_oled_pwr_dis()
{
    ssz_gpio_clear(OLED_PWR_EN_PORT,OLED_PWR_EN_PIN);
}

void drv_oled_cs_en()
{
    ssz_gpio_clear(OLED_CS_PORT,OLED_CS_PIN);
}

void drv_oled_cs_dis()
{
    ssz_gpio_set(OLED_CS_PORT,OLED_CS_PIN);
}

void drv_oled_rest_en()
{
    ssz_gpio_clear(OLED_RES_PORT,OLED_RES_PIN);
}

void drv_oled_rest_dis()
{
    ssz_gpio_set(OLED_RES_PORT,OLED_RES_PIN);
}

void drv_oled_write_command_mode()
{
    ssz_gpio_clear(OLED_DC_PORT,OLED_DC_PIN);
}

void drv_oled_write_data_mode()
{
    ssz_gpio_set(OLED_DC_PORT,OLED_DC_PIN);
}

void drv_oled_write_command(uint8_t command, uint8_t spi_mode)
{
    if(spi_mode == SPI_HARDWARE){
        drv_oled_cs_en();
        drv_oled_write_command_mode();
        __NOP();
        __NOP();
        drv_oled_spi_write(&command,1);
        __NOP();
        __NOP();
        drv_oled_cs_dis();
    }else if(spi_mode == SPI_SIM_3_WIRE){
        drv_oled_sim_spi_write(&command, 1, COMMAND_MODE);
    }
}

void drv_oled_write_data(uint8_t data, uint8_t spi_mode)
{
    if(spi_mode == SPI_HARDWARE){
        drv_oled_cs_en();
        drv_oled_write_data_mode();
        __NOP();
        __NOP();
        drv_oled_spi_write(&data,1);
        __NOP();
        __NOP();
        drv_oled_cs_dis();
    }else if(spi_mode == SPI_SIM_3_WIRE){
        drv_oled_sim_spi_write(&data, 1, DATA_MODE);
    }
}

void drv_oled_write_datas(uint8_t *data, int data_size, uint8_t spi_mode)
{
	if (spi_mode == SPI_HARDWARE) {
		drv_oled_cs_en();
		drv_oled_write_data_mode();
		drv_oled_spi_write(data,data_size);
		drv_oled_cs_dis();
	}
	else if (spi_mode == SPI_SIM_3_WIRE) {
		drv_oled_sim_spi_write(data,data_size, DATA_MODE);
	}
}

// buff = 0x12 unlock; buff = 0x16 lock
void drv_oled_command_lock_mode(uint8_t buff)
{
    drv_oled_write_command(0xfd, SPI_MODE_SELECT);
    drv_oled_write_data(buff, SPI_MODE_SELECT);
}

//解锁命令
void drv_oled_conmmand_unlock()
{
    drv_oled_command_lock_mode(0x12);
}

//锁定之后只能接收解锁命令
void drv_oled_command_lock()
{
    drv_oled_command_lock_mode(0x16);
}

//buff = 0xae sleep on; buff = 0xaf sleep off
void drv_oled_sleep_mode(uint8_t buff)
{
    drv_oled_write_command(buff, SPI_MODE_SELECT);
}

void drv_oled_sleep_on()
{
    drv_oled_sleep_mode(0xAE);
}

void drv_oled_sleep_off()
{
    drv_oled_sleep_mode(0xAF);
}

//start = 0; end = 0xff
void drv_oled_set_column_start_end_addr(uint8_t start_addr, uint8_t end_addr)
{
    drv_oled_write_command(0x15, SPI_MODE_SELECT);
    drv_oled_write_data(start_addr, SPI_MODE_SELECT);
    drv_oled_write_data(end_addr, SPI_MODE_SELECT);
}

void drv_oled_set_row_start_end_addr(uint8_t start_addr, uint8_t end_addr)
{
    drv_oled_write_command(0x75, SPI_MODE_SELECT);
    drv_oled_write_data(start_addr, SPI_MODE_SELECT);
    drv_oled_write_data(end_addr, SPI_MODE_SELECT);
}

/**************************************************************
    re_map = A ;dual_com = B

    A[0]=0b, Horizontal address increment [reset]
    A[0]=1b, Vertical address increment
    
    A[1]=0b, Disable Column Address Re-map [reset]
    A[1]=1b, Enable Column Address Re-map
    
    A[2]=0b, Disable Nibble Re-map [reset]
    A[2]=1b, Enable Nibble Re-map
    
    A[4]=0b, Scan from COM0 to COM[N –1] [reset]
    A[4]=1b, Scan from COM[N-1] to COM0, where N is the Multiplex ratio
    
    A[5]=0b, Disable COM Split Odd Even [reset]
    A[5]=1b, Enable COM Split Odd Even

    B[4], Enable / disable Dual COM Line mode
    0b, Disable Dual COM mode [reset]
    1b, Enable Dual COM mode (MUX < 63)
*****************************************************************/
void drv_oled_set_Re_map_and_Dual_COM_line_mode(uint8_t re_map, uint8_t dual_com)
{
    drv_oled_write_command(0xA0, SPI_MODE_SELECT);
    drv_oled_write_data(re_map, SPI_MODE_SELECT);
    drv_oled_write_data(dual_com, SPI_MODE_SELECT);
}

void drv_oled_set_display_start_line(uint8_t start_line)
{
    drv_oled_write_command(0xA1, SPI_MODE_SELECT);
    drv_oled_write_data(start_line, SPI_MODE_SELECT);
}

void drv_oled_set_display_offset(uint8_t offset)
{
    drv_oled_write_command(0xA2, SPI_MODE_SELECT);
    drv_oled_write_data(offset, SPI_MODE_SELECT);
}

//function=0b, Select external VDD
//function=1b, Enable internal VDD regulator [reset]
void drv_oled_function_selection(uint8_t function)
{
    drv_oled_write_command(0xAB, SPI_MODE_SELECT);
    drv_oled_write_data(function, SPI_MODE_SELECT);
}

void drv_oled_set_phase_length(uint8_t phase_length)
{
    drv_oled_write_command(0xB1, SPI_MODE_SELECT);
    drv_oled_write_data(phase_length, SPI_MODE_SELECT);
}

void drv_oled_set_front_clock_divider_or_oscillator_frequency(uint8_t divset)
{
    drv_oled_write_command(0xB3, SPI_MODE_SELECT);
    drv_oled_write_data(divset, SPI_MODE_SELECT);
}

void drv_oled_set_display_enhancement_A(uint8_t enhanc_1, uint8_t enhanc_2)
{
    drv_oled_write_command(0xB4, SPI_MODE_SELECT);
    drv_oled_write_data(enhanc_1, SPI_MODE_SELECT);
    drv_oled_write_data(enhanc_2, SPI_MODE_SELECT);
}

void drv_oled_set_gpio(uint8_t gpio)
{
    drv_oled_write_command(0xB5, SPI_MODE_SELECT);
    drv_oled_write_data(gpio, SPI_MODE_SELECT);
}

void drv_oled_set_second_precharge_period(uint8_t period)
{
    drv_oled_write_command(0xB6, SPI_MODE_SELECT);
    drv_oled_write_data(period, SPI_MODE_SELECT);
}

void drv_oled_select_default_linear_gray_scale_table()
{
    drv_oled_write_command(0xB9, SPI_MODE_SELECT);
}

void drv_oled_set_pre_charge_voltage(uint8_t vol)
{
    drv_oled_write_command(0xBB, SPI_MODE_SELECT);
    drv_oled_write_data(vol, SPI_MODE_SELECT);
}

void drv_oled_set_Vcomh(uint8_t vol_level)
{
    drv_oled_write_command(0xBE, SPI_MODE_SELECT);
    drv_oled_write_data(vol_level, SPI_MODE_SELECT);
}

void drv_oled_set_contrast_current(uint8_t cur)
{
    drv_oled_write_command(0xC1, SPI_MODE_SELECT);
    drv_oled_write_data(cur, SPI_MODE_SELECT);
}

void drv_oled_set_master_contrast_current_control(uint8_t master_cur)
{
    drv_oled_write_command(0xC7, SPI_MODE_SELECT);
    drv_oled_write_data(master_cur, SPI_MODE_SELECT);
}

void drv_oled_set_MUX_ratio(uint8_t ratio)
{
    drv_oled_write_command(0xCA, SPI_MODE_SELECT);
    drv_oled_write_data(ratio, SPI_MODE_SELECT);
}

void drv_oled_set_display_enhancement_B(uint8_t enhanc_1, uint8_t enhanc_2)
{
    drv_oled_write_command(0xD1, SPI_MODE_SELECT);
    drv_oled_write_data(enhanc_1, SPI_MODE_SELECT);
    drv_oled_write_data(enhanc_2, SPI_MODE_SELECT);
}

/****************************************************
mode = A4h = Entire Display OFF, all pixels turns OFF in GS level 0
mode = A5h = Entire Display ON, all pixels turns ON in GS level 15
mode = A6h = Normal Display [reset]
mode = A7h = Inverse Display 
****************************************************/
void drv_oled_set_display_mode(uint8_t mode)
{
    drv_oled_write_command(mode, SPI_MODE_SELECT);
}



void drv_oled_init()
{
	if(g_oled_is_init){
		return;
	}
	g_oled_is_init = true;
    if(SPI_MODE_SELECT == SPI_SIM_3_WIRE){
        SIM_SPI_SCLK_OUT;
        SIM_SPI_SDIN_OUT;
    }else{
	  
		#ifdef SSZ_TARGET_MACHINE
			MX_SPI1_Init();
		#endif			
        ssz_spi_init(&g_oled_spi, &OLED_SPI, NULL);
    }

    drv_oled_pwr_en();
	if (g_oled_is_first_init){
		g_oled_is_first_init = false;
		//need delay some ms to display right
		ssz_delay_ms(10);
	}else{
    	ssz_delay_ms(10);
	}
    drv_oled_rest_en();
    ssz_delay_us(200);
    drv_oled_rest_dis();
    ssz_delay_us(200);

    drv_oled_conmmand_unlock();
    //drv_oled_sleep_on();
    drv_oled_set_column_start_end_addr(0x1c, 0x5b);
    drv_oled_set_row_start_end_addr(0x00, 0x3f);
    drv_oled_set_Re_map_and_Dual_COM_line_mode(0x14, 0x11);
    drv_oled_set_display_start_line(0x00);
    drv_oled_set_display_offset(0x00);
    drv_oled_function_selection(0x01);//enable internal VDD 00:disable
    drv_oled_set_phase_length(0xE2);
    drv_oled_set_front_clock_divider_or_oscillator_frequency(0x91);
    drv_oled_set_display_enhancement_A(0xA0, 0xFD);
    //drv_oled_set_gpio(0x00);
    drv_oled_set_second_precharge_period(0x0f);
    drv_oled_select_default_linear_gray_scale_table();
    drv_oled_set_pre_charge_voltage(0x1F);
    drv_oled_set_Vcomh(0x07);
    drv_oled_set_contrast_current(0x20); //  0xFf
    drv_oled_set_master_contrast_current_control(0x0f);
    drv_oled_set_MUX_ratio(0x3f);
    drv_oled_set_display_enhancement_B(0x82, 0x20);
    drv_oled_set_display_mode(0xA6);
    drv_oled_clear_all_screen();
    drv_oled_sleep_off();
}
void drv_oled_deinit(){
	g_oled_is_init = false;
}

void drv_oled_clear_all_screen()
{
    int i,j;
    drv_oled_set_column_start_end_addr(0x1c, 0x5b);
    drv_oled_set_row_start_end_addr(0x00, 0x3f);
    drv_oled_write_command(0x5C, SPI_MODE_SELECT);

    for(i = 0; i < 64; i++){
        for(j = 0; j < 128; j++){
            drv_oled_write_data(0x00, SPI_MODE_SELECT);
        }
    }
}

void drv_oled_display_all_screen()
{
    int i,j;

    drv_oled_set_column_start_end_addr(0x1c, 0x5b);
    drv_oled_set_row_start_end_addr(0x00, 0x3f);
    drv_oled_write_command(0x5C, SPI_MODE_SELECT);

    for(i = 0; i < 64; i++){
        for(j = 0; j < 128; j++){
            drv_oled_write_data(0xff, SPI_MODE_SELECT);
        }
    }
}


//列地址增加的方式，每次点亮两个点，共128(256/2)个
//x代表行，y代表列
void drv_oled_display_text_at_pos(int col, int row, uint8_t* str, int size)
{
    ssz_assert(size <= (COLUMN_MAX - col + 1));
    
    int i;
    
    drv_oled_set_column_start_end_addr(col+0x1c, 0x5b);//COLUMN_MAX
    drv_oled_set_row_start_end_addr(row, ROW_MAX);
    drv_oled_write_command(0x5C, SPI_MODE_SELECT);

    for(i = 0; i < size; i++){
        drv_oled_write_data(str[i], SPI_MODE_SELECT);
    }
}

//set start and end addr
void drv_oled_set_pos(int col, int row)
{
	//set column start address
	drv_oled_write_command(0x15, SPI_MODE_SELECT);
	drv_oled_write_data(col+0x1C, SPI_MODE_SELECT);

	//set row start address
	drv_oled_write_command(0x75, SPI_MODE_SELECT);
	drv_oled_write_data(row, SPI_MODE_SELECT);

    //drv_oled_set_column_start_end_addr(col+0x1c, 0x5b);
    //drv_oled_set_row_start_end_addr(row, ROW_MAX);
}

//enable write RAM 
void drv_oled_write_RAM_en()
{
    drv_oled_write_command(0x5C, SPI_MODE_SELECT);
}

//write the data
void drv_oled_write_byte(uint8_t data)
{
    drv_oled_write_data(data, SPI_MODE_SELECT);
}

void drv_oled_write_bytes(uint8_t * data, int data_size)
{
	drv_oled_write_datas(data, data_size, SPI_MODE_SELECT);
}

void drv_oled_all_pixels_on()
{
    drv_oled_write_command(0xAF, SPI_MODE_SELECT); //display on
    drv_oled_write_command(0xA5, SPI_MODE_SELECT); //display all pixels
}

void drv_oled_all_pixels_off()
{
    drv_oled_write_command(0xA4, SPI_MODE_SELECT); //all pixels off
    //drv_oled_write_command(0xAE, SPI_MODE_SELECT); //display off
    drv_oled_set_display_mode(0xA6); //set to normal mode
}

void drv_oled_mcu_io_as_gpio(void){
#ifdef SSZ_TARGET_MACHINE
	GPIO_InitTypeDef GPIO_InitStruct;
	/*Configure GPIO pins : PBPin PBPin PBPin PBPin */
	GPIO_InitStruct.Pin = OLED_SPI2_CS_Pin|OLED_RES_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = OLED_DC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

#endif
}
void drv_oled_mcu_io_as_analog(void){
#ifdef SSZ_TARGET_MACHINE
 	GPIO_InitTypeDef GPIO_InitStruct;		
 	GPIO_InitStruct.Pin = OLED_CS_PIN|OLED_SPI2_SCLK_Pin|OLED_SPI2_SDIN_Pin|OLED_RES_PIN;
 	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
 	GPIO_InitStruct.Pull = GPIO_NOPULL;
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	

 	GPIO_InitStruct.Pin = OLED_DC_PIN;
 	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
 	GPIO_InitStruct.Pull = GPIO_NOPULL;
 	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	
#endif
}


