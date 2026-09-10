/*
 * lv_port_disp.h
 *
 * LVGL 디스플레이 드라이버 등록.
 *
 * LVGL 과 패널 드라이버 사이의 유일한 접점이다. 이 모듈만
 * bsp_display 를 호출하고, 화면 코드는 LVGL API 만 쓴다.
 */

#ifndef LV_PORT_DISP_H_
#define LV_PORT_DISP_H_

/**
 * \brief 드로우 버퍼를 준비하고 LVGL 에 디스플레이 드라이버를 등록한다.
 *
 * lv_init() 이후, 화면을 만들기 전에 호출한다.
 */
void lv_port_disp_init(void);

#endif /* LV_PORT_DISP_H_ */
