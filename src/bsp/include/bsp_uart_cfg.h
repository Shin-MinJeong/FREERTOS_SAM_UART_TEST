/*
 * bsp_uart_cfg.h
 *
 * PC 통신 UART 파라미터
 *
 * 통신 조건을 바꿀 때 건드리는 값은 해당 파일에서 관리
 *
 * === 보드에 UART 2개 ===
 *   USART1 (PA21/PB4) : 콘솔. printf / LOG_* 가 나가는 곳 (EDBG 가상 COM 포트로 연결)
 *                       board_init() 과 bsp_board.c 가 담당.
 
 *   USART0 (PB0/PB1)  : 확장 헤더. USB-Serial 어댑터를 따로 연결해야 쓸 수 있다.
 *
 * 프로토콜을 둘 중 어디에 얹을지는 아래 BSP_UART_USE_CONSOLE_PORT 로 고름
 */

#ifndef BSP_UART_CFG_H_
#define BSP_UART_CFG_H_

/**
 * 프로토콜 UART 를 콘솔(EDBG USB) 포트에 얹을지 고르는 스위치.
 *
 *   1 = USART1 (PA21/PB4, EDBG 가상 COM)   
 *       보드 USB 케이블 하나로 프로토콜을 주고받는다. USB-Serial 어댑터가 없을 때 쓰는 설정이다.
 *
 *   0 = USART0 (PB0/PB1, 확장 헤더)        
 *       USB-Serial 어댑터를 따로 연결해야 한다. 로그와 프로토콜이 완전히 분리
 *       어댑터가 생기면 이 값만 0 으로 되돌리면 됨
 *
 */
#ifndef BSP_UART_USE_CONSOLE_PORT
#define BSP_UART_USE_CONSOLE_PORT 1
#endif

#if BSP_UART_USE_CONSOLE_PORT

#define BSP_UART_PORT    USART1
#define BSP_UART_ID      ID_USART1
#define BSP_UART_IRQn    USART1_IRQn
#define BSP_UART_HANDLER USART1_Handler

#define BSP_UART_RX_PIN   USART1_RXD_GPIO
#define BSP_UART_RX_MUX   USART1_RXD_FLAGS
#define BSP_UART_TX_PIN   USART1_TXD_GPIO
#define BSP_UART_TX_MUX   USART1_TXD_FLAGS

#define BSP_UART_PORT_NAME "USART1 (EDBG USB, PA21/PB4, shared with log)"

#else /* 원래 설계: 전용 프로토콜 포트 */

#define BSP_UART_PORT    USART0
#define BSP_UART_ID      ID_USART0
#define BSP_UART_IRQn    USART0_IRQn
#define BSP_UART_HANDLER USART0_Handler

#define BSP_UART_RX_PIN   USART0_RXD_GPIO
#define BSP_UART_RX_MUX   USART0_RXD_FLAGS
#define BSP_UART_TX_PIN   USART0_TXD_GPIO
#define BSP_UART_TX_MUX   USART0_TXD_FLAGS

#define BSP_UART_PORT_NAME "USART0 (PB0/PB1, needs USB-Serial adapter)"

#endif
/** @} */

/** \name 통신 조건 (PC 터미널 설정과 일치시킬 것) */
/** @{ */
#define BSP_UART_BAUDRATE    115200
#define BSP_UART_CHAR_LENGTH US_MR_CHRL_8_BIT
#define BSP_UART_PARITY      US_MR_PAR_NO
#define BSP_UART_STOP_BITS   US_MR_NBSTOP_1_BIT
/** @} */

#define BSP_UART_RX_RING_SIZE 256
#define BSP_UART_LINE_MAX 96
#define BSP_UART_EOL '\n'
#define BSP_UART_TX_EOL "\r\n"
#define BSP_UART_IRQ_PRIORITY 5

#define BSP_UART_TX_SPIN_LIMIT 100000

#endif /* BSP_UART_CFG_H_ */
