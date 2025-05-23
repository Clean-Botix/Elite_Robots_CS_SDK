# VersionInfo Class

## Introduction
The VersionInfo class is used to contain and manage version information, providing functions for storing, comparing, and converting version numbers.

## Header File
```cpp
#include <Elite/VersionInfo.hpp>
```

## Interfaces

### Constructor
```cpp
constexpr VersionInfo(int ma, int mi, int bug, int bui)
```
- ***Function***
Creates a VersionInfo object with the specified version number components.
- ***Parameters***
    - ma: Major version number
    - mi: Minor version number
    - bug: Bug fix number
    - bui: Build number

---

### String Constructor
```cpp
explicit VersionInfo(const std::string& version)
```
- ***Function***
Creates a VersionInfo object from a version string.
- ***Parameters***
    - version: A version string in the format of "major.minor.bugfix.build".

---

### Default Constructor
```cpp
VersionInfo()
```
- ***Function***
Creates a default VersionInfo object (all version number components are initialized to 0).

---

### Convert to String
```cpp
std::string toString() const
```
- ***Function***
Converts the version information into a string.
- ***Return Value***: A string in the format of "major.minor.bugfix.build".

---

### Parse from String
```cpp
static VersionInfo fromString(const std::string& str)
```
- ***Function***
Parses the version information from a string.
- ***Parameters***
    - str: The version string to be parsed.
- ***Return Value***: The parsed VersionInfo object.

---

## Operator Overloading

### Assignment Operator
```cpp
VersionInfo& operator=(const VersionInfo&)
```
- ***Function***
Performs the version information assignment operation.

---

### Equality Comparison Operator
```cpp
bool operator==(const VersionInfo& v) const
```
- ***Function***
Compares whether two versions are equal.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if equal, false otherwise.

---

### Inequality Comparison Operator
```cpp
bool operator!=(const VersionInfo& v) const
```
- ***Function***
Compares whether two versions are not equal.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if not equal, false otherwise.

---

### Greater Than Comparison Operator
```cpp
bool operator>(const VersionInfo& v) const
```
- ***Function***
Compares whether the current version is greater than the specified version.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if greater, false otherwise.

---

### Greater Than or Equal to Comparison Operator
```cpp
bool operator>=(const VersionInfo& v) const
```
- ***Function***
Compares whether the current version is greater than or equal to the specified version.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if greater than or equal, false otherwise.

---

### Less Than Comparison Operator
```cpp
bool operator<(const VersionInfo& v) const
```
- ***Function***
Compares whether the current version is less than the specified version.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if less, false otherwise.

---

### Less Than or Equal to Comparison Operator
```cpp
bool operator<=(const VersionInfo& v) const
```
- ***Function***
Compares whether the current version is less than or equal to the specified version.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if less than or equal, false otherwise.

---

### Constant Expression Equality Comparison
```cpp
constexpr bool operator==(VersionInfo& v) const
```
- ***Function***
Performs an equality comparison in the form of a constant expression.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if equal, false otherwise.

---

### Constant Expression Inequality Comparison
```cpp
constexpr bool operator!=(VersionInfo& v) const
```
- ***Function***
Performs an inequality comparison in the form of a constant expression.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if not equal, false otherwise.

---

### Constant Expression Greater Than Comparison
```cpp
constexpr bool operator>(VersionInfo& v) const
```
- ***Function***
Performs a greater than comparison in the form of a constant expression.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if greater, false otherwise.

---

### Constant Expression Greater Than or Equal to Comparison
```cpp
constexpr bool operator>=(VersionInfo& v) const
```
- ***Function***
Performs a greater than or equal to comparison in the form of a constant expression.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if greater than or equal, false otherwise.

---

### Constant Expression Less Than Comparison
```cpp
constexpr bool operator<(VersionInfo& v) const
```
- ***Function***
Performs a less than comparison in the form of a constant expression.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if less, false otherwise.

---

### Constant Expression Less Than or Equal to Comparison
```cpp
constexpr bool operator<=(VersionInfo& v) const
```
- ***Function***
Performs a less than or equal to comparison in the form of a constant expression.
- ***Parameters***
    - v: The version to be compared.
- ***Return Value***: Returns true if less than or equal, false otherwise.

---

## Member Variables

### Major Version Number
```cpp
uint32_t major = 0
```

### Minor Version Number
```cpp
uint32_t minor = 0
```

### Bug Fix Number
```cpp
uint32_t bugfix = 0
```

### Build Number
```cpp
uint32_t build = 0
```

## Global Constants

### SDK Version Information
```cpp
constexpr VersionInfo SDK_VERSION_INFO(ELITE_SDK_VERSION_MAJOR, ELITE_SDK_VERSION_MINOR, ELITE_SDK_VERSION_BUGFIX, 0)
```
- ***Function***
A constant representing the SDK version information.

---