/*
 * lv_port_indev.c
 */

#include "main.h"

static lv_indev_drv_t s_driver;

/*
 * 폴링 태스크가 쓰고 LVGL 태스크가 읽는 최신 터치 상태.
 * 세 필드를 한 번에 갱신해야 좌표와 눌림 상태가 어긋나지 않으므로
 * 접근을 크리티컬 섹션으로 묶는다.
 */
static volatile bsp_touch_point_t s_latest;

/** 터치 컨트롤러를 주기적으로 읽어 s_latest 를 갱신한다. */
static void touch_poll_task(void *params)
{
	UNUSED(params);

	for (;;) {
		bsp_touch_point_t point;

		bsp_touch_poll(&point);

		taskENTER_CRITICAL();
		s_latest.x       = point.x;
		s_latest.y       = point.y;
		s_latest.pressed = point.pressed;
		taskEXIT_CRITICAL();

		vTaskDelay(pdMS_TO_TICKS(APP_TOUCH_POLL_PERIOD_MS));
	}
}

/**
 * \brief raw 좌표를 화면 좌표로 변환한다.
 *
 * 회전/반전 -> 스케일 -> 클램프 순서로 처리한다.
 */
static void raw_to_screen(uint16_t raw_x, uint16_t raw_y,
		lv_coord_t *screen_x, lv_coord_t *screen_y)
{
	uint32_t x = raw_x;
	uint32_t y = raw_y;

#if LV_PORT_TOUCH_SWAP_XY
	{
		const uint32_t swapped = x;
		x = y;
		y = swapped;
	}
#endif
#if LV_PORT_TOUCH_INVERT_X
	x = BSP_TOUCH_RAW_MAX_X - 1u - x;
#endif
#if LV_PORT_TOUCH_INVERT_Y
	y = BSP_TOUCH_RAW_MAX_Y - 1u - y;
#endif

	x = (x * BSP_DISPLAY_WIDTH) / BSP_TOUCH_RAW_MAX_X;
	y = (y * BSP_DISPLAY_HEIGHT) / BSP_TOUCH_RAW_MAX_Y;

	if (x >= BSP_DISPLAY_WIDTH) {
		x = BSP_DISPLAY_WIDTH - 1u;
	}
	if (y >= BSP_DISPLAY_HEIGHT) {
		y = BSP_DISPLAY_HEIGHT - 1u;
	}

	*screen_x = (lv_coord_t)x;
	*screen_y = (lv_coord_t)y;
}

/** LVGL 이 입력 상태를 물어볼 때 호출한다. */
static void read_touch(lv_indev_drv_t *driver, lv_indev_data_t *data)
{
	/* 손을 뗀 뒤에도 LVGL 은 마지막 좌표를 필요로 하므로 유지한다. */
	static lv_coord_t last_x = 0;
	static lv_coord_t last_y = 0;

	bsp_touch_point_t point;

	LV_UNUSED(driver);

	taskENTER_CRITICAL();
	point.x       = s_latest.x;
	point.y       = s_latest.y;
	point.pressed = s_latest.pressed;
	taskEXIT_CRITICAL();

	if (point.pressed) {
		raw_to_screen(point.x, point.y, &last_x, &last_y);
	}

	data->point.x = last_x;
	data->point.y = last_y;
	data->state   = point.pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

bool lv_port_indev_init(void)
{
	lv_indev_drv_init(&s_driver);
	s_driver.type    = LV_INDEV_TYPE_POINTER;
	s_driver.read_cb = read_touch;

	if (lv_indev_drv_register(&s_driver) == NULL) {
		return false;
	}

	return os_task_create(touch_poll_task, "Touch",
			APP_TASK_TOUCH_STACK_BYTES, APP_TASK_TOUCH_PRIORITY, NULL);
}
