# Log 模块

## 简介

Log模块提供了Elite_Robots_CS_SDK中的日志功能相关设置，包括日志级别定义、日志处理器接口和日志输出功能。

## 头文件
```cpp
#include <Elite/Log.hpp>
```

## 日志级别枚举

```cpp
enum class LogLevel {
    ELI_DEBUG,
    ELI_INFO,
    ELI_WARN,
    ELI_ERROR,
    ELI_FATAL,
    ELI_NONE
};
```
- ***描述***
  
  定义了日志的级别，从低到高依次为：
  - `ELI_DEBUG`: 调试信息
  - `ELI_INFO`: 普通信息
  - `ELI_WARN`: 警告信息
  - `ELI_ERROR`: 错误信息
  - `ELI_FATAL`: 严重错误信息
  - `ELI_NONE`: 不输出任何日志

## 日志宏定义

### 调试日志
```cpp
#define ELITE_LOG_DEBUG(...)
```
- ***功能***
  
  输出调试级别日志

### 信息日志
```cpp
#define ELITE_LOG_INFO(...)
```
- ***功能***
  
  输出信息级别日志

### 警告日志
```cpp
#define ELITE_LOG_WARN(...)
```
- ***功能***
  
  输出警告级别日志

### 错误日志
```cpp
#define ELITE_LOG_ERROR(...)
```
- ***功能***
  
  输出错误级别日志

### 严重错误日志
```cpp
#define ELITE_LOG_FATAL(...)
```
- ***功能***
  
  输出严重错误级别日志

## LogHandler 类

### 简介
```cpp
class LogHandler
```
- ***描述***
  
  日志处理器抽象基类，可通过继承此类并实现`log()`方法来自定义日志处理方式。

### 构造函数
```cpp
LogHandler() = default;
```
- ***功能***
  
  默认构造函数

### 析构函数
```cpp
virtual ~LogHandler() = default;
```
- ***功能***
  
  虚析构函数

### 日志处理函数
```cpp
virtual void log(const char* file, int line, LogLevel loglevel, const char* log) = 0;
```
- ***功能***
  
  纯虚函数，用于处理日志消息

- ***参数***
  - `file`: 日志来源文件
  - `line`: 日志来源行号
  - `loglevel`: 日志级别
  - `log`: 日志消息内容

## 全局函数

### 注册日志处理器
```cpp
void registerLogHandler(std::unique_ptr<LogHandler> hanlder);
```
- ***功能***
  
  注册自定义的日志处理器

- ***参数***
  - `hanlder`: 指向新日志处理器对象的unique_ptr

### 注销日志处理器
```cpp
void unregisterLogHandler();
```
- ***功能***
  
  注销当前日志处理器，将启用默认日志处理器

### 设置日志级别
```cpp
void setLogLevel(LogLevel level);
```
- ***功能***
  
  设置日志级别，低于此级别的日志将不会被输出

- ***参数***
  - `level`: 要设置的日志级别

### 日志输出函数
```cpp
void log(const char* file, int line, LogLevel level, const char* fmt, ...);
```
- ***功能***
  
  内部使用的日志输出函数，建议使用宏而非直接调用此函数

- ***参数***
  - `file`: 日志来源文件
  - `line`: 日志来源行号
  - `level`: 日志级别
  - `fmt`: 格式化字符串
  - `...`: 可变参数，用于格式化字符串

## 使用示例

```cpp
// 自定义日志处理器
class MyLogHandler : public ELITE::LogHandler {
public:
    void log(const char* file, int line, ELITE::LogLevel loglevel, const char* log) override {
        // 自定义日志处理逻辑
    }
};

// 注册自定义日志处理器
ELITE::registerLogHandler(std::make_unique<MyLogHandler>());

// 设置日志级别
ELITE::setLogLevel(ELITE::LogLevel::ELI_INFO);

// 使用日志宏
ELITE_LOG_INFO("This is an info message");
ELITE_LOG_ERROR("Error code: %d", 404);
```