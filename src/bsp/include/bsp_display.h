/*
 * bsp_display.h
 *
 * 디스플레이 패널 
 */

#ifndef BSP_DISPLAY_H_
#define BSP_DISPLAY_H_

#include <stdint.h>

#include <ili9488.h>

/** 패널 해상도 */
#define BSP_DISPLAY_WIDTH  ILI9488_LCD_WIDTH
#define BSP_DISPLAY_HEIGHT ILI9488_LCD_HEIGHT

typedef ili9488_color_t bsp_display_pixel_t;

/**
 * \brief 패널을 초기화
 */
void bsp_display_init(void);

/**
 * \param x       좌상단 X
 * \param y       좌상단 Y
 * \param width   폭 (픽셀)
 * \param height  높이 (픽셀)
 * \param pixels  width * height 개의 픽셀
 */
void bsp_display_blit(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bsp_display_pixel_t *pixels);

#endif /* BSP_DISPLAY_H_ */
