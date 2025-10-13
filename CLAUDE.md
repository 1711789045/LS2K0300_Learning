# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 📝 代码编辑规范（重要）

### 文件编码要求

**⚠️ 强制要求：所有代码文件必须使用 UTF-8 编码！**

- **新建文件**：必须使用 UTF-8 编码
- **编辑文件**：保持文件原有的 UTF-8 编码，不要改变
- **注释**：中文注释使用 UTF-8 编码

**Claude Code 编辑器设置：**
- Read/Write/Edit 工具默认使用 UTF-8
- 避免使用其他编码格式（GBK、GB2312等）

**现有问题说明：**
- 部分旧文件（特别是 `C_Car_project/loongson_project/code/` 下的文件）可能使用 GBK 编码
- 这些文件中的中文注释在 UTF-8 环境下会显示为乱码
- **修改这些文件时**：只修改代码，不要试图转换编码或修复中文注释
- **新增注释时**：使用英文或者确保编辑器使用 UTF-8

## 项目概述

这是逐飞科技针对龙芯久久派 LS2K0300 开发的开源库，用于竞赛和产品开发。本项目基于 Linux 环境，使用交叉编译工具链为 LoongArch64 架构编译应用程序。

**关键特性:**
- 目标平台: 龙芯 LS2K0300 (LoongArch64 架构)
- 开发环境: Ubuntu 24.04
- 编程语言: C/C++ (C++17标准)
- 构建系统: CMake
- 依赖库: OpenCV 4.10

## 环境配置

### 交叉编译工具链
- 路径: `/opt/ls_2k0300_env/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.6`
- 编译器: `loongarch64-linux-gnu-g++` 和 `loongarch64-linux-gnu-gcc`
- 配置文件: `user/cross.cmake` (每个项目都有)

### OpenCV 配置
- 安装路径: `/opt/ls_2k0300_env/opencv_4_10_build`
- 用于图像处理和摄像头相关功能

## 项目结构

```
LS2K0300_Library/
├── Seekfree_LS2K0300_Opensource_Library/   # 核心开源库
│   ├── libraries/                           # 库文件
│   │   ├── zf_common/                      # 通用功能(字体、FIFO、类型定义等)
│   │   ├── zf_driver/                      # 驱动层(GPIO、PWM、ADC、编码器、网络等)
│   │   ├── zf_device/                      # 设备驱动(IMU、UVC摄像头、IPS200显示屏等)
│   │   └── zf_components/                  # 应用组件(逐飞助手接口等)
│   └── project/                            # 模板项目
│       └── user/                           # 用户代码目录
│           ├── main.cpp                    # 主程序
│           ├── CMakeLists.txt              # CMake配置
│           ├── cross.cmake                 # 交叉编译配置
│           └── build.sh                    # 构建脚本
├── Example/                                # 示例程序
│   ├── Coreboard_Demo/                     # 核心板示例
│   └── Motherboard_Demo/                   # 主板示例
│       ├── E01_xx_xxx_demo/               # 各类功能示例
│       │   └── user/                       # 与project结构相同
│       │       ├── main.cpp
│       │       ├── CMakeLists.txt
│       │       ├── cross.cmake
│       │       └── build.sh
│       └── ...
└── ls2k0300_linux_4.19/                   # Linux内核源码
```

## 构建和编译

### 标准构建流程

每个示例项目和模板项目都遵循相同的构建流程:

```bash
# 进入项目的user目录
cd Example/Motherboard_Demo/E01_01_button_switch_buzzer_demo/user

# 执行构建脚本
./build.sh
```

### build.sh 脚本功能

1. 进入 `../out` 目录
2. 清理旧的构建文件(保留 `本文件夹作用.txt`)
3. 运行 `cmake ../user` 生成构建文件
4. 运行 `make -j12` 编译项目
5. 通过 SCP 将编译好的可执行文件传输到目标设备 (默认: `root@172.20.10.7:/home/root/`)

**注意**: 构建脚本会自动通过 SCP 传输可执行文件到设备，IP地址可能需要根据实际情况修改。

### 手动构建

如果需要手动构建或调试:

```bash
cd out
cmake ../user
make -j12
```

生成的可执行文件名称与项目文件夹名称相同(例如 `E01_01_button_switch_buzzer_demo`)。

## CMakeLists.txt 配置要点

