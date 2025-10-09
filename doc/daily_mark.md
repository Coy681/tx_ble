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

