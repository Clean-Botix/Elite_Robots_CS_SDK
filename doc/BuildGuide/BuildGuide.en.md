# Compilation Guide

## Requirements
* The socket in the SDK uses **boost::asio**. Therefore, the **boost** library needs to be installed.
* This SDK requires a compiler that supports C++17 or C++14. Note that if the C++14 standard is used, `boost::variant` will be utilized.
* The SDK provides an interface for downloading files via ssh. It is recommended to install libssh. If not installed, you need to ensure that the ssh, scp, and sshpass commands can be run.
* The cmake version should be >= 3.22.1.

## Dependency Installation

### Ubuntu

- Basic Dependencies
```bash
sudo apt update

sudo apt install libboost-all-dev

sudo apt install libssh-dev # Optional, recommended installation

# sudo apt install sshpass # Install this command if libssh-dev is not installed
```

- Test Case Dependencies (Optional)
```bash
sudo apt update

sudo apt install libgtest-dev
```

- Documentation Compilation Dependencies (Optional)
```bash
sudo apt-get install doxygen
sudo apt-get install doxygen-gui
```

### Windows (Visual Studio)
Use vcpkg to install dependencies on Windows. First, you need to download vcpkg and create a folder to save it. Note that the path of this file will be used in subsequent compilations:
```shell
git clone https://github.com/microsoft/vcpkg.git

.\bootstrap-vcpkg.bat

```

- Basic Dependencies
```shell
.\vcpkg install boost

.\vcpkg install libssh

vcpkg integrate install
```

- Test Case Dependencies (Optional)
```bash
vcpkg install gtest

vcpkg integrate install
```

## Compilation

### CMake Configuration Options
You can set the compilation options for this project using the -D parameter. The syntax is as follows:
```bash
cmake -D<variable name>=<value> [other parameters] <path to CMakeLists.txt>
```

For example, if you need to compile using the C++14 standard, use the following command:
```bash
cmake -DCMAKE_CXX_STANDARD=14 ..
```

In addition to the common CMake configuration options, this project also has the following options:
- ELITE_COMPILE_EXAMPLES
    - Value: BOOL
    - Description: If set to TRUE, the code in the example directory will be compiled; otherwise, it will not be compiled.
- ELITE_COMPILE_TESTS
    - Value: BOOL
    - Description: If set to TRUE, the code in the test directory will be compiled; otherwise, it will not be compiled.
- ELITE_COMPILE_DOC
    - Value: BOOL
    - Description: If set to TRUE, documentation will be generated using doxygen.
    - Note: This option is currently unavailable on Windows.

### Ubuntu Compilation and Installation
```shell
cd <clone of this repository>

mkdir build && cd build

cmake ..

make -j

# Installation
sudo make install

sudo ldconfig
```

### Windows Compilation
```shell
cd <clone of this repository>

mkdir build && cd build

# Note that you need to replace the path to vcpkg
cmake -DCMAKE_TOOLCHAIN_FILE=<your vcpkg path>\scripts\buildsystems\vcpkg.cmake ..

cmake --build ./
```
After compilation, you will get two library files, `libelite-cs-series-sdk_static.lib` and `libelite-cs-series-sdk.dll`, and an `include` folder containing header files.