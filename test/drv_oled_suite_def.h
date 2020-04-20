#ifdef __cplusplus
extern "C" {
#endif
SUITE(drv_oled) {
	SET_SETUP(test_drv_oled_setup, NULL);
	SET_TEARDOWN(test_drv_oled_teardown, NULL);
	RUN_TEST(drv_oled_spi_write);
	RUN_TEST(drv_oled_pwr_en);
	RUN_TEST(drv_oled_pwr_dis);
	RUN_TEST(drv_oled_cs_en);
	RUN_TEST(drv_oled_cs_dis);
	RUN_TEST(drv_oled_rest_en);
	RUN_TEST(drv_oled_rest_dis);
	RUN_TEST(drv_oled_write_command_mode);
	RUN_TEST(drv_oled_write_data_mode);
	RUN_TEST(drv_oled_write_command);
	RUN_TEST(drv_oled_write_data);
	RUN_TEST(drv_oled_command_lock_mode);
	RUN_TEST(drv_oled_conmmand_unlock);
	RUN_TEST(drv_oled_command_lock);
	RUN_TEST(drv_oled_sleep_mode);
	RUN_TEST(drv_oled_sleep_on);
	RUN_TEST(drv_oled_sleep_off);
	RUN_TEST(drv_oled_set_column_start_end_addr);
	RUN_TEST(drv_oled_set_row_start_end_addr);
	RUN_TEST(drv_oled_set_Re_map_and_Dual_COM_line_mode);
	RUN_TEST(drv_oled_set_display_start_line);
	RUN_TEST(drv_oled_set_display_offset);
	RUN_TEST(drv_oled_function_selection);
	RUN_TEST(drv_oled_set_phase_length);
	RUN_TEST(drv_oled_set_front_clock_divider_or_oscillator_frequency);
	RUN_TEST(drv_oled_set_display_enhancement_A);
	RUN_TEST(drv_oled_set_gpio);
	RUN_TEST(drv_oled_set_second_precharge_period);
	RUN_TEST(drv_oled_select_default_linear_gray_scale_table);
	RUN_TEST(drv_oled_set_pre_charge_voltage);
	RUN_TEST(drv_oled_set_Vcomh);
	RUN_TEST(drv_oled_set_contrast_current);
	RUN_TEST(drv_oled_set_master_contrast_current_control);
	RUN_TEST(drv_oled_set_MUX_ratio);
	RUN_TEST(drv_oled_set_display_enhancement_B);
	RUN_TEST(drv_oled_set_display_mode);
	RUN_TEST(drv_oled_init);
	RUN_TEST(drv_oled_clear_all_screen);
	RUN_TEST(drv_oled_display_all_screen);
	RUN_TEST(drv_oled_display_text_at_pos);
	RUN_TEST(drv_oled_set_pos);
	RUN_TEST(drv_oled_write_RAM_en);
	RUN_TEST(drv_oled_write_byte);
}
#ifdef __cplusplus
}
#endif

