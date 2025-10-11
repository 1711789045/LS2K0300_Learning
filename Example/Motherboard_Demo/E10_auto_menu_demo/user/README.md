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

## 系统要求
- 龙芯久久派2K0300开发板
- 逐飞学习板
- IPS200屏幕
- LoongOS操作系统

## 文件结构
```
E10_auto_menu_demo/
├── user/
│   ├── main.cpp              # 主程序文件
│   ├── CMakeLists.txt        # CMake配置文件
│   ├── cross.cmake           # 交叉编译配置
│   ├── build.sh              # 构建脚本
│   ├── update.sh             # 更新脚本
│   ├── auto_menu.service     # systemd服务文件
│   ├── install_service.sh    # 服务安装脚本
│   └── update_instructions.md # 更新说明文档
└── libraries/                # 依赖库文件
```

## 安装说明
1. 确保系统已安装必要的依赖库
2. 运行安装脚本：
   ```bash
   ./install_service.sh
   ```
3. 重启系统或手动启动服务：
   ```bash
   systemctl start auto_menu.service
   ```

## 使用说明

### 开机启动
系统启动后会自动运行菜单程序，在IPS200屏幕上显示以下菜单：
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

### 菜单选项
1. **project** - 项目工程
2. **Single Motor Control** - 单电机控制（实际为双电机控制）
3. **Servo Control** - 舵机控制（实际为按键开关蜂鸣器）
4. **Encoder Test** - 编码器测试（实际为IPS200显示）

## 更新程序

### 方法一：使用一键更新脚本
```bash
./update.sh
```

### 方法二：手动更新
1. 修改main.cpp文件
2. 重新编译：
   ```bash
   ./build.sh
   ```
3. 传输到开发板：
   ```bash
   base64 out/E10_auto_menu_demo | ssh root@172.20.10.7 "base64 -d > /tmp/E10_auto_menu_demo_new && chmod +x /tmp/E10_auto_menu_demo_new"
   ```
4. 更新开发板上的程序：
   ```bash
   ssh root@172.20.10.7 "
       systemctl stop auto_menu.service
       cp /tmp/E10_auto_menu_demo_new /home/root/E10_auto_menu_demo/E10_auto_menu_demo
       systemctl start auto_menu.service
   "
   ```

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

## 技术支持
如有问题，请联系技术支持或查看相关文档。

## 版本信息
- 版本：1.0
- 作者：Kilo Code
- 日期：2025-10-08
