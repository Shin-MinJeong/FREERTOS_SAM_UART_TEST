/*
 * bsp_display.c
 */

#include "main.h"

#define TAG "DISP"

#define DISPLAY_RESET_ASSERT_MS  20
#define DISPLAY_RESET_RELEASE_MS 150

void bsp_display_init(void)
{
	struct ili9488_opt_t options;

	bsp_board_shared_reset(DISPLAY_RESET_ASSERT_MS, DISPLAY_RESET_RELEASE_MS);

	options.ul_width         = BSP_DISPLAY_WIDTH;
	options.ul_height        = BSP_DISPLAY_HEIGHT;
	options.foreground_color = COLOR_CONVERT(COLOR_WHITE);
	options.background_color = COLOR_CONVERT(COLOR_WHITE);

	ili9488_init(&options);

	LOG_I(TAG, "ready %ux%u", (unsigned)BSP_DISPLAY_WIDTH, (unsigned)BSP_DISPLAY_HEIGHT);
}

void bsp_display_blit(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const bsp_display_pixel_t *pixels)
{
	ili9488_set_window(x, y, width, height);
	ili9488_write_ram_prepare();
	ili9488_write_ram_buffer(pixels, (uint32_t)width * height);
}
