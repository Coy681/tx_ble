# wave — 软件逻辑分析仪

wave 是 debug 子系统的结构化事件流模块：固件打点输出 `[时间戳][层级 id][值]` 三元组，经 debug_port 的 UART 通道送到 PC，上位机按 id 层级树分道画波形——用一根串口线替代 16 根 GPIO 探针的逻辑分析仪。

设计吸收了《Firmware Log可视化升级方案》（AML 波形方案）的两个核心抽象：**Level/Num 两种记录类型**与 **instance 运行实例维度**，并保持 tx_ble 的拼接宏定义方式。

## 一、设计定位

### 1.1 wave 与 log / gpio 的分工

| 方式 | 回答的问题 | 上下文开销 | 适用场景 |
|------|-----------|-----------|---------|
| log | "程序走到哪了、数据是什么" | 中 | 状态流转、打印收发包 |
| **wave** | **"事件流随时间怎么变化"** | 中（μs 级） | 状态机 trace、事件时序、数值曲线，PC 端结构化画图 |
| gpio | "这段代码精确执行多久" | 极低（几十 ns） | ISR 热路径、μs 级时序测量，配合示波器/逻辑分析仪 |

边界约定：**ISR 时序敏感的热路径用 gpio，其余事件 trace 优先用 wave**。wave 信息密度高于 gpio（每条记录带时间戳 + 32bit 值 + 层级 id），但不适合放在 radio_irq 的 ramp-up 等纳秒级段落。

### 1.2 传输路径

```
WAVE_EVENT(...) ─> wave_pack() [ts|id|value 12字节] ─> debug_port_write()
                                                          │
                                              portOutputRb ring buffer (32×240)
                                                          │ tx_task_set_event(LOG_TX)
                                                          ▼
                                              port_task_event_tx() ─> hal_uart ─> PC
```

- wave 自身不碰 UART，所有传输细节归 debug_port（块格式 `[4B len][payload]`）
- 满即丢：ring buffer 满时记录被丢弃，debug 数据允许丢、不允许阻塞业务
- 记录定长 12 字节，PC 端解析是固定步长循环，无需状态机

## 二、ID 层级设计（核心）

### 2.1 三级 32-bit 结构

```
32-bit id = [ type(1) | module(7) | feature(8) | instance(8) | evt(8) ]
              记录类型   模块         特性           运行实例      事件号
```

（吸收自 AML 方案的 type/subsystem/module/feature/instance 五级树，压缩到固件单芯片够用的粒度：subsystem 由 module 空间兼任——tx_ble 目前只有 BT 一个子系统；type 从"展示形式"升级为记录语义位。）

- **type（bit31）**：记录类型，两种
  - `LEVEL` 电平类：value 1=开始 / 0=结束，同轨道一对边沿在上位机画出门（gate），适合状态/任务起止
  - `NUM` 数值类：value 即采样值，画数值/模拟曲线；探针（marker）= value 0 的 NUM
- **module**：架构分层（adv/conn/scheduler/hci...），中央注册，稳定
- **feature**：模块内功能单元（adv 的 legacy/ext/periodic），各模块自定义（原 sub 更名，与 AML 术语对齐）
- **instance**：运行实例编号（conn handle、adv set handle、CE 序号...），**运行时数值**，不是编译期枚举——0~255 共 256 条固定分道，同一 feature 的所有实例在上位机对齐到同一组轨道
- **evt**：事件号（0xFF = feature 级探针）；rsvd 第 4 级判定不变——AML 实践证实 instance 已覆盖多对象维度，若觉空间不够应拆 feature 而非加深

### 2.2 拼接宏定义层级（token pasting）

层级不是手写位运算，而是**全部用宏拼接生成**，编译期展开，零运行时开销：

