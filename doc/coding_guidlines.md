# CODING STANDARDS

大部分读者都应该有经验，阅读代码所花费的时间要远远超过编写代码所花费的时间，即便代码是你自己一行行写的，过了一段时间之后你都要重新花费时间熟悉。如果编程习惯不够好，变量/函数/逻辑的实现没有标准，随心所欲的实现功能，你再次熟悉的时候简直是灾难，你会埋怨过往的自己，为什么不好好实现这段代码呢？除此之外，在多人协作编程的时候，所有人遵守一套固定的编程规范更加重要，这直接决定了你项目的可维护性/可扩展性/可继承性（将你的项目交与他人维护的时候）

一套好的编程规范，重要性之高再如何过分形容都不为过，它可以

- 提高不同编程水平人的下限（只要遵守编程规范，架构/逻辑功底差的人写出的代码也不会很差）
- 增强系统的可扩展性（没有人希望每增加一个功能，整个代码就要重构）
- 增强系统的可维护性（你再也不用看着自己过往惨不忍睹的代码望而兴叹了）
- 增强系统的可继承性（别人只要熟悉了这套编程规范，就能很快熟悉你的系统架构和功能实现）
- 降低沟通成本（你在看同组别人实现的代码的时候，能够很快的切入别人的视角，熟悉相应功能）

本规范结合我过往的编程经验、编程类书籍总结，并参考 Zephyr / FreeRTOS / Linux Kernel / MISRA-C 的规范内容（只取其规定的**方面与思路**，具体条文全部按本工程习惯制定）。参考来源对应关系：

| 章节 | 主要参考 |
|------|----------|
| 分层与模块化 | Linux 内核（子系统目录结构）、Zephyr（subsystem 划分） |
| 命名与类型 | FreeRTOS（前缀编码）、MISRA（类型安全） |
| 函数/排版 | FreeRTOS、Linux kernel coding style |
| 断言与错误处理 | Zephyr（__ASSERT / 错误码约定）、MISRA |
| 并发与中断 | Zephyr（ISR 约束）、FreeRTOS（FromISR 模式） |
| 文档 | Linux kernel-doc、内核 Documentation 组织 |

---

# 第一部分 架构与组织（宏观层）

## 1. 总体原则

**这五条是规范的宪法，其余所有条款都是它们在某类问题上的展开：**

1. **分层只能向下依赖**：`service/debug/app → system → platform/hal`，任何模块不得 include 上层头文件。`platform/hal` 是芯片无关抽象，`platform/telink/<chip>` 是芯片实现，上层永远只 include 抽象头
2. **谁用资源，谁持有配置**：模块自己的板级引脚表（`debugPinMap`、`ledPinMap`）放在自己目录的 `.c` 文件里，不外泄引脚细节，上层只见逻辑索引
3. **抽象不泄漏实现细节**：HAL 头文件不得出现芯片专属枚举/寄存器/宏；枚举用独立顺序值，实现层查表映射（参考 `parityMap[]`）
4. **模块头不依赖 debug**：任何子模块头文件不得 include `debug.h`，依赖单向：`debug.h → log/gpio/wave/port`，用户代码只 include `debug.h`
5. **失败必须可见**：错误要么 ASSERT 炸出来，要么返回错误码，禁止静默吞掉（静默失效是嵌入式最难查的一类 bug）

## 2. 分层架构

### 2.1 层级图

```
+--------------------------------------------------+
|  app / tx_ble        应用逻辑                     |
+--------------------------------------------------+
|  service             板级业务服务(led...)         |
|  debug               调试子系统                   |
+--------------------------------------------------+
|  system              框架: task/scheduler/ble/wsm |
+--------------------------------------------------+
|  platform/hal        芯片无关硬件抽象             |
|  platform/telink     芯片实现(TLSR9528...)        |
+--------------------------------------------------+
|  common              通用组件: assert/bit/rb/mmu  |
|  boot                启动代码                     |
+--------------------------------------------------+
```

### 2.2 依赖规则

| 目录 | 职责 | 允许依赖 | 禁止 |
|------|------|----------|------|
| `boot/` | 启动、中断向量 | 无 | 一切 |
| `common/` | assert、bit、mmu/rb 等纯软件组件 | common | 上层任何头 |
| `platform/hal/` | 芯片无关硬件抽象 | common | 芯片 SDK 头 |
| `platform/telink/<chip>/` | 芯片实现 | platform/hal, common | 上层头 |
| `system/` | 框架（task/scheduler/ble） | system, platform | service/debug/app |
| `service/` | 板级业务服务 | service, system, platform | app |
| `debug/` | 调试子系统 | debug, system, platform | service/app |
| `app/` | 应用逻辑 | 任意下层 | — |

