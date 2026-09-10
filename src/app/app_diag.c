/*
 * app_diag.c
 */

#include "main.h"

#if APP_ENABLE_DIAG_TASKS

#define TAG "DIAG"

/**
 * DIAG_PRINT_TASK_LIST: 1 >> 태스크 상태 리포트를 콘솔에 출력
 */
#define DIAG_PRINT_TASK_LIST 0

#if DIAG_PRINT_TASK_LIST
static portCHAR s_task_list[256];
#endif

static void led_blink_task(void *params)
{
	UNUSED(params);

	for (;;) {
#if SAM4CM
		LED_Toggle(LED4);
#else
		LED_Toggle(LED0);
#endif
		vTaskDelay(pdMS_TO_TICKS(APP_LED_BLINK_PERIOD_MS));
	}
}

static void monitor_task(void *params)
{
	UNUSED(params);

	for (;;) {
#if DIAG_PRINT_TASK_LIST
		vTaskList((signed portCHAR *)s_task_list);
		LOG_I(TAG, "tasks (%u)\r\n%s",
				(unsigned)uxTaskGetNumberOfTasks(), s_task_list);
#endif
		vTaskDelay(pdMS_TO_TICKS(APP_MONITOR_PERIOD_MS));
	}
}

bool app_diag_start(void)
{
	bool ok = true;

	ok &= os_task_create(monitor_task, "Monitor",
			APP_TASK_MONITOR_STACK_BYTES, APP_TASK_MONITOR_PRIORITY, NULL);
	ok &= os_task_create(led_blink_task, "Led",
			APP_TASK_LED_STACK_BYTES, APP_TASK_LED_PRIORITY, NULL);

	return ok;
}

#else /* !APP_ENABLE_DIAG_TASKS */

bool app_diag_start(void)
{
	return true;
}

#endif /* APP_ENABLE_DIAG_TASKS */
