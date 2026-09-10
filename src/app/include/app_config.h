/*
 * app_config.h
 *
 * 애플리케이션 튜닝 파라미터 단일 정의소.
 *
 * === 스택 크기 단위 ===
 * 여기 값은 모두 "바이트"다. os_task_create() 가 워드로 환산한다.
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "FreeRTOS.h"
#include "task.h"

/**
 * \name 태스크 스택 (바이트) 과 우선순위
 *
 * 우선순위 설계:
 *   Touch(+2)  터치 샘플링이 밀리면 입력이 씹힌다.
 *   Comm(+2)   UART 수신. 링버퍼 넘침을 막으려면 밀리지 않아야 한다.
 *   Ui(+1)     LVGL 렌더링. 조금 밀려도 화면만 느려진다.
 *   Diag(+0)   진단. 남는 시간에만 돌면 된다.
 */
/** @{ */
#define APP_TASK_MONITOR_STACK_BYTES 2048u
#define APP_TASK_MONITOR_PRIORITY    (tskIDLE_PRIORITY)

#define APP_TASK_LED_STACK_BYTES     1024u
#define APP_TASK_LED_PRIORITY        (tskIDLE_PRIORITY)

#define APP_TASK_LVGL_STACK_BYTES    8192u
#define APP_TASK_LVGL_PRIORITY       (tskIDLE_PRIORITY + 1)

#define APP_TASK_TOUCH_STACK_BYTES   2048u
#define APP_TASK_TOUCH_PRIORITY      (tskIDLE_PRIORITY + 2)

#define APP_TASK_COMM_STACK_BYTES    2048u
#define APP_TASK_COMM_PRIORITY       (tskIDLE_PRIORITY + 2)
/** @} */

/** \name 태스크 주기 (ms) */
/** @{ */
#define APP_LVGL_TICK_PERIOD_MS  5u
#define APP_TOUCH_POLL_PERIOD_MS 10u
#define APP_LED_BLINK_PERIOD_MS  1000u
#define APP_MONITOR_PERIOD_MS    1000u

/**
 * UART 수신 링버퍼를 비우는 주기 (ms).
 *
 * 115200 baud = 약 11.5 byte/ms 이므로 이 주기 동안 최대 약 58바이트가
 * 쌓인다. BSP_UART_RX_RING_SIZE(256) 대비 4배 이상 여유가 있다.
 * 주기를 늘리려면 링버퍼도 함께 키울 것.
 */
#define APP_COMM_POLL_PERIOD_MS 5u
/** @} */

/**
 * 0 으로 바꾸면 진단용 Monitor/Led 태스크를 만들지 않는다.
 * 기능에는 영향이 없다.
 */
#ifndef APP_ENABLE_DIAG_TASKS
#define APP_ENABLE_DIAG_TASKS 1
#endif

#endif /* APP_CONFIG_H_ */
