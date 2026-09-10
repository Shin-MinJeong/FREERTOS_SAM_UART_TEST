/*
 * ui_theme.h
 *
 * 화면 공통 색/치수.
 *
 */

#ifndef UI_THEME_H_
#define UI_THEME_H_

/** \name 색 (RGB888) */
/** @{ */
#define UI_COLOR_LIST_BG   0xE0E0E0 /**< 목록 화면 배경 (밝은 회색) */
#define UI_COLOR_VIEW_BG   0x202020 /**< 뷰어 화면 배경 (어두운 회색) */
#define UI_COLOR_VIEW_TEXT 0xFFFFFF /**< 어두운 배경 위의 글자 */
/** @} */

/** \name 치수 (픽셀) */
/** @{ */
#define UI_MARGIN        10 /**< 화면 가장자리 여백 */
#define UI_BUTTON_WIDTH  100
#define UI_BUTTON_HEIGHT 45
/** @} */

#endif /* UI_THEME_H_ */
