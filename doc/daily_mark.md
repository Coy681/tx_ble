# 20250818

坚持，耐心，专注，高要求，高品质

做难的事情

把事情做复杂很简单，但是把事情做简单很复杂，以此明志

# 20250819

高度的模块化，需要什么调用什么，没有一行多余的代码

人类经过千万年的时间，走到今天，信念和坚持的力量无与伦比

BLE的版本，应该是一个确定的值，BLE的具体特性是由Feature bit来确定的，更高版本的version意味着可裁剪性更高，有更多BLE功能可供选择，但不意味着想比于低版本的功能更全面，需要透彻的理解


C语言语法，下面是对的
```
    struct 
    {
        _u8 index;
        _u8 addrType;
        _u8 addr[6];
    }filterAcceptList[BLE_FILTER_ACCEPT_LIST_SIZE];
```
对比，下面是不对的，是因为数组不属于结构体声明的一部分，结构体声明很确定，就是 struct 结构体名 { 成员列表 };不能插入其他定义，如数组的声明
```
    struct filterAcceptList[BLE_FILTER_ACCEPT_LIST_SIZE]
    {
        _u8 index;
        _u8 addrType;
        _u8 addr[6];
    }
```
# 20250820

BLE HCI命令，特别是获取信息类别的命令，可以直接用宏来解决，controller宏是表达controller特性和配置的一种方法。

# 20250821

判断力，实践性，耐心，见识，希望我的焦虑越来越少

# 20250825

## BLE Active Clock Accuracy

1. 含义：系统在PHY Active状态的时钟精度，从开始收发包到收发包结束之间的PHY活动，称为Active状态
2. 核心指标
    - 瞬时抖动在±2us以内
    - 时钟偏移在±50ppm以内
## BLE Sleep Clock Accuracy
1. 含义：系统在PHY Inactive状态的时钟精度，从上一次停止活动到下一次开始活动，称为Sleep状态
2. 核心指标
    - 瞬时抖动在±16us以内
    - 时钟偏移在±500ppm以内

## Window Widen

发射机的绝对时间偏移：
TransmitterAllowance = (txCA÷1000000)*(receiveWindowEnd - timeOfLastSync) + Jus

window widening = TransmitterAllowance+receiverAllowance
receiverAllowance悠接收机自己确定


# 20250906

对象定义是很讲究的，如何做到精准，空间利用率最大化，在保证实现完整功能的前提下，使得资源占用最少，是非常讲究的

# 20251009

设计模式：
- 观察者模式：核心目的是解决对象间的一对多依赖关系
- 责任链模式：核心价值在于解耦请求的发送者和接收者

# 20251016

- ACL DATA从HOST下发会存储到Conctroller的HCI ACL DATA BUFFER里面，同理，ISO DATA从HOST下发会存储到Conctroller的HCI ISO DATA BUFFER里面
- Host通过HCI下发的ACL DATA和ISO DATA会通过拆包拆到Controller的PHY TX FIFO里面

# 20251125

对于Subrate和Latency的理解

## Subrate的意义

对于连续的connection event,central和peripheral可以协商，从指定的事件开始，只再固定的事件中活动，活动的规则如下

subrateBaseEvent   - 即subrate生效的起始事件
subrateEvent       - subrate生效的连接事件，注意，只有一个事件
subrateFactor      - 两个subrateEvent的间隔，单位是connection event，范围是1-500，为1的时候相当于subrate不生效
continuationEvent  - 当前事件之前continuationEvent个事件中有非零数据包的事件称为continuationEvent，这个比较难以理解

具体参考core sepc 6.0，page 3091

## Subrate不生效的时候，peripheral latency的意义：

peripheral可以忽略central的事件数(connection event)，注意是忽略,从某个sync成功的事件开始，peripheral可以忽略latency number个数量的主机事件

注意：**subrate不生效的时候，peripheral latency生效的基础是connnection event**

## Subrate生效的时候，和peripheral latency结合起来的意义

peripheral可以忽略central的subrate事件数，相当于在基础peripheral latency的基础上叠加了一个subrateEvent的概念    

注意：**subrate生效的时候，peripheral latency生效的基础是subrate event**

# 20251213

phy channel在ACL和CIS/BIS/PDA事件当中的不同

跳频算法1只能计算事件的phy channel，无法计算子事件的phy channel，所以ACL如果有more data，都是在当前事件的phy channel当中完成的，即channel不变

