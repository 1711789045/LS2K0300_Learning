# LS2K0300 自动菜单系统

## 项目简介
这是一个为龙芯久久派2K0300开发的自动菜单系统，可以在开机时自动启动，并在IPS200屏幕上显示菜单选项，用户可以通过按键选择并启动不同的工程。

## 功能特点
- 开机自启动
- IPS200屏幕显示菜单
- 按键控制（KEY_0-KEY_3）
- 支持多个工程启动
- 英文界面显示
- 白色背景显示
- **新增**: 可以通过KEY_3+KEY_2退出菜单到终端
- **新增**: 项目在前台运行，可以接收Ctrl+C信号
- **新增**: 菜单和项目都可以正常与终端交互
- **新增**: 改进了进程管理，使用fork+exec替代system
- **新增**: 提供交互式启动脚本

## 系统要求
- 龙芯久久派2K0300开发板
- 逐飞学习板
- IPS200屏幕
- LoongOS操作系统

## 文件结构
```
E10_auto_menu_demo/
├── user/                    # 源代码目录
│   ├── main.cpp            # 主程序文件（支持退出菜单功能）
│   ├── CMakeLists.txt      # CMake配置文件
│   ├── cross.cmake         # 交叉编译配置
│   ├── build.sh            # 构建脚本
│   ├── start_menu.sh       # 交互式启动脚本
│   ├── auto_menu.service   # systemd服务文件
│   ├── install_service.sh  # 服务安装脚本
│   └── fix_service_path.sh # 服务路径修复脚本
├── deploy.sh               # 完整部署脚本
└── README.md              # 说明文档
```

## 快速开始

### 方法一：完整安装（推荐）

#### 版本3.0（推荐）- 支持前台运行和Ctrl+C

1. **编译程序**
   ```bash
   cd user
   ./build.sh
   ```

2. **使用新版本部署脚本**
   
   在项目根目录中运行：
   ```bash
   ./deploy_v3.sh
   ```

3. **重启系统**
   ```bash
   reboot
   ```

#### 版本2.0 - 后台运行

1. **编译程序**
   ```bash
   cd user
   ./build.sh
   ```

2. **完整传输和安装**
   
   您可以在以下任一目录中运行传输脚本：
   
   **方法1：在user目录中运行**
   ```bash
   ./transfer_all.sh 172.20.10.7
   ```
   
   **方法2：在项目根目录中运行**
   ```bash
   ./transfer_all.sh 172.20.10.7
   ```

3. **重启系统**
   ```bash
   reboot
   ```

### 方法二：分步安装

1. **编译程序**
   ```bash
   cd user
   ./build.sh
   ```

2. **传输到开发板**
   ```bash
   # 传输程序文件
   scp user/out/E10_auto_menu_demo root@172.20.10.7:/tmp/
   
   # 传输安装文件
   scp install/* root@172.20.10.7:/tmp/
   ```

3. **在开发板上安装**
   ```bash
   ssh root@172.20.10.7
   cd /tmp
   chmod +x *.sh
   ./install.sh
   ```

4. **重启系统**
   ```bash
   reboot
   ```

## 使用说明

### 开机启动
系统启动后会自动运行菜单程序，在IPS200屏幕上显示以下菜单：

### 交互式启动脚本
除了开机自动启动外，还提供了交互式启动脚本，允许用户选择启动方式：

```bash
# 在板卡上运行
cd /home/root/E10_auto_menu_demo
./start_menu.sh
```

交互式启动脚本提供以下选项：
1. 启动菜单（默认）
2. 直接运行project工程
3. 直接运行单电机控制
4. 直接运行舵机控制
5. 直接运行编码器测试
```
SEEKFREE Auto Menu
Select Program:
> 1. project
  2. Single Motor Control
  3. Servo Control
  4. Encoder Test
KEY_3:Up KEY_2:Down
KEY_1:Enter KEY_0:Cancel
```

### 按键操作
- **KEY_3**: 向上选择菜单项
- **KEY_2**: 向下选择菜单项
- **KEY_1**: 确认选择，启动对应工程
- **KEY_0**: 取消选择，返回第一项
- **新增**: **KEY_3+KEY_2**: 退出菜单到终端

### 菜单选项
1. **project** - 项目工程
2. **Single Motor Control** - 单电机控制（实际为双电机控制）
3. **Servo Control** - 舵机控制（实际为按键开关蜂鸣器）
4. **Encoder Test** - 编码器测试（实际为IPS200显示）

