/*
 * bsp_touch.h
 *
 * 정전식 터치 패널
*
 */

#ifndef BSP_TOUCH_H_
#define BSP_TOUCH_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * raw 좌표 범위
 */

/** @{ */
#define BSP_TOUCH_RAW_MAX_X 4095
#define BSP_TOUCH_RAW_MAX_Y 4095
/** @} */

/** 터치 상태 */
typedef struct {
	uint16_t x;       /**< raw X (0 ~ BSP_TOUCH_RAW_MAX_X) */
	uint16_t y;       /**< raw Y (0 ~ BSP_TOUCH_RAW_MAX_Y) */
	bool     pressed; /**< 눌린 상태 여부 */
} bsp_touch_point_t;

/**
 * \brief 터치 컨트롤러 초기화
 */
bool bsp_touch_init(void);
void bsp_touch_poll(bsp_touch_point_t *point);

#endif /* BSP_TOUCH_H_ */
