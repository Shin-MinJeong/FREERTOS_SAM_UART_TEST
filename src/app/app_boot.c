/*
 * app_boot.c
 */

#include "main.h"

#define TAG "BOOT"

static void print_banner(void)
{
	LOG_I(TAG, "FreeRTOS + LVGL + UART command test");
	LOG_I(TAG, "board: %s", BOARD_NAME);
	LOG_I(TAG, "built: %s %s", __DATE__, __TIME__);
	LOG_I(TAG, "console=USART1(EDBG)  protocol=%s", BSP_UART_PORT_NAME);
}

/**
 * 하드웨어 초기화
 *
 */
static void init_hardware(void)
{
	bsp_board_init();
	print_banner();

	if (!bsp_touch_init()) {
		LOG_W(TAG, "continuing without touch");
	}

	bsp_display_init();

	/* PC 통신 UART. 공유 리셋 핀과 무관하므로 순서 제약이 없다. */
	bsp_uart_init();
}

static void start_tasks(void)
{
	app_comm_start();
	app_diag_start();
	app_ui_start();
}

void app_boot_run(void)
{
	init_hardware();
	start_tasks();

	vTaskStartScheduler();

	LOG_E(TAG, "scheduler did not start: out of heap");
	for (;;) {
	}
}