```c
/* 1. 模块在自己的头文件里定义 feature 列表（谁的代码谁定义） */
#define WAVE_ADV_FEATS(X)       \
    X(ADV, LEGACY,   0x01)      \
    X(ADV, EXT,      0x02)      \
    X(ADV, PERIODIC, 0x03)
WAVE_FEAT_ENUM(WAVE_ADV_FEATS)

/* 2. 打点处用短名，拼接 + 运行时 instance */
WAVE_EVENT(ADV, EXT, 0x03, state);              /* 数值采样 */
WAVE_LEVEL(ADV, EXT, advHandle, 0x01, on);      /* 电平边沿：任务窗口 */
WAVE_EVENT_I(CONN, CE, ceIdx, 0x02, rssi);      /* 带实例的数值采样 */
/*   展开: wave_event( WAVE_TYPE(WAVE_ID_I(ADV, EXT, inst), WAVE_TYPE_NUM) | 0x03, state )
 *        WAVE_ID 拼接出 WAVE_MOD_ADV(<<24) | WAVE_FEAT_ADV_EXT(<<16)，instance OR 进低 16 位 */
```

拼接机制：

| 宏 | 作用 |
|----|------|
| `WAVE_PASTE(a,b)` / `WAVE_PASTE3(a,b,c)` | 两级展开的 `##` 拼接（先展开参数再粘贴） |
| `WAVE_ID(mod, feat)` / `WAVE_ID_I(mod, feat, inst)` | 拼 `WAVE_MOD_<mod>` 与 `WAVE_FEAT_<mod>_<feat>` 枚举 token，instance 运行时 OR 入 |
| `WAVE_FEAT_ENUM(list)` | 把模块的 X-macro 列表展开成**匿名 enum**（无 typedef，多模块可各自调用不冲突） |
| `WAVE_TYPE(id, type)` / `WAVE_TYPEOF(id)` | 置位/读取 bit31（LEVEL vs NUM） |
| `WAVE_MOD_ID(mod, inst)` | 跳过 feature 层的模块级探针（feature=0 保留） |

### 2.3 上位机匹配规则

- 固件只发 32-bit id，**层级树由上位机从同样的头文件生成**：脚本 grep `WAVE_MODULE_LIST` 与各模块的 `WAVE_XXX_FEATS` 列表，得到
  ```
  track = module（"adv"）
    └ feature-track = feature（"EXT"）
        └ lane = instance × event
  ```
- **类型分道**：bit31=LEVEL 的轨道画门（电平），bit31=NUM 的轨道画数值曲线/点——同一 feature 的 level 与 num 自动分为两组轨道
- **容错**：收到未知 feature/instance 时按数字显示、不丢数据 → 固件和上位机不需要版本同步发布

## 三、使用方法

### 3.1 接口

| 宏 | 类型 | 时间戳 | 用途 |
|----|------|--------|------|
| `WAVE_EVENT(mod, feat, evt, value)` | NUM | 自动（`system_time()`） | 任务上下文常规数值打点 |
| `WAVE_EVENT_I(mod, feat, inst, evt, value)` | NUM | 自动 | 带实例的数值打点（多连接/多广播） |
| `WAVE_LEVEL(mod, feat, inst, state, on)` | LEVEL | 自动 | 状态/任务起止边沿，成对使用画门 |
| `WAVE_EVENT_AT(mod, feat, evt, ts, val)` | NUM | 手动 | ISR 捕获时刻、task 补发 |
| `WAVE_LEVEL_AT(mod, feat, inst, state, on, ts)` | LEVEL | 手动 | ISR 捕获的精确起止沿 |

所有宏在 `TX_DEBUG_WAVE_ENABLE=0` 时空展开，零成本移除。

### 3.2 记录格式（线格式）

```
12 字节，小端：
[0..3]  u32 timestamp   — system_time() 同单位（tick，1 tick = 1/CLOCK_TICK_US 秒）
[4..7]  u32 id          — [type(1)|module(7)|feature(8)|instance(8)|evt(8)]
[8..11] u32 value       — LEVEL: 1=begin/0=end；NUM: 数据
```

### 3.3 接入步骤（以 adv 模块为例）

1. **模块头文件定义 feature 列表**：
   ```c
   #define WAVE_ADV_FEATS(X)   X(ADV, LEGACY, 0x01)  X(ADV, EXT, 0x02)
   WAVE_FEAT_ENUM(WAVE_ADV_FEATS)
   ```