### 项目运行特性（版本3.0）
- **前台运行**: 所有项目都在前台运行，可以接收Ctrl+C信号正常结束
- **终端交互**: 项目可以正常与终端交互，输出信息显示在终端上
- **进程管理**: 使用fork+exec替代system，提供更好的进程控制
- **信号处理**: 正确处理Ctrl+C信号，可以优雅地终止项目
- **菜单控制**: 项目运行期间可以通过按键返回菜单或退出到终端

### 项目运行特性（版本2.0）
- **后台运行**: 所有项目都在后台运行，不会阻塞菜单系统
- **终端访问**: 使用KEY_3+KEY_2可以退出菜单到终端，方便调试和直接运行命令
- **灵活启动**: 可以通过菜单、交互式脚本或直接命令行启动项目

## 更新程序

### 方法一：使用一键更新脚本
```bash
cd user
./build.sh --transfer
```

这个命令会：
1. 编译程序
2. 停止正在运行的服务
3. 传输新的程序文件
4. 启动服务
5. 检查服务状态

### 方法二：手动更新
1. 修改main.cpp文件
2. 重新编译：
   ```bash
   ./build.sh
   ```
3. 传输到开发板并更新：
   ```bash
   ssh root@172.20.10.7
   systemctl stop auto_menu.service
   scp user/out/E10_auto_menu_demo root@172.20.10.7:/home/root/E10_auto_menu_demo/
   systemctl start auto_menu.service
   ```

### 服务路径修复

如果遇到服务无法启动的问题（显示"status=203/EXEC"错误），可以使用修复脚本：

```bash
./fix_service_path.sh 172.20.10.7
```

这个脚本会自动修复服务配置文件中的程序路径问题。

## 故障排除

### 程序无法启动
1. 检查服务状态：
   ```bash
   systemctl status auto_menu.service
   ```
2. 查看服务日志：
   ```bash
   journalctl -u auto_menu.service
   ```
3. 检查程序文件是否存在：
   ```bash
   ls -la /home/root/E10_auto_menu_demo/E10_auto_menu_demo
   ```

### 工程无法启动
1. 检查工程路径是否正确
2. 确认工程文件是否有执行权限
3. 检查工程文件是否存在

### 屏幕显示问题
1. 检查IPS200屏幕连接是否正确
2. 确认屏幕驱动是否正常
3. 检查屏幕接线是否正确

### 按键无响应
1. 检查按键连接是否正确
2. 确认按键驱动是否正常
3. 检查按键接线是否正确

## 开发指南

### 修改菜单内容
编辑 `user/main.cpp` 文件中的以下部分：

1. **菜单文本**：
   ```cpp
   const char* menu_texts[MENU_ITEMS] = {
       "1. project",
       "2. Single Motor Control", 
       "3. Servo Control",
       "4. Encoder Test"
   };
   ```

2. **工程路径**：
   ```cpp
   const char* project_paths[MENU_ITEMS] = {
       "/home/root/project",
       "/home/root/E02_04_drv8701e_double_motor_control_demo",
       "/home/root/E01_01_button_switch_buzzer_demo", 
       "/home/root/E06_04_ips200_display_demo"
   };
   ```

3. **启动提示**：
   在 `run_selected_project()` 函数中修改启动提示文本。

### 添加新的菜单项
1. 在 `menu_texts` 数组中添加新的菜单文本
2. 在 `project_paths` 数组中添加对应的工程路径
3. 在 `run_selected_project()` 函数中添加新的 case 分支
4. 修改 `MENU_ITEMS` 宏定义

## 技术支持
如有问题，请联系技术支持或查看相关文档。

## 版本历史

- v1.0.0 - 初始版本
  - 基本菜单功能
  - 支持多个工程启动
  - 开机自启动功能

- v1.1.0 - 改进版本
  - 新增KEY_3+KEY_2退出菜单功能
  - 项目在后台运行
  - 新增交互式启动脚本
  - 改进服务管理
  - 完善部署脚本

- v1.2.0 - 前台运行版本
  - 项目在前台运行，可以接收Ctrl+C信号
  - 改进了进程管理，使用fork+exec替代system
  - 菜单和项目都可以正常与终端交互
  - 新增auto_menu_v3.service服务配置
  - 新增deploy_v3.sh部署脚本

## 版本信息
- 当前版本：1.2.0
- 作者：Kilo Code
- 日期：2025-10-08
- 许可证：GPL v3

## 开源协议
本项目采用 GPL v3 许可证，详情请参阅 LICENSE 文件。
