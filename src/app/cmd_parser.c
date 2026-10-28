/*
 * cmd_parser.c
 *
 * ASCII 명령 엔진 본체. 등록 / 해석 / 출력
 */

#include "main.h"

#include <stdarg.h>

#define TAG "CMD"

typedef struct {
	const char   *name;
	const char   *args;
	cmd_handler_t handler;
} cmd_entry_t;

static cmd_entry_t    s_table[CMD_MAX_ENTRIES];
static uint32_t       s_count;
static cmd_write_fn_t s_write;

static char *s_cursor;

/** cmd_printf() 조립용 버퍼. 위와 같은 이유로 태스크 하나 전용이다. */
static char s_tx[CMD_TX_MAX];

/* ─────────────────────── 출력 ─────────────────────── */

static void emit(uint32_t ch, const char *text)
{
	if ((s_write != NULL) && (ch < CMD_CH_NUM)) {
		s_write(ch, text);
	}
}

static void format_and_emit(uint32_t ch, const char *fmt, va_list args,
		bool add_eol)
{
	int length;

	length = CMD_VSNPRINTF(s_tx, sizeof(s_tx) - 3u, fmt, args);
	if (length < 0) {
		return;
	}
	if ((uint32_t)length >= (sizeof(s_tx) - 3u)) {
		length = (int)(sizeof(s_tx) - 3u) - 1; /* 잘렸다 */
	}

	if (add_eol) {
		s_tx[length]      = '\r';
		s_tx[length + 1]  = '\n';
		s_tx[length + 2]  = '\0';
	}

	emit(ch, s_tx);
}

void cmd_printf(uint32_t ch, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	format_and_emit(ch, fmt, args, true);
	va_end(args);
}

void cmd_printf_raw(uint32_t ch, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	format_and_emit(ch, fmt, args, false);
	va_end(args);
}

/* ─────────────────────── 인자 꺼내기 ─────────────────────── */

char *cmd_arg_str(void)
{
	char *token;

	if (s_cursor == NULL) {
		return NULL;
	}

	while ((*s_cursor == ' ') || (*s_cursor == '\t')) {
		s_cursor++;
	}
	if (*s_cursor == '\0') {
		s_cursor = NULL;
		return NULL;
	}

	token = s_cursor;

	while ((*s_cursor != '\0') && (*s_cursor != ' ') && (*s_cursor != '\t')) {
		s_cursor++;
	}
	if (*s_cursor != '\0') {
		*s_cursor = '\0';
		s_cursor++;
	} else {
		s_cursor = NULL;
	}

	return token;
}

char *cmd_arg_rest(void)
{
	char *rest;

	if (s_cursor == NULL) {
		return NULL;
	}

	while ((*s_cursor == ' ') || (*s_cursor == '\t')) {
		s_cursor++;
	}
	if (*s_cursor == '\0') {
		s_cursor = NULL;
		return NULL;
	}

	rest     = s_cursor;
	s_cursor = NULL;

	return rest;
}

bool cmd_arg_dec(uint32_t *value)
{
	const char *token = cmd_arg_str();
	bool        ok    = false;
	uint32_t    parsed;

	if ((token == NULL) || (value == NULL)) {
		return false;
	}

	parsed = ascii_to_decimal(token, &ok);
	if (ok) {
		*value = parsed;
	}

	return ok;
}

bool cmd_arg_sdec(int32_t *value)
{
	const char *token = cmd_arg_str();
	bool        ok    = false;
	int32_t     parsed;

	if ((token == NULL) || (value == NULL)) {
		return false;
	}

	parsed = ascii_to_signed_decimal(token, &ok);
	if (ok) {
		*value = parsed;
	}

	return ok;
}

bool cmd_arg_hex(uint32_t *value)
{
	const char *token = cmd_arg_str();
	bool        ok    = false;
	uint32_t    parsed;

	if ((token == NULL) || (value == NULL)) {
		return false;
	}

	parsed = ascii_to_integer(token, &ok);
	if (ok) {
		*value = parsed;
	}

	return ok;
}

bool cmd_arg_onoff(bool *on)
{
	const char *token = cmd_arg_str();

	if ((token == NULL) || (on == NULL)) {
		return false;
	}

	return ascii_to_on_off(token, on);
}

/* ─────────────────────── 등록 / HELP ─────────────────────── */

static const cmd_entry_t *find_entry(const char *name)
{
	uint32_t i;

	for (i = 0; i < s_count; i++) {
		if (s_table[i].handler == NULL) {
			continue; /* 구분선 */
		}
		if (ascii_equal_nocase(name, s_table[i].name)) {
			return &s_table[i];
		}
	}

	return NULL;
}

bool cmd_register(const char *name, const char *args, cmd_handler_t handler)
{
	if (name == NULL) {
		return false;
	}
	if (s_count >= CMD_MAX_ENTRIES) {
		LOG_E(TAG, "table full, '%s' dropped (raise CMD_MAX_ENTRIES)", name);
		return false;
	}
	if ((handler != NULL) && (find_entry(name) != NULL)) {
		LOG_E(TAG, "duplicate command '%s'", name);
		return false;
	}

	s_table[s_count].name    = name;
	s_table[s_count].args    = args;
	s_table[s_count].handler = handler;
	s_count++;

	return true;
}

uint32_t cmd_count(void)
{
	return s_count;
}

void cmd_print_help(uint32_t ch)
{
	uint32_t i;

	cmd_printf(ch, "==== command list");

	for (i = 0; i < s_count; i++) {
		if (s_table[i].handler == NULL) {
			cmd_printf(ch, "%s", s_table[i].name); /* 구분선 */
			continue;
		}

		cmd_printf(ch, " %-16s %s", s_table[i].name,
				(s_table[i].args != NULL) ? s_table[i].args : "");
	}
}

static void cmd_help(uint32_t ch)
{
	cmd_print_help(ch);
}

/* ─────────────────────── 해석기 ─────────────────────── */

void cmd_parser_init(cmd_write_fn_t write_fn)
{
	s_write  = write_fn;
	s_count  = 0;
	s_cursor = NULL;

	cmd_register("help", 0, cmd_help);
}

bool cmd_parser_handle_line(uint32_t ch, char *line)
{
	const cmd_entry_t *entry;
	char              *name;

	if (line == NULL) {
		return false;
	}

	s_cursor = line;

	name = cmd_arg_str();
	if (name == NULL) {
		return false; /* 빈 줄 */
	}

	entry = find_entry(name);
	if (entry == NULL) {
		LOG_W(TAG, "unknown command '%s'", name);
		cmd_printf(ch, "ERR: unknown command (%s) - type 'help'", name);
		return false;
	}

	LOG_I(TAG, "exec '%s'", entry->name);
	entry->handler(ch);

	return true;
}
