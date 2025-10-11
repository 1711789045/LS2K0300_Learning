#!/bin/bash

# LS2K0300 自动菜单系统构建脚本
# 作者：Kilo Code
# 日期：2025-10-08

echo "=========================================="
echo "LS2K0300 自动菜单系统构建脚本"
echo "=========================================="

# 检查是否在正确的目录
if [ ! -f "main.cpp" ] || [ ! -f "CMakeLists.txt" ]; then
    echo "错误：请在正确的目录下运行此脚本"
    exit 1
fi

# 创建输出目录
mkdir -p out

# 步骤1：清理之前的构建
echo "步骤1：清理之前的构建..."
make clean 2>/dev/null || true

# 步骤2：配置CMake
echo "步骤2：配置CMake..."
cmake -B out -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "CMake配置失败"
    exit 1
fi

# 步骤3：编译程序
echo "步骤3：编译程序..."
cmake --build out

if [ $? -ne 0 ]; then
    echo "编译失败"
    exit 1
fi

# 步骤4：检查编译结果
if [ ! -f "out/E10_auto_menu_demo" ]; then
    echo "错误：编译失败，未生成可执行文件"
    exit 1
fi

echo "=========================================="
echo "编译成功！"
echo "=========================================="
echo "可执行文件：out/E10_auto_menu_demo"
echo "文件大小：$(ls -lh out/E10_auto_menu_demo | awk '{print $5}')"

# 步骤5：传输到开发板（可选）
if [ "$1" = "--transfer" ] || [ "$1" = "-t" ]; then
    echo "步骤5：传输程序到开发板..."
    
    # 检查是否可以访问开发板
    if ! ssh root@172.20.10.7 "echo '连接测试成功'" 2>/dev/null; then
        echo "警告：无法连接到开发板，请检查网络连接"
        echo "您可以稍后手动传输："
        echo "base64 out/E10_auto_menu_demo | ssh root@172.20.10.7 \"base64 -d > /tmp/E10_auto_menu_demo_new && chmod +x /tmp/E10_auto_menu_demo_new\""
    else
        # 停止服务
        echo "停止服务..."
        ssh root@172.20.10.7 "systemctl stop auto_menu.service"
        
        # 传输文件
        echo "传输文件..."
        base64 out/E10_auto_menu_demo | ssh root@172.20.10.7 "base64 -d > /home/root/E10_auto_menu_demo/E10_auto_menu_demo && chmod +x /home/root/E10_auto_menu_demo/E10_auto_menu_demo"
        
        if [ $? -eq 0 ]; then
            echo "传输成功！"
            echo "文件已更新到：/home/root/E10_auto_menu_demo/E10_auto_menu_demo"
            
            # 启动服务
            echo "启动服务..."
            ssh root@172.20.10.7 "systemctl start auto_menu.service"
            
            # 检查服务状态
            echo "检查服务状态..."
            ssh root@172.20.10.7 "systemctl status auto_menu.service | head -5"
        else
            echo "传输失败"
            # 如果传输失败，尝试重新启动服务
            ssh root@172.20.10.7 "systemctl start auto_menu.service"
        fi
    fi
fi

echo "=========================================="
echo "构建完成！"
echo "=========================================="
