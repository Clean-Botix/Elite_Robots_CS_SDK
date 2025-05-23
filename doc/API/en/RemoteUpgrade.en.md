# RemoteUpgrade Module

## Introduction
The RemoteUpgrade module provides the function of remote upgrading for the robot control software.

## Header File
```cpp
#include <Elite/RemoteUpgrade.hpp>
```

## Upgrade the Control Software

```cpp
bool upgradeControlSoftware(std::string ip, std::string file, std::string password)
```
- ***Function***
Upgrades the robot control software.
- ***Parameters***
  - `ip`: The IP address of the robot.
  - `file`: The path of the upgrade file.
  - `password`: The SSH password of the robot controller.
- ***Return Value***
  - `true`: The upgrade is successful.
  - `false`: The upgrade fails.
- ***Notes***
  1. Under the Linux system, if `libssh` is not installed, it is necessary to ensure that the computer running the SDK has the `scp`, `ssh`, and `sshpass` commands available.
  2. Under the Windows system, if `libssh` is not installed, this interface is not available. 