/*
 * app_diag.h
 *
 * 진단용 태스크 (LED 점멸, 태스크 상태 리포트).
 */

#ifndef APP_DIAG_H_
#define APP_DIAG_H_

#include <stdbool.h>

/**
 * \brief 진단 태스크들을 만든다.
 *
 * APP_ENABLE_DIAG_TASKS 가 0 이면 아무것도 만들지 않고 true 를
 * 반환한다.
 *
 * \return 모든 태스크 생성에 성공했으면 true
 */
bool app_diag_start(void);

#endif /* APP_DIAG_H_ */
