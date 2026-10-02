# 技术文档与架构图学习参考

学习写技术文档、架构图、模块设计图的参考资源索引，针对本 SDK（BLE controller / scheduler / HAL 分层）筛选。

## 一、文档 + 架构图都值得抄的

### 1. Zephyr（最贴近本 SDK，首选）

在线文档站：**https://docs.zephyrproject.org/latest/**

| 内容 | 链接 |
|------|------|
| **Bluetooth 架构图**（Host/Controller/HCI 分层，最值得抄） | https://docs.zephyrproject.org/latest/connectivity/bluetooth/index.html |
| **内核服务总览**（调度/线程/中断，对应 system/task/scheduler） | https://docs.zephyrproject.org/latest/kernel/index.html |
| **外设驱动 API 文档**（UART/GPIO 的 HAL 说明书标准答案） | https://docs.zephyrproject.org/latest/peripherals/index.html |
| **Board Porting 指南**（HAL/芯片适配，对应换芯片演进） | https://docs.zephyrproject.org/latest/hardware/porting/index.html |
| 架构总图（分层全景） | 首页 Introduction 的"Zephyr architecture"框图 |

源码仓：**https://github.com/zephyrproject-rtos/zephyr**，文档在 `doc/`：

- `doc/connectivity/bluetooth/` — 蓝牙文档源码（rst + 图片）
- `doc/hardware/peripherals/uart.rst`、`gpio.rst` — **强烈推荐**，一页纸说清 API 表 + 使用示例 + 时序约束，是 `platform/hal/README.md` 要对齐的格式
- `doc/development/` — 开发流程、编码规范相关
- 图源文件多为 `dot`/`svg`，在对应目录 `images/` 子目录

Zephyr 文档套路：**每章开头一张图 + 职责一句话 + 数据流图**，没有废话。

### 2. Linux 内核 Documentation

- `Documentation/driver-api/`：gpio、dma、串口子系统的文档是"HAL 层说明书"的标准答案——接口约定表、调用上下文、生命周期图
- `Documentation/process/howto.rst`：文档分层（用户手册/API 文档/内核内幕），对应本工程 README.md / doxygen / 问题记录四段式
- kernel-doc 注释格式已在 coding_guidlines.md 第 10 章采用

## 二、协议/状态机图（BLE LL 层关键）

### 3. 蓝牙 Core Spec

Core Vol 6 Part B 的状态图是 drawio 仿画最佳对象：

- 状态机图（idle/advertising/scanning/connecting 转移）
- 时序图（conn event 结构）

本工程 `doc/drawio/module/adv/sm.drawio` 已在走这条路，格式对齐它。

### 4. Nordic InfoCenter / TI SimpleLink 文档

商业 SDK 文档的工业级范本：每个 LL 模块一页"概述图 + 时序图 + API 表"，比 Zephyr 更面向使用者，适合抄 README.md 的写法。

## 三、图工具与方法论

### C4 model（c4model.com）

4 层图法，一张图只回答一个层级的问题：

| 层 | 回答的问题 | 本工程对应 |
|----|-----------|-----------|
| C1 Context | 系统边界、与外部交互 | stack_arch.drawio 的最外层 |
| C2 Container | 模块划分、依赖方向 | 分层架构图（coding_guidlines §2.1） |
| C3 Component | 模块内部构成 | adv.drawio、sche.drawio |
| C4 Code | 类/函数级 | 时序图、状态机图 |

一张图别想塞下所有东西。

### UML 时序图

`doc/drawio/sch/timing/` 六种 Case 时序图已是此路子，A~F 命名法保持。

## 四、落地三原则

1. **每图一议**：一张图只回答一个问题（数据流向？调用时序？状态转移？），现有 drawio 按 malloc_header/middle/tail 拆分的粒度是对的，保持
2. **图随代码走**：drawio 放模块目录（`adv.drawio` 与 `adv.c` 同目录），导出图放 `doc/picture/` 镜像路径
3. **补数据流图**：目前缺"一个 packet 从 RF 中断到 HCI 上报经过哪些模块"的**数据流/调用链图**——学 Zephyr Connectivity 章的 pipeline 图，给 `debug_port` 和 `hci` 各画一张（`rf_irq → ring buffer → task → 回调 → 上层`），这是新成员上手最快的一类图

## 五、优先行动

先精读 **Zephyr 在线版 UART 外设文档**（15 分钟），对照 `platform/hal/README.md` 差什么——它多了"中断配置"、"数据流图"、"错误处理"三块，补齐即完成第一次抄作业。BLE 架构图存下来，与 `doc/picture/arch/stack_arch.png` 对照画自己的版本。

优先级：Zephyr Connectivity Bluetooth 架构图 + 一个外设驱动文档 > 内核 driver-api > C4 方法论。
