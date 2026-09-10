/*
 * cmd_parser.h
 *
 * ASCII 명령어 해석기.
 *
 * === 이 모듈이 모르는 것 ===
 * UART, 화면, 하드웨어를 전혀 모른다. 문자열 한 줄을 받아 해석하고,
 * 답할 내용이 있으면 등록된 콜백으로 넘긴다. 그래서 통신 수단을
 * USB CDC 나 이더넷으로 바꿔도 이 파일은 수정 대상이 아니다.
 *
 * === 명령 형식 ===
 *   <NAME> [ARG1] [ARG2] ...
 *   - 공백으로 구분, 이름은 대소문자 무시
 *   - 줄 종단은 전송 계층(bsp_uart)이 이미 제거한 상태로 들어온다
 *
 * === 새 명령을 추가하는 방법 ===
 *   1) 핸들러 함수를 static 으로 하나 만든다
 *   2) cmd_parser.c 의 COMMAND_TABLE 에 한 줄 추가한다
 *   그 외에는 아무것도 건드리지 않는다.
 */

#ifndef CMD_PARSER_H_
#define CMD_PARSER_H_

#include <stdbool.h>
#include <stdint.h>

/** 한 명령에서 받을 수 있는 최대 인자 수. */
#define CMD_ARG_MAX 4u

/**
 * \brief 해석기가 답신을 보낼 때 호출한다.
 *
 * 구현은 app_comm 이 주입한다(보통 bsp_uart_write_line).
 * 해석기가 전송 계층을 직접 부르지 않게 하는 지점이다.
 */
typedef void (*cmd_parser_reply_cb_t)(const char *text);

/**
 * \brief 명령 핸들러.
 *
 * \param argc 인자 수 (명령 이름은 제외)
 * \param argv 인자 배열
 * \param reply 답신 함수 (NULL 이 아님이 보장된다)
 * \return 처리 성공 여부. false 면 해석기가 오류 답신을 보낸다.
 */
typedef bool (*cmd_handler_t)(uint32_t argc, char *argv[],
		cmd_parser_reply_cb_t reply);

/** 명령 테이블 한 줄. */
typedef struct {
	const char   *name;  /**< 명령 이름 (대소문자 무시) */
	cmd_handler_t handler;
	const char   *help;  /**< HELP 명령이 출력할 한 줄 설명 */
} cmd_entry_t;

/**
 * \brief 해석기를 초기화한다.
 *
 * \param reply 답신 함수. NULL 이면 답신을 보내지 않는다.
 */
void cmd_parser_init(cmd_parser_reply_cb_t reply);

/**
 * \brief 한 줄을 해석해 해당 명령을 실행한다.
 *
 * 문자열을 제자리에서 토큰화하므로 line 은 쓰기 가능한 버퍼여야 한다.
 *
 * \param line 널 종단 문자열 (줄바꿈 없음)
 * \return 알려진 명령을 실행했으면 true
 */
bool cmd_parser_handle_line(char *line);

#endif /* CMD_PARSER_H_ */
