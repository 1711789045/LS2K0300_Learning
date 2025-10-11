#!/bin/bash

# LS2K0300 自动菜单系统完整传输脚本
# 作者：Kilo Code
# 日期：2025-10-08

echo "=========================================="
echo "LS2K0300 自动菜单系统完整传输脚本"
echo "=========================================="

# 检查参数
if [ $# -eq 0 ]; then
    echo "用法：$0 <目标IP>"
    echo "示例：$0 172.20.10.7"
    exit 1
fi

TARGET_IP=$1
TARGET_DIR="/home/root/E10_auto_menu_demo"
INSTALL_DIR="$TARGET_DIR/install"

echo "目标IP: $TARGET_IP"
echo "目标目录: $TARGET_DIR"

# 检查是否可以连接到目标IP
if ! ssh root@$TARGET_IP "echo '连接测试成功'" 2>/dev/null; then
    echo "错误：无法连接到目标IP $TARGET_IP"
    echo "请检查："
    echo "1. 网络连接"
    echo "2. SSH服务是否运行"
    echo "3. 防火墙设置"
    exit 1
fi

echo "连接测试成功！"

# 步骤1：创建目录结构
echo "步骤1：创建目录结构..."
ssh root@$TARGET_IP "mkdir -p $TARGET_DIR && mkdir -p $INSTALL_DIR"

# 步骤2：传输安装文件
echo "步骤2：传输安装文件..."

# 传输auto_menu.service
base64 install/auto_menu.service | ssh root@$TARGET_IP "base64 -d > $INSTALL_DIR/auto_menu.service"

# 传输install.sh
base64 install/install.sh | ssh root@$TARGET_IP "base64 -d > $INSTALL_DIR/install.sh"

# 传输install_service.sh
base64 install/install_service.sh | ssh root@$TARGET_IP "base64 -d > $INSTALL_DIR/install_service.sh"

# 步骤3：传输编译好的程序
if [ -f "user/out/E10_auto_menu_demo" ]; then
    echo "步骤3：传输程序文件..."
    base64 user/out/E10_auto_menu_demo | ssh root@$TARGET_IP "base64 -d > $TARGET_DIR/E10_auto_menu_demo"
else
    echo "警告：程序文件不存在，请先编译"
    echo "运行：cd user && ./build.sh"
    exit 1
fi

# 步骤4：设置执行权限
echo "步骤4：设置执行权限..."
ssh root@$TARGET_IP "chmod +x $TARGET_DIR/E10_auto_menu_demo $INSTALL_DIR/*.sh"

# 步骤5：安装服务
echo "步骤5：安装服务..."
ssh root@$TARGET_IP "cd $INSTALL_DIR && ./install.sh"

# 步骤6：验证安装
echo "步骤6：验证安装..."
ssh root@$TARGET_IP "systemctl status auto_menu.service | head -10"

echo "=========================================="
echo "传输完成！"
echo "=========================================="
echo "如果安装成功，系统将在下次开机时自动启动菜单"
echo "您可以通过以下命令检查状态："
echo "  ssh root@$TARGET_IP \"systemctl status auto_menu.service\""
echo "  ssh root@$TARGET_IP \"ps aux | grep E10_auto_menu_demo | grep -v grep\""