### 2.3 架构演进规则

- **换芯片** = 只写 `platform/telink/<newchip>/`，hal 抽象头零修改；若抽象头需要加能力，先改抽象层定义再补实现
- **换板** = 只改各模块 `.c` 里的板级配置表；出现第二块板时统一抽到 `board/<name>/board_config.c`，模块代码零修改
- **加模块** = 新建目录（三件套：`.c/.h/README.md`）+ 构建配置注册，不改动任何现有模块
- HAL 新增能力流程：`platform/hal/xxx.h` 定义接口与独立枚举 → 实现层查表映射 → 调用点迁移。**禁止上层直接调芯片 SDK 函数绕过 HAL**

## 3. 模块化

### 3.1 模块三件套

每个模块 = `xxx.h`（对外契约）+ `xxx.c`（实现）+ `README.md`（职责文档）。头文件即文档：看头文件就该会用这个模块。

### 3.2 头文件即接口

- debug.h 这类**门面头只做聚合**：开关联动 + include 子模块头，不含任何实现内容——门面管"别人能用什么"，.c 管"怎么实现"
- 门面头（debug.h）是唯一出现总开关的地方；子模块 `.c` 只判断自己的直接开关，不重复判断总开关（联动由门面保证）
- 子模块头不 include 门面头，依赖只能从门面向下

### 3.3 资源与所有权

- 共享资源（pin、DMA 通道、任务 ID）由使用方向框架**申请-持有-释放**，不预定义静态分配表
- 每个模块的 init 只做：资源申请、回调注册、状态复位；不阻塞、不等待
- 用 initcall 层级注册（`txAttribute.h`）：`HAREWARE_INIT`(硬件) → `ARCH_INIT`(架构) → `TASK_INIT`(任务/服务) → `APP_INIT`(应用)，禁止手工调用各模块 init

### 3.4 模块文档（问题记录模式）

每个 LL/框架模块配 `xxx.md`（参考 `adv.md`），问题记录固定四段式：

```markdown
### Issue record-YYYYMMDD
**问题现象:** ...
**问题分析:** ...（尝试路径，排除项）
**问题定位:** ...（根因）
**问题解决:** ...（改了什么）
```

架构图用 drawio 放同目录（`adv.drawio`、`sche.drawio`），导出图放 `doc/picture/` 镜像路径。

---

# 第二部分 代码级规范（微观层）

## 4. 文件与目录

### 4.1 文件命名

- 全小写单词，下划线分隔：`debug_port.c`、`txAssert.c`（历史遗留除外）
- 一个模块一对 `.c/.h`，同名放置
- 每个模块目录一个 `README.md`：职责边界、对外接口表（含调用上下文）、用法示例、内部依赖、注意事项（参考 `platform/hal/README.md`）

### 4.2 文件组织顺序

`.c` 文件内容按固定顺序排布，任何人打开文件都知道去哪找什么：

```
1. 文件头注释
2. include
3. 宏定义（私有）
4. 类型定义（私有）
5. 静态变量
6. 静态函数（工具函数 → 业务函数）
7. 对外函数
8. initcall 注册
```

### 4.3 文件头

```c
/*
 * uart.h
 *
 *  Created on: 2024年12月6日
 *      Author: Admin
 *
 *  Generic gpio abstraction: logical pin group driven by a caller-supplied
 *  pin map. No debug/led policy here, callers (debug/gpio, system/led)
 *  own their pin maps.
 */
```

头注释必须包含模块一句话职责说明（第 5 行起），说明"这是什么、边界在哪"。

## 5. 命名规范

### 5.1 总表

