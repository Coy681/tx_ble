# wave 调试平台 — 设计与实现记录

> 供后续 AI / 开发者接续分析维护。描述现状、依据、协议、已知问题与路线。
> 仓库：`https://github.com/Coy681/tx_ble.git`（SDK 根 = 本文件上级目录）。
> 状态：**固件侧联调通过（2026-10-10，COM7 1M 波特率实测）**；上位机主力为 `debug_hub_qt.py`（PySide6），tkinter 版兜底。波形导出为 gz-VCD（层级化、按需声明）。
> 最近更新：2026-10-10 — 波形导出为层级化裸 VCD（5.3）；Surfer 兼容坑与 scope 配对已记录（9.7/9.8）；adv 门悬空问题定位并修复——end 补齐 CANCELED/PASSED 等全部出口路径（4.4、9.9）

---

## 1. 目标与背景

在 Telink BLE SDK 上做软件逻辑分析仪（打点 → 串口 → PC 波形/日志），解决：

1. 串口工具"ASCII/HEX 二选一"的痛点：log 文本和 wave 二进制混流无法同时看
2. ISR/热路径不适合 printf，但需要时序可视化（任务/状态窗口）
3. 波形查看不自研（自绘 matplotlib 效果差），接开源 **Surfer**（数字波形查看器，吃 VCD）

分层原则：**固件只产生结构化记录，格式化/渲染全部在上位机**。协议变更只改一处。

## 2. 总体架构

```
固件 (C)                                 上位机 (Python)
┌──────────────────────────────┐   UART   ┌─────────────────────────────┐
│ 业务代码                      │  1M baud │ debug_hub_qt.py (PySide6)    │
│  WAVE_EVENT / WAVE_LEVEL     │ ───────► │  ├ FrameParser: A5 帧状态机  │
│      │                        │          │  ├ load_map: grep X-macro    │
│ wave.c: 12B 记录打包          │          │  ├ 左栏: log 文本(着色/过滤) │
│      │                        │          │  ├ 右栏: wave 原始流监视     │
│ log.c: 文本格式化             │          │  └ [surfer]按钮: 全量记录     │
│      ▼                        │          │     → live.vcd.gz → Surfer   │
│ debug_port.c: rb+TX任务+UART │          │ Surfer (surfer_win/) 做波形  │
└──────────────────────────────┘          │    缩放/光标/信号树           │
                                          └─────────────────────────────┘
```

- **gz-VCD 是解耦层**：任何 EDA 工具（Surfer/GTKWave）都能打开，联调历史可回放
- ISR 热路径（μs 级）**不打 wave 点**，用 GPIO trace（`TX_DEBUG_GPIO_ENABLE`）

## 3. 线上协议（唯一权威定义）

### 3.1 帧格式（host 实际收到的字节）

```
[0xA5][type][len][payload...]        3 字节帧头 + payload
  type: 0x00 = log 文本（log.c 产出，自带 \r\n）
        0x01 = wave 记录流（12B 对齐）
  len  = payload 字节数（1~236）
```

- wave 帧的 payload 为 N×12 字节记录，可一条帧含多条记录
- host 解析：搜 `0xA5` → 校验 type≤1、len≤236 → 取 payload；失配则步进 1 字节重同步
- 单帧 payload 上限 = `DEBUG_PORT_OUTPUT_BUFFER_SIZE(240) - PORT_FRAME_SIZE(3)`

### 3.2 12B wave 记录（小端）

```
[0..3]  u32 timestamp   （system_time() tick，由 wave_event 自动填或手动传）
[4..7]  u32 id
[8..11] u32 value
```

### 3.3 id 位布局（32 位）

```
[type(1) | module(7) | feature(8) | instance(8) | evt(8)]
  bit31     bit30-24   bit23-16     bit15-8        bit7-0
```

- `type`: 1=LEVEL（value 1=begin / 0=end 门）；0=NUM（value 为数据）；value=0 且 evt=0xFF 为 marker
- `module`: 中央注册表 `WAVE_MODULE_LIST`（X-macro），SCHED=0x01 … APP=0x0B
- `feature`: 各模块自建子表（"谁使用，谁定义"），如 `WAVE_ADV_FEATS`：LEGACY=0x01/EXT=0x02/PERIODIC=0x03
- `instance`: **运行时值**（conn handle / adv set handle…），不进编译期枚举
- `evt`: 同 feature 内的事件道；LEVEL 时语义为 state（门编号）；0xFF=feature 级探针