每个项目的 `CMakeLists.txt` 都包含:

1. **交叉编译工具链引入**: `include(cross.cmake)`
2. **编译选项**: `-O3 -pthread -Wall` (C++), `-O3 -Wall` (C)
3. **OpenCV路径**: `/opt/ls_2k0300_env/opencv_4_10_build`
4. **头文件包含路径**:
   - `../user` - 用户代码
   - `../code` - 项目特定代码
   - `../../libraries/zf_*` - 核心库的各个模块
5. **源文件收集**: 使用 `aux_source_directory` 自动收集所有 C/C++ 文件
6. **项目名称**: 自动从父级目录名称提取

## 代码架构

### 分层设计

1. **通用层 (zf_common)**: 基础功能、类型定义、字体、FIFO等
2. **驱动层 (zf_driver)**: 硬件外设接口抽象
   - GPIO: `gpio_set_level()`, `gpio_get_level()`
   - PWM: `pwm_set_duty()`, `pwm_get_dev_info()`
   - ADC: 电池电压检测等
   - Encoder: 编码器方向和计数
   - PIT: 定时器
   - Network: UDP/TCP客户端
3. **设备层 (zf_device)**: 外接设备驱动
   - IMU传感器: IMU660RA, IMU660RB, IMU963RA
   - UVC摄像头: `/dev/video0` 设备访问
   - IPS200显示屏: Framebuffer方式
4. **应用组件层 (zf_components)**: 高级功能
   - 逐飞助手接口: 用于TCP/UDP调试和图像传输

### 设备路径方式

驱动使用 Linux 设备路径字符串操作硬件:
- GPIO: `/sys/class/gpio/...`
- PWM: `/sys/class/pwm/...`
- 摄像头: `/dev/video0`

### 头文件引入

所有用户代码只需包含: `#include "zf_common_headfile.h"`

这个头文件自动包含所有必要的库文件。

## 常用示例说明

- **E01_01**: 按键、开关、蜂鸣器控制
- **E01_02**: 蜂鸣器PWM控制
- **E02_04**: DRV8701E双电机控制
- **E02_06**: 舵机PWM控制
- **E03_02**: 编码器方向检测
- **E04_01**: ADC电池电压检测
- **E05_07**: IMU传感器数据读取
- **E06_04**: IPS200显示屏显示
- **E07_06**: UDP网络通信
- **E07_07/08/09**: TCP客户端(示波器、摄像头等)
- **E09_14**: UVC摄像头+IPS200显示

## C_Car_project/loongson_project - 四轮摄像头循迹小车项目

这是主要的开发项目，位于 `C_Car_project/loongson_project/`，实现了一个基于视觉的四轮循迹小车系统。

### 项目结构

```
C_Car_project/loongson_project/
├── user/                           # 用户代码目录
│   ├── main.cpp                    # 主程序入口
│   ├── CMakeLists.txt              # CMake配置
│   ├── cross.cmake                 # 交叉编译配置
│   └── build.sh                    # 构建和传输脚本
├── code/                           # 功能模块代码
│   ├── image.cpp/h                 # 图像处理和赛道识别
│   ├── control.cpp/h               # 运动控制
│   ├── pit.cpp/h                   # 定时器中断
│   ├── key.cpp/h                   # 按键处理
│   ├── beep.cpp/h                  # 蜂鸣器控制
│   └── auto_menu.cpp/h             # 自动菜单系统
├── out/                            # 构建输出目录
└── ../libraries/                   # 共享库（符号链接指向主库）
```

### 核心功能模块

#### 1. 图像处理模块 (image.cpp/h)

**主要功能:**
- **图像采集**: 从 UVC 摄像头获取灰度图像 (120x180像素)
- **赛道识别**: 基于对比度的边线检测算法
- **特殊元素识别**: 十字路口、环岛、停止线检测
- **中线计算**: 加权平均算法计算赛道中线

**关键算法:**

1. **参考点获取** (`get_reference_point`):
   - 在图像底部区域统计平均灰度值作为参考点
   - 动态计算黑白阈值 (`white_max_point`, `white_min_point`)

2. **参考列搜索** (`search_reference_col`):
   - 从下往上扫描，寻找最短远距离列作为参考列
   - 使用对比度检测: `(temp1-temp2)*200/(temp1+temp2)`

