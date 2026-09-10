/*
 * bsp_board.c
 */

#include "main.h"

#define BSP_SHARED_RESET_PIN IOPORT_CREATE_PIN(PIOC, 13)

/** UART */
static void console_init(void)
{
	const usart_serial_options_t options = {
		.baudrate = CONF_UART_BAUDRATE,
#if (defined CONF_UART_CHAR_LENGTH)
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#if (defined CONF_UART_STOP_BITS)
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	stdio_serial_init(CONF_UART, &options);

#if defined(__GNUC__)
	setbuf(stdout, NULL);
#endif
}

void bsp_board_init(void)
{
	sysclk_init();
	board_init();
	console_init();

	pmc_enable_periph_clk(ID_PIOC);
}

void bsp_board_shared_reset(uint32_t assert_ms, uint32_t release_ms)
{
	ioport_set_pin_dir(BSP_SHARED_RESET_PIN, IOPORT_DIR_OUTPUT);

	ioport_set_pin_level(BSP_SHARED_RESET_PIN, IOPORT_PIN_LEVEL_LOW);
	delay_ms(assert_ms);
	ioport_set_pin_level(BSP_SHARED_RESET_PIN, IOPORT_PIN_LEVEL_HIGH);
	delay_ms(release_ms);
}
