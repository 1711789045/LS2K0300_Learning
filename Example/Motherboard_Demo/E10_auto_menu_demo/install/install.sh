#!/bin/bash

# LS2K0300 自动菜单系统安装脚本
# 作者：Kilo Code
# 日期：2025-10-08

echo "=========================================="
echo "LS2K0300 自动菜单系统安装脚本"
echo "=========================================="

# 检查是否以root用户运行
if [ "$(id -u)" -ne 0 ]; then
    echo "错误：请使用root用户运行此脚本"
    echo "使用命令：sudo ./install.sh"
    exit 1
fi

# 步骤1：创建目录
echo "步骤1：创建目录..."
mkdir -p /home/root/E10_auto_menu_demo
mkdir -p /home/root/E10_auto_menu_demo/install

# 步骤2：复制服务文件
echo "步骤2：复制服务文件..."
cp auto_menu.service /etc/systemd/system/

# 步骤3：重新加载systemd
echo "步骤3：重新加载systemd..."
systemctl daemon-reload

# 步骤4：启动服务
echo "步骤4：启动服务..."
systemctl enable auto_menu.service
systemctl start auto_menu.service

# 步骤5：检查服务状态
echo "步骤5：检查服务状态..."
systemctl status auto_menu.service

# 步骤6：验证安装
echo "步骤6：验证安装..."
if systemctl is-active --quiet auto_menu.service; then
    echo "=========================================="
    echo "安装成功！"
    echo "=========================================="
    echo "服务已启动并设置为开机自启"
    echo "您可以通过以下命令检查服务状态："
    echo "  systemctl status auto_menu.service"
    echo "  ps aux | grep E10_auto_menu_demo | grep -v grep"
else
    echo "=========================================="
    echo "安装失败！"
    echo "=========================================="
    echo "请检查错误信息："
    echo "  journalctl -u auto_menu.service"
    exit 1
fi

echo ""
echo "使用说明："
echo "- 开机后会自动启动菜单系统"
echo "- 使用KEY_0-KEY_3按键控制菜单"
echo "- 查看日志：journalctl -u auto_menu.service"
echo "- 重启服务：systemctl restart auto_menu.service"