3. **边线搜索** (`search_line`):
   - 从参考列向左右两侧搜索边线
   - 支持二次搜索机制，提高鲁棒性
   - 检测边线断点和黑色区域

4. **环岛识别** (`image_circle_analysis`):
   - 5个状态机: 0-识别环岛 → 1-入环 → 2-环内巡线 → 3-出环判断 → 4-补线 → 5-完成
   - 基于边线跳变点和最小值点识别
   - 时间控制: `CIRCLE_1_TIME`, `CIRCLE_2_TIME`, `CIRCLE_4_TIME`, `CIRCLE_5_TIME`

5. **十字路口处理** (`image_cross_analysis`):
   - 检测赛道宽度突变识别十字
   - 边线补线和延伸算法
   - 标志位控制状态转换

6. **中线计算** (`image_calculate_mid`):
   - 三种模式: 0-双边中线, 1-左边单边, 2-右边单边
   - 加权平均: 使用 `mid_weight[]` 数组，近处权重高
   - 低通滤波: `final_mid_line = temp*0.8 + last_final_mid_line*0.2`

**关键变量:**
```cpp
uint8 user_image[IMAGE_H][IMAGE_W];        // 灰度图像缓冲
uint16 left_edge_line[IMAGE_H];            // 左边线数组
uint16 right_edge_line[IMAGE_H];           // 右边线数组
uint16 reference_line[IMAGE_H];            // 参考线
uint8 mid_line[IMAGE_H];                   // 中线
uint8 final_mid_line;                      // 加权中线值(控制输出)
uint8 circle_flag;                         // 环岛状态 0-5
uint8 cross_flag;                          // 十字标志
uint8 mid_mode;                            // 循线模式 0-双边 1-左单边 2-右单边
```

**图像处理流程** (`image_process`):
```
1. get_image()                    // 获取图像
2. get_reference_point()          // 获取参考点
3. search_reference_col()         // 搜索参考列
4. search_line()                  // 搜索左右边线
5. image_circle_analysis()        // 环岛识别和处理
6. image_cross_analysis()         // 十字识别和处理
7. stop_analysis()                // 停止线识别
8. image_calculate_mid()          // 计算中线
9. image_display_edge_line()      // 显示到IPS200屏幕
```

#### 2. 控制模块 (control.cpp/h)

**核心变量:**
```cpp
uint8 go_flag;      // 运行标志
uint8 stop_flag;    // 停止标志
uint8 stop_time;    // 停止计时
```

**待实现功能:**
- 电机PID控制
- 舵机转向控制
- 速度规划
- 编码器反馈

#### 3. 定时器模块 (pit.cpp/h)

使用 `timer_fd` 类实现定时中断:

```cpp
timer_fd *pit_10ms_timer;     // 10ms定时器(预留)
timer_fd *pit_100ms_timer;    // 100ms定时器(蜂鸣器控制)
```

**回调函数:**
- `pit_10ms_callback()`: 快速周期任务
- `pit_100ms_callback()`: 蜂鸣器控制、环岛计时

#### 4. 按键模块 (key.cpp/h)

**硬件接口:**
```cpp
KEY_1: /dev/zf_driver_gpio_key_0
KEY_2: /dev/zf_driver_gpio_key_1
KEY_3: /dev/zf_driver_gpio_key_2
KEY_4: /dev/zf_driver_gpio_key_3
```

**功能:**
- 按键消抖和长按检测 (600ms)
- 边沿触发检测
- 配合菜单系统使用

#### 5. 蜂鸣器模块 (beep.cpp/h)

```cpp
BEEP: /dev/zf_driver_gpio_beep
beep_flag: 蜂鸣器触发计数器
```

用于状态提示音(环岛识别、停止线等)

#### 6. 自动菜单系统 (auto_menu.cpp/h)

- 基于IPS200屏幕的图形化参数调试界面
- 支持参数实时修改(float/int/uint16/uint32)
- 支持PID参数显示和调节
- Flash参数保存和读取

### 构建和调试

#### 快速构建

```bash
cd C_Car_project/loongson_project/user
./build.sh
```

构建脚本自动完成:
1. 清理旧构建文件
2. CMake配置
3. 并行编译 (12线程)
4. SCP传输到设备 (172.20.10.7)

#### 手动构建

```bash
cd C_Car_project/loongson_project/out
cmake ../user
make -j12
scp -O loongson_project root@172.20.10.7:/home/root/
```

