/*
 * cmd_table.c
 *
 *     DEC_SPLT(ch, "명령", "설명", uint32_t변수)   10진수   "100"
 *     HEX_SPLT(ch, "명령", "설명", uint32_t변수)   16진수   "1A" / "0x1A"
 *     SDEC_SPLT(ch, "명령", "설명", int32_t변수)   부호 있음 "-5"
 *     STR_SPLT(ch, "명령", "설명", char*변수)      문자열
 *     ONOFF_SPLT(ch, "명령", bool변수)             "on"/"off"
 *     cmd_arg_rest()                               남은 줄 전체(공백 포함)
 *
 *   출력
 *     cmd_printf(ch, "fmt", ...)      한 줄 (줄바꿈 자동)
 *     cmd_printf_raw(ch, "fmt", ...)  줄바꿈 없음 (표/덤프 조립용)
 *
 *   구분선
 *     cmd_register("= 제목 ====", 0, 0);   핸들러가 0 이면 HELP 의 제목 줄
 */

#include "main.h"

#define TAG "CMDT"

/* ─────────────────────── SYSTEM ─────────────────────── */

static void cmd_ping(uint32_t ch)
{
	cmd_printf(ch, "PONG");
}

static void cmd_ver(uint32_t ch)
{
	cmd_printf(ch, "ver: %s", BOARD_NAME);
	cmd_printf(ch, "     built %s %s", __DATE__, __TIME__);
	cmd_printf(ch, "     port  %s", BSP_UART_PORT_NAME);
}

static void cmd_echo(uint32_t ch)
{
	const char *text = cmd_arg_rest();

	cmd_printf(ch, "echo: %s", (text != NULL) ? text : "");
}

static void cmd_status(uint32_t ch)
{
	const uint32_t uptime_ms = (uint32_t)(xTaskGetTickCount() * (TickType_t)portTICK_PERIOD_MS);

	cmd_printf(ch, "==== status");
	cmd_printf(ch, " uptime   : %lu.%03lu s", (unsigned long)(uptime_ms / 1000), (unsigned long)(uptime_ms % 1000));
	cmd_printf(ch, " tasks    : %lu", (unsigned long)uxTaskGetNumberOfTasks());
	cmd_printf(ch, " commands : %lu", (unsigned long)cmd_count());
	cmd_printf(ch, " uart drop: %lu byte", (unsigned long)bsp_uart_rx_dropped());
}

static void cmd_reset(uint32_t ch)
{
	cmd_printf(ch, "reset: system reset");
	delay_ms(10);
	NVIC_SystemReset();
}

/* ─────────────────────── BOARD ─────────────────────── */

/**
 * 보드 LED 제어.
 */
static void cmd_led(uint32_t ch)
{
	bool on;

	ONOFF_SPLT(ch, "led", on);

	if (on) {
		LED_On(LED0);
	} else {
		LED_Off(LED0);
	}

	cmd_printf(ch, "led: %s", on ? "on" : "off");
}

static void cmd_delay(uint32_t ch)
{
	uint32_t value_ms;

	DEC_SPLT(ch, "delay", "ms", value_ms);
	RETURN_IF(ch, (value_ms > 5000), "delay: too long (max 5000)");

	cmd_printf(ch, "delay: start - %lu ms", (unsigned long)value_ms);
	vTaskDelay(pdMS_TO_TICKS(value_ms));
	cmd_printf(ch, "delay: end");
}

static void cmd_mem(uint32_t ch)
{
	uint32_t start_addr;
	uint32_t length;
	uint32_t addr;

	HEX_SPLT(ch, "mem", "addr_hex", start_addr);
	DEC_SPLT(ch, "mem", "length", length);

	RETURN_IF(ch, ((start_addr % 4u) != 0u),
			"mem: 0x%08lX not 4-byte aligned", (unsigned long)start_addr);
	RETURN_IF(ch, (length > 256u), "mem: too long (max 256)");

	cmd_printf(ch, "---------------- OFFSET=0x%08lX -------------",
			(unsigned long)start_addr);
	cmd_printf(ch, "          00000000 00000004 00000008 0000000C");

	for (addr = start_addr; addr < (start_addr + length); addr += 4) {
		if ((addr % 16) == 0) {
			if (addr != start_addr) {
				cmd_printf(ch, "%s", "");
			}
			cmd_printf_raw(ch, "%08lX: ", (unsigned long)(addr & 0xFFFFFFF0));
		}

		cmd_printf_raw(ch, "%08lX ",
				(unsigned long)(*(const volatile uint32_t *)(uintptr_t)addr));
	}

	cmd_printf(ch, "%s", "");
	cmd_printf(ch, "mem: done");
}

/* ═══════════════════════════════════════════════════════════
 *  명령 등록 
 * ═══════════════════════════════════════════════════════════ */
void cmd_table_register_all(void)
{
	cmd_register("= SYSTEM =============", 0, 0);
	cmd_register("ping",   0,                    cmd_ping);
	cmd_register("ver",    0,                    cmd_ver);
	cmd_register("echo",   "(text)",             cmd_echo);
	cmd_register("status", 0,                    cmd_status);
	cmd_register("reset",  0,                    cmd_reset);

	cmd_register("= BOARD ==============", 0, 0);
	cmd_register("led",    "(on/off)",           cmd_led);
	cmd_register("delay",  "(ms)",               cmd_delay);
	cmd_register("mem",    "(addr_hex) (length)", cmd_mem);

	/* [TODO] 새 명령은 여기부터 추가하세요.
	 *
	 * cmd_register("= MY GROUP ===========", 0, 0);
	 * cmd_register("beep", "(freq) (dur_ms)", cmd_beep);
	 */

	LOG_I(TAG, "%lu command(s) registered", (unsigned long)cmd_count());
}
