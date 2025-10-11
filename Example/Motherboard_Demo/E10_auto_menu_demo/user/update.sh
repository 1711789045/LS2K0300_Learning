#!/bin/bash

# LS2K0300 自动菜单系统一键更新脚本
# 作者：Kilo Code
# 日期：2025-10-08

echo "=========================================="
echo "LS2K0300 自动菜单系统更新脚本"
echo "=========================================="

# 检查是否在正确的目录
if [ ! -f "main.cpp" ] || [ ! -f "build.sh" ]; then
    echo "错误：请在正确的目录下运行此脚本"
    exit 1
fi

# 步骤1：编译程序
echo "步骤1：编译程序..."
./build.sh

if [ $? -ne 0 ]; then
    echo "编译失败，请检查错误信息"
    exit 1
fi

# 步骤2：传输到开发板
echo "步骤2：传输程序到开发板..."
base64 out/E10_auto_menu_demo | ssh root@172.20.10.7 "base64 -d > /tmp/E10_auto_menu_demo_new && chmod +x /tmp/E10_auto_menu_demo_new"

if [ $? -ne 0 ]; then
    echo "传输失败，请检查网络连接"
    exit 1
fi

# 步骤3：更新开发板上的程序
echo "步骤3：更新开发板上的程序..."
ssh root@172.20.10.7 "
    echo '停止当前服务...'
    systemctl stop auto_menu.service
    
    echo '替换程序文件...'
    cp /tmp/E10_auto_menu_demo_new /home/root/E10_auto_menu_demo/E10_auto_menu_demo
    
    echo '启动服务...'
    systemctl start auto_menu.service
    
    echo '检查服务状态...'
    systemctl status auto_menu.service
"

if [ $? -eq 0 ]; then
    echo "=========================================="
    echo "更新成功！"
    echo "=========================================="
    echo "程序已成功更新并启动"
    echo "您可以通过以下命令检查程序状态："
    echo "ssh root@172.20.10.7 \"ps aux | grep E10_auto_menu_demo | grep -v grep\""
else
    echo "更新失败，请检查错误信息"
    exit 1
fi
