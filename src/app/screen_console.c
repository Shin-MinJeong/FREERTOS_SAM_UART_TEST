/*
 * screen_console.c
 *
 * 버튼으로 ASCII 명령을 실행하는 화면.
 *
 * 버튼 하나는 "터미널에 이 문자열을 타이핑한 것과 같다"는 뜻이다.
 * 그래서 버튼을 추가할 때 새로 만들 함수는 없다. cmd_table.c 에 이미
 * 등록된 명령 이름을 아래 표에 적기만 하면 된다.
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
	const char *command; /**< 실행할 ASCII 명령 (cmd_table.c 에 등록된 것) */
} console_button_t;

/*
 * 버튼 추가: 줄 하나만 늘리면 된다.
 * command 는 터미널에 치는 문자열 그대로다. 대소문자는 구분하지 않고
 * "led on" 처럼 인자도 함께 적을 수 있다.
 */
static const console_button_t BUTTON_TABLE[] = {
	{ "PING"   , "ping"    },
	{ "ON"     , "led on"  },
	{ "OFF"    , "led off" },
	{ "VERSION", "ver"     },
	/* { "STATUS", "status" }, */
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

	/* 실행은 app_comm 이 한다. 이 화면은 UART 도 파서도 모른다. */
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
