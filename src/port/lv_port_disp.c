/*
 * lv_port_disp.c
 */

#include "main.h"

/** 드로우 버퍼 픽셀 수. */
#define DISP_BUF_PIXELS (BSP_DISPLAY_WIDTH * LV_PORT_DISP_BUF_LINES)

static lv_disp_draw_buf_t s_draw_buf;
static lv_color_t         s_pixels[DISP_BUF_PIXELS];
static lv_disp_drv_t      s_driver;

/** LVGL 이 갱신된 영역을 패널로 내보낼 때 호출한다. */
static void flush_area(lv_disp_drv_t *driver, const lv_area_t *area,
		lv_color_t *colors)
{
	const uint16_t width  = (uint16_t)(area->x2 - area->x1 + 1);
	const uint16_t height = (uint16_t)(area->y2 - area->y1 + 1);

	bsp_display_blit((uint16_t)area->x1, (uint16_t)area->y1, width, height,
			(const bsp_display_pixel_t *)colors);

	lv_disp_flush_ready(driver);
}

void lv_port_disp_init(void)
{
	lv_disp_draw_buf_init(&s_draw_buf, s_pixels, NULL, DISP_BUF_PIXELS);

	lv_disp_drv_init(&s_driver);
	s_driver.hor_res  = BSP_DISPLAY_WIDTH;
	s_driver.ver_res  = BSP_DISPLAY_HEIGHT;
	s_driver.flush_cb = flush_area;
	s_driver.draw_buf = &s_draw_buf;

	lv_disp_drv_register(&s_driver);
}
