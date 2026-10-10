# wave 调试平台 — 设计与实现记录

> 供后续 AI / 开发者接续分析维护。描述现状、依据、协议、已知问题与路线。
> 仓库：`https://github.com/Coy681/tx_ble.git`（SDK 根 = 本文件上级目录）。
> 状态：**固件侧联调通过（2026-10-10，COM7 1M 波特率实测）**；上位机唯一 GUI 为 `debug_hub_qt.py`（PySide6，**v2.0.0**），另有独立 exe 构建（`build_exe.ps1`）。波形导出为层级化裸 VCD。产品定义见 §1。
> 最近更新：2026-10-10 — **v2.0.0**：UART 配置折叠为 ⚙ 按钮（补 parity/flow）；LOG/WAVE 各加 save、WAVE 加 import（bin 回放闭环）；session 日志（每次连接自动落盘，小时级保真）；布局改 LOG 通高左列 + 右侧 WAVE/TRACKS 上下分；MONITOR/TRACKS 两层折叠树；HTX 图标（exe 资源 + 运行时任务栏，AppUserModelID 修复 python.exe 归组）；PyInstaller 单文件 exe

---

## 1. 产品定义（2026-10-10 重新定义）

> 从"工程师顺手写的调试脚本"升级为"产品"。差距不在框架，在**交互模型、信息密度、工作流闭环**。

**一句话**：把"烧录 → 观察 → 定位 → 归档"做成一个不换窗口的闭环。

**用户与场景**（唯一用户：嵌入式固件工程师，单板单口）：

| 场景 | 痛点 | 产品应答 |
|---|---|---|
| 烧录后看板子活没活 | 开串口工具看文本 | 秒开 log 流，一眼看到心跳 |
| adv 周期/门宽对不对 | 数打印、掐秒表 | TRACKS 门摘要直接读数，详细分析去 Surfer |
| 异常抓回来慢慢看 | 抓包文件散落 | 一键 snapshot 带时间戳归档 |
| 新模块加打点 | 翻文档回忆宏 | 本文件 3.3/9.9 即手册 |

**界面形态（v2.0.0，2026-10-10 定稿并实现）**：

```
顶部: [⚙ uart] [connect ●药丸] ... [⏸] [⏺record] [📸snapshot] [clear all]
      ⚙ 展开: [port baud parity flow]（默认折叠，留白给操作按钮）
┌─ LOG 通高 45% ─────────┬─ WAVE (右上) ─────────────┐
│ [filter__]n/m save clear│ [surfer▶][import][save][clear] │
│ 本机时间戳+着色          │ value|ago|▓▓░░|track      │
│ session 日志自动落盘     ├─ TRACKS (右下 40%) ────────┤
│ [send___][hex][tx]      │ [all][none] 信号树勾选      │
└─────────────────────────┴───────────────────────────┘
状态栏: log:N lines | live.vcd/rec/snap .... wave:N | KB/s | gate ok
```

设计原则：
- **操作归属各自面板**（面板头部条），顶部只留全局件；UART 配置折叠成
  ⚙ 按钮（port/baud/parity/flow 展开），顶部空间留给高频操作
- **布局**：LOG 行长（时间戳+文本）横向空间最值钱 -> 左列通高；WAVE
  4 列表与 TRACKS 树是"宽扁"内容 -> 右侧上下分，导出选择（下）与数据
  流（上）视线近，勾选完马上 surfer
- **log 三层存储（小时级保真）**：session 文件（每次连接一个
  `logs/session_*.txt`，无限、20 行一 flush）> log_store 留底（默认
  无限，`--log-max N` 可设上限）> 显示面板（最近 20000 行，渲染性能
  保护；重放只画这段）
- **数据闭环**：实时采集 -> save 存底片（12B bin）/ snapshot 归档 ->
  import 回放（灌入现有 MONITOR/TRACKS 管线）-> 勾选 -> surfer 分析
- **MONITOR 两层折叠树**：组行 = module/feature/实例（带道数 + 组内
  ago 聚合，默认折叠）；道行 = value/ago/sparkline。几百轨道不爆屏
- `⏸` 冻结显示不丢数据（留底/session 继续收，恢复时全量重放）；
  `⏺record` 原始字节流落盘 `record_*.bin`，录制中红色闪烁
- **log 过滤作用于历史**：留底 + 过滤变化 300ms 防抖后全量重放；
  save/snapshot 从留底导出（不受过滤影响，永远完整）
- 状态栏左右分段：左 log 统计、中全局文件提示、右 wave 统计
- **图标**：HTX 艺术字（H 横梁断开=帧意象 + 底部青色门形方波）。
  exe 资源（--icon）只管资源管理器；运行时任务栏需 setWindowIcon +
  SetCurrentProcessExplicitAppUserModelID（否则 python.exe 启动按
  进程归组显示 Python 图标）
