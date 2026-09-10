/*
 * app_comm.h
 *
 * 통신 유즈케이스: UART 전송 계층과 명령 해석기를 연결한다.
 *
 * 수신 태스크가 bsp_uart 에서 한 줄씩 꺼내 cmd_parser 로 넘기고,
 * 해석기의 답신은 다시 bsp_uart 로 내보낸다.
 * bsp_uart 도 cmd_parser 도 서로를 모르며, 이 모듈만 둘을 안다.
 */

#ifndef APP_COMM_H_
#define APP_COMM_H_

#include <stdbool.h>

/**
 * \brief 명령 해석기를 준비하고 수신 태스크를 만든다.
 *
 * bsp_uart_init() 이후, 스케줄러 시작 전에 호출한다.
 *
 * \return 태스크 생성 성공 여부
 */
bool app_comm_start(void);

/**
 * \brief ASCII 명령 한 줄을 PC 로 보낸다.
 *
 * 화면 버튼 콜백에서 쓰는 함수다. 줄바꿈은 전송 계층이 붙인다.
 *
 * \param text 널 종단 문자열 (줄바꿈 없이)
 * \return 전송 성공 여부
 */
bool app_comm_send(const char *text);

#endif /* APP_COMM_H_ */