| 对象 | 风格 | 示例 |
|------|------|------|
| 对外函数 | `模块前缀_动宾` 小写下划线 | `hal_uart_init`、`debug_gpio_toggle`、`log_str` |
| static 函数 | `模块前缀_动宾` 或省前缀 | `port_input_init`、`debug_gpio_init` |
| 对外类型/枚举 | `tx_` 或模块前缀 + `_t/_e` | `tx_rb_t`、`hal_uart_parity_e` |
| 枚举成员 | `前缀_全大写` | `HAL_UART_PARITY_NONE`、`HAL_UART_BAUDRATE_115200` |
| 宏常量 | `模块前缀_全大写` | `DEBUG_GPIO_NUM`、`PORT_STATUS_BUSY` |
| 全局变量 | 小驼峰，模块前缀开头 | `portRbInput`、`portOutputRb`、`hal_uart_rx_cb` |
| static 变量 | 小驼峰 | `debugPinMap`、`halGroup`、`ledPinMap` |
| 局部变量 | 小驼峰 | `dataLen`、`buf`、`rxBuf` |
| 回调类型 | `..._cb_f` / `..._task` | `debug_port_rx_cb_f`、`hal_uart_rx_task` |

### 5.2 命名语义要求

- **函数名 = 模块 + 动作**：`hal_gpio_set_high` 而非 `gpioHigh`；同一模块的同类函数保持动词组一致（`high/low/toggle` 三件套）
- **布尔量用 is/has/enable 前缀或状态词**：`isEmpty`、`txBusy`
- **长度/数量后缀**：`...Len`、`...Num`、`...NumMax`（`DEBUG_PORT_OUTPUT_BUFFER_SIZE`、`HAL_GPIO_PIN_NUM_MAX`）
- **不用无意义缩写**：`dataLen` 优于 `dl`；历史 SDK 类型 `_u8/_u32` 保留使用
- **映射表命名**：`xxxMap`（`parityMap`、`debugPinMap`），板级配置注释标注 `/* board config: ... */`
- **拼写必须正确**（存量错误逐步修正）：`serach→search`、`ahchor→anchor`、`Reveive→Receive`、`messsage→message`、`currenTime→currentTime`——搜索是代码维护的基本操作，拼写错误让符号搜不到

### 5.3 类型定义

- 枚举成员显式赋值当且仅当值有协议/硬件含义；纯顺序枚举不赋值（`hal_uart_parity_e`）
- 函数指针 typedef 单独一行，星号贴类型名：`typedef void(*debug_port_rx_cb_f)(_u8* data,_u32 dataLen);`

## 6. 类型与可移植性

参考 FreeRTOS"自定义类型屏蔽编译器差异"的思路，本工程的规则：

1. **接口签名禁止裸 `int/short/long` 表示数据宽度**：定宽数据一律 `_u8/_u16/_u32/_s32`（telink SDK 类型）；只有"任意整数"场景（如 `hal_gpio_init` 返回值）可用 int
2. **指针宽度量用 `_u32` 转**（RISC-V 32 位平台），地址入 message 传递时按 `_u32` 拆字节（参考 sch.c 注释样例）
3. **位域仅用于压缩存储**（`_u32 period:24; _u32 type:8;`），位域成员不可取地址，跨字节序的协议结构慎用位域
4. **packed 结构体**：定时/协议相关必须 packed；telink 工具链用 `typedef struct _PACKED xxx_t`，通用代码用 `__attribute__((packed))`，同一文件内统一其一
5. **结构体成员按大小降序**减少 padding，保留字段命名 `rsvd`
6. **不使用浮点**（无 FPU 场景），定点运算注释标明 Q 格式

## 7. 函数

### 7.1 布局

```c
void debug_gpio_toggle(_u8 n)
{
    ASSERT(n < DEBUG_GPIO_NUM);
    if (halGroup >= 0)
    {
        hal_gpio_toggle(halGroup, n);
    }
}
```

- **大括号**：函数体、所有 if/else/for/while **一律换行加大括号**（Allman 局部风格），即使只有一条语句。禁止 `if (x) do_something();` 无括号单行——这是 gotofail 类事故的根源
- **唯一例外**：守卫与语句同一物理行可接受，如 `if (!ptr) return;`
- **数组初始化**：`{` 跟在行尾（K&R），顺序列表按语义分组换行；按下标对位的表每行一个元素并带行尾逗号：

```c
/* 顺序列表：分组换行 */
static const int debugPinMap[DEBUG_GPIO_NUM] = {
    GPIO_PB0, GPIO_PB1, GPIO_PB2, GPIO_PB3,
    GPIO_PB4, GPIO_PB5, GPIO_PB6, GPIO_PB7,
    GPIO_PC0, GPIO_PC3,
    GPIO_PE0, GPIO_PE1, GPIO_PE2, GPIO_PE3, GPIO_PE4, GPIO_PE5
};

/* 对位表：每行一个，行尾逗号 */
static const uart_parity_e parityMap[] = {
    [HAL_UART_PARITY_NONE] = UART_PARITY_NONE,
    [HAL_UART_PARITY_EVEN] = UART_PARITY_EVEN,
    [HAL_UART_PARITY_ODD]  = UART_PARITY_ODD,
};
```

