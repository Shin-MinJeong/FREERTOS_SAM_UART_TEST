/*
 * bsp_uart.c
 */

#include "main.h"

#define TAG "UART"

#if (BSP_UART_RX_RING_SIZE & (BSP_UART_RX_RING_SIZE - 1)) != 0
#error "BSP_UART_RX_RING_SIZE must be a power of two"
#endif
#define RING_MASK (BSP_UART_RX_RING_SIZE - 1)

static volatile uint32_t s_head;
static volatile uint32_t s_tail;
static volatile uint32_t s_dropped;
static uint8_t           s_ring[BSP_UART_RX_RING_SIZE];

static char     s_line[BSP_UART_LINE_MAX];
static uint32_t s_line_len;
static bool     s_line_overflow;

void BSP_UART_HANDLER(void)
{
	const uint32_t status = usart_get_status(BSP_UART_PORT);

	if ((status & US_CSR_RXRDY) != 0u) {
		uint32_t received = 0;

		if (usart_read(BSP_UART_PORT, &received) == 0u) {
			const uint32_t next = (s_head + 1u) & RING_MASK;

			if (next == (s_tail & RING_MASK)) {
				s_dropped++;
			} else {
				s_ring[s_head & RING_MASK] = (uint8_t)received;
				s_head = next;
			}
		}
	}

	if ((status & (US_CSR_OVRE | US_CSR_FRAME | US_CSR_PARE)) != 0) {
		usart_reset_status(BSP_UART_PORT);
	}
}

void bsp_uart_init(void)
{
	const sam_usart_opt_t options = {
		.baudrate     = BSP_UART_BAUDRATE,
		.char_length   = BSP_UART_CHAR_LENGTH,
		.parity_type   = BSP_UART_PARITY,
		.stop_bits     = BSP_UART_STOP_BITS,
		.channel_mode  = US_MR_CHMODE_NORMAL,
		.irda_filter   = 0,
	};

	s_head          = 0;
	s_tail          = 0;
	s_dropped       = 0;
	s_line_len      = 0;
	s_line_overflow = false;

	ioport_set_pin_mode(BSP_UART_RX_PIN, BSP_UART_RX_MUX);
	ioport_disable_pin(BSP_UART_RX_PIN);
	ioport_set_pin_mode(BSP_UART_TX_PIN, BSP_UART_TX_MUX);
	ioport_disable_pin(BSP_UART_TX_PIN);

	pmc_enable_periph_clk(BSP_UART_ID);

	usart_init_rs232(BSP_UART_PORT, &options, sysclk_get_peripheral_hz());
	usart_enable_tx(BSP_UART_PORT);
	usart_enable_rx(BSP_UART_PORT);

	usart_enable_interrupt(BSP_UART_PORT, US_IER_RXRDY);
	NVIC_ClearPendingIRQ(BSP_UART_IRQn);
	NVIC_SetPriority(BSP_UART_IRQn, BSP_UART_IRQ_PRIORITY);
	NVIC_EnableIRQ(BSP_UART_IRQn);

	LOG_I(TAG, "%s ready @ %lu baud", BSP_UART_PORT_NAME,
			(unsigned long)BSP_UART_BAUDRATE);
}

static bool write_byte(uint8_t value)
{
	uint32_t spins = 0;

	while (usart_is_tx_ready(BSP_UART_PORT) == 0) {
		if (++spins >= BSP_UART_TX_SPIN_LIMIT) {
			LOG_E(TAG, "TX timeout");
			return false;
		}
	}

	return usart_write(BSP_UART_PORT, value) == 0;
}

bool bsp_uart_write(const void *data, uint32_t length)
{
	const uint8_t *cursor = (const uint8_t *)data;
	uint32_t i;

	for (i = 0; i < length; i++) {
		if (!write_byte(cursor[i])) {
			return false;
		}
	}

	return true;
}

bool bsp_uart_write_line(const char *text)
{
	const char *eol = BSP_UART_TX_EOL;

	if (!bsp_uart_write(text, (uint32_t)strlen(text))) {
		return false;
	}

	return bsp_uart_write(eol, (uint32_t)strlen(eol));
}

uint32_t bsp_uart_read(void *dst, uint32_t max_length)
{
	uint8_t *cursor = (uint8_t *)dst;
	uint32_t count = 0;

	while (count < max_length && (s_tail != s_head)) {
		cursor[count] = s_ring[s_tail & RING_MASK];
		s_tail = (s_tail + 1) & RING_MASK;
		count++;
	}

	return count;
}

bool bsp_uart_read_line(char *dst, uint32_t size)
{
	while (s_tail != s_head) {
		const char c = (char)s_ring[s_tail & RING_MASK];

		s_tail = (s_tail + 1) & RING_MASK;

		if (c == '\r' || c == BSP_UART_EOL) {
			const bool overflowed = s_line_overflow;
			const uint32_t length = s_line_len;

			s_line_len      = 0;
			s_line_overflow = false;

			if (overflowed) {
				LOG_W(TAG, "line too long, dropped");
				continue;
			}
			if (length == 0) {
				continue; 
			}

			s_line[length] = '\0';
			strncpy(dst, s_line, size - 1);
			dst[size - 1] = '\0';
			return true;
		}

		if (s_line_len < (BSP_UART_LINE_MAX - 1)) {
			s_line[s_line_len++] = c;
		} else {
			s_line_overflow = true;
		}
	}

	return false;
}

uint32_t bsp_uart_rx_dropped(void)
{
	return s_dropped;
}