#### 库文件配置

项目使用独立的库副本 (`C_Car_project/libraries/`)，与主库结构相同:
- `C_Car_project/libraries/zf_common/`
- `C_Car_project/libraries/zf_driver/`
- `C_Car_project/libraries/zf_device/`
- `C_Car_project/libraries/zf_components/`

CMakeLists.txt 中的包含路径:
```cmake
include_directories(../../libraries/zf_common)
include_directories(../../libraries/zf_device)
include_directories(../../libraries/zf_driver)
include_directories(../../libraries/zf_components)
```

### 开发建议

1. **图像处理优化**:
   - 调整 `REFERENCE_ROW`, `REFERENCE_COL` 改变参考区域
   - 修改 `WHITEMAXMUL`, `WHITEMINMUL` 调整阈值倍数
   - 调整 `mid_weight[]` 数组改变中线权重分布

2. **环岛参数**:
   - `CIRCLE_1_TIME`: 入环持续时间
   - `CIRCLE_2_TIME`: 环内循线时间
   - `CIRCLE_4_TIME`: 出环补线时间
   - `CIRCLE_5_TIME`: 完成阶段时间

3. **十字路口**:
   - 宽度阈值检测
   - 跳变点识别
   - 补线算法参数

4. **调试工具**:
   - IPS200屏幕实时显示边线和中线
   - 串口打印关键数据
   - 菜单系统实时调参

5. **待实现功能**:
   - 电机驱动 (PWM + 方向控制)
   - 舵机控制 (PWM)
   - 编码器读取
   - PID控制器
   - 速度规划算法

## Git 工作流程（重要）

**⚠️ 强制要求：在 C_Car_project 下工作时，每次代码更改后必须提交到 Git 并推送到远程仓库！**

### 标准工作流程

每次完成代码修改后，**必须**立即执行以下步骤：

```bash
# 1. 查看更改
git status
git diff

# 2. 添加更改
git add C_Car_project/

# 3. 提交（使用描述性消息）
git commit -m "描述性的提交信息

- 具体修改内容1
- 具体修改内容2

🤖 Generated with Claude Code"

# 4. 推送到 GitHub（必须执行！）
git push github master
```

**重要提醒：**
- 第4步（push）是**必须的**，不是可选的！
- 只有 commit 而不 push，代码只保存在本地，无法在远程仓库备份
- 每次修改后都要完成 commit + push 两步操作

### 提交原则

1. **小步提交**: 每完成一个小功能或修复一个问题就提交
2. **清晰消息**: 提交信息要说明改了什么、为什么改
3. **可回滚**: 每个提交都应该是可独立回滚的功能单元
4. **保持频繁**: 宁可多提交也不要积累大量未提交的修改

### 示例场景

- ✅ 修改了图像处理参数 → 立即提交
- ✅ 添加了电机控制代码 → 立即提交
- ✅ 调试修复了一个 bug → 立即提交
- ✅ 更新了 PID 参数 → 立即提交

### 回滚方法

如果需要回滚到之前的版本：

```bash
# 查看提交历史
git log --oneline

# 回滚到指定提交
git reset --hard <commit-id>

# 或者只回滚某个文件
git checkout <commit-id> -- C_Car_project/loongson_project/code/image.cpp
```

## 开发注意事项

1. **不要下载ZIP压缩包**: 必须使用 `git clone` 克隆仓库，否则会导致文件权限问题
2. **SCP传输地址**: 构建脚本默认传输到 `root@172.20.10.7`，需要根据实际设备IP修改
3. **电源供电**: 使用外部电源供电，下载器供电可能导致电压不足
4. **内核依赖**: 某些功能(如UVC摄像头、PIT等)需要使用逐飞科技提供的定制内核
5. **TCP/UDP调试**: 配合逐飞助手上位机软件使用
6. **图像传输**: 支持多种边界显示模式(仅图像、X边界、Y边界、XY边界、仅边界)
7. **工作目录**: 主要开发在 `C_Car_project/loongson_project/` 进行，这是四轮循迹小车的完整实现
8. **Git 提交**: **每次代码修改后必须立即提交到 Git，方便回滚和版本管理**

## 许可证

本项目使用 GPL3.0 开源许可证，由成都逐飞科技有限公司维护。
