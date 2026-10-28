/*
 * app_comm.h
 *
 * 통신 유즈케이스: UART 전송 계층과 명령 엔진을 연결한다.
 *
 * 명령이 들어오는 길은 둘(PC 터미널 / 화면 버튼)이지만 실행하는 곳은
 * Comm 태스크 하나다. 그래서 어느 쪽으로 들어와도 cmd_table.c 의 같은
 * 핸들러가 돌고 결과도 같은 터미널로 나간다.
 *
 * bsp_uart 도 cmd_parser 도 서로를 모르며, 이 모듈만 둘을 안다.
 */

#ifndef APP_COMM_H_
#define APP_COMM_H_

#include <stdbool.h>

/**
 * \brief 명령 엔진을 준비하고 수신 태스크를 만든다.
 *
 * bsp_uart_init() 이후, 스케줄러 시작 전에 호출한다.
 *
 * \return 준비 성공 여부
 */
bool app_comm_start(void);

/**
 * \brief ASCII 명령 한 줄을 이 보드에서 실행한다.
 *
 * 화면 버튼 콜백이 쓰는 함수다. 터미널에 그 명령을 직접 타이핑한 것과
 * 똑같이 동작하고, 결과도 터미널로 나간다.
 *
 * 실행은 즉시 일어나지 않는다. 명령을 큐에 넣고 바로 돌아오며 Comm
 * 태스크가 꺼내 실행한다(파서를 한 태스크에서만 돌리기 위해서다).
 * 그래서 LVGL 태스크에서 불러도 화면이 멈추지 않는다.
 *
 * \param ascii_cmd 널 종단 문자열 (줄바꿈 없이). 상수 문자열도 된다.
 * \return 큐에 넣는 데 성공했는지 여부 (명령의 실행 결과가 아니다)
 */
bool app_comm_exec(const char *ascii_cmd);

/**
 * \brief 문자열 한 줄을 PC 로 내보내기만 한다 (해석하지 않는다).
 *
 * 이 보드가 PC 쪽 프로그램에 무언가를 알릴 때 쓴다.
 *
 * \param text 널 종단 문자열 (줄바꿈 없이)
 * \return 전송 성공 여부
 */
bool app_comm_send(const char *text);

#endif /* APP_COMM_H_ */
