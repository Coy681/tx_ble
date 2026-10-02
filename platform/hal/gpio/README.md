# GPIO HAL

GPIO 硬件抽象层：逻辑 pin 组 + 板级 pin 表模式。上层只依赖 `platform/hal/gpio.h`，只见抽象 pin 枚举（`hal_gpio_pin_e`），不接触芯片 pin 编码。

## 职责边界

负责：
- 逻辑 pin 组的注册与初始化（方向/输出使能/输入禁止配置）
- 按组索引 + 组内序号的高/低/翻转操作，RAM 代码路径

不负责：
- 用哪些 pin 的决策（板级配置在 `debug/gpio`、`service/led` 各自的 pin 表）
- 中断输入、复用功能切换、上下拉配置（需要时按同一抽象模式扩展）
- 引脚冲突检查（见注意事项）

## 对外接口

| 函数 | 说明 | 调用上下文 |
|------|------|-----------|
| `hal_gpio_init` | 注册一个逻辑 pin 组，返回组索引（-1 失败） | task（各模块 init 阶段调用一次） |
| `hal_gpio_set_high/low/toggle` | 操作组内第 pin 个 pin | task / ISR（`_RAM_CODE`） |

## 用法示例

```c
#include "platform/hal/gpio.h"

/* board config: which physical pins are leds */
static const hal_gpio_pin_e ledPinMap[LED_NUM] = {
    HAL_GPIO_PD0, HAL_GPIO_PD1, HAL_GPIO_PE6, HAL_GPIO_PE7
};

static int halGroup = -1;

void led_hw_init(void)
{
    halGroup = hal_gpio_init(ledPinMap, LED_NUM);
    ASSERT(halGroup >= 0);      /* init 失败必须暴露 */
}

void led_set(_u8 n, int on)
{
    if (on) { hal_gpio_set_high(halGroup, n); }
    else    { hal_gpio_set_low(halGroup, n); }
}
```

## 内部结构

- 抽象头：`platform/hal/gpio.h` —— `hal_gpio_pin_e` 顺序枚举（PA0~PF7），不使用芯片编码
- 实现：`platform/telink/TLSR9528/hal/gpio.c` —— 通过 `hal_gpio_pin_lut[]` 查表将抽象 pin 映射为芯片 pin（`GPIO_PBx`），芯片 pin 常量全部固化在此文件
- 依赖（向下）：telink SDK driver（`gpio_function_en`、`gpio_set_high_level` 等）
- 被依赖（向上）：`debug/gpio`、`service/led`

## 换芯片时的改动范围

芯片差异全部收口在"抽象枚举 + LUT + 实现"三处，均在 platform 内，上层零改动：

| 改动点 | 文件 | 内容 |
|--------|------|------|
| 1. 重排抽象枚举 | 新芯片 `platform/<vendor>/<chip>/hal/gpio.h` | `hal_gpio_pin_e` 按新芯片 pin 布局重排（如 STM32 每口 16 pin 就每组列 16 个；芯片专属 pin 不纳入抽象） |
| 2. 重写映射表 | 新芯片 `hal/gpio.c` | `hal_gpio_pin_lut[HAL_GPIO_PIN_NUM]` 抽象枚举 → 芯片 pin 编码，全模块唯一出现芯片 pin 常量的地方 |
| 3. 配置/操作实现 | 同上 | init 配置、set_high/low/toggle 调新芯片驱动 API |
| 4. 上层 | 零修改 | 调用方只写 `HAL_GPIO_PB0` 等符号名，枚举数值怎么排不感知 |

要点：

- **枚举成员名跟着芯片走**：成员名借用 PA0/PB7 组号标记是可读性妥协，换芯片后重排（LPC 风格 P0_0 也可以）。上层若引用新芯片不存在的 pin（如 `HAL_GPIO_PF7`），编译期直接报错，比静默错误安全
- **pin 表持有者零改动**：板级配置（`debugPinMap`、`ledPinMap`）里的符号名与新芯片抽象枚举对上即可，这正是分层买到的隔离
- 新增能力（中断回调、上下拉、复用选择）时，先在抽象层定义接口与独立枚举值，实现层查表映射，禁止上层直接调芯片驱动

## 注意事项

- `hal_gpio_init` 对每个 pin 做范围校验（`>= HAL_GPIO_PIN_NUM` 返回 -1），返回值调用方必须 ASSERT
- 同一 pin 被两个 group init 无冲突检查，板级配置需人工保证不重复分配
- 操作函数是 `_RAM_CODE`，实现内一次 LUT 下标，可放心在中断时序 trace 中高频调用
