[中文](./README.cn.md)
# Elite Robots CS SDK

This SDK is a C++ library for Elibot Robots' CS series robotic arms. With this library, developers can implement C++-based drivers to leverage the versatility of Elibot CS series robotic arms for building external applications.

## Requirements
 * **CS Controller** (the control software for robots) version >= **2.13.4** (for CS-Series). If the version of the robot's control software is lower than this, it is recommended to upgrade it.
 * The socket in the SDK uses **boost::asio**. Therefore, the **boost** library needs to be installed.
 * This SDK requires a compiler that supports C++17 or C++14. Note that if the C++14 standard is used, **boost::variant** will be used.
 * cmake version >= 3.22.1

## Build & Install
If your system is Ubuntu20.04, Ubuntu22.04 or Ubuntu24.04, you can run the following command to install elite-cs-series-sdk:
```bash
sudo add-apt-repository ppa:elite-robots/cs-robot-series-sdk
sudo apt update
sudo apt install elite-cs-series-sdk
```

If compilation and installation are required, please refer to the [Compilation Guide](./doc/BuildGuide/BuildGuide.en.md). 

## User guide
[English guide](./doc/UserGuide/en/UserGuide.en.md)

## Architecture
[Code architecture](./doc/Architecture/Arch.en.md)

## API document
[API](./doc/API/en/API.en.md)

## Compatible Operating Systems
Tested on the following system platforms:

 * Ubuntu 22.04 (Jammy Jellyfish)
 * Ubuntu 16.04 (Xenial Xerus)
 * Windows 11

## Compiler
Currently compiled with the following compilers:

 * gcc 11.4.0
 * gcc 5.5.0
 * msvc 19.40.33808