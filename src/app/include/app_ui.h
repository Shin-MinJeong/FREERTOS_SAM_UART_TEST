/*
 * app_ui.h
 *
 * LVGL 런타임 태스크.
 *
 * LVGL 초기화 -> 포팅 계층 등록 -> 화면 시작 -> lv_timer_handler 루프 까지를 한 태스크에서 수행한다.
 *
 */

#ifndef APP_UI_H_
#define APP_UI_H_

#include <stdbool.h>

/**
 * \brief LVGL 태스크를 만든다.
 *
 * 스케줄러 시작 전에 호출한다. 디스플레이/터치 BSP 초기화가 끝난
 * 뒤여야 한다.
 *
 * \return 태스크 생성 성공 여부
 */
bool app_ui_start(void);

#endif /* APP_UI_H_ */
