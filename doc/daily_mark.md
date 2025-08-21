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