### 3.4 关键约定

- 层级省略只能从深往浅填零：module→feature→instance→evt
- LEVEL 门配对键 = `(轨道, state)`；同键不可嵌套
- 宏命名拼接依赖严格约定：`WAVE_MOD_<short>` / `WAVE_FEAT_<MOD>_<FEAT>`，改名必须成对
- 上位机与固件共享同一 X-macro 源（host grep 头文件），未知 id 降级为数字命名，**无需同步发版**

## 4. 固件侧实现

### 4.1 文件与职责

| 文件 | 职责 |
|---|---|
| `debug/wave/wave_id.h` | id 布局、X-macro 注册表、`WAVE_ID`/`WAVE_FEAT_ENUM` 宏 |
| `debug/wave/wave.h/.c` | 调用宏 + 12B 打包，`debug_port_write_typed(DEBUG_BLK_WAVE,…)` |
| `debug/log/log.c` | 文本格式化 → `debug_port_write`（=LOG 类型） |
| `debug/debug_port.h/.c` | 传输层：环形缓冲 + TX/RX 任务 + 带内帧头 |
| `common/txBit.h` | `U32_TO_STREAM` 等流式打包宏 |
| `config.h` | `TX_DEBUG_WAVE_ENABLE=1` 总开关 |

### 4.2 debug_port 双头设计（重要，曾踩坑）

- **rb 内部 4 字节头是给硬件的，不上线**（RX 侧 `hal_uart_set_receive_buffer(p+4,…)` 同样跳过）。曾误把"发 header"当修复，已回退
- **带内帧**（`0xA5…`）由 `debug_port_write_typed` 写进 payload 区；rb 头存的长度 = `len + PORT_FRAME_SIZE`
- TX 任务只发 `pData, dataLen`（硬件头已剥）

### 4.3 已修复的三个固件 bug（教训记录）

1. **`txBit.h U32_TO_STREAM`**：原版 `(_u8)n>>8` 先截断再移位，高 3 字节恒 0 → ts/id 全部缺失。修为 `((_u8)((n)>>8))`。该宏全 SDK 共用，属潜伏地雷
2. **`wave_id.h WAVE_ID` 双移位**：`WAVE_FEAT_*` 枚举值已含 `mod<<24|feat<<16`，`WAVE_ID` 又移一次导致 feature 恒 0。修为直接取枚举值；`WAVE_MOD_ID` 从注册表直接构 `mod<<24`
3. **块边界缺失**：TX 只发 payload 不发任何头 → 上位机无法定界。方案取舍：曾试"rb 头改带 type 位上线"（错，见 4.2）、曾试 "681" 文本前缀（错，被打包覆写），**最终定为带内 3 字节 `A5` 帧**

### 4.4 现有打点（adv）

```c
// system/ble/controller/ll/module/adv/adv.c —— begin 一处、end 四处
adv.c:1294  WAVE_LEVEL(ADV, LEGACY, 0, 1, 1);   // 门 begin：step_phy_send_advertising（EVENT_START）
adv.c:1419  WAVE_LEVEL(ADV, LEGACY, 0, 1, 0);   // 门 end：step_default_process（PHY 事件路径）
adv.c:1426  WAVE_LEVEL(ADV, LEGACY, 0, 1, 0);   // 门 end：step_sch_stop（EVENT_STOP）
adv.c:1454  WAVE_LEVEL(ADV, LEGACY, 0, 1, 0);   // 门 end：step_sch_passed（EVENT_PASSED）
adv.c:1473  WAVE_LEVEL(ADV, LEGACY, 0, 1, 0);   // 门 end：step_sch_canceled（EVENT_CANCELED）
```

实测 id = `0x81010001`（1M 波特率下真实发送窗口 ~1.8ms）。

**门配对语义（实测教训，2026-10-10）**：begin 只在 EVENT_START 打一次，但 end 必须**覆盖所有可能离开发送状态的路径**（STOP / CANCELED / PASSED / PHY 事件默认路径）。最初只在 sch_stop 打 end，而 CANCELED 路径不经过它 → 门悬空 ~22.8ms 直到下一次 START 前才闭合，波形呈现"空闲高、发送低"的反相假象。排查方法：`debug/wave/analyze_gates.py` 统计每个符号的高电平窗口长度与边沿间隔分布，异常长的窗口即悬空门。

