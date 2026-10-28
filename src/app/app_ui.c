/*
 * app_ui.c
 *
 * LVGL 런타임 태스크
 *
 */

#include "main.h"

#define TAG "UI"

static lv_obj_t *s_console_screen;

/**
 * 화면 버튼이 눌렸다. 터미널에 그 명령을 타이핑한 것과 똑같이 실행한다.
 *
 * 실제 실행은 Comm 태스크가 한다(app_comm_exec 참고). 여기서 파서를 직접
 * 부르면 UART 로 들어온 명령과 파서 상태가 엉킨다.
 */
static void on_console_send(const char *ascii_cmd)
{
	if (!app_comm_exec(ascii_cmd)) {
		LOG_E(TAG, "exec failed: %s", ascii_cmd);
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
