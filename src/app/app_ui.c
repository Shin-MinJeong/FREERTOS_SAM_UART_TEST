/*
 * app_ui.c
 *
 * LVGL 런타임 태스크
 *
 */

#include "main.h"

#define TAG "UI"

static lv_obj_t *s_console_screen;

static void on_console_send(const char *ascii_cmd)
{
	if (!app_comm_send(ascii_cmd)) {
		LOG_E(TAG, "send failed: %s", ascii_cmd);
	}
}

static void ui_task(void *params)
{
	UNUSED(params);

	lv_init();
	lv_port_disp_init();

	if (!lv_port_indev_init()) {
		LOG_E(TAG, "touch input unavailable");
	}

	s_console_screen = screen_console_create(on_console_send);
	lv_scr_load(s_console_screen);

	for (;;) {
		lv_timer_handler();
		vTaskDelay(pdMS_TO_TICKS(APP_LVGL_TICK_PERIOD_MS));
	}
}

bool app_ui_start(void)
{
	return os_task_create(ui_task, "Ui", APP_TASK_LVGL_STACK_BYTES,
			APP_TASK_LVGL_PRIORITY, NULL);
}
