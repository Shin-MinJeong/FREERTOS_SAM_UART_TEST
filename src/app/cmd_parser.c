/*
 * cmd_parser.c
 *
 * ===========================================================
 *  골격 파일입니다. 아래 [TODO] 부분을 채우세요.
 * ===========================================================
 */

#include "main.h"

#define TAG "CMD"

static cmd_parser_reply_cb_t s_reply;

static void reply(const char *text)
{
	if (s_reply != NULL) {
		s_reply(text);
	}
}

/* ─────────────────────── 명령 핸들러 ─────────────────────── */

/**
 * 예시 명령: PING -> PONG
 *
 * 새 명령은 이 함수를 복사해서 만들고 COMMAND_TABLE 에 한 줄 추가하면
 * 끝입니다.
 */
static bool cmd_ping(uint32_t argc, char *argv[], cmd_parser_reply_cb_t out)
{
	UNUSED(argc);
	UNUSED(argv);

	out("PONG");
	return true;
}

/* [TODO] 여기에 명령 핸들러를 추가하세요. 예시:
 *
 * static bool cmd_led(uint32_t argc, char *argv[], cmd_parser_reply_cb_t out)
 * {
 *     if (argc < 1) { out("ERR need ON|OFF"); return false; }
 *     if (strcasecmp(argv[0], "ON") == 0) {
 *         LED_On(LED0);
 *         out("OK LED ON");
 *     } else {
 *         LED_Off(LED0);
 *         out("OK LED OFF");
 *     }
 *     return true;
 * }
 *
 * static bool cmd_echo(uint32_t argc, char *argv[], cmd_parser_reply_cb_t out);
 * static bool cmd_ver (uint32_t argc, char *argv[], cmd_parser_reply_cb_t out);
 */

/* ─────────────────────── 명령 테이블 ─────────────────────── */
/*
 * [TODO] 여기에 명령을 등록하세요.
 * 이름은 대소문자를 구분하지 않습니다.
 */
static const cmd_entry_t COMMAND_TABLE[] = {
	/* help 문자열은 HELP 명령이 UART 로 그대로 내보낸다. 터미널이 UTF-8
	 * 로 해석하지 않는 경우가 많으므로 ASCII 로만 적는다. */
	{ "PING", cmd_ping, "PING - link check, replies PONG" },
	/* { "LED",  cmd_led,  "LED ON|OFF - 보드 LED 제어" }, */
	/* { "ECHO", cmd_echo, "ECHO <text> - 받은 문자열을 그대로 돌려준다" }, */
	/* { "VER",  cmd_ver,  "VER - 펌웨어 빌드 정보" }, */
};

#define COMMAND_COUNT (sizeof(COMMAND_TABLE) / sizeof(COMMAND_TABLE[0]))

/* ─────────────────────── 해석기 본체 ─────────────────────── */

static bool name_matches(const char *a, const char *b)
{
	while (*a != '\0' && *b != '\0') {
		if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
			return false;
		}
		a++;
		b++;
	}

	return (*a == '\0' && *b == '\0');
}

static uint32_t tokenize(char *line, char *tokens[], uint32_t max_tokens)
{
	uint32_t count = 0;
	char *cursor = line;

	while (*cursor != '\0' && count < max_tokens) {
		while (*cursor == ' ' || *cursor == '\t') {
			*cursor = '\0';
			cursor++;
		}
		if (*cursor == '\0') {
			break;
		}

		tokens[count++] = cursor;

		while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t') {
			cursor++;
		}
	}

	return count;
}

void cmd_parser_init(cmd_parser_reply_cb_t reply_cb)
{
	s_reply = reply_cb;
	LOG_I(TAG, "%lu command(s) registered", (unsigned long)COMMAND_COUNT);
}

bool cmd_parser_handle_line(char *line)
{
	char    *tokens[CMD_ARG_MAX + 1u];
	uint32_t count;
	uint32_t i;

	count = tokenize(line, tokens, CMD_ARG_MAX + 1u);
	if (count == 0u) {
		return false; /* 빈 줄 */
	}

	for (i = 0; i < COMMAND_COUNT; i++) {
		if (!name_matches(tokens[0], COMMAND_TABLE[i].name)) {
			continue;
		}

		LOG_I(TAG, "exec '%s'", COMMAND_TABLE[i].name);

		if (!COMMAND_TABLE[i].handler(count - 1u, &tokens[1], reply)) {
			reply("ERR command failed");
		}
		return true;
	}

	LOG_W(TAG, "unknown command '%s'", tokens[0]);
	reply("ERR unknown command");
	return false;
}
