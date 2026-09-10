/*
 * app_comm.c
 *
 * ===========================================================
 *  [TODO] 는 화면에 수신 내용을 표시하고 싶을 때만 채우면 됩니다.
 * ===========================================================
 */

#include "main.h"

#define TAG "COMM"

static void send_reply(const char *text)
{
	LOG_I(TAG, "TX< %s", text);
	bsp_uart_write_line(text);
}

/**
 * 수신 태스크.
 *
 */
static void comm_rx_task(void *params)
{
	static char line[BSP_UART_LINE_MAX];
	uint32_t last_dropped = 0;

	UNUSED(params);

	for (;;) {
		const uint32_t dropped = bsp_uart_rx_dropped();

		while (bsp_uart_read_line(line, sizeof(line))) {
			LOG_I(TAG, "RX> %s", line);

			/* [TODO] 화면에도 표시하려면 여기서 호출하세요.
			 *   screen_console_log(line); */

			cmd_parser_handle_line(line);
		}

		if (dropped != last_dropped) {
			LOG_W(TAG, "rx ring overflow: %lu byte(s) dropped",
					(unsigned long)dropped);
			last_dropped = dropped;
		}

		vTaskDelay(pdMS_TO_TICKS(APP_COMM_POLL_PERIOD_MS));
	}
}

bool app_comm_start(void)
{
	cmd_parser_init(send_reply);

	return os_task_create(comm_rx_task, "Comm", APP_TASK_COMM_STACK_BYTES,
			APP_TASK_COMM_PRIORITY, NULL);
}

bool app_comm_send(const char *text)
{
	LOG_I(TAG, "TX< %s", text);

	return bsp_uart_write_line(text);
}
