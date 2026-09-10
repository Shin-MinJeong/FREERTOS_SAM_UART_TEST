/*
 * app_boot.h
 *
 * 부팅 시퀀스
 */

#ifndef APP_BOOT_H_
#define APP_BOOT_H_

/**
 * \brief 시스템을 초기화하고 스케줄러를 시작한다.
 *
 * 정상 동작 중에는 반환하지 않는다. 
 * 반환했다면 스케줄러를 시작할 메모리가 부족했다는 뜻
 */
void app_boot_run(void);

#endif /* APP_BOOT_H_ */
