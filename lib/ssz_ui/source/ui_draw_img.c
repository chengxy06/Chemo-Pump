#include "ui.h"
#include "ui_internal.h"
#include "ssz_common.h"
/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

void ui_draw_img_at(int x, int y, const Image* img)
{
	M_ui_canvas_to_display_dev_pos(x, y);
	M_ui_return_if_rect_invalid(x, y, img->width, img->height);

	int column_step = (img->bits_per_pixel + 7) / 8;
	//draw each line
	for (int line = 0; line < img->height; ++line) {
		ui_start_line_cache(x, y+line, g_ui_draw_context->draw_mode);
		//draw one line
		for (int column_byte = 0; column_byte < img->bytes_per_line; column_byte += column_step) {
			if (img->bits_per_pixel == 1) {
				//1bit, only have two color
				uint8_t ch = img->data[line*img->bytes_per_line + column_byte];
				int column = column_byte * 8;
				//get and set each pixel
				for (int i = 0; i < 8 && column < img->width; i++, column++) {
					if (ch & 0x80) {
						ui_set_pixel_at_line_cache(x + column, UI_BLACK);
					}
					else
					{
						ui_set_pixel_at_line_cache(x + column, UI_WHITE);
					}
					ch <<= 1;
				}
			}
			else {
				ssz_assert_fail();
			}
		}
		ui_flush_line_cache();
	}
}