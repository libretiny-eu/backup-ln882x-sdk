- [1. CMake 简介](#1-cmake-简介)
- [2. 环境配置](#2-环境配置)
  - [2.1 Win10 环境配置](#21-win10-环境配置)
  - [2.2 Ubuntu 环境配置](#22-ubuntu-环境配置)
- [3. 项目构建](#3-项目构建)
  - [3.1 项目结构](#31-项目结构)
  - [3.2 如何构建库](#32-如何构建库)
  - [3.3 如何发布库](#33-如何发布库)
    - [3.3.1 第一步，编译 Release 版本的库](#331-第一步编译-release-版本的库)
    - [3.3.2 第二步，编译、拷贝并发布](#332-第二步编译拷贝并发布)
  - [3.4 如何构建](#34-如何构建)
    - [3.4.1 简易构建步骤](#341-简易构建步骤)
      - [3.4.1.1 脚本 `start_build.sh` 和 `start_build.bat` 使用指南](#3411-脚本-start_buildsh-和-start_buildbat-使用指南)
      - [3.4.1.2 注意事项](#3412-注意事项)
    - [3.4.2 命令行构建步骤](#342-命令行构建步骤)

# 1. CMake 简介

- CMake 是一个跨平台的构建工具，能够输出各种各样的 Makefile 或者 project 文件；
- CMake 并不直接构建出最终的目标，而是生成标准的 Makefile 文件或者 build.ninja 项目文件，然后再使用 Make 工具读取 Makefile 文件进行编译 或者 Ninja 工具读取 build.ninja 进行编译；
- CMake 可以选择生成器，在同一个平台可以生成多种不同类型的项目文件，例如在 Windows 平台既可以生成 VisualStudio 项目文件，也可以生成 MinGW Makefiles 或者 Unix Makefiles。

工程目录中使用 `CMakeLists.txt` 文件来描述 CMake 规则和构建目标，类似于 `Makefile`。

# 2. 环境配置

## 2.1 Win10 环境配置

1. 在 Windows 上使用 CMake 编译，需要提前安装 **python**, [**GNU Make**](http://gnuwin32.sourceforge.net/packages/make.htm) 或者 **Ninja** 工具和 **CMake** 并添加到环境变量 **PATH** 中；
2. 配置交叉编译器，以 Win10 为例，从 [Arm官网](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) 下载交叉编译器 **gcc-arm-none-eabi-10-2020-q4-major-win32.zip**，并解压缩到某个目录，例如 `D:\PortableInstalls`，并添加到环境变量 **PATH** 中，确保命令行中输入 `arm-none-eabi-gcc --version` 能看到版本信息；
3. 配置环境变量，给用户添加一个环境变量 `CROSS_TOOLCHAIN_ROOT`，它的值就是 `D:\PortableInstalls\gcc-arm-none-eabi-10-2020-q4-major`；
4. 重新启动一个终端输入命令 `echo %CROSS_TOOLCHAIN_ROOT%` 检查值是否正确；

## 2.2 Ubuntu 环境配置

1. 安装编译基础套件，命令行输入 `sudo apt update` 然后输入 `sudo apt install build-essential`；
2. Ubuntu 上使用 CMake 编译，需要提前安装 **python**, **make** 和 **cmake**；
3. 确保当前的 shell 是 bash，参考 [Linux修改某用户默认shell](https://www.jianshu.com/p/1661b8c03edd)；
4. 配置交叉编译器，以 Ubuntu 18.04.1 x64 版本为例，从 [Arm官网](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads) 下载交叉编译器 **gcc-arm-none-eabi-10-2020-q4-major-x86_64-linux.tar.bz2**，并解压到某个目录，例如 `$HOME/portableInstalls/`；
5. 在文件 `$HOME/.bashrc` 末尾添加如下内容，设置交叉编译器环境变量：
```sh
CROSS_TOOLCHAIN_ROOT=$HOME/portableInstalls/gcc-arm-none-eabi-10-2020-q4-major
export CROSS_TOOLCHAIN_ROOT
```
4. 重新启动终端或者输入命令 `source $HOME/.bashrc` 使得环境变量生效；

# 3. 项目构建
## 3.1 项目结构

- `components` 目录存放各种组件，例如 `atcmd`，`airkiss` 以及 `wifi` 固件， 每一个组件自身都包含一个 `CMakeLists.txt`，编译过程中组件都会编译成库，绝大部分源码提供给客户，但是 `wifi` 固件编译为库 `libwifi.a`，存放在 `lib/gcclib/` 目录下；
- `doc` 目录存放文档；
- `project` 目录下存放的是用户产品相关的代码，全部为源码；
- `lib` 目录存放库文件， 无论是 Keil 还是 GCC 编译的库都存放在此目录；
- `mcu` 目录下存放的是 LN882X 芯片相关的驱动，通过 CMake 编译为库 `libdriver_ln882x.a`，存放在 `lib/gcclib/` 目录下；
- `tools` 目录存放构建过程中使用的工具；
- `CMakeLists.txt` 顶层文件，描述了组件和用户工程最终目标之间的关系，通过 `option()` 选项来控制组件是否参与编译；顶层目录 `CMakeLists.txt` 中 `option()`设定的是默认状态，可以在用户工程中设定组件是否参与编译，例如在 *wifi_mcu_basic_example* 工程中使能 atmcd 组件只需要添加一行语句 `set(COMP_ATCMD_SUPPORT  ON  PARENT_SCOPE)` ；

## 3.2 如何构建库

每一个组件都包含一个 `CMakeLists.txt` 文件，此文件描述了组件依赖的源文件和头文件路径，以及最终生成的库文件名字。

`CMakeLists.txt` 顶层文件，描述了组件和用户工程最终目标之间的关系，通过 `option()` 选项来控制组件是否参与编译；顶层目录 `CMakeLists.txt` 中 `option()`设定的是默认状态，可以在用户工程中设定组件是否参与编译，例如 `set(COMP_AIRKISS_SUPPORT  ON  PARENT_SCOPE)` 使能组件 `airkiss` 参与编译。

## 3.3 如何发布库

### 3.3.1 第一步，编译 Release 版本的库

编译生成库文件的时候需要在顶层 `CMakeLists.txt` 文件中打开如下设置。
需要注意，当发布 release 版本这两个选项必须要设置为 `OFF`，否则编译失败；仅在开发阶段两个都设置为 `ON`。

```
# 编译 release 版本
set(CMAKE_BUILD_TYPE Release)

# driver_ln882x 纳入编译
option(COMP_MCU_LN882X_SUPPORT  "MCU LN882X driver."                            ON)

# wifi 库纳入编译
option(COMP_WIFI_SUPPORT        "WiFi firmware."                                ON)
```

### 3.3.2 第二步，编译、拷贝并发布

例如命令行输入 **`start_build.bat  wifi_mcu_basic_example  rebuild`** 开始编译，步生成的库文件存在于 **build/lib** 目录，拷贝其中的 `libwifi.a` 和 `libdriver_ln882x.a` 两个文件到 **lib/gcclib/** 目录中。

## 3.4 如何构建

### 3.4.1 简易构建步骤

参见顶层目录 `start_build.sh`， 传递给脚本的第一个参数就是用户工程的名字，即 `project/` 目录下的用户工程名字，例如 **wifi_mcu_basic_example** 或者 **wifi_mcu_smartconfig_demo** 等。

它通过 `cd build && cmake -DUSER_PROJECT=wifi_mcu_basic_example ..` 传递给 CMake，即 `CMakeLists.txt` 文件中的 `USER_PROJECT` 变量指代用户工程的名字。

GCC 编译输出的文件是 `build/xxx.elf`，在经过我们的工具加工之后生成的可烧录到 Flash 上的文件是 **build/bin/flashimage.bin** 。

#### 3.4.1.1 脚本 `start_build.sh` 和 `start_build.bat` 使用指南

Linux 平台使用 `start_build.sh` 脚本；Windows 平台使用 `start_build.bat` 脚本。两个脚本功能一致，参数和用法也相同。

下面以 Windows 平台的 `start_build.bat` 脚本为例说明，

1. 在顶层目录 `start_build.bat` 不带参数， 会打印脚本的用法，请详细阅读用法说明；
2. 在顶层目录 `start_build.bat   wifi_mcu_basic_example   clean` 清除 `build/` 目录，清空上一次编译结果；
3. 在顶层目录 `start_build.bat   wifi_mcu_basic_example   config`  配置用户工程生成于  `build/` 目录 ；
4. 在顶层目录 `start_build.bat   wifi_mcu_basic_example   build` 在上一步的基础上开始编译工程；
5. 在顶层目录 `start_build.bat   wifi_mcu_basic_example   rebuild` 会自动依次执行上面的3个步骤；
6. 在顶层目录 `start_build.bat   wifi_mcu_basic_example   jflash` 会调用 **J-Flash** 工具下载生成的 `build/bin/flashimage.bin` 文件到模块上（请先确保J-Link USB 连线正确，模块已经上电）；

#### 3.4.1.2 注意事项

如在 Windows 平台编译，请先退出杀毒软件（XXX安全卫士或者XXX电脑管家），因为杀毒软件把构建工具放在隔离沙盒中运行并且扫描编译生成的文件，导致编译速度非常慢。

### 3.4.2 命令行构建步骤

1. 在工程顶层目录创建一个 **build/** 文件夹，专门用来存放构建中间文件和最终的输出文件；
2. 命令行进入 **build/** 文件夹，然后输入 `cmake -DXXX_ARG=XXX_VAL ..` 开始生成 Makefile，这里向 CMake 传递了一个参数 `XXX_ARG` 它的值为 `XXX_VAL`，其中的 `..` 这表示源码位于上一层目录，变量 `CMAKE_SOURCE_DIR` 表示源码顶层目录，而 `CMAKE_BINARY_DIR` 表示构建所在的目录，通常是指 **build/** 目录；
3. 命令行位于 **build/** 文件夹，输入 `make` 开始编译。