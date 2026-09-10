/*
 * lv_port_indev.h
 *
 * LVGL 입력 장치(터치) 드라이버 등록.
 *
 * 터치 컨트롤러를 폴링하는 태스크와, raw 좌표를 화면 좌표로 바꾸는
 * 변환이 이 모듈에 있다. 변환 파라미터는 lv_port_cfg.h 참조.
 */

#ifndef LV_PORT_INDEV_H_
#define LV_PORT_INDEV_H_

#include <stdbool.h>

/**
 * \brief 터치 폴링 태스크를 만들고 LVGL 에 입력 드라이버를 등록한다.
 *
 * lv_init() 이후에 호출한다. 폴링을 별도 태스크로 분리한 이유는,
 * TWI 통신이 LVGL 렌더링 주기를 늘려 화면이 끊기지 않게 하기 위함이다.
 *
 * \return 등록 및 태스크 생성 성공 여부
 */
bool lv_port_indev_init(void);

#endif /* LV_PORT_INDEV_H_ */