- **逗号后加空格**：`hal_gpio_set_high(halGroup, n)`（新增代码执行；存量代码逐步统一）
- **缩进**：Tab（延续工程现状），`case` 与 `switch` 对齐或缩进一级均可，同文件保持一致
- **单行长度**：不硬性限制，但函数签名过长时参数换行并对齐（参考 `hal_uart_register_task`），`\` 续行符对齐
- **一个函数一件事**，超过一屏（约 60 行）考虑拆分
- **静态内部函数放使用点之前**，避免前置声明；确需前置声明的在文件头集中声明
- **禁止 goto 跨初始化跳转**；错误清理路径统一用单一出口或 goto cleanup（同一函数内保持一致）

### 7.2 函数设计

- **参数防御**：对外函数入口做参数校验，越界/空指针用 `ASSERT` 炸出来而不是静默返回：

```c
void debug_gpio_high(_u8 n)
{
    ASSERT(n < DEBUG_GPIO_NUM);   /* 越界必须暴露，不能静默写错脚 */
    ...
}
```

- **init 结果必须断言**：模块 init 依赖的底层初始化失败时 `ASSERT(ret >= 0)`，禁止静默失效
- **ISR 函数**：加 `_RAM_CODE` 前缀（`_RAM_CODE static void port_hardware_rx_irq`），回调内只做搬数据/置事件，不调用阻塞和非 RAM_CODE 逻辑
- **状态机/任务函数**：统一签名 `_u32 xxx_event(_u16 taskId, _u32 event)`，返回值 `event ^ 处理掉的事件位`（参考 `port_task_event_rx`）

## 8. 变量与常量

- **能 static 就 static**：模块内变量一律 `static`，全局可见面最小化；确需跨文件的全局变量在头文件 extern 声明并在注释说明
- **常量表 const**：pin 表、映射表、字符串表一律 `static const`
- **指针风格**：`_u8* p`（星号贴类型，延续现状）；`const` 修饰输入参数的指针和内容
- **局部变量在函数头集中声明**（嵌入式 C89 习惯），循环变量可在 for 内声明（`for(_u32 i = 0; ...)`）
- **volatile**：ISR 与任务共享的标志必须 volatile（`volatile static _u8`）
- **魔法数字**：一律具名宏；缓冲区大小、事件位、状态位都要定义在头文件顶部
- **单赋值点**：一个变量一个语义，禁止同一个变量在函数内先当长度再当标志复用

## 9. 宏与预处理器

参考 MISRA 对预处理器约束的思路：

- **函数式宏必须 `do { } while(0)` 包裹**，参数加括号（`LOG_STR` 是样板）：

```c
#define LOG_HEX(EN,STR,DATA,LEN)    do { \
                                        if (EN) { log_hex((const char *)(STR),(const _u8 *)(DATA),(_u32)(LEN));} \
                                    } while (0);
```

- **功能开关宏**：模块开关包实现（`#if(TX_DEBUG_GPIO_ENABLE) ... #endif//TX_DEBUG_GPIO_ENABLE`），关闭后对外接口退化为空宏，保证零开销可编译
- **宏不定义在 .c 中对外暴露**；有取值语义的常量优先用枚举（编译器可查类型）
- **带参数的宏禁止有副作用参数复用**（如 `MAX(a++, b)` 类陷阱），需要时改 inline 函数
- **`#endif` 后一律标注**对应条件宏

## 10. 注释与文档

### 10.1 对外 API：doxygen 块

```c
/**
 * @brief     This function is used to init uart and register rx/tx irq callbacks.
 * @param[in] baudrate - uart baudrate.
 * @param[in] rxCb - callback invoked in uart irq when rx dma done.
 * @return    none
 */
```

- `@brief` 一句话说清用途；`@param[in]` 逐参数说明，带语义而非复述类型
- 说明**调用上下文**（task / ISR / 任意）当存在约束时，如 "callback invoked in debug task when rx data arrives"
- 有返回值时说明含义与错误值（`-1 on error`）

### 10.2 普通注释

