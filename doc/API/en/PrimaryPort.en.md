# Primary Port

## Introduction
The SDK provides interfaces for connecting to the robot's port 30001, sending scripts, and a framework for parsing the data from port 30001. Only partial data packets are parsed in the SDK. If you want to parse other data packets, you need to write the parsing code manually.

# PrimaryPortInterface Class

## Introduction
This interface provides methods for communicating with the robot's primary port interface.

## Header File of PrimaryPortInterface
```cpp
#include <Elite/PrimaryPortInterface.hpp>
```

## Constructor of the PrimaryPortInterface Class

### ***Constructor***
```cpp
PrimaryPortInterface::PrimaryPortInterface()
```
- ***Function***
Initializes the data. Note that this constructor does not connect to the robot.

---

## Communication

### ***Connection***
```cpp
bool connect(const std::string& ip, int port = PRIMARY_PORT)
```
- ***Function***
Connects to the robot's port 30001 (by default).
- ***Parameters***
    - ip: The IP address of the robot.
    - timeout_ms: Sets the timeout for the robot to read the next instruction. If it is less than or equal to 0, it will wait indefinitely.
- ***Return Value***: Returns true if successful, and false if failed.

- ***Note***
    1. Warning: Repeated calls to this function without intermediate disconnect() will force-close the active connection.
    2. Usage constraint: Call rate must be ≤ 2Hz (once per 500ms minimum interval).

---

### ***Disconnection***
```cpp
void disconnect()
```
- ***Function***
Disconnects from the robot.

- ***Note***

    Suggest adding ~500ms delay between this function and connect calls.

---

### ***Send Script***
```cpp
bool sendScript(const std::string& script)
```
- ***Function***
Sends an executable script to the robot.
- ***Parameters***
    - script: The script to be sent.
- ***Return Value***: Returns true if the sending is successful, and false if failed.

---

### Get Data Packet
```cpp
bool getPackage(std::shared_ptr<PrimaryPackage> pkg, int timeout_ms)
```
- ***Function***
Retrieves and parses the robot's data packet.
- ***Parameters***
    - pkg: The data packet to be retrieved.
    - timeout_ms: The waiting timeout.
- ***Return Value***: Returns true if the retrieval is successful, and false if failed.

---

### Get local IP
```cpp
std::string getLocalIP()
```
- ***Function***

    Get local IP address

- ***Return Value***：Local IP address. If empty, connection had some errors.

---

# PrimaryPackage Class

## Introduction
This class is mainly used to be inherited to obtain the data from the Primary port.
The SDK does not provide parsing for all data packets. If you need other data packets, you can refer to the implementation of `KinematicsInfo` in `RobotConfPackage.hpp` to write the data packet parsing code you need. Refer to the Elite official document "CS_User Manual_Robot Status Message.xlsx" for the message format.

## Header File of PrimaryPackage
```cpp
#include <Elite/PrimaryPackage.hpp>
```

## Constructor of the PrimaryPackage Class
### ***Constructor***
```cpp
PrimaryPackage::PrimaryPackage(int type)
```
- ***Function***
Initializes the data.
- ***Parameters***
    - type: The type of the data packet (refer to the Elite official document: CS_User Manual_Robot Status Message.xlsx).

---

## Pure Virtual Function

### Parse Message
```cpp
void parser(int len, const std::vector<uint8_t>::const_iterator& iter)
```
- ***Function***
The specific implementation is to be completed by subclasses. It parses the sub-message of the robot status message from the Primary port. When an instance of a subclass is passed as a parameter to `PrimaryPortInterface::getPackage()`, this function will be called.
- ***Parameters***
    - len: The length of the sub-message.
    - iter: The position of the sub-message in the whole message.

---

## Others

### ***Get Message Type***
```cpp
int getType()
```
- ***Function***
Gets the type of the data packet.
- ***Return Value***: The type of the data packet.

---

## Internal Use in the SDK
### Wait for Update
```cpp
bool waitUpdate(int timeout_ms)
```
- ***Function***
Waits for the data packet to be updated. It is called in the `getPackage()` function.
- ***Parameters***
    - timeout_ms: The timeout.
- ***Return Value***: Returns true if it does not time out, and false if it times out.

---

# KinematicsInfo Class

## Introduction
This is for parsing the kinematics data packet in the robot configuration data. `PrimaryPackage` is the parent class of this interface. In addition to the methods in the `PrimaryPackage` class, the main content of this interface is the robot's DH parameters.

## Header File of KinematicsInfo
```cpp
#include <Elite/RobotConfPackage.hpp>
```

## DH Parameters

- `vector6d_t dh_a_`

- `vector6d_t dh_d_`

- `vector6d_t dh_alpha_`