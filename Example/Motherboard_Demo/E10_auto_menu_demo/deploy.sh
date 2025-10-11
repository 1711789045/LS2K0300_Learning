#!/bin/bash

# 部署脚本 - 将自动菜单系统部署到LS2K0300板卡
echo "=== LS2K0300 自动菜单系统部署脚本 ==="

# 设置板卡IP地址
BOARD_IP="192.168.1.101"
BOARD_USER="root"

# 检查必要文件是否存在
echo "检查必要文件..."
if [ ! -f "user/main.cpp" ]; then
    echo "错误: main.cpp 文件不存在!"
    exit 1
fi

if [ ! -f "user/CMakeLists.txt" ]; then
    echo "错误: CMakeLists.txt 文件不存在!"
    exit 1
fi

if [ ! -f "user/cross.cmake" ]; then
    echo "错误: cross.cmake 文件不存在!"
    exit 1
fi

if [ ! -f "user/auto_menu.service" ]; then
    echo "错误: auto_menu.service 文件不存在!"
    exit 1
fi

# 编译项目
echo "编译项目..."
cd user
chmod +x build.sh
./build.sh

if [ $? -ne 0 ]; then
    echo "编译失败!"
    exit 1
fi

# 创建临时目录
TEMP_DIR="/tmp/auto_menu_deploy"
mkdir -p $TEMP_DIR

# 复制所有必要文件
echo "准备部署文件..."
cp E10_auto_menu_demo $TEMP_DIR/
cp auto_menu.service $TEMP_DIR/
cp start_menu.sh $TEMP_DIR/
cp install_service.sh $TEMP_DIR/
cp ../README.md $TEMP_DIR/

# 创建板卡上的目录
echo "创建板卡目录..."
ssh $BOARD_USER@$BOARD_IP "mkdir -p /home/root/E10_auto_menu_demo"

# 传输文件
echo "传输文件到板卡..."
scp -r $TEMP_DIR/* $BOARD_USER@$BOARD_IP:/home/root/E10_auto_menu_demo/

# 设置执行权限
echo "设置执行权限..."
ssh $BOARD_USER@$BOARD_IP "chmod +x /home/root/E10_auto_menu_demo/E10_auto_menu_demo"
ssh $BOARD_USER@$BOARD_IP "chmod +x /home/root/E10_auto_menu_demo/start_menu.sh"
ssh $BOARD_USER@$BOARD_IP "chmod +x /home/root/E10_menu_demo/install_service.sh"

# 停止现有服务
echo "停止现有服务..."
ssh $BOARD_USER@$BOARD_IP "systemctl stop auto_menu"

# 安装服务
echo "安装服务..."
ssh $BOARD_USER@$BOARD_IP "cd /home/root/E10_auto_menu_demo && ./install_service.sh"

# 启动服务
echo "启动服务..."
ssh $BOARD_USER@$BOARD_IP "systemctl daemon-reload"
ssh $BOARD_USER@$BOARD_IP "systemctl start auto_menu"
ssh $BOARD_USER@$BOARD_IP "systemctl enable auto_menu"

# 检查服务状态
echo "检查服务状态..."
ssh $BOARD_USER@$BOARD_IP "systemctl status auto_menu"

# 清理临时文件
rm -rf $TEMP_DIR

echo "=== 部署完成 ==="
echo "板卡IP: $BOARD_IP"
echo "用户名: $BOARD_USER"
echo "菜单程序已安装并设置为开机自启"
echo "使用方法:"
echo "1. 开机后自动显示菜单"
echo "2. 使用KEY_3/KEY_2选择项目"
echo "3. 使用KEY_1运行项目"
echo "4. 使用KEY_0返回菜单"
echo "5. 使用KEY_3+KEY_2退出菜单到终端"
echo ""
echo "如需手动启动菜单，请运行:"
echo "ssh $BOARD_USER@$BOARD_IP"
echo "cd /home/root/E10_auto_menu_demo"
echo "./start_menu.sh"