跳频算法2既能计算事件的phy channel，也能计算子事件的phy channel，所以CIS/BIS/PDA的每个subevent的channel都不一样，既channel会变化，而且跳频算法2的特性会使得每两个相邻的subevent phy channel不同。

# 20251216

读Bluetooeh Core有感

## Host架构

### Channel manager

channel manager主要用来创建，管理，关闭L2CAP Channel,主要职责

 - 和对端设备写作，创建L2CAP Channel
 - 和本地logic link manager写作，创建新的logic link,从而为当前需要传输的数据进行服务质量保障

### L2CAP Resource Manager

负责管理L2CAP PDU分片向Baseband的提交顺序，确保各个L2CAP Channel得到应有的服务质量保障。
  - 由于controller的缓冲不是无限的，L2CAP Resource Manager需要为多个L2CAP Channel提供带宽和时序协调，确保某个L2CAP的数据不会被Controller拒绝
  - HCI通道的流量不是无限的，不能承载无限的数据流量
  - 提供流量合规性校验，确保L2CAP Channel的SDU符合规定的Qos范围

### Security Maganer Protocol

SMP是一个对等协议，没有主从之分。主要用来产生encryption keys和identity keys，在一个固定的L2CAP Channel之上，主要职责

 - 存储encryption keys和identity keys
 - 产生随机地址(random address)
 - 将随机地址解析为已知设备
 - 与controller协调，在加密和认证过程中提供预存密钥

 SMP在LE 系统里面存在于Host，为了减少单LE Controller的资源开销
 SMP在BR/EDR系统里面存在于controller

 ### Attribute Protocol

 ATT协议是ATT Client和ATT Server之间的一个对等协议，ATT Client可以通过固定的L2CAP通道访问ATT Server

  - ATT Client可以发送命令(commands)，请求(request)，确认(confirmation)
  - ATT Server可以发送响应(response)，通知(notification)，指示(indication)

### Generic Attribute Profile

GATT协议定义了ATT Server中Services，Characteristics,Attributes的层级结构，并且提供了基于service characteristics和attributes的发现，读写，指示等功能接口
GATT只用于LE设备

### Generic Access Profile

GAP承载了所有蓝牙设备的基础功能，例如传输层，协议，应用配置使用的工作模式和流程，主要功能包括
 - 设备发现
 - 连接模式配置
 - 安全管控
 - 身份认证
 - 关键模型建立
 - 服务发现

 ### Service Discovery Protocol

 SDP协议提供了一种机制，允许Client根据服务的特定属性搜索所需服务，包括基于服务类型的检索以及对完整数据库的遍历。该服务用于BR/EDR设备

 值得注意的是，SDP并未定义发现服务之后访问具体服务数据的方法-具体的访问方式应该由服务自身来规定。

## Controller架构

### Device manager

设备管理是baseband中负责控制蓝牙设备整体行为的功能模块，承担与数据传输无关的操作，例如

- 查询附近设备的存在状态
- 与蓝牙设备建立连接
- 配置本地蓝牙设备状态，如可连接或者可被发现

device manager 需要向baseband resource controller申请控制权限，以执行自身功能

### Link manager

Link Manager负责创建，修改和释放logic link。Link manager通过使用BR/EDR系统中的LMP和LE系统中的LL来实现。

### Baseband resource manager

Baseband resource manager负责管控对射频介质的所有访问，主要有两项核心功能

- 调度器，为所有约定的交互(连接)分配物理层信道的时隙，可以理解为调度器
- 为所有实体协商访问质量，以便实现所有实体预期的服务质量，可以理解为优先级分配

### Link Controller

主要负责对physical channel,logical transport,logical link层的蓝牙数据包进行加解密

### PHY

主要负责在Physical channel中传输和收包、

### Isochronous adaptation layer

IAL可以使上层通过一个灵活的方式向链路层发送或者收取数据包，上层数据的size和interval可以和LL层的数据size和Interval不同

ISO使用数据分片重组以及分段重装操作，将上层数据单元转换为下层数据单元。

### Channel Sounding 

Channel Sounding 模块负责创建，修改，释放channel sounding physical links。


## 今日问答
问：传统广播和扩展广播，成功建立连接之后，是否都要退出广播状态，广播需要停止，直到手动开启？

答：LL状态机有个状态上的改变，即从广播态转向连接态，在这个状态的改变过程中，设备应该退出广播态。无论是传统广播还是扩展广播，设备在进入连接态时都应该退出广播态。

# 20251217 

方向和努力同样重要，认知能力是衡量世界的尺子，知道好坏对错要比单纯的做事更重要

认知！！！
认知！！！
认知！！！

