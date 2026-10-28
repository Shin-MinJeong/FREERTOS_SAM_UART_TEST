/*
 * app_config.h
 *
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "FreeRTOS.h"
#include "task.h"


#define APP_TASK_MONITOR_STACK_BYTES 2048
#define APP_TASK_MONITOR_PRIORITY    (tskIDLE_PRIORITY)

#define APP_TASK_LED_STACK_BYTES     1024
#define APP_TASK_LED_PRIORITY        (tskIDLE_PRIORITY)

#define APP_TASK_LVGL_STACK_BYTES    8192
#define APP_TASK_LVGL_PRIORITY       (tskIDLE_PRIORITY + 1)

#define APP_TASK_TOUCH_STACK_BYTES   2048
#define APP_TASK_TOUCH_PRIORITY      (tskIDLE_PRIORITY + 2)

#define APP_TASK_COMM_STACK_BYTES    2048
#define APP_TASK_COMM_PRIORITY       (tskIDLE_PRIORITY + 2)
/** @} */

/** \name 태스크 주기 (ms) */
/** @{ */
#define APP_LVGL_TICK_PERIOD_MS  5
#define APP_TOUCH_POLL_PERIOD_MS 10
#define APP_LED_BLINK_PERIOD_MS  1000
#define APP_MONITOR_PERIOD_MS    1000


#define APP_COMM_POLL_PERIOD_MS 5

/**
 * 화면 버튼이 넣은 명령을 담아 두는 큐의 깊이 (줄 수).
 */
#define APP_COMM_CMD_QUEUE_DEPTH 4
/** @} */

#ifndef APP_ENABLE_DIAG_TASKS
#define APP_ENABLE_DIAG_TASKS 0
#endif

#endif /* APP_CONFIG_H_ */
