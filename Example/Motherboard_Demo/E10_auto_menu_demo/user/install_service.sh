#!/bin/bash

# LS2K0300 自动菜单系统服务安装脚本
# 作者: Kilo Code
# 日期: 2025-10-07

echo "=========================================="
echo "LS2K0300 自动菜单系统服务安装脚本"
echo "=========================================="

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
echo "脚本目录: $SCRIPT_DIR"

# 检查编译后的程序是否存在
if [ ! -f "$SCRIPT_DIR/out/E10_auto_menu_demo" ]; then
    echo "错误: 编译后的程序不存在，请先运行 ./build.sh"
    exit 1
fi

# 复制编译后的程序到 /root 目录
echo "正在复制程序到 /root 目录..."
cp "$SCRIPT_DIR/out/E10_auto_menu_demo" /root/
if [ $? -eq 0 ]; then
    echo "? 程序复制成功"
else
    echo "? 程序复制失败"
    exit 1
fi

# 设置执行权限
chmod +x /root/E10_auto_menu_demo

# 复制服务文件
echo "正在安装服务文件..."
cp "$SCRIPT_DIR/auto_menu.service" /etc/systemd/system/
if [ $? -eq 0 ]; then
    echo "? 服务文件安装成功"
else
    echo "? 服务文件安装失败"
    exit 1
fi

# 重新加载systemd
echo "正在重新加载systemd..."
systemctl daemon-reload

# 启用服务
echo "正在启用开机自启..."
systemctl enable auto_menu.service

# 启动服务
echo "正在启动服务..."
systemctl start auto_menu.service

# 检查服务状态
echo "正在检查服务状态..."
sleep 2
systemctl status auto_menu.service | head -20

echo ""
echo "=========================================="
echo "安装完成！"
echo "=========================================="
echo "服务已设置为开机自启"
echo "您可以使用以下命令管理服务:"
echo "  查看状态: systemctl status auto_menu.service"
echo "  启动服务: systemctl start auto_menu.service"
echo "  停止服务: systemctl stop auto_menu.service"
echo "  重启服务: systemctl restart auto_menu.service"
echo "  禁用自启: systemctl disable auto_menu.service"
echo "  启用自启: systemctl enable auto_menu.service"
echo "  查看日志: journalctl -u auto_menu.service -f"
echo ""
echo "如果需要卸载服务，请运行:"
echo "  systemctl stop auto_menu.service"
echo "  systemctl disable auto_menu.service"
echo "  rm /etc/systemd/system/auto_menu.service"
echo "  systemctl daemon-reload"
echo "=========================================="