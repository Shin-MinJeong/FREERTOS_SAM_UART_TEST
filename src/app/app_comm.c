/*
 * app_comm.c
 *
 * 전송 계층(bsp_uart)과 명령 엔진(cmd_parser)을 연결하는 유일한 지점.
 *
 * === 명령이 들어오는 길은 두 개, 실행하는 곳은 하나 ===
 *
 *   PC 터미널 입력 ─ bsp_uart ─┐
 *                              ├─> Comm 태스크 ─> cmd_parser ─> 핸들러
 *   화면 버튼 ─ app_comm_exec ─┘         (큐)
 *
 * 어느 쪽으로 들어와도 같은 핸들러가 돌고, 결과는 같은 터미널로 나간다.
 * 버튼 콜백이 파서를 직접 부르지 않고 큐를 거치는 이유는 두 가지다.
 *   1) 파서는 인자 커서와 출력 버퍼를 하나씩만 가진다. 두 태스크가 동시에
 *      해석하면 서로의 상태를 덮어쓴다. 실행을 Comm 태스크 한 곳으로 모은다.
 *   2) 파서는 문자열을 제자리에서 토큰화한다. 버튼 표의 문자열은 flash 에
 *      있는 상수라 그대로 넘길 수 없다. 큐가 복사본을 만들어 준다.
 *
 * 명령을 추가할 때 이 파일은 건드리지 않는다. cmd_table.c 만 고치면 된다.
 */

#include "main.h"

#define TAG "COMM"

/** 화면 버튼이 넣어 둔 명령을 Comm 태스크가 꺼내 가는 큐. */
static QueueHandle_t s_cmd_queue;

/**
 * 엔진이 만든 문자열을 실제로 내보낸다.
 *
 * 줄바꿈은 엔진이 이미 붙여서 넘긴다. 그래서 여기서는 write_line 이 아니라
 * write 를 쓴다(hex 덤프처럼 줄 중간에 끊어 찍는 출력이 있기 때문).
 */
static void cmd_output(uint32_t ch, const char *text)
{
	switch (ch) {
	case CMD_CH_UART:
		bsp_uart_write(text, (uint32_t)strlen(text));
		break;

	case CMD_CH_SCREEN:
		/* [TODO] 화면으로도 답신을 보내려면 여기를 채운다.
		 * 단, screen_console_log() 는 LVGL 태스크에서만 호출해야 하므로
		 * 큐를 하나 더 두고 app_ui 쪽에서 꺼내 찍는 형태가 되어야 한다. */
		break;

	default:
		break;
	}
}

/**
 * 수신 태스크.
 *
 * UART 로 들어온 줄과 버튼이 넣은 명령을 모두 여기서 실행한다.
 */
static void comm_rx_task(void *params)
{
	static char line[BSP_UART_LINE_MAX];
	uint32_t last_dropped = 0;

	UNUSED(params);

	for (;;) {
		const uint32_t dropped = bsp_uart_rx_dropped();

		/* PC 터미널에서 온 줄 */
		while (bsp_uart_read_line(line, sizeof(line))) {
			LOG_I(TAG, "RX> %s", line);

			cmd_parser_handle_line(CMD_CH_UART, line);
		}

		/* 화면 버튼이 넣은 명령 */
		while (xQueueReceive(s_cmd_queue, line, 0) == pdTRUE) {
			LOG_I(TAG, "UI> %s", line);

			/* 버튼을 눌렀다는 사실도 터미널에 남긴다. 결과 줄만 툭
			 * 튀어나오면 무슨 명령이 돈 것인지 알 수 없다. */
			cmd_printf(CMD_CH_UART, "> %s", line);
			cmd_parser_handle_line(CMD_CH_UART, line);
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
	cmd_parser_init(cmd_output);
	cmd_table_register_all();

	s_cmd_queue = xQueueCreate(APP_COMM_CMD_QUEUE_DEPTH, BSP_UART_LINE_MAX);
	if (s_cmd_queue == NULL) {
		LOG_E(TAG, "command queue alloc failed");
		return false;
	}

	return os_task_create(comm_rx_task, "Comm", APP_TASK_COMM_STACK_BYTES,
			APP_TASK_COMM_PRIORITY, NULL);
}

bool app_comm_exec(const char *ascii_cmd)
{
	char item[BSP_UART_LINE_MAX];

	if ((ascii_cmd == NULL) || (s_cmd_queue == NULL)) {
		return false;
	}

	strncpy(item, ascii_cmd, sizeof(item) - 1u);
	item[sizeof(item) - 1u] = '\0';

	if (xQueueSend(s_cmd_queue, item, 0) != pdTRUE) {
		LOG_W(TAG, "command queue full, dropped: %s", ascii_cmd);
		return false;
	}

	return true;
}

bool app_comm_send(const char *text)
{
	LOG_I(TAG, "TX< %s", text);

	return bsp_uart_write_line(text);
}