# 20251218

Bluetooth Core Traffic Bears:

BR/EDR ACL可以承载
  - ACL-C:ACL的控制数据
  - ACL-U:ACL的用户数据
    - L2CAP PDU：L2CAP层数据
    - User DATA：用户数据

LE可以承载

  - LE-C:LE的控制数据
  - LE-U:LE的用户数据
    - L2CAP PDU：L2CAP层数据
    - User DATA: 用户数据

SCO/ESCO可以承载
  - SCO-S:恒定速率的用户同步数据
  - ESCO-S：恒定速率的用户同步数据

CIS可以承载
 
  - LE-S: unframed同步数据
  - LE-F：framed同步数据

BIS可以承载

  - LE-S: unframed同步数据
  - LE-F：framed同步数据
  - LEB-C：LE Broadcast 控制数据


在用户角度来看，数据分为以下类型

  - 高层协议数据 - Higher Layer Protocol Signalling，可以理解为Profile协议数据
  - 异步可靠用户数据 - Reliable Asynchronous Framed User Data，可以理解为用户数据
  - 高层同步数据- Higher Layer Framed Isochronous User Data，可以理解为用户的framed音频数据
  - 恒定速率同步数据 - Constant Rate Isochronous User Data，可以理解为unframed音频数据


## framed data traffic

L2CAP层提供了一种供同步和异步数据面向帧的传输方式，这种方式不限制每帧的具体帧长，且不需要应用额外添加额外帧信息

## unframed data traffic

应用层不需要使用framed方式传输的情况

 - 流内帧界定 - 数据是在帧内嵌入特定标识，用以区分数据段边界，不需要L2CAP层的帧封装功能。
 - 纯流式数据 - 例如音频流，数据没有明显的边界

 这种情况应用层可以绕过L2CAP层直接使用baseband逻辑链路

 注意：CIS里面的framed数据是在IAL层进行的，不通过L2CAP层。

 LE不支持unframed data traffic的模式


## 传输可靠性

### BR/EDR传输可靠性

#### Baseband传输可靠性

     - 包头使用FEC(前向纠错码)
     - 使用HEC(头部差错校验)
     - 部分Baseband包还使用了CRC校验
     - ESCO使用重传来保证传输可靠性
 
#### L2CAP传输可靠性

     - L2CAP层使用了额外的差错控制功能


### LE传输可靠性

使用24-bit的CRC来提供差错检测，由于CRC的长度足够，不需要L2CAP层额外提供差错检测


## Bluetooth PHY Channels

### BT Classic Channels

频点从2402-2480，共79个信道，所有模式都使用这79个信道

#### Basic piconet channel

跳频序列由Central的地址决定
跳频相位由主机的时钟决定
所有从机跟随主机的跳频序列

Central只从偶数slot上发包，每个slot跳一次频
Central最多占据5个slot
一个slot时间是625us
一个Basic piconet只允许有一个central
每个slot都要重新跳频
#### Adapted piconet channel

和basic piconet channel的区别主要在两个方面

 - 从机频点跟随主机
 - 信道数量可少于79个

总结  basic piconet channel是全频、主从独立跳频、双向有限连接的基础模式；
     Adapted piconet channel是减频、主从同频、支持单向无限广播的优化模式；
     二者时序兼容、拓扑一致。

Adapted piconet channel是一个feature，刚开始连接使用的是Basic piconet channel，使用Adapted piconet channel需要经过主从交互开启

#### Inquiry scan channel

 使用79个信道中32个信道的子集

 Inquiry 主动扫、高速率、全覆盖；
 Inquiry Scan 被动听、周期性、省功耗；
 二者通过专用信道与接入码配合，一快一慢，实现设备快速发现与定向寻址。

#### Page scan channel

page channel使用32个信道，这32个channel基于目标地址(page scan的设备)动态生成

page scan设备基于自身地址，生成专有的access code，在page channels上周期性的监听
page设备如果知道目标设备时钟，可以精准预测目标channel，快速建立连接
page设备如果不知道目标设备时钟，需要快速逐频发送page

### LE PHY Channels

五种LE PHY Channels

- LE piconet physical channel
- LE advertising physical channel
- LE periodic physical channel
- LE isochronous physical channel
- LE Channel Sounding physical channel

一个设备在同一时间只能使用一种类型的信道

#### LE piconet physical channel

37个channel，跳频序列



#### LE advertising physical channel

分为两类

 - primary advertising channels,可以使用LE 1M PHY和LE Coded PHY
 - secondary advertising channels