/*
 * main.h
 *
 * 프로젝트 집합 헤더 (project-wide aggregate header).
 *
 * 모든 .c 파일은 이 헤더 하나만 포함한다.
 *
 * === 새 모듈을 추가할 때 ===
 *   1) <계층>/include/새모듈.h  와  <계층>/새모듈.c  를 만든다.
 *   2) 이 파일의 해당 계층 구역에 #include "새모듈.h" 를 한 줄 추가한다.
 *   3) 새 .c 는 #include "main.h" 만 쓴다.
 *   4) Atmel Studio 프로젝트(.cproj)에 두 파일을 등록한다.
 *
 * === include 순서 규칙 ===
 *   표준 C  ->  벤더(ASF/FreeRTOS/LVGL)  ->  설정  ->  port  ->  bsp  ->  app
 *
 * 의존 방향:  app -> port -> bsp -> 벤더  (역방향 호출 금지)
 */

#ifndef MAIN_H_
#define MAIN_H_

/* ─────────────── C ─────────────── */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ─────────────── ASF ─────────────── */
#include <asf.h>
#include <compiler.h>
#include <delay.h>
#include <ioport.h>
#include <usart.h>

/* ─────────────── 프로젝트 설정 ─────────────── */
#include "conf_board.h"
#include "conf_uart_serial.h"

/* ─────────────── FreeRTOS ─────────────── */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "portable.h"

/* ─────────────── LVGL ─────────────── */
#include "lvgl.h"

/* ─────────────── port : 플랫폼 어댑터 / 공통 유틸 ─────────────── */
#include "log.h"
#include "os_task.h"
#include "rtos_hooks.h"
#include "lv_port_cfg.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

/* ─────────────── bsp : 하드웨어 직결 ─────────────── */
#include "bsp_board.h"
#include "bsp_display.h"
#include "bsp_touch_cfg.h"
#include "bsp_touch.h"
#include "bsp_uart_cfg.h"
#include "bsp_uart.h"

/* ─────────────── app : 애플리케이션 로직 ─────────────── */
#include "app_config.h"
#include "ui_theme.h"
#include "ascii_string.h"
#include "cmd_parser.h"
#include "cmd_table.h"
#include "app_comm.h"
#include "screen_console.h"
#include "app_diag.h"
#include "app_ui.h"
#include "app_boot.h"

#endif /* MAIN_H_ */
