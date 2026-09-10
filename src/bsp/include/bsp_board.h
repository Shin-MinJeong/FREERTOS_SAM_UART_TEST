/*
 * bsp_board.h
 *
 * 보드 초기화
 */

#ifndef BSP_BOARD_H_
#define BSP_BOARD_H_

#include <stdint.h>

/**
 * \brief 클럭 / 보드 / 콘솔 UART 를 초기화한다.
 *
 * 다른 bsp_* 초기화보다 먼저 호출해야 한다.
 */
void bsp_board_init(void);

/**
 * \brief LCD 패널과 maXTouch 가 공유하는 리셋 라인에 펄스를 준다.
 *
 * === 이 보드의 배선 제약 ===
 * PIOC13 은 ILI9488 패널과 maXTouch 컨트롤러의 리셋 핀에 함께 연결되어 있다. 즉 어느 한쪽을 리셋하면 다른 쪽도 리셋된다.
 *
 *   1) bsp_touch_init()    리셋 후 maXTouch 설정  -> 설정은 BACKUPNV 로 NV 메모리에 영구 저장되므로 이후 리셋되어도 컨트롤러가 스스로 복구한다.
 *   2) bsp_display_init()  리셋 후 ILI9488 초기화  -> 마지막 리셋 펄스 이후에 패널을 초기화하므로 화면이 살아남는다.
 *
 * 순서를 뒤집으면 (2)->(1) 이 되어, 터치 초기화의 리셋 펄스가 이미
 * 초기화된 패널을 리셋해 화면이 백지가 된다. app_boot.c 의 호출 순서를 임의로 바꾸지 말 것.
 *
 * \param assert_ms   리셋을 LOW 로 유지할 시간 (ms)
 * \param release_ms  HIGH 복귀 후 대기 시간 (ms)
 */
void bsp_board_shared_reset(uint32_t assert_ms, uint32_t release_ms);

#endif /* BSP_BOARD_H_ */
