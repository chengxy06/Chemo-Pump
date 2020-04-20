#ifdef __cplusplus
extern "C" {
#endif
SUITE(ui) {
	SET_SETUP(test_ui_setup, NULL);
	SET_TEARDOWN(test_ui_teardown, NULL);
	RUN_TEST(ui_draw_mode);
	RUN_TEST(ui_set_draw_mode);
	RUN_TEST(ui_bk_color);
	RUN_TEST(ui_set_bk_color);
	RUN_TEST(ui_canvas_rect);
	RUN_TEST(ui_set_canvas_rect);
	RUN_TEST(ui_draw_context);
	RUN_TEST(ui_set_draw_context);
	RUN_TEST(ui_move_to);
	RUN_TEST(ui_pen_color);
	RUN_TEST(ui_set_pen_color);
	RUN_TEST(ui_fill_color);
	RUN_TEST(ui_set_fill_color);
	RUN_TEST(ui_pen_size);
	RUN_TEST(ui_set_pen_size);
	RUN_TEST(ui_draw_pixel_at);
	RUN_TEST(ui_draw_hline_at);
	RUN_TEST(ui_draw_vline_at);
	RUN_TEST(ui_draw_rect_at);
	RUN_TEST(ui_draw_rect_ex);
	RUN_TEST(ui_fill_rect_at);
	RUN_TEST(ui_fill_rect_ex);
	RUN_TEST(ui_clear);
	RUN_TEST(ui_draw_img_at);
	RUN_TEST(ui_font);
	RUN_TEST(ui_set_font);
	RUN_TEST(ui_text_color);
	RUN_TEST(ui_set_text_color);
	RUN_TEST(ui_text_mode);
	RUN_TEST(ui_set_text_mode);
	RUN_TEST(ui_disp_char_at);
	RUN_TEST(ui_disp_text_at);
	RUN_TEST(ui_disp_text_in_rect);
	RUN_TEST(ui_get_text_xize);
	RUN_TEST(ui_get_char_xize);
	RUN_TEST(ui_get_font_yize);
	RUN_TEST(ui_draw_to_dev);
}
#ifdef __cplusplus
}
#endif

