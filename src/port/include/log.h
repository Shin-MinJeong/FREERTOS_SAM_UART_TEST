/*
 * log.h
 *
 * 진단 출력 단일 창구.
 *
 * 리팩토링 이전에는 모듈마다 printf 형식이 달랐다("\n" vs "\r\n",
 * 태그 있음/없음). 모든 출력을 이 매크로로 통일해 터미널 로그를
 * 기계적으로 파싱/필터링할 수 있게 한다.
 *
 * 사용법:  LOG_I("TOUCH", "report id base = %d", base);
 *          -> "[TOUCH] report id base = 1\r\n"
 */

#ifndef PORT_LOG_H_
#define PORT_LOG_H_

#include <stdio.h>

#define LOG_LEVEL_NONE   0
#define LOG_LEVEL_ERROR  1
#define LOG_LEVEL_WARN   2
#define LOG_LEVEL_INFO   3

/** 빌드 시 -DLOG_LEVEL=... 로 덮어쓸 수 있다. */
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#define LOG_RAW(prefix, tag, fmt, ...) \
	printf(prefix "[" tag "] " fmt "\r\n", ##__VA_ARGS__)

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_I(tag, fmt, ...) LOG_RAW("", tag, fmt, ##__VA_ARGS__)
#else
#define LOG_I(tag, fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_W(tag, fmt, ...) LOG_RAW("W:", tag, fmt, ##__VA_ARGS__)
#else
#define LOG_W(tag, fmt, ...) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_E(tag, fmt, ...) LOG_RAW("E:", tag, fmt, ##__VA_ARGS__)
#else
#define LOG_E(tag, fmt, ...) ((void)0)
#endif

#endif /* PORT_LOG_H_ */