> 待办：`WAVE_LEVEL` 第 4 参 `state=1` 应用语义常量（如 `WV_ADV_LEGACY_WINDOW`）替换裸数字；补齐 adv EXT/PERIODIC 及更多模块打点。

## 5. 上位机实现

### 5.1 文件

| 文件 | 说明 |
|---|---|
| `debug/wave/debug_hub_qt.py` | **主力**。PySide6 界面（见 5.2） |
| `debug/wave/debug_hub.py` | tkinter 版（功能对齐，兜底用） |
| `debug/wave/wave_view.py` | 早期 matplotlib 原型，已被取代（grep 逻辑同源） |
| `debug/wave/test_hub.py` | 端到端离线测试：合成流 → hub → 校验 VCD |
| `debug/wave/test_fst.py` | 测试 VCD 的层级 scope 与"只含出现过的信号" |
| `debug/wave/analyze_gates.py` | 门形分析：统计 live.vcd 各符号高电平窗口/边沿间隔，定位悬空门 |
| `surfer_win/surfer.exe` | 开源波形查看器（MIT），看 VCD（不支持 gz；也支持 FST） |

三个上位机（hub/hub_qt/wave_view）各自内嵌同一套核心（帧解析、X-macro grep、VCD writer），**修改协议时三处需同步**；后续可拆 `hub_core.py` 统一 import。

### 5.2 Qt 版界面与行为

```
工具栏: port▾ baud▾ [connect] 状态 | send [hex□] [tx] | filter▾ | [surfer][clear] wave:N
┌──────── 左: log（着色文本, 2万行上限）────────┬──── 右: wave 原始流（同样上限）────┐
│ 每行一条 log，关键词着色                        │ 每条记录一行: ts|id|begin|end/值|轨道│
└───────────────────────────────────────────────┴──────────────────────────────────┘
状态栏: id 映射统计
```

- 串口在 QThread worker 中读，Signal/Slot 投递 UI 线程；1M 波特率持续流不卡
- **[surfer] 按钮**：把本次连接以来**全部**记录重写 `live.vcd.gz` 并拉起 Surfer（Surfer 不会自动刷新，重开即最新快照）
- 右栏是**原始流监视**（确认数据在打印），精细分析全部交给 Surfer
- 发送：ASCII 或 HEX（勾选），自动补 `\r\n`；板子侧经 `debug_port_rx_register` 回调接收
- 命令行：`--port COM7 --baud 1000000 --input dump.bin --vcd … --no-surfer --raw-dump`
- 离线模式：`--input dump.bin` 把串口助手抓的原始 dump 转 gz-VCD 回放

### 5.3 波形导出（VCD，层级化，按需声明）

- 导出格式：**裸 VCD**（`live.vcd`）。FST 需要 pylibfst（MSVC 编译，本机无 MSVC，失败）；**实测本机 Surfer 构建不支持 gzip**（合法 `.vcd.gz` 报 `UnknownFileFormat`），故放弃 gz-VCD
- **Surfer VCD 解析器两个硬性要求**（踩坑记录）：
  1. **CRLF 行尾**——LF-only 文件 body 解析失败（"expected an id for a value change"），写入必须 `newline='\r\n'`
  2. **vector 值变更 `b` 与信号 id 之间必须有空格**（`b1010 !#`，规范本就如此）
- **层级结构**：`$scope module tx_ble > $scope module <module> > $scope module <feature>`，Surfer 信号树按 module/feature 分组；`$scope/$upscope` 必须严格配对（见 9.7）
- **只声明出现过的信号**：导出前先扫描记录收集实际轨道集（`collect_tracks`），未出现的通道不产生 `$var`，面板无空占位
- LEVEL → 1-bit wire（begin=1/end=0），信号名 `s<state>`；NUM → 32-bit wire，信号名 `s<evt>_num`；probe（evt=0xFF）→ `probe`
- 未知 id 降级 `mod0x0/feat0x0`
- 时间戳保持固件 tick 原值，`$timescale 1us` 仅名义
- 导出后校验文件含 `$enddefinitions` 才拉起 Surfer（防截断）

## 6. 验证记录

