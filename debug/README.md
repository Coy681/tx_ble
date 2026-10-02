# debug 子系统说明

调试子系统总览：模块划分、各调试方式的关系与选型、数据流、使用示例。

## 一、目录结构

```
debug/
├── debug.h          门面头文件：总开关 TX_DEBUG_ENABLE + 子开关联动
├── debug_port.c/h   传输层：ring buffer + tx/rx task + UART 链路
├── log/             日志：字符串与 hex 输出（log_str / log_hex）
├── gpio/            GPIO 翻转 tracing（16 个逻辑调试点 DBG_GPIO_n）
└── wave/            波形数据流（预留，wave.c/h 待实现）
```

## 二、架构与数据流

核心分层：**log / wave / gpio 是"生产者"，debug_port 是"传输层"，UART 是"物理链路"**。
所有字节输出统一走 debug_port，任何子模块不直接碰 UART。

```
        [生产者层]                    [传输层]                  [硬件]
LOG_STR / LOG_HEX ──> log_str/log_hex ─┐
                                       ├─> debug_port_write()
wave 数据         ──> wave ────────────┤       │
                                       │       ▼
DEBUG_GPIO_HIGH/LOW ──> (不经 port)    │   portOutputRb (32×240 ring buffer)
                                       │       │  tx_task_set_event(LOG_TX)
                                       │       ▼
                                       │   port_task_event_tx()  ──> hal_uart_send_data() ──> UART TX
                                       │
PC/host 数据 ──> UART RX ──> port_hardware_rx_irq()
                                       │
                                 portRbInput (4×240 ring buffer)
                                       │  tx_task_set_event(LOG_RX)
                                       ▼
                             port_task_event_rx() ──> portRxCb() 上层回调
```

要点：
- **异步解耦**：`debug_port_write()` 只把数据拷进 ring buffer 并置 task event，真正的 UART 发送在 `TX_TASK_ID_LOG_TX` 任务里完成，中断/协议栈上下文绝不被 UART 阻塞
- **块格式**：ring buffer 每个 block 为 `[4 字节长度][payload]`，长度头由 port 层统一添加/剥离，上层（log/wave/PC 端脚本）只面对纯 payload
- **流控**：ring buffer 满时 `debug_port_write()` 直接丢弃（返回不拷贝）——debug 数据允许丢，不允许阻塞业务，这是设计取舍
- **方向**：output 与 input 是两条独立链路（独立 buffer、独立 task），rx 侧经 `debug_port_rx_register()` 注册回调，在 LOG_RX 任务上下文执行（非中断）

## 三、各调试方式的关系与选型

| 方式 | 回答的问题 | 开销 | 何时用 |
|------|-----------|------|--------|
| **log**（LOG_STR/LOG_HEX） | "程序走到哪了、数据是什么" | 中（格式化+拷贝，占 UART 带宽） | 状态流转、打印收发包内容、错误定位 |
| **gpio**（DEBUG_GPIO_xxx） | "这段代码执行了多长时间、中断什么时候来" | 极低（一次查表+寄存器写，_RAM_CODE 级别） | ISR 时序测量、task 切换 trace、与逻辑分析仪/示波器联测 |
| **wave** | "连续数据流随时间的变化" | 中 | 传感器曲线、音频/RF 样点导出到 PC 画图（预留中） |
| **uart 环回/rx 回调** | "上位机和固件之间的命令通道" | 低 | host 下发配置、触发测试动作（走 portRxCb） |

选型口诀：
- 看数值/流程 → log；看时间/时序 → gpio；看趋势 → wave
- 中断上下文里只用 gpio（或确保 log 数据量极小），log 放任务上下文
- 三者可同时开，互不干扰：log/wave 共用 port 输出通道，gpio 是独立物理引脚

## 四、开关体系

两级开关，都在 `debug.h` 联动：

```
TX_DEBUG_ENABLE            总开关，0 时全部子模块关闭、port task 不注册
├── TX_DEBUG_LOG_ENABLE    log 宏展开为空
├── TX_DEBUG_GPIO_ENABLE   gpio 宏展开为空
└── TX_DEBUG_WAVE_ENABLE   wave 关闭
```

- 每个 .c 只判断自己的直接开关；总开关关闭时子开关被强制置 0
- 关闭后残留成本：LOG/GPIO 宏为空展开，零代码；port task 不注册，零 RAM

## 五、使用示例

```c
#include "debug/debug.h"

/* 1. log：打印状态与数据 */
LOG_STR(1, "adv started\r\n");
LOG_HEX(1, "rx pkt: ", pPkt->data, pPkt->len);

/* 2. gpio：测量一段代码耗时（示波器/逻辑分析仪看 DBG_GPIO_2） */
DEBUG_GPIO_HIGH(DBG_GPIO_2);
do_something();
DEBUG_GPIO_LOW(DBG_GPIO_2);

/* 3. 中断里打 trace 标记（每进一次 ISR 翻转一次） */
DEBUG_GPIO_TOGGLE(DBG_GPIO_3);

/* 4. wave：导出数据流（实现后） */
// wave_write(samples, len);

/* 5. host 命令通道：注册 rx 回收上位机数据 */
static void host_cmd_cb(_u8 *data, _u32 len) { /* parse cmd */ }
debug_port_rx_register(host_cmd_cb);
```

## 六、注意事项

1. **port 未初始化时调用安全**：`debug_port_write()` 检查 `portOutputRb.p == NULL`，task 注册前调用直接丢弃
2. **单块上限**：一次 write 最大 `DEBUG_PORT_OUTPUT_BUFFER_SIZE - 4`（236 字节），更大数据自行分块
3. **满即丢**：输出突发超过 UART 吞吐（1M baud ≈ 100KB/s）会丢后面的数据，高频 log 建议配合 gpio 降频
4. **rx 回调上下文**：在 LOG_RX task 中执行，可安全调用协议栈 API，但不要阻塞
5. **wave 子模块**：目前为空实现，接口规划沿用 log 的模式——提供 `wave_write()` 原语，数据经 debug_port 输出，PC 端脚本解析画图
6. **依赖方向**：debug.h → log/gpio/wave/port → hal_uart / system task；debug 不被任何子模块头 include，业务代码需要 debug 功能时 include `debug/debug.h`