- 浅色主题（VS Code Light+）；按钮宽度一律内容自适应
- tkinter 版已删除；exe 单文件分发（build_exe.ps1 可复现，exe 不入库）

**明确不做**：不重写波形渲染（Surfer 是产品一部分，UI 是它的发射台）；不做仪表盘美化；不做多设备。

**v0.2 实现说明**：
- `snapshot` 按钮 → `snapshots/YYYYMMDD-HHMMSS/`：`log.txt` + `wave.bin`（12B 原格式，可直接 `--input` 回放）+ `raw.bin`（连接中才有，原始字节流保真）+ `config.txt`
- TRACKS 树懒构建（首次出现的轨道自动挂入，默认全勾选 = 与旧版全量导出行为一致）；叶子第二列显示 LEVEL 门 `val=/win=`（win=`OPEN!` 即悬空门现场证据）
- `surfer` 只导出勾选轨道；`clear` 重置门摘要但保留勾选
- SerialWorker 常驻缓存 `self.raw` 原始字节流（快照用）

---

## 1.x 目标与背景（原始设计动因）

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
│ wave.c: 12B 记录打包          │          │  ├ LOG 栏: 文本(时间戳/过滤) │
│      │                        │          │  ├ WAVE 栏: 每轨一行摘要     │
│ log.c: 文本格式化             │          │  ├ TRACKS: 勾选=导出范围     │
│      ▼                        │          │  └ [surfer▶]: 勾选轨道       │
│ debug_port.c: rb+TX任务+UART │          │     → live.vcd → Surfer.exe  │
└──────────────────────────────┘          └─────────────────────────────┘
```

- **VCD 是解耦层**：任何 EDA 工具（Surfer/GTKWave）都能打开，联调历史可回放
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

**门配对语义（实测教训，2026-10-10）**：begin 只在 EVENT_START 打一次，但 end 必须**覆盖所有可能离开发送状态的路径**（STOP / CANCELED / PASSED / PHY 事件默认路径）。最初只在 sch_stop 打 end，而 CANCELED 路径不经过它 → 门悬空 ~22.8ms 直到下一次 START 前才闭合，波形呈现"空闲高、发送低"的反相假象。排查方法：`tools/debug_hub/analyze_gates.py` 统计每个符号的高电平窗口长度与边沿间隔分布，异常长的窗口即悬空门。

> 待办：`WAVE_LEVEL` 第 4 参 `state=1` 应用语义常量（如 `WV_ADV_LEGACY_WINDOW`）替换裸数字；补齐 adv EXT/PERIODIC 及更多模块打点。

## 5. 上位机实现

### 5.1 文件

| 文件 | 说明 |
|---|---|
| `tools/debug_hub/debug_hub_qt.py` | **唯一 GUI**。PySide6（见 §1 界面形态 / 5.2） |
| `tools/debug_hub/test_hub.py` | 端到端离线测试：合成流 → hub → 校验 VCD |
| `tools/debug_hub/test_fst.py` | 测试 VCD 的层级 scope 与"只含出现过的信号" |
| `tools/debug_hub/analyze_gates.py` | 门形分析：统计 live.vcd 各符号高电平窗口/边沿间隔，定位悬空门 |
| `tools/debug_hub/surfer.exe` | 开源波形查看器（MIT），看 VCD（不支持 gz；也支持 FST） |
| `tools/debug_hub/snapshots/` | 会话快照归档（snapshot 按钮产出） |

（早期原型 `wave_view.py`、tkinter 版 `debug_hub.py` 均已删除，可从 git 历史找。）

### 5.2 Qt 版行为要点

界面形态以 §1 为准（v1.1：面板头部条/历史过滤/状态栏分段）。补充实现要点：

- 串口在 QThread worker 中读，Signal/Slot 投递 UI 线程；1M 波特率持续流不卡
- **[surfer▶]**：把勾选轨道的记录重写 `live.vcd` 并拉起 Surfer（Surfer 不会自动刷新，重开即最新快照）；三种"导不出"在状态栏分别提示（没记录/没勾选/勾选与记录无交集）
- 发送：ASCII 或 HEX（勾选），自动补 `\r\n`；板子侧经 `debug_port_rx_register` 回调接收
- 命令行：`--port COM7 --baud 1000000 --input dump.bin --vcd … --no-surfer --raw-dump`
- 离线模式：`--input dump.bin` 把串口助手抓的原始 dump 转 VCD 回放
- **PySide6 局部窗口 GC 坑**：QMainWindow 等控件是 run_gui() 局部变量，
  若 return 后再 exec()，引用被回收 → 窗口创建即销毁、进程空转（实测
  hwnd 消失）。修复：事件循环必须在 run_gui() 内执行
- **多显示器坑**：双屏（副屏负坐标）下 win.show() 可能落在不可见区域，
  需显式移到主屏中心 + raise_/activateWindow()

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

- `python tools/debug_hub/test_hub.py`：合成 log+wave+garbage 流 → 解帧/重同步/轨道名全部通过
- `python tools/debug_hub/test_fst.py`：VCD 导出验证——scope 层级（`tx_ble > adv > LEGACY > s1`、`tx_ble > scheduler > STIMER > s2_num`）、只含出现过的信号、Surfer 完整解析（`Loaded the body`）
- 真机（COM7, 1M）：raw dump 确认 `A5 01 0C [ts][id=0x81010001][val 1/0]` 帧流正确；`analyze_gates.py` 验证 adv 门修复后真实发送窗口 ~1.8ms、周期 ~24ms
- Qt GUI 冒烟测试通过（OS 级 EnumWindows 验证窗口可见 + 存活）

## 7. 已知限制与路线

| 项 | 现状 | 建议 |
|---|---|---|
| FST 输出 | 裸 VCD（本机 Surfer 不支持 gz；FST 需 MSVC 编译 pylibfst） | 装 VS Build Tools 后可切 pylibfst（FST 体积更小） |
| 缓冲无上限 | records 列表只增（surfer 导出全量） | 长时间联调加环形上限或分片导出 |
| ts 回绕 | u32 差值处理，未处理绝对回绕 | 桥内做 unwrap |
| Surfer 刷新 | 手动按按钮重开 | 可调研 Surfer 的 VCD 流式/重载插件 |
| Replay 完整页签 | import 按钮已覆盖单文件回放；record_*.bin（含 log 的原始流）仍需 `--input` | 后续：Replay 页签（选快照目录 GUI 回放 raw 流） |
| 打点覆盖率 | 无量化 | 后续：扫描 `WAVE_` 宏统计面板 |
| GUI 中文 | 界面英文 | 按需 |
| 带宽 | 定长 12B + 3B 帧头 | 撞带宽墙先降打点频率，勿急着压缩协议 |

## 8. 快速上手

```bash
# 固件（已配置 TX_DEBUG_WAVE_ENABLE=1）
cd cmake_builds/tx_text/TL32_ELF_MCULIB_V5F_GCC12_2
# PATH 前置 Telink cygwin + nds32 工具链后: make.exe

