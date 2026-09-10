/*
 * lv_port_cfg.h
 *
 * 터치 좌표를 화면 좌표에 맞추는 파라미터.
 *
 * 여기 값은 "패널과 터치 센서가 어떤 방향으로 조립되었는가" 에 관한
 * 것이다. 컨트롤러 자체의 특성(raw 좌표 범위)은 bsp_touch.h 가 갖는다.
 *
 * 터치 위치가 어긋날 때 조정 순서:
 *   1) 축이 바뀐 느낌이면 SWAP_XY
 *   2) 좌우가 반대면 INVERT_X, 상하가 반대면 INVERT_Y
 */

#ifndef LV_PORT_CFG_H_
#define LV_PORT_CFG_H_

/** X 와 Y 를 맞바꾼다 (패널을 90도 돌려 장착한 경우). */
#define LV_PORT_TOUCH_SWAP_XY 1

/** X 축을 반전한다. */
#define LV_PORT_TOUCH_INVERT_X 0

/** Y 축을 반전한다. */
#define LV_PORT_TOUCH_INVERT_Y 0

/**
 * 드로우 버퍼 높이 (라인 수).
 *
 * 버퍼 크기는 (화면 폭 x 이 값 x 픽셀당 바이트) 다. 키우면 플러시
 * 횟수가 줄어 화면이 매끄러워지지만 RAM 을 더 쓴다.
 */
#define LV_PORT_DISP_BUF_LINES 120

#endif /* LV_PORT_CFG_H_ */
