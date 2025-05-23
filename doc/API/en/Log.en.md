# Log Module

## Introduction
The Log module provides settings related to the logging functionality in the Elite_Robots_CS_SDK, including log level definitions, log handler interfaces, and log output functions.

## Header File
```cpp
#include <Elite/Log.hpp>
```

## Log Level Enumeration

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
- ***Description***
Defines the log levels, from lowest to highest:
  - `ELI_DEBUG`: Debug information
  - `ELI_INFO`: General information
  - `ELI_WARN`: Warning information
  - `ELI_ERROR`: Error information
  - `ELI_FATAL`: Severe error information
  - `ELI_NONE`: Do not output any logs

## Log Macro Definitions

### Debug Log
```cpp
#define ELITE_LOG_DEBUG(...)
```
- ***Function***
Outputs debug-level logs.

### Information Log
```cpp
#define ELITE_LOG_INFO(...)
```
- ***Function***
Outputs information-level logs.

### Warning Log
```cpp
#define ELITE_LOG_WARN(...)
```
- ***Function***
Outputs warning-level logs.

### Error Log
```cpp
#define ELITE_LOG_ERROR(...)
```
- ***Function***
Outputs error-level logs.

### Severe Error Log
```cpp
#define ELITE_LOG_FATAL(...)
```
- ***Function***
Outputs severe error-level logs.

## LogHandler Class

### Introduction
```cpp
class LogHandler
```
- ***Description***
An abstract base class for log handlers. You can inherit from this class and implement the `log()` method to customize the log handling method.

### Constructor
```cpp
LogHandler() = default;
```
- ***Function***
Default constructor.

### Destructor
```cpp
virtual ~LogHandler() = default;
```
- ***Function***
Virtual destructor.

### Log Handling Function
```cpp
virtual void log(const char* file, int line, LogLevel loglevel, const char* log) = 0;
```
- ***Function***
A pure virtual function used to handle log messages.
- ***Parameters***
  - `file`: The file where the log originated.
  - `line`: The line number where the log originated.
  - `loglevel`: The log level.
  - `log`: The content of the log message.

## Global Functions

### Register Log Handler
```cpp
void registerLogHandler(std::unique_ptr<LogHandler> hanlder);
```
- ***Function***
Registers a custom log handler.
- ***Parameters***
  - `hanlder`: A unique_ptr pointing to the new log handler object.

### Unregister Log Handler
```cpp
void unregisterLogHandler();
```
- ***Function***
Unregisters the current log handler and enables the default log handler.

### Set Log Level
```cpp
void setLogLevel(LogLevel level);
```
- ***Function***
Sets the log level. Logs below this level will not be output.
- ***Parameters***
  - `level`: The log level to be set.

### Log Output Function
```cpp
void log(const char* file, int line, LogLevel level, const char* fmt, ...);
```
- ***Function***
An internally used log output function. It is recommended to use macros instead of calling this function directly.
- ***Parameters***
  - `file`: The file where the log originated.
  - `line`: The line number where the log originated.
  - `level`: The log level.
  - `fmt`: The format string.
  - `...`: Variable arguments for the format string.

## Usage Example

```cpp
// Custom log handler
class MyLogHandler : public ELITE::LogHandler {
public:
    void log(const char* file, int line, ELITE::LogLevel loglevel, const char* log) override {
        // Custom log handling logic
    }
};

// Register the custom log handler
ELITE::registerLogHandler(std::make_unique<MyLogHandler>());

// Set the log level
ELITE::setLogLevel(ELITE::LogLevel::ELI_INFO);

// Use log macros
ELITE_LOG_INFO("This is an info message");
ELITE_LOG_ERROR("Error code: %d", 404);
```