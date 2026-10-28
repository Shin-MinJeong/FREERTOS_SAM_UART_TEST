# FREERTOS_SAM_UART_TEST

SAMV71 Xplained Ultra + ILI9488 + maXTouch 기반 **UART ASCII 명령 테스트** 프로젝트.

## 목표 기능

- LVGL 버튼을 누르면 PC 로 ASCII 명령 전송
- PC 에서 ASCII 명령을 보내면 MCU 가 반응
- 양방향 모두 콘솔 로그(`LOG_*`)로 검증

## 배선 — 지금은 EDBG USB 하나로 씁니다

`src/bsp/include/bsp_uart_cfg.h` 의 `BSP_UART_USE_CONSOLE_PORT` 스위치로 고릅니다.

| 값 | 프로토콜 포트 | 핀 | 필요한 연결 |
|---|---|---|---|
| **1 (현재)** | **USART1** | PA21(RX) / PB4(TX) | 보드 DEBUG USB 하나. 로그와 같은 스트림 |
| 0 (원래 설계) | USART0 | PB0(RX) / PB1(TX) | USB-Serial 어댑터를 따로 연결 |

콘솔 로그(`printf` / `LOG_*`)는 어느 쪽이든 항상 USART1(EDBG)로 나갑니다.
둘 다 **115200 8N1**.

원래는 `LOG_*` 출력과 ASCII 프로토콜이 섞이지 않게 포트를 나눈 설계였지만,
USB-Serial 어댑터가 없으면 USART0 은 물리적으로 아무 데도 연결되지 않아
PC 에서 보낸 명령이 그대로 버려집니다. 그래서 어댑터가 생길 때까지는
스위치를 `1` 로 두고 EDBG 포트 하나를 공유합니다.

> **대가:** 프로토콜 응답(`PONG`)과 로그 줄(`[COMM] RX> PING`)이 한 터미널에
> 섞여 나옵니다. 사람이 보기에는 오히려 편하지만, PC 쪽에서 자동 파싱할 때는
> `[TAG] ` 로 시작하는 줄을 걸러내야 합니다.
> 어댑터가 생기면 스위치만 `0` 으로 되돌리면 됩니다 — 다른 코드는 그대로입니다.

> ⚠️ **PIOC13 은 LCD 패널과 maXTouch 의 리셋 핀을 공유합니다.**
> 그래서 `app_boot.c` 의 초기화 순서(터치 → 디스플레이)를 바꾸면 화면이 백지가 됩니다.
> 자세한 내용은 `bsp_board.h` 의 `bsp_board_shared_reset()` 주석 참조.

## 계층 구조

```
app   애플리케이션 로직 (하드웨어를 모름)
 ↓
port  LVGL / FreeRTOS 어댑터 + 공통 유틸
 ↓
bsp   하드웨어 직결
 ↓
ASF / lvgl / config   (벤더)
```

모든 `.c` 는 `src/main.h` 하나만 포함합니다. 새 모듈을 추가할 때는
`main.h` 의 해당 계층 구역에 `#include` 한 줄만 넣으면 됩니다.

## 데이터 흐름

```
[버튼]  screen_console ──on_send──> app_ui ──> app_comm_send ──> bsp_uart (프로토콜 UART TX) ──> PC

[수신]  PC ──> 프로토콜 UART RX 인터럽트 ──> 링버퍼 ──> app_comm(수신 태스크)
              ──> bsp_uart_read_line() ──> cmd_parser_handle_line()
              ──> 명령 핸들러 ──reply──> bsp_uart (TX) ──> PC
```

`screen_console` 은 UART 를 모르고, `cmd_parser` 는 UART 와 화면을 모릅니다.
연결은 `app_ui.c` 와 `app_comm.c` 두 곳에서만 합니다.

## 지금 상태

**동작하는 것**
- 부팅, 클럭, 콘솔 로그, LCD, 터치, LVGL
- 프로토콜 UART 초기화 / 인터럽트 수신 / 링버퍼 / 줄 단위 조립 / 송신
- 명령 토큰화 + 테이블 디스패치 + 답신 경로
- 버튼 1개(`PING`)와 화면 로그 영역
- `PING` 명령 → `PONG` 답신 (양방향 왕복 확인용)

**채워야 하는 것 (`[TODO]` 주석 위치)**
| 파일 | 할 일 |
|---|---|
| `src/app/cmd_parser.c` | `COMMAND_TABLE` 에 명령 추가 (`LED`, `ECHO`, `VER` 예시가 주석으로 있음) |
| `src/app/screen_console.c` | `BUTTON_TABLE` 에 버튼 추가 |
| `src/app/app_comm.c` | 수신 내용을 화면에도 표시하려면 `screen_console_log()` 호출 |

명령을 하나 추가하려면 핸들러 함수 하나 + 테이블 한 줄이면 끝입니다.
버튼도 테이블 한 줄이면 됩니다 (이벤트 콜백을 새로 만들 필요 없음).

## 테스트 방법

1. 빌드 후 플래싱
2. EDBG 가상 COM 을 터미널에서 115200 8N1 로 열기 (**딱 하나만** 열면 됩니다)
3. 부팅 로그에 `[BOOT] console=USART1(EDBG)  protocol=USART1 (EDBG USB, PA21/PB4) - 로그와 공유`
   와 `[UART] ... ready @ 115200 baud` 가 보이면 포트 설정이 맞은 것입니다
4. **MCU → PC**: 화면의 `PING` 버튼을 누르면
   ```
   [UI] button 'PING'
   [COMM] TX< PING
   PING              ← 프로토콜 바이트 (로그가 아님)
   ```
5. **PC → MCU**: 터미널에 `PING` + Enter
   ```
   [COMM] RX> PING
   [CMD] exec 'PING'
   [COMM] TX< PONG
   PONG
   ```
   `[COMM] RX>` 줄이 안 뜨면 수신 경로가 죽은 것이고,
   뜨는데 `exec` 가 없으면 파서/명령 테이블 문제입니다 — 여기서 층이 갈립니다.

> 터미널이 입력을 에코하지 않습니다(MCU 가 에코를 안 보냄). 타이핑이 안 보여도
> 정상입니다. 줄 끝은 CR/LF/CRLF 아무거나 됩니다.
