/*
 * screen_console.h
 *
 * 버튼으로 ASCII 명령을 보내는 화면.
 *
 * 이 모듈은 UART 를 모른다. "어떤 명령을 보내달라"는 요청만 콜백으로
 * 올린다. 그래서 버튼을 추가해도 통신 코드는 손대지 않는다.
 * (기존 프로젝트의 screen_gallery 가 쓰던 것과 같은 패턴)
 */

#ifndef SCREEN_CONSOLE_H_
#define SCREEN_CONSOLE_H_

#include "lvgl.h"

/**
 * \brief 버튼이 눌렸을 때, 보낼 ASCII 명령을 들고 호출된다.
 *
 * 구현은 app_console 이 주입한다(보통 app_comm_send).
 */
typedef void (*screen_console_send_cb_t)(const char *ascii_cmd);

/**
 * \brief 콘솔 화면을 만든다.
 *
 * \param on_send 버튼 눌림 콜백 (NULL 이면 아무 일도 하지 않는다)
 * \return 생성된 화면 객체 (lv_scr_load 대상)
 */
lv_obj_t *screen_console_create(screen_console_send_cb_t on_send);

/**
 * \brief 화면의 로그 영역에 한 줄 추가한다.
 *
 * LVGL 태스크 컨텍스트에서만 호출해야 한다.
 * PC 에서 온 명령을 화면으로 확인하고 싶을 때 쓴다.
 */
void screen_console_log(const char *text);

#endif /* SCREEN_CONSOLE_H_ */
