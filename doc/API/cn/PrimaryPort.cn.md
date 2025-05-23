# Primary Port

## 简介

SDK中提供了与机器人30001端口连接、发送脚本的接口，以及解析30001端口数据的框架。SDK中只有部分数据包的解析，如果要解析其他数据包，需要手动编写解析代码。

# PrimaryPortInterface 类

## 简介

此接口提供了与机器人primary port接口通讯的方法。

## PrimaryPortInterface 头文件
```cpp
#include <Elite/PrimaryPortInterface.hpp>
```

## PrimaryPortInterface 类的构造函数

### ***构造函数***
```cpp
PrimaryPortInterface::PrimaryPortInterface()
```
- ***功能***

    初始化数据。注意，此构造函数不会连接机器人。

---

## 通讯

### ***连接***
```cpp
bool connect(const std::string& ip, int port = PRIMARY_PORT)
```

- ***功能***

    连接到机器人的30001端口（默认）。

- ***参数***
    - ip：机器人端口

    - timeout_ms：设置机器人读取下一条指令的超时时间，小于等于0时会无限等待。

- ***返回值***：成功返回 true，失败返回 false。

- ***注意***
    1. 如果重复调用此函数而没有调用disconnect，那么会断开原来的连接。
    2. 重复调用此函数的频率最好不要超过2HZ。

---

### ***断开连接***
```cpp
void disconnect()
```
- ***功能***

    断开与机器人的连接

- ***注意***

    建议在调用此函数之后进行500ms左右延迟再调用connect。

---

### ***发送脚本***
```cpp
bool sendScript(const std::string& script)
```
- ***功能***

    向机器人发送可执行脚本

- ***参数***
    - script：待发送的脚本。

- ***返回值***：发送成功返回 true，失败返回 false。

---

### 获取数据包
```cpp
bool getPackage(std::shared_ptr<PrimaryPackage> pkg, int timeout_ms)
```
- ***功能***

    获取机器人数据包并解析

- ***参数***
    - pkg：待获取的数据包

    - timeout_ms：等待超时时间。

- ***返回值***：获取成功返回 true，失败返回 false。

---

### 获取本地的IP地址
```cpp
std::string getLocalIP()
```
- ***功能***

    获取本机IP地址。

- ***返回值***：本机IP地址。如果为空，与机器人30001的连接则存在问题。

---

# PrimaryPackage 类

## 简介

此类主要用于被继承，以此来获取Primary端口数据。

SDK中没有提供全部的数据包解析，如果需要其他的数据包，可以参考`RobotConfPackage.hpp`中`KinematicsInfo`的实现方式来编写自己需要的数据包解析。报文的格式参考Elite官方文档“CS_用户手册_机器人状态报文.xlsx”。

## PrimaryPackage 头文件
```cpp
#include <Elite/PrimaryPackage.hpp>
```

## PrimaryPackage 类的构造函数
### ***构造函数***
```cpp
PrimaryPackage::PrimaryPackage(int type)
```
- ***功能***

    初始化数据

- ***参数***
    - type：数据包类型（参考Elite官方文档：CS_用户手册_机器人状态报文.xlsx）

---

## 纯虚函数

### 解析报文
```cpp
void parser(int len, const std::vector<uint8_t>::const_iterator& iter)
```
- ***功能***

     由子类来完成具体实现，解析Primary端口机器人状态报文的子报文。当子类实例作为参数传入`PrimaryPortInterface::getPackage()`中会被调用。
    
- ***参数***
    - len：子报文的长度。

    - iter：子报文在整个报文中的位置。

---

## 其余

### ***获取报文类型***
```cpp
int getType()
```
- ***功能***

    获取数据包的类型。

- ***返回值***：数据包类型。

---

## SDK内部使用
### 等待更新
```cpp
bool waitUpdate(int timeout_ms)
```
- ***功能***

    等待数据包数据更新。在getPackage（）函数中调用。

- ***参数***
    - timeout_ms：超时时间
    
- ***返回值***：未超时返回 true，超时返回 false。

---

# KinematicsInfo 类

## 简介

机器人配置数据中运动学的数据包解析。PrimaryPackage 是此接口的父类，除了 PrimaryPackage 类中的方法以外，此接口主要内容是机器人的DH参数。

## KinematicsInfo 头文件

```cpp
#include <Elite/RobotConfPackage.hpp>
```

## DH参数

- `vector6d_t dh_a_`

- `vector6d_t dh_d_`

- `vector6d_t dh_alpha_`
