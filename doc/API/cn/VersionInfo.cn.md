# VersionInfo 类

## 简介

VersionInfo类用于包含和管理版本信息，提供了版本号的存储、比较和转换功能。

## 头文件
```cpp
#include <Elite/VersionInfo.hpp>
```

## 接口

### 构造函数
```cpp
constexpr VersionInfo(int ma, int mi, int bug, int bui)
```
- ***功能***

    使用指定的版本号组件创建VersionInfo对象

- ***参数***

    - ma：主版本号
    - mi：次版本号
    - bug：补丁号
    - bui：构建号

---

### 字符串构造函数
```cpp
explicit VersionInfo(const std::string& version)
```
- ***功能***

    从版本字符串创建VersionInfo对象

- ***参数***

    - version：格式为"major.minor.bugfix.build"的版本字符串

---

### 默认构造函数
```cpp
VersionInfo()
```
- ***功能***

    创建默认的VersionInfo对象(所有版本号组件初始化为0)

---

### 转换为字符串
```cpp
std::string toString() const
```
- ***功能***

    将版本信息转换为字符串

- ***返回值***：格式为"major.minor.bugfix.build"的字符串

---

### 从字符串解析
```cpp
static VersionInfo fromString(const std::string& str)
```
- ***功能***

    从字符串解析版本信息

- ***参数***

    - str：要解析的版本字符串

- ***返回值***：解析得到的VersionInfo对象

---

## 运算符重载

### 赋值运算符
```cpp
VersionInfo& operator=(const VersionInfo&)
```
- ***功能***

    版本信息赋值操作

---

### 相等比较运算符
```cpp
bool operator==(const VersionInfo& v) const
```
- ***功能***

    比较两个版本是否相等

- ***参数***

    - v：要比较的版本

- ***返回值***：相等返回true，否则返回false

---

### 不等比较运算符
```cpp
bool operator!=(const VersionInfo& v) const
```
- ***功能***

    比较两个版本是否不等

- ***参数***

    - v：要比较的版本

- ***返回值***：不等返回true，否则返回false

---

### 大于比较运算符
```cpp
bool operator>(const VersionInfo& v) const
```
- ***功能***

    比较当前版本是否大于指定版本

- ***参数***

    - v：要比较的版本

- ***返回值***：大于返回true，否则返回false

---

### 大于等于比较运算符
```cpp
bool operator>=(const VersionInfo& v) const
```
- ***功能***

    比较当前版本是否大于等于指定版本

- ***参数***

    - v：要比较的版本

- ***返回值***：大于等于返回true，否则返回false

---

### 小于比较运算符
```cpp
bool operator<(const VersionInfo& v) const
```
- ***功能***

    比较当前版本是否小于指定版本

- ***参数***

    - v：要比较的版本

- ***返回值***：小于返回true，否则返回false

---

### 小于等于比较运算符
```cpp
bool operator<=(const VersionInfo& v) const
```
- ***功能***

    比较当前版本是否小于等于指定版本

- ***参数***

    - v：要比较的版本

- ***返回值***：小于等于返回true，否则返回false

---

### 常量表达式相等比较
```cpp
constexpr bool operator==(VersionInfo& v) const
```
- ***功能***

    常量表达式版本的相等比较

- ***参数***

    - v：要比较的版本

- ***返回值***：相等返回true，否则返回false

---

### 常量表达式不等比较
```cpp
constexpr bool operator!=(VersionInfo& v) const
```
- ***功能***

    常量表达式版本的不等比较

- ***参数***

    - v：要比较的版本

- ***返回值***：不等返回true，否则返回false

---

### 常量表达式大于比较
```cpp
constexpr bool operator>(VersionInfo& v) const
```
- ***功能***

    常量表达式版本的大于比较

- ***参数***

    - v：要比较的版本

- ***返回值***：大于返回true，否则返回false

---

### 常量表达式大于等于比较
```cpp
constexpr bool operator>=(VersionInfo& v) const
```
- ***功能***

    常量表达式版本的大于等于比较

- ***参数***

    - v：要比较的版本

- ***返回值***：大于等于返回true，否则返回false

---

### 常量表达式小于比较
```cpp
constexpr bool operator<(VersionInfo& v) const
```
- ***功能***

    常量表达式版本的小于比较

- ***参数***

    - v：要比较的版本

- ***返回值***：小于返回true，否则返回false

---

### 常量表达式小于等于比较
```cpp
constexpr bool operator<=(VersionInfo& v) const
```
- ***功能***

    常量表达式版本的小于等于比较

- ***参数***

    - v：要比较的版本

- ***返回值***：小于等于返回true，否则返回false

---

## 成员变量

### 主版本号
```cpp
uint32_t major = 0
```

### 次版本号
```cpp
uint32_t minor = 0
```

### 补丁号
```cpp
uint32_t bugfix = 0
```

### 构建号
```cpp
uint32_t build = 0
```

## 全局常量

### SDK版本信息
```cpp
constexpr VersionInfo SDK_VERSION_INFO(ELITE_SDK_VERSION_MAJOR, ELITE_SDK_VERSION_MINOR, ELITE_SDK_VERSION_BUGFIX, 0)
```
- ***功能***

    表示SDK版本信息的常量

--- 
