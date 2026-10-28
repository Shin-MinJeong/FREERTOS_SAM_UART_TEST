/*
 * cmd_parser.h
 *
 * ASCII 명령어 엔진.
 *
 * === 이 파일은 거의 손댈 일이 없다 ===
 * 새 명령을 추가할 때 여는 파일은 cmd_table.c 하나다. 여기는 그 명령들이
 * 쓰는 도구(등록 함수, 출력 매크로, 인자 split 매크로)를 정의한다.
 *
 * === 이 모듈이 모르는 것 ===
 * UART, 화면, 하드웨어를 전혀 모른다. 문자열 한 줄을 받아 해석하고,
 * 출력은 등록된 콜백으로 넘긴다. 그래서 통신 수단을 USB CDC 나 이더넷으로
 * 바꿔도 이 파일은 수정 대상이 아니다.
 *
 * === 명령 형식 ===
 *   <NAME> [ARG1] [ARG2] ...
 *   - 공백/탭으로 구분, 이름은 대소문자 무시
 *   - 줄 종단은 전송 계층(bsp_uart)이 이미 제거한 상태로 들어온다
 *
 * === 새 명령을 추가하는 방법 (cmd_table.c 에서) ===
 *   1) 핸들러 함수 static void cmd_xxx(uint32_t ch) 를 하나 만든다
 *   2) cmd_table_register_all() 에 cmd_register("xxx", "(인자)", cmd_xxx);
 *      한 줄을 추가한다
 *   그 외에는 아무것도 건드리지 않는다. HELP 출력도 자동으로 따라온다.
 */

#ifndef CMD_PARSER_H_
#define CMD_PARSER_H_

#include <stdbool.h>
#include <stdint.h>

#include "ascii_string.h"

/** \name 크기 제한 */
/** @{ */
/** 등록할 수 있는 명령 수 (구분선 포함). 넘치면 등록이 거부되고 경고가 뜬다. */
#define CMD_MAX_ENTRIES 48u

/** cmd_printf() 한 번이 만들 수 있는 최대 길이. */
#define CMD_TX_MAX 160u
/** @} */

/**
 * 형식 출력에 쓸 함수.
 *
 * 이 프로젝트는 printf=iprintf 로 빌드한다(부동소수 출력 코드를 링크에서
 * 빼기 위해서다). 그래서 여기서도 정수 전용인 vsniprintf 를 쓴다.
 * %f 를 꼭 써야 한다면 이 값을 vsnprintf 로 바꾼다(코드 크기가 는다).
 */
#ifndef CMD_VSNPRINTF
#define CMD_VSNPRINTF vsniprintf
#endif

/**
 * \name 출력 채널
 *
 * 핸들러는 자기가 어디서 불렸는지 모른 채 받은 ch 로 그대로 답한다.
 * 그래서 같은 명령을 UART 에서도, 화면 버튼에서도 쓸 수 있다.
 */
/** @{ */
#define CMD_CH_UART   0u /**< PC 터미널 (bsp_uart) */
#define CMD_CH_SCREEN 1u /**< LVGL 콘솔 화면 */
#define CMD_CH_NUM    2u
/** @} */

/**
 * \brief 명령 핸들러.
 *
 * 인자는 매개변수로 받지 않는다. 핸들러 안에서 DEC_SPLT / HEX_SPLT /
 * STR_SPLT / ONOFF_SPLT 로 필요한 만큼 하나씩 꺼내 쓴다.
 *
 * \param ch 답신을 보낼 출력 채널 (CMD_CH_*)
 */
typedef void (*cmd_handler_t)(uint32_t ch);

/**
 * \brief 엔진이 문자를 내보낼 때 호출한다.
 *
 * 구현은 app_comm 이 주입한다. 줄바꿈은 엔진이 이미 붙여서 넘기므로
 * 콜백은 받은 문자열을 그대로 쓰기만 하면 된다.
 */
typedef void (*cmd_write_fn_t)(uint32_t ch, const char *text);

/**
 * \brief 엔진을 초기화하고 HELP 명령을 등록한다.
 *
 * \param write_fn 출력 함수. NULL 이면 아무것도 내보내지 않는다.
 */
void cmd_parser_init(cmd_write_fn_t write_fn);

/**
 * \brief 명령을 하나 등록한다.
 *
 * \param name    명령 이름 (대소문자 무시). 상수 문자열이어야 한다.
 * \param args    HELP 에 보일 인자 설명. 인자가 없으면 NULL.
 * \param handler 실행할 함수. NULL 이면 HELP 의 구분선으로만 쓰인다.
 * \return 등록 성공 여부 (이름 중복이거나 표가 꽉 차면 false)
 */
bool cmd_register(const char *name, const char *args, cmd_handler_t handler);

/**
 * \brief 한 줄을 해석해 해당 명령을 실행한다.
 *
 * 문자열을 제자리에서 토큰화하므로 line 은 쓰기 가능한 버퍼여야 한다.
 *
 * \param ch   답신을 보낼 채널
 * \param line 널 종단 문자열 (줄바꿈 없음)
 * \return 알려진 명령을 실행했으면 true
 */