- 段落分隔用等号线：`/******************define port buffer*************************/`
- 行尾注释双斜杠：`len = DEBUG_PORT_OUTPUT_BUFFER_SIZE - 7;//defensive programing`
- 注释解释**为什么**，不复述代码；板级配置必须注释 `/* board config: which physical pins are debug pins */`
- 每个功能段（一组相关函数）前用分隔注释标出小节名，如 `port output process`
- **复杂分支逻辑必须画图注释**：sch.c 的 Case A~F ASCII 时序图是本工程样板——算法分支先画图再写码，图留在代码里

## 11. 错误处理与断言

### 11.1 状态码

- 每个对外模块定义自己的状态枚举，禁用裸 int 魔数：`sch_status_e`、`planner_ret_e`、`txTask_e`、`txMessage_e`
- 枚举成员显式赋值当有协议含义（`SCH_STATUS_SUCCESS = 0x00`）
- "未找到"是合法返回值不是错误（`PLANNER_NOT_FOUND`、`sch_remove_task` 返回 0），注释里区分**拒绝**与**不存在**

### 11.2 断言（ASSERT）

参考 Zephyr `__ASSERT` 的分级思路：

| 场景 | 手段 | 示例 |
|------|------|------|
| 编程错误（不该发生的参数/状态） | `ASSERT` 炸出来，release 零开销 | `ASSERT(n < DEBUG_GPIO_NUM)` |
| 运行时可恢复的失败（资源忙、队列满） | 返回错误码，调用方处理 | `SCH_STATUS_REJECTED` |
| 不可恢复的系统错误 | tx_assert_handler 打印后停机 | ASSERT 内部路径 |

- 断言条件必须是**无副作用的表达式**（`ASSERT(n++)` 禁止——release 版会丢副作用）
- 防御宏判空：指针/句柄校验不用裸 `!= NULL`，用具名宏（见 12.1）

## 12. 并发与中断（重点）

### 12.1 防御宏（Validity Guard Macros）

```c
#define TASK_VALID(task)        ((task)!=NULL)
#define TASK_NOT_VALID(task)    ((task)==NULL)
#define POINTER_VALID(p)        /* in txCommon.h */
#define NODE_VALID(node)        (node!=NULL)   /* planner.c 局部定义 */
```

规则：**每个模块可以用通用宏（TASK_/POINTER_），模块私有的在模块 `.c` 顶部定义（NODE_）**；同一工程宏语义必须一致（都是"判非空"），禁止同名异义。

### 12.2 时间比较（回绕安全）

所有时间比较必须走 `txCompareTime()`，禁止直接 `>`/`<` 比较 tick——32 位 us tick 会回绕：

```c
if (txCompareTime(currentTime, startTime))   /* currentTime >= startTime（回绕安全） */
```

时间量单位一律 us，变量名带语义后缀（`startTime`、`durationMin`、`stopLatency`），时间常量在宏里带单位注释（`#define TASK_SCH_PROCESS_TIME  30  //us`）。

### 12.3 临界区

- 成对使用 `IRQ_DISABLE; ... IRQ_RESTORE;`（临界区短，只包链表操作，不包回调调用）
- 有返回值的局部关中断用 `irq_disable()/irq_restore(r)`（planner 模式），`r` 必须配对恢复
- **回调/上层通知放在临界区外**，避免在关中断状态下执行用户代码
- ISR 与可重入函数标注 `_RAM_CODE`，且内部不得调用非 `_RAM_CODE` 的大函数
- **ISR 路径约束**（参考 FreeRTOS FromISR 隔离思想）：ISR 内禁止 malloc、禁止 VLA、禁止阻塞、栈上变量合计不超百字节级；与任务的数据交接一律走 ring buffer + 事件

### 12.4 回调与函数指针签名

框架回调统一"类型+id"或"事件+任务"模式，全工程一致：

```c
typedef void(*sch_cb_f)(_u8 type, _u8 id);              /* 调度器回调：事件类型 + 任务 id */
typedef _u32(*pTaskProcess_f)(_u16 taskId, _u32 event); /* 任务处理：返回 event^已处理位 */
typedef void(*planner_cb_f)(_u16);                       /* 参数变更广播 */
```

- 任务事件处理函数**必须**返回"清除掉已处理事件位后的 event"，漏返回会导致事件重复处理或死循环
- 事件位用 `BIT(n)` 定义，保留 `TX_TASK_EVENT_MESSAGE = BIT(31)` 作为框架消息位，模块自定义事件从 BIT(0) 向上且不得越入 BIT(31)
- 回调枚举先定义事件集（`sch_callback_e`：START/STOP/CANCELED/PASSED），回调内按事件分支，不猜上下文

