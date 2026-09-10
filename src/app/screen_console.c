/*
 * screen_console.c
 *
 * ===========================================================
 *  골격 파일입니다.
 *  버튼 1개(PING)와 로그 영역까지 동작합니다.
 *  [TODO] 의 BUTTON_TABLE 에 줄을 추가해 버튼을 늘리세요.
 * ===========================================================
 */

#include "main.h"

#define TAG "CONSOLE"

#define CONSOLE_LOG_MAX_LINES 32

static lv_obj_t *s_screen;
static lv_obj_t *s_log;     
static uint32_t  s_log_lines;

static screen_console_send_cb_t s_on_send;

typedef struct {
	const char *label;   /**< 화면에 보일 글자 */
	const char *command; /**< ASCII 명령 */
} console_button_t;

/*
 * [TODO] 여기에 버튼을 추가하세요.
 */
static const console_button_t BUTTON_TABLE[] = {
	{ "PING", "PING"    },
	{ "ON"  , "LED ON"  },
	{ "OFF" , "LED OFF" },
	/* { "VERSION", "VER"     }, */
};

#define BUTTON_COUNT (sizeof(BUTTON_TABLE) / sizeof(BUTTON_TABLE[0]))

static void on_button_clicked(lv_event_t *event)
{
	const console_button_t *button =
			(const console_button_t *)lv_event_get_user_data(event);

	if (button == NULL || s_on_send == NULL) {
		return;
	}

	LOG_I(TAG, "button '%s' -> \"%s\"", button->label, button->command);
	s_on_send(button->command);

	screen_console_log(button->command);
}

lv_obj_t *screen_console_create(screen_console_send_cb_t on_send)
{
	lv_obj_t *panel;
	uint32_t  i;

	s_on_send   = on_send;
	s_log_lines = 0;

	s_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(s_screen, lv_color_hex(UI_COLOR_LIST_BG), 0);

	/* 위쪽: 버튼 패널 */
	panel = lv_obj_create(s_screen);
	lv_obj_set_size(panel, BSP_DISPLAY_WIDTH - (UI_MARGIN * 2), (BSP_DISPLAY_HEIGHT / 2) - UI_MARGIN);
	lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, UI_MARGIN);
	lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	for (i = 0; i < BUTTON_COUNT; i++) {
		lv_obj_t *button = lv_btn_create(panel);
		lv_obj_t *label;

		lv_obj_set_size(button, BSP_DISPLAY_WIDTH - (UI_MARGIN * 6), UI_BUTTON_HEIGHT);
		lv_obj_add_event_cb(button, on_button_clicked, LV_EVENT_CLICKED, (void *)&BUTTON_TABLE[i]);

		label = lv_label_create(button);
		lv_label_set_text(label, BUTTON_TABLE[i].label);
		lv_obj_center(label);
	}

	/* 아래쪽: 로그 영역 */
	s_log = lv_label_create(s_screen);
	lv_obj_set_size(s_log, BSP_DISPLAY_WIDTH - (UI_MARGIN * 2), (BSP_DISPLAY_HEIGHT / 2) - (UI_MARGIN * 2));
	lv_obj_align(s_log, LV_ALIGN_BOTTOM_MID, 0, -UI_MARGIN);
	lv_label_set_long_mode(s_log, LV_LABEL_LONG_WRAP);
	lv_label_set_text(s_log, "ready\n");

	return s_screen;
}

void screen_console_log(const char *text)
{
	if (s_log == NULL) {
		return;
	}

	if (s_log_lines >= CONSOLE_LOG_MAX_LINES) {
		lv_label_set_text(s_log, "");
		s_log_lines = 0;
	}

	lv_label_ins_text(s_log, LV_LABEL_POS_LAST, text);
	lv_label_ins_text(s_log, LV_LABEL_POS_LAST, "\n");
	s_log_lines++;
}
