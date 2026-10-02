# UART HAL

串口硬件抽象层，为上层提供与芯片无关的 UART 收发接口。上层只依赖 `platform/hal/uart.h`，不接触任何芯片寄存器或 SDK 驱动 API。

## 职责边界

负责：
- 引脚、波特率、校验位、停止位配置
- TX：DMA 异步发送
- RX：DMA 接收到调用方提供的缓冲区，完成后在 ISR 中回调

不负责：
- 数据的组包/解析（在 `debug/`、`hci` 等使用方）
- 接收缓冲策略与流控（由调用方持有 buffer 并决定大小）
- 中断优先级配置（在 `platform.c` 统一设置）

## 对外接口

| 函数 | 说明 | 调用上下文 |
|------|------|-----------|
| `hal_uart_register_task` | 初始化 UART，注册 RX/TX 回调 | task（仅 HAREWARE_INIT 阶段调用一次） |
| `hal_uart_send_data` | DMA 异步发送，返回即表示数据已交给 DMA | task |
| `hal_uart_set_receive_buffer` | 装载接收缓冲区，重新武装 RX DMA | task（通常在 RX 回调中重装） |

## 用法示例

```c
#include "platform/hal/uart.h"

static _u8 rxBuf[240];

static void app_rx(int len)        /* ISR 上下文，见下方注意事项 */
{
    /* 处理 rxBuf 前 len 字节，然后重新武装 RX */
    hal_uart_set_receive_buffer(rxBuf, sizeof(rxBuf));
}

void uart_init_once(void)
{
    hal_uart_register_task(HAL_UART_BAUDRATE_115200, app_rx, NULL,
                           HAL_UART_PARITY_NONE, HAL_UART_STOP_BIT_ONE);
    hal_uart_set_receive_buffer(rxBuf, sizeof(rxBuf));
    hal_uart_send_data((_u8 *)"hello", 5);
}
```

## 内部结构

- 抽象头：`platform/hal/uart.h` —— 枚举值是独立顺序值（0,1,2…），不使用芯片编码
- 实现：`platform/telink/TLSR9528/hal/uart.c` —— 通过 `parityMap[]`/`stopBitMap[]` 查表将抽象值映射为芯片值，芯片细节（引脚 PC6/PC7、DMA2/DMA3、UART0）全部固化在此文件
- 依赖（向下）：telink SDK driver（`uart_*`、`plic_*`）
- 被依赖（向上）：`debug/debug_port`、`hci`

## 注意事项

- **RX/TX 回调在 ISR 上下文执行**（`uart_irq_handler` → `PLIC_ISR_REGISTER`）：回调内不得阻塞、不得调用非 RAM_CODE 的大函数；建议只拷贝数据/置标志，重活留给任务
- RX 是一次性 DMA：`RXDONE` 后 DMA 停止，必须在回调里调用 `hal_uart_set_receive_buffer` 重新武装，否则后续数据丢失
- `hal_uart_send_data` 的 buffer 在 DMA 完成前必须保持有效；发送中再次调用会打断/冲突，上层需自行保证时序（如用 TX 回调 + BUSY 标志串行化）
- 本 HAL 头不得 include 任何芯片头文件；需要新增能力时，先在抽象层定义接口和独立枚举值，再在实现层映射

## 维护约定

- 修改引脚/DMA 通道：只改 `uart.c` 顶部的 `static` 常量
- 新增校验/停止位模式：先加 `hal_uart_parity_e`/`hal_uart_stopbit_e`，再补 `parityMap[]`/`stopBitMap[]` 两处
