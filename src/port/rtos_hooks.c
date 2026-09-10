/*
 * rtos_hooks.c
 *
 * FreeRTOS 가 요구하는 콜백 훅 모음.
 *
 * 리팩토링 이전에는 이 훅들이 bsp_sys.c 에 있었고, 그 때문에 BSP
 * 계층이 lv_tick_inc() 를 호출하며 GUI 라이브러리에 의존했다.
 * 훅은 하드웨어가 아니라 RTOS 어댑터의 관심사이므로 port 로 옮긴다.
 */

#include "main.h"

#define TAG "RTOS"

/** 태스크가 스택을 넘겼을 때. 복구가 불가능하므로 정지시킨다. */
void vApplicationStackOverflowHook(xTaskHandle *task, signed char *name)
{
	LOG_E(TAG, "stack overflow in '%s' (handle %p)", (const char *)name,
			(void *)task);

	/* 어느 태스크가 넘겼는지는 pxCurrentTCB 를 확인한다. */
	for (;;) {
	}
}

/*
 * vApplicationIdleHook 은 두지 않는다.
 * FreeRTOSConfig.h 의 configUSE_IDLE_HOOK 이 0 이라 커널이 호출하지
 * 않기 때문이다(기존 코드에는 빈 함수가 남아 있었다).
 * 유휴 훅이 필요해지면 그 설정을 1 로 바꾸고 여기에 정의를 추가한다.
 */

/** 매 틱마다 호출. LVGL 의 시간 기준을 여기서 공급한다. */
void vApplicationTickHook(void)
{
	lv_tick_inc(1000u / configTICK_RATE_HZ);
}

/** FreeRTOS 힙이 부족할 때. configTOTAL_HEAP_SIZE 를 확인해야 한다. */
void vApplicationMallocFailedHook(void)
{
	LOG_E(TAG, "pvPortMalloc failed: FreeRTOS heap exhausted");

	configASSERT((volatile void *)NULL);
}