- `python debug/wave/test_hub.py`：合成 log+wave+garbage 流 → 解帧/重同步/轨道名全部通过（tkinter 版）
- `python debug/wave/test_fst.py`：VCD 导出验证——scope 层级（`tx_ble > adv > LEGACY > s1`、`tx_ble > scheduler > STIMER > s2_num`）、只含出现过的信号、Surfer 完整解析（`Loaded the body`）
- 真机（COM7, 1M）：raw dump 确认 `A5 01 0C [ts][id=0x81010001][val 1/0]` 帧流正确；`analyze_gates.py` 验证 adv 门修复后真实发送窗口 ~1.8ms、周期 ~24ms
- Qt GUI 冒烟测试通过（启动/退出无异常）

## 7. 已知限制与路线

| 项 | 现状 | 建议 |
|---|---|---|
| FST 输出 | 裸 VCD（本机 Surfer 不支持 gz；FST 需 MSVC 编译 pylibfst） | 装 VS Build Tools 后可切 pylibfst（FST 体积更小） |
| 缓冲无上限 | records 列表只增（surfer 导出全量） | 长时间联调加环形上限或分片导出 |
| ts 回绕 | u32 差值处理，未处理绝对回绕 | 桥内做 unwrap |
| Surfer 刷新 | 手动按按钮重开 | 可调研 Surfer 的 VCD 流式/重载插件 |
| log 仅着色 | 无正则过滤/搜索 | filter 已留 UI，未实现正则 |
| 三份核心代码 | hub / hub_qt / wave_view 重复 | 拆 `hub_core.py` |
| GUI 中文 | 界面英文 | 按需 |
| 带宽 | 定长 12B + 3B 帧头 | 撞带宽墙先降打点频率，勿急着压缩协议 |

## 8. 快速上手

```bash
# 固件（已配置 TX_DEBUG_WAVE_ENABLE=1）
cd cmake_builds/tx_text/TL32_ELF_MCULIB_V5F_GCC12_2
# PATH 前置 Telink cygwin + nds32 工具链后: make.exe

# 上位机
pip install pyserial PySide6
python debug/wave/debug_hub_qt.py            # GUI 里选 COM/波特率 → connect
python debug/wave/debug_hub_qt.py --input dump.bin   # 离线 dump → gz-VCD
python debug/wave/test_hub.py                # tkinter 版回归测试
python debug/wave/test_fst.py                # gz-VCD 层级/按需声明测试
```

## 9. 设计决策存档（为什么这样做）

1. **定长 12B 优先于变长压缩**：上位机零状态切片，解析器 50 行；带宽不够先降频
2. **instance 是数据不是枚举**："谁使用谁定义"只到 feature 级；instance 是运行时对象号，第 4 级（原 rsvd）语义不成立
4. **带内 A5 帧 vs 块类型位上线**：rb 4B 头属硬件协议不可动；文本前缀方案（"681"）会与定长记录冲突且被覆写，弃用
5. **gz-VCD 中间格式（而非 FST/裸 VCD）**：换来 Surfer 全部能力（信号树/缩放/光标），任何 EDA 工具可开；gzip 压缩后体积接近 FST 且零原生依赖（FST 绑定 pylibfst 需 MSVC，本机构建失败，见 5.3）；我们只写 12B→`$change` 的几十行。信号声明按需收集（`collect_tracks`），避免面板空占位
6. **改开源工具 vs 换 UI**：Serial Studio 双许可（GPL+商业）且要把自定义逻辑翻译成它的 C++/Lua 层，fork 维护成本高；PlotJuggler 缺 log 页；最终保留 Python 核心资产 + PySide6 换皮，核心逻辑零重写
7. **VCD scope 平衡教训**：`$scope/$upscope` 必须严格配对（root 除外）。首版在"切换同级 feature"时只弹了一层 `$upscope`，scope 栈失配导致 Surfer 报 `wave_source` 错误且面板为空。修复后导出前应断言 `opens == ups + 1`（root 不闭）
8. **Surfer 对 VCD 的兼容性坑**：本机构建的 Surfer ① 不识别 gzip（合法 gz 也报 UnknownFileFormat，窗口标题正常但内容空）；② VCD 必须 CRLF 行尾、vector 值 `b` 后必须有空格再接 id，否则 body 解析失败。排查方法：`surfer.exe <file> --command quit` 抓 stderr 日志比 GUI 信息全
9. **LEVEL 门必须路径完备**：begin/end 是跨函数的状态机配对，end 必须覆盖**所有**离开该状态的路径（STOP/CANCELED/PASSED/默认路径），漏一条就产生悬空门（表现为空闲时段高电平、窗口长度异常大）。写打点前先枚举状态机的全部出口；用 `analyze_gates.py` 量化验证
