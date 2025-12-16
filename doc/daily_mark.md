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

# 20251216

疑问，传统广播和扩展广播，成功建立连接之后，是否都要退出广播状态，广播需要停止，直到手动开启？

答案：

读Bluetooeh Core有感