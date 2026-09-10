/*
 * rtos_hooks.h
 *
 * FreeRTOS 가 요구하는 콜백 훅 선언.
 *
 * 이 훅들은 커널이 직접 호출하므로 애플리케이션 코드가 부를 일은 없다.
 * 그래도 프로토타입을 노출해, 시그니처가 FreeRTOSConfig.h 의 설정과
 * 어긋나면 컴파일 단계에서 드러나게 한다.
 *
 * 어떤 훅이 실제로 호출되는지는 config/FreeRTOSConfig.h 의
 * configUSE_TICK_HOOK / configUSE_IDLE_HOOK /
 * configCHECK_FOR_STACK_OVERFLOW / configUSE_MALLOC_FAILED_HOOK 이
 * 결정한다.
 */

#ifndef PORT_RTOS_HOOKS_H_
#define PORT_RTOS_HOOKS_H_

/* xTaskHandle 을 쓰는 선언이 있어 여기까지는 필요하다. */
#include "FreeRTOS.h"
#include "task.h"

/** \brief 태스크가 스택을 넘겼을 때 커널이 호출한다. 반환하지 않는다. */
void vApplicationStackOverflowHook(xTaskHandle *task, signed char *name);

/** \brief 매 틱마다 커널이 호출한다. LVGL 의 시간 기준을 공급한다. */
void vApplicationTickHook(void);

/** \brief FreeRTOS 힙이 부족할 때 커널이 호출한다. */
void vApplicationMallocFailedHook(void);

#endif /* PORT_RTOS_HOOKS_H_ */
