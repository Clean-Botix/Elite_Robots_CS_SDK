# 编译向导

## Requirements
* SDK中的socket使用了 **boost::asio**。 因此需要安装 **boost** 库。
* 此SDK需要支持 C++17 或 C++14 的编译器。注意，如果是C++14的标准，会使用到`boost::variant`。
* SDK提供了通过ssh下载文件的接口，建议安装 libssh。如果不安装的话，则需要确保能运行ssh、scp、sshpass指令。
* cmake版本 >=3.22.1

## 依赖安装

### Ubuntu

- 基础依赖
```bash
sudo apt update

sudo apt install libboost-all-dev

sudo apt install libssh-dev # 可选，建议安装

# sudo apt install sshpass #如果没安装 libssh-dev 则需要安装此指令
```

- 测例依赖（可选）
```bash
sudo apt update

sudo apt install libgtest-dev
```

- 文档编译依赖（可选）
```bash
sudo apt-get install doxygen
sudo apt-get install doxygen-gui
```

### Windows（vistual studio）
使用vcpkg在Windows中安装依赖，首先需要下载vcpkg，创建一个文件夹保存vcpkg，注意此文件路径在后续的编译中会用到：
```shell
git clone https://github.com/microsoft/vcpkg.git

.\bootstrap-vcpkg.bat

```

- 基础依赖
```shell
.\vcpkg install boost

.\vcpkg install libssh

vcpkg integrate install
```

- 测例依赖
```bash
vcpkg install gtest

vcpkg integrate install
```

## 编译

### cmake 配置选项
通过 -D 参数可设置本项目编译选项，语法格式为：
```bash
cmake -D<变量名>=<值> [其他参数] <CMakeLists.txt 所在路径>
```

例如，需要使用C++14标准编译，则使用下面指令：
```bash
cmake -DCMAKE_CXX_STANDARD=14 ..
```

除了cmake通用的配置选项以外，本项目中还有下面的选项：
- ELITE_COMPILE_EXAMPLES
    - 值：BOOL
    - 说明：如果为TRUE，则会编译example目录下的代码，否则不会编译。
- ELITE_COMPILE_TESTS
    - 值：BOOL
    - 说明：如果为TRUE，则会编译test目录下的代码，否则不会编译。
- ELITE_COMPILE_DOC
    - 值：BOOL
    - 说明：如果为TRUE，则会使用doxygen生成文档。
    - 注：Windows中此选项暂不可用。

### Ubuntu 编译安装
```shell
cd <clone of this repository>

mkdir build && cd build

cmake ..

make -j

# 安装
sudo make install

sudo ldconfig
```


### Windows 编译
```shell
cd <clone of this repository>

mkdir build && cd build

# 注意需要替换vcpkg的路径
cmake -DCMAKE_TOOLCHAIN_FILE=<your vcpkg path>\scripts\buildsystems\vcpkg.cmake ..

cmake --build ./
```
编译完成后会得到`libelite-cs-series-sdk_static.lib` 和 `libelite-cs-series-sdk.dll` 两个库文件、包括了头文件的`include`文件夹。