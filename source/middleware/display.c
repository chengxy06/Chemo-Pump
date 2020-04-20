/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-04 xqzhao
* Initial revision.
*
************************************************/
#include "display.h"
#include "ui.h"
#include "ui_internal.h"
#include "event.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#endif
#include "ssz_common.h"
#include "drv_oled.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static uint8_t g_display_cache[64][128];//64 row,128*2 column
static Rect g_display_dirty_rect;
static bool g_display_is_dirty = false;
static bool g_display_is_opened = false;
/************************************************
* Function 
************************************************/

//flush the display cache to lcd
void display_flush(int x0, int y0, int x1, int y1)
{
	if (!g_display_is_opened) {
		display_open();
	}
    int x_index_start;
    int x_index_end;
	int x_pos;

	if (x0<0) {
		x0 = 0;
	}
	if (y0 < 0) {
		y0 = 0;
	}
	if (x1 > DISPLAY_WIDTH - 1) {
		x1 = DISPLAY_WIDTH - 1;
	}
	if (y1 > DISPLAY_HEIGHT- 1) {
		y1 = DISPLAY_HEIGHT - 1;
	}

	//start pos must at edge of 4
	x_pos = x0 / 4;
	x0 = x_pos*4;
	
    x_index_start = x0/2;
    x_index_end = x1/2;

	//must write 4 pixel as one unit, so need increase it if need
	if ((x_index_end-x_index_start)%2!=1) {
		x_index_end++;
	}
    for(int y=y0; y<=y1; y++)
    {
#ifdef SSZ_TARGET_MACHINE
        //DrvLcdSetPos(x, y);
		drv_oled_set_pos(x_pos, y);
		drv_oled_write_RAM_en();
		
		drv_oled_write_bytes(&g_display_cache[y][x_index_start], (x_index_end-x_index_start+1));
#else
        for(int x=x_index_start; x<=x_index_end; x++)
        {
			sim_LCD_set_pixel(NULL, x * 2, y, ui_dev_color_to_common_color((g_display_cache[y][x] & 0xF0)>>4) );
			sim_LCD_set_pixel(NULL, x * 2+1, y, ui_dev_color_to_common_color(g_display_cache[y][x] & 0x0F) );
        }
#endif
    }
    
}
//flush the dirty area to lcd
void display_flush_dirty()
{
    if(g_display_is_dirty)
    {
        g_display_is_dirty = false;
        display_flush(g_display_dirty_rect.x0, g_display_dirty_rect.y0, 
            g_display_dirty_rect.x1, g_display_dirty_rect.y1);
    }
}
//is exist dirty area
bool display_is_dirty()
{
    return g_display_is_dirty;
}
//set dirty area
void display_set_dirty(int x, int y, int width, int height)
{
	if (width<=0 || height<=0) {
		return;
	}
    if(!g_display_is_dirty)
    {
        g_display_is_dirty = true;
		ui_rect_init_by_size(g_display_dirty_rect, x, y, width, height);
		event_set(kEventDisplayDirty);
    }
    else
    {
		Rect tmp;
		ui_rect_init_by_size(tmp, x, y, width, height);
		ui_rect_merge(&g_display_dirty_rect, &tmp);
    }
}

//open display for show, when need show, it will auto open
void display_open() {
	if (!g_display_is_opened) {
		//drv_oled_mcu_io_as_gpio();		
		g_display_is_opened = true;
		drv_oled_pwr_en();
		drv_oled_init();		
		drv_oled_sleep_off();
	}
}

//close display for reduce power
void display_close() {
	if (g_display_is_opened) {
		g_display_is_opened = false;
		drv_oled_sleep_on();
		drv_oled_pwr_dis();
		//drv_oled_mcu_io_as_analog();		
	}
}

void display_data_standard(int x, int y, const uint8_t * pixels, int pixels_count) {
	int pixel_index;
	int pixel_x_index_at_cache;
	uint8_t one_pixel;
	for (int i = 0; i < pixels_count; i++) {
		pixel_index = i / 2;
		if (i % 2 == 0) {
			//get 4 high bit
			one_pixel = (pixels[pixel_index] & 0xF0) >> 4;
		}
		else {
			//get 4 low bit
			one_pixel = pixels[pixel_index] & 0x0F;
		}
		pixel_x_index_at_cache = (x + i) / 2;
		if ((x + i) % 2 == 0) {
			//clear 4 high bit as 0
			g_display_cache[y][pixel_x_index_at_cache] &= 0x0F;
			g_display_cache[y][pixel_x_index_at_cache] |= (one_pixel << 4);
		}
		else {
			//clear 4 low bit as 0
			g_display_cache[y][pixel_x_index_at_cache] &= 0xF0;
			g_display_cache[y][pixel_x_index_at_cache] |= (one_pixel);
		}
	}
}
int display_data_quick(int x, int y, const uint8_t * pixels, int pixels_count) {
	//can copy directly
	memcpy(&g_display_cache[y][x], pixels, pixels_count / 2);
	return pixels_count - pixels_count % 2;
}

void display_data_at(int x, int y, const uint8_t * pixels, int pixels_count)
{
	if (pixels_count == 1 || x % 2 == 1) {
		//can not copy, need set one by one
		display_data_standard(x, y, pixels, pixels_count);
	}else{
		//can copy directly
		memcpy(&g_display_cache[y][x/2], pixels, pixels_count / 2);
		if (pixels_count%2 !=0 ) {
			//last pixel, can not copy, need set one by one
			display_data_standard(x+ pixels_count-1, y, pixels+ pixels_count/2, 1);
		}
	}
	display_set_dirty(x, y, pixels_count, 1);
#ifdef TEST
	display_flush_dirty();
#endif

}

void display_init()
{
	event_set_handler(kEventDisplayDirty, display_flush_dirty);
}
