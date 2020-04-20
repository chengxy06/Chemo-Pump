#ifdef __cplusplus
extern "C" {
#endif
SUITE(ui_view) {
	SET_SETUP(test_ui_view_setup, NULL);
	SET_TEARDOWN(test_ui_view_teardown, NULL);
	RUN_TEST(ui_view_window);
	RUN_TEST(ui_view_lable);
	RUN_TEST(ui_view_image);
	RUN_TEST(ui_view_text_list);
	RUN_TEST(ui_view_horizontal_text_list);
	RUN_TEST(ui_view_image_list);
}
#ifdef __cplusplus
}
#endif