# 上位机
pip install pyserial PySide6
python tools/debug_hub/debug_hub_qt.py            # GUI 里选 COM/波特率 → connect
python tools/debug_hub/debug_hub_qt.py --input dump.bin   # 离线 dump → VCD
python tools/debug_hub/test_hub.py                # 回归测试（解析核心）
python tools/debug_hub/test_fst.py                # VCD 层级/按需声明测试

# exe 构建（无需 python 的分发方式）
powershell -File tools/debug_hub/build_exe.ps1    # -> debug_hub.exe（44MB）
# exe 分发布局：SDK 根/ debug_hub.exe + surfer.exe（自动扫描源码树建 id 映射）
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
10. **log 过滤 = 留底 + 重放**：流式 appendHtml 渲染完即丢，历史无从筛起。留底列表（纯文本+上限）与面板并行维护，过滤变化 300ms 防抖后全量重放；实时行与重放共用同一渲染管线。附带收益：快照从留底导出，不受过滤影响
11. **surfer 空导出必须三态提示**："没记录 / 没勾选 / 勾选与记录无交集"混报成一句，用户会以为有 bug（实测发生过）。错误提示要区分到可操作的动作（如 `TRACKS > all?`）
12. **按钮尺寸禁用固定像素**：`setFixedWidth(48)` 在不同字体/DPI 下裁字；统一 `minimumWidth = minimumSizeHint().width() + pad` 内容自适应
13. **QSS 裸 background 会被子控件继承**：面板头部条容器样式必须用 `#objectName` 选择器限定，否则按钮 hover/checked 态被冲淡（hex 按钮选中态几乎看不出来就是这么来的）
14. **save 存 bin（底片）而非 vcd（照片）**：bin 无损全量、可回放重新导出任意轨道组合的 vcd；vcd 固化勾选结果，轨道选漏就废。闭环靠 WAVE 头部 import 按钮（bin -> 现有管线 -> 勾选 -> surfer）
15. **log 小时级保真 = 三层存储**：session 文件（无限/自动落盘）> 留底（默认无限）> 显示面板（2 万行）。只靠内存列表必然受显示上限或内存约束；落盘文件是唯一断电安全的层
16. **任务栏图标归组**：exe 资源图标只管资源管理器；运行时需 setWindowIcon。python.exe 命令行启动时任务栏按进程显示 Python 图标，必须 `SetCurrentProcessExplicitAppUserModelID` 注册独立应用身份
17. **打包路径自适应**：PyInstaller onefile 下 `__file__` 指向临时解包目录，路径定位改 `sys.executable`；图标/资源用 `sys._MEIPASS` 查找；exe 按放位置自动推断 id 映射扫描根（SDK 根旁 / tools/debug_hub 内 / 独立分发降级）
