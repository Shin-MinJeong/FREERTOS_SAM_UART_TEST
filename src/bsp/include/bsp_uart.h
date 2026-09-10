/*
 * bsp_uart.h
 *
 * PC 통신용 UART 전송 계층.
 *
 * "바이트와 줄을 주고받는 일"만 담당
 * 명령어 해석은 app 계층의 cmd_parser 가 담당
 *
 * 설정값은 bsp_uart_cfg.h 참조
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include <stdbool.h>
#include <stdint.h>

void bsp_uart_init(void);

bool bsp_uart_write(const void *data, uint32_t length);
bool bsp_uart_write_line(const char *text);
bool bsp_uart_read_line(char *dst, uint32_t size);
uint32_t bsp_uart_read(void *dst, uint32_t max_length);
uint32_t bsp_uart_rx_dropped(void);

#endif /* BSP_UART_H_ */
