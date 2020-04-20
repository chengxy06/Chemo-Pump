#include "ui.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#endif
#include "ssz_common.h"
#include "display.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
void ui_config()
{
	ui_set_default_draw_context();
}

//convert color to dev color index
int ui_common_color_to_dev_color(color_t color)
{
#if DISPLAY_BIT_PER_PIXEL==4
	if (color == UI_WHITE) {
		return 0x0F;
	}
	else if (color == UI_BLACK) {
		return 0;
	}
	else {
		//return color * 0x0F / 0x00FFFFFF;
		return 0x08;
	}
#elif DISPLAY_BIT_PER_PIXEL==24
	return color;
#else
#error "not support current bit width"
#endif
}
//convert dev color index to color
color_t ui_dev_color_to_common_color(int dev_color_index) {
#if DISPLAY_BIT_PER_PIXEL==4
	if (dev_color_index == 0x0F) {
		return UI_WHITE;
	}
	else if (dev_color_index == 0) {
		return UI_BLACK;
	}
	else {
		return dev_color_index * 0x00FFFFFF / 0x0F;
	}
#elif DISPLAY_BIT_PER_PIXEL==24
	return dev_color_index;
#else
#error "not support current bit width"
#endif
}
void ui_draw_to_dev(int x, int y, uint8_t *dev_pixels, int pixels_count)
 {
#if USE_SIMULATOR_DEPTH
	 int color;
	 for (int i = 0; i < pixels_count; i++) {
		 memcpy(&color, dev_pixels + i * 3, 3);
		 sim_LCD_set_pixel(NULL, x+i, y, color);
	 }
#else
	 display_data_at(x, y, dev_pixels, pixels_count);
#endif
 }

 int ui_read_pixel_from_dev(int x, int y)
 {
#ifdef SSZ_TARGET_SIMULATOR
	return ui_common_color_to_dev_color(sim_LCD_get_pixel(NULL, x, y));
#else
	 ssz_assert_fail();
	 return 0;
#endif
 }