2. **代码里打点**：
   ```c
   /* 数值：每个 ext adv event 的包计数 */
   WAVE_EVENT(ADV, EXT, 0x01, advEvtCount);

   /* 电平：adv event 窗口（开始/结束成对） */
   WAVE_LEVEL(ADV, EXT, advHandle, 0x01, 1);
   ... radio activity ...
   WAVE_LEVEL(ADV, EXT, advHandle, 0x01, 0);

   /* 多实例：per-connection 采样 */
   WAVE_EVENT_I(CONN, CE, conn->handle, 0x02, rssi);

   /* ISR 捕获 + 补发范式 */
   _u32 t = system_time();   /* radio irq 里记时刻 */
   WAVE_EVENT_AT(RF, IRQ, 0x01, t, pktType);        /* task 里带原时刻发出 */
   ```
3. **PC 端**：串口收流 → 剥 `[len][payload]` 块头 → 按 12 字节步长解析 → LEVEL 轨道画门、NUM 轨道画曲线

### 3.4 编号约定

- feature 值与 event/evt 值都从 0x01 开始（0x00 保留：feature=0 表示模块级探针）
- evt 0xFF = feature 级探针（原 marker 语义），不分配给普通事件
- instance 用运行时对象号（handle/索引），固定轨道 0~255；实例结束后其轨道自然空白
- LEVEL 必须成对（begin/end），配对键 = 完整 id（含 state 字节）；同一轨道可定义多个 state 门
- 一条 WAVE 记录表达一个事实；要区分同探针的不同实例用 instance，不要造新 event

## 四、注意事项

1. **不在 ISR 热路径使用**：一次打点 ≈ 拷贝 12B + ring buffer 操作 + task event 置位，μs 级；纳秒级时序用 DEBUG_GPIO
2. **value 是自由 32bit**：NUM 可放状态号、计数、长度、采样值；LEVEL 只有 0/1 语义
3. **带宽预算**：1M baud ≈ 有效 100KB/s ≈ 8000 条/秒；超过会触发满即丢。高频事件建议用 gpio 或降频
4. **timestamp 回绕**：u32 在当前 tick 粒度下数小时回绕一次，PC 端画图按差值处理即可
5. **与 AML 方案的映射**：本实现的 module ≈ AML 的 subsystem+module 合并、feature ≈ AML 的 feature、instance 直接同名；将来若要输出 FST 供 GTKWave 分析，PC 脚本把 LEVEL/NUM 分别映射为 FST 的 wire/real 信号即可，id 层级树照搬
6. **新增模块的 checklist**：
   - `wave_id.h` 的 `WAVE_MODULE_LIST` 加一行（module 级，中央唯一改动点）
   - 模块自己头文件加 `WAVE_XXX_FEATS` + `WAVE_FEAT_ENUM(...)`
   - 打点、跑通、让 PC 脚本 grep 到新表

## 五、文件清单

| 文件 | 内容 |
|------|------|
| `debug/wave/wave.h` | 接口：WAVE_EVENT / WAVE_EVENT_I / WAVE_LEVEL / _AT 宏与开关 |
| `debug/wave/wave_id.h` | id 布局（type/module/feature/instance/evt）、拼接宏、module 注册表 |
| `debug/wave/wave.c` | 打包 `[ts|id|value]` 12B 记录，交给 debug_port |
| `debug/wave/wave_view.py` | PC 端查看器：grep 头文件建映射表 + 串口收流解析 + matplotlib 画图 |
| `debug/README.md` | debug 子系统总览（wave 与 log/gpio/port 的关系） |
| `debug/wave/Firmware Log可视化升级方案.md` | AML 波形方案原始文档（Level/Num、instance 维度出处） |

### wave_view.py 用法

```bash
pip install pyserial matplotlib
python debug/wave/wave_view.py --port COM5 --baud 1000000 --root .
```

- `--root` 指向固件源码根目录，脚本 grep `WAVE_MODULE_LIST` 与 `WAVE_XXX_FEATS` 生成映射表（与固件同源，无需手工同步）
- LEVEL 记录画门（加粗横条），NUM 画阶梯曲线，probe（evt=0xFF）画竖线标记；只显示最近 `--window` 秒
- 上位机容错：非法块长自动重同步，未知 id 显示 `mod0xX/feat0xY`
