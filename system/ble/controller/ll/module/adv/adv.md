# BLE Adv开发随笔

## LOGIC

## SCH

## PHY
### Issue record-20250811

**问题现象:** 广播模式下发包异常，硬件信号只有TX EN，没有TX ON

**问题分析:** 平行尝试各种RF Setting，镜像配置，逐步发现问题，使用手动造的rf packet没问题，可以正常发出，但使用模块生成的rf packet就会异常，只有TX EN，没有TX ON

**问题定位:** 发现模块内设置的rf tx address，没有按照四字节对齐，判断是硬件RF模块不支持非对齐RF地址导致的问题。

**问题解决:** 将模块内RF地址进行4字节对齐，RF模块工作正常

