/*
 * os_task.c
 */

#include "main.h"

#define TAG "OS"

bool os_task_create(TaskFunction_t entry, const char *name,
		uint32_t stack_bytes, UBaseType_t priority, void *arg)
{
	const configSTACK_DEPTH_TYPE stack_words =
			(configSTACK_DEPTH_TYPE)(stack_bytes / sizeof(portSTACK_TYPE));

	if (xTaskCreate(entry, name, stack_words, arg, priority, NULL) != pdPASS) {
		LOG_E(TAG, "task '%s' create failed (%lu bytes stack)",
				name, (unsigned long)stack_bytes);
		return false;
	}

	return true;
}
