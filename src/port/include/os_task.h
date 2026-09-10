/*
 * os_task.h
 *
 * FreeRTOS 태스크 생성 래퍼.
 *
 * 리팩토링 이전에는 xTaskCreate() 실패 검사가 5곳에 흩어져 있었고
 * 그중 3곳은 빈 블록이어서 실패해도 아무 흔적이 남지 않았다.
 * 생성 실패 처리를 이 한 곳으로 모은다.
 *
 * 스택 크기는 "바이트" 단위로 받는다. 기존 코드는 일부 태스크만
 * (N / sizeof(portSTACK_TYPE)) 로 환산하고 나머지는 워드 값을 그대로
 * 넘겨서 실제 스택이 의도와 4배 달랐다. 단위를 하나로 못박는다.
 */

#ifndef PORT_OS_TASK_H_
#define PORT_OS_TASK_H_

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

/**
 * \brief 태스크를 생성한다. 실패 시 원인을 로그로 남긴다.
 *
 * \param entry        태스크 함수
 * \param name         디버거/vTaskList 표시 이름
 * \param stack_bytes  스택 크기 (바이트)
 * \param priority     우선순위
 * \param arg          태스크 인자 (없으면 NULL)
 *
 * \return 생성 성공 여부
 */
bool os_task_create(TaskFunction_t entry, const char *name,
		uint32_t stack_bytes, UBaseType_t priority, void *arg);

#endif /* PORT_OS_TASK_H_ */