bool cmd_parser_handle_line(uint32_t ch, char *line);

/** \brief 등록된 명령 목록을 출력한다 (HELP 명령이 쓰는 함수). */
void cmd_print_help(uint32_t ch);

/** \brief 등록된 명령 수. */
uint32_t cmd_count(void);

/**
 * \name 출력
 *
 * 참조 프로젝트의 UART_CONTROL() 자리에 오는 함수들이다.
 */
/** @{ */
/** \brief 형식 출력 + 줄바꿈. 한 줄을 완성해 내보낸다. */
void cmd_printf(uint32_t ch, const char *fmt, ...)
		__attribute__((__format__(__printf__, 2, 3)));

/** \brief 형식 출력, 줄바꿈 없음. 표/hex 덤프를 조각내 찍을 때 쓴다. */
void cmd_printf_raw(uint32_t ch, const char *fmt, ...)
		__attribute__((__format__(__printf__, 2, 3)));
/** @} */

/**
 * \name 인자 꺼내기 (핸들러 안에서만 호출)
 *
 * 호출할 때마다 다음 인자로 넘어간다. 인자가 더 없으면 실패한다.
 */
/** @{ */
/** \brief 다음 인자를 문자열로. 없으면 NULL. */
char *cmd_arg_str(void);

/** \brief 다음 인자를 10진수로. */
bool cmd_arg_dec(uint32_t *value);

/** \brief 다음 인자를 부호 있는 10진수로. */
bool cmd_arg_sdec(int32_t *value);

/** \brief 다음 인자를 16진수로 ("1A" / "0x1A"). */
bool cmd_arg_hex(uint32_t *value);

/** \brief 다음 인자를 on/off 로. */
bool cmd_arg_onoff(bool *on);

/**
 * \brief 남은 줄 전체를 통째로. 공백을 포함한 문장을 받을 때 쓴다.
 * \return 남은 문자열. 없으면 NULL.
 */
char *cmd_arg_rest(void);
/** @} */

/**
 * \name 인자 split 매크로
 *
 * 참조 프로젝트의 DEC_SPLT / HEX_SPLT / STR_SPLT 와 같은 역할이다.
 * 인자가 없거나 형식이 틀리면 사용법을 출력하고 핸들러에서 곧바로
 * return 한다. 그래서 핸들러 본문에 if 문이 쌓이지 않는다.
 *
 *   static void cmd_led(uint32_t ch)
 *   {
 *       uint32_t num;
 *       bool     on;
 *
 *       DEC_SPLT(ch, "led", "num", num);      // 없으면 여기서 return
 *       ONOFF_SPLT(ch, "led", on);            // 없으면 여기서 return
 *       ...
 *   }
 */
/** @{ */
/** 다음 인자를 문자열로 꺼낸다. (out 은 char * 변수) */
#define STR_SPLT(ch, cmd, expected, out)                                      \
	do {                                                                      \
		(out) = cmd_arg_str();                                                \
		if ((out) == NULL) {                                                  \
			cmd_printf((ch), "%s: split err(%s)", (cmd), (expected));         \
			return;                                                           \
		}                                                                     \
	} while (0)

/** 다음 인자를 10진수로 꺼낸다. (out 은 uint32_t 변수) */
#define DEC_SPLT(ch, cmd, expected, out)                                      \
	do {                                                                      \
		if (!cmd_arg_dec(&(out))) {                                           \
			cmd_printf((ch), "%s: split err(%s)", (cmd), (expected));         \
			return;                                                           \
		}                                                                     \
	} while (0)

/** 다음 인자를 부호 있는 10진수로 꺼낸다. (out 은 int32_t 변수) */
#define SDEC_SPLT(ch, cmd, expected, out)                                     \
	do {                                                                      \
		if (!cmd_arg_sdec(&(out))) {                                          \
			cmd_printf((ch), "%s: split err(%s)", (cmd), (expected));         \
			return;                                                           \
		}                                                                     \
	} while (0)

/** 다음 인자를 16진수로 꺼낸다. (out 은 uint32_t 변수) */
#define HEX_SPLT(ch, cmd, expected, out)                                      \
	do {                                                                      \
		if (!cmd_arg_hex(&(out))) {                                           \
			cmd_printf((ch), "%s: split err(%s)", (cmd), (expected));         \
			return;                                                           \
		}                                                                     \
	} while (0)

/** 다음 인자를 on/off 로 꺼낸다. (out 은 bool 변수) */
#define ONOFF_SPLT(ch, cmd, out)                                              \
	do {                                                                      \
		if (!cmd_arg_onoff(&(out))) {                                         \
			cmd_printf((ch), "%s: split err(on/off)", (cmd));                 \
			return;                                                           \
		}                                                                     \
	} while (0)

/** 조건이 참이면 메시지를 출력하고 핸들러를 빠져나온다. */
#define RETURN_IF(ch, cond, ...)                                              \
	do {                                                                      \
		if (cond) {                                                           \
			cmd_printf((ch), __VA_ARGS__);                                    \
			return;                                                           \
		}                                                                     \
	} while (0)
/** @} */

#endif /* CMD_PARSER_H_ */