### 12.5 任务间通信

- 只用既有框架：`tx_task_set_event` / `tx_message_receive` / ring buffer，不自旋锁、不忙等
- 事件位、任务 ID 在头文件用宏定义，命名 `XXX_TASK_EVENT_YYY`、`TX_TASK_ID_XXX`
- 共享数据的所有权明确：buffer 由发送方填、接收方读后重装（`hal_uart_set_receive_buffer` 模式），杜绝双写者

## 13. 数据结构与算法

- **头结点/控制块命名**：`xxx_ctrl_t` 全局唯一（`schCtrl`、`gPlannerCtrl`），链表节点 `xxx_node_t`，链表指针成员统一叫 `next`
- **动态数组长度必须在受控范围**：VLA（`sch_map_slot_t blockList[mapNodeCount]`）只允许在输入可控的算法层使用，且要评估最坏栈深；ISR 路径禁止 VLA 和大栈局部变量
- **链表操作三件套**拆成独立 static 函数复用：`xxx_delete_node_from_list`、`xxx_serach_node_in_list`、`xxx_insert_to_list`（见 sch.c），禁止在每个 API 里内联重写遍历
- 插入保持链表**有序**（按 start time），删除反向遍历用 prev 指针，头结点删除单独处理 `*list = (*list)->next`
- 排序用简单插入/冒泡即可（n 小），但比较函数必须回绕安全
- 动态内存只在 init/配置路径使用（`tx_malloc`），运行时热路径禁动态分配（碎片与不确定延迟）

## 14. 测试代码组织

- 每模块一个 `test.c`（`adv/test.c`、`conn/test.c`…），测试桩/压力脚本只住 test.c
- **主逻辑文件不留注释掉的实验代码**（sch.c 尾部 120 行注释掉的 task1/2/3 必须清理）；确要保留的实验放 test.c 或 git 历史
- 临时调试变量（如 planner.c 的 `AAA_MAP[]`）调试完必须删除，不留 `volatile _u8 AAA_MAP` 这类遗骸
- 关键算法（调度冲突判定、map 计算）的测试样例留在 test.c，作为回归基线

---

# 第三部分 工程级规范

## 15. 构建与配置

- 新增顶层目录：`.cproject` 的 `<sourceEntries>` + `cmake_configs/*_cmake.json` 的 `directories` + `CMakeLists.txt` 的 `SOURCE_DIR_LIST` 三处同步（前两者是源头，CMake 追加项是免疫插件再生成的保险）
- 子模块的功能开关统一收口在模块总头（如 `debug.h`）：总开关关闭时强制子开关为 0，子开关 `#ifndef` 兜底默认值；子模块 `.c` 只判断自己的直接开关
- **配置集中**（FreeRTOSConfig.h 模式）：全工程配置唯一入口是 `config.h`，模块内部不得私藏 `#define` 配置开关（调试宏除外）
- 关闭开关后代码必须可编译且零开销
- RAM 函数（`_RAM_CODE`）、保留内存段等链接属性在代码中标注，不在链接脚本里隐式处理

## 16. 版本控制与协作

- **一个 commit 一件事**：修 bug 的提交不夹带格式化，格式化单独提交
- commit message 说明**为什么**（问题现象/根因），不只是"fix bug"
- 提交前自查：编译零警告（-Wall 级别）、命名拼写、`#endif` 标注、无调试遗骸
- 新目录/新文件提交必须同时包含：三件套、README.md、构建配置三处注册——缺一即是破坏构建的提交

## 17. 评审检查单（Code Review Checklist）

提交/评审时按此清单过一遍（按章节索引）：

- [ ] 依赖方向正确，没有 include 上层头（§1/§2）
- [ ] 新模块有三件套，README 覆盖职责边界（§3/§4）
- [ ] 命名符合总表，拼写正确（§5）
- [ ] 所有 if/for 有大括号，数组初始化风格正确（§7）
- [ ] 魔法数字已具名，常量已 const（§8）
- [ ] 宏参数有括号，开关可整体关闭（§9）
- [ ] 对外 API 有 doxygen，复杂分支有图（§10）
- [ ] 越界/空指针有 ASSERT，错误码用枚举（§11）
- [ ] 时间比较走 txCompareTime，临界区不包回调（§12）
- [ ] ISR 内无阻塞/无大栈（§12.3）
- [ ] 无注释掉的实验代码、无调试遗骸（§13/§14）
- [ ] 构建配置三处同步（§15）
