#!/bin/bash

# LS2K0300 Auto Menu System Transfer Script
# Author: Kilo Code
# Date: 2025-10-08

echo "=========================================="
echo "LS2K0300 Auto Menu System Transfer Script"
echo "=========================================="

# Check parameters
if [ $# -eq 0 ] || [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "Usage: $0 <target_ip>"
    echo "Example: $0 172.20.10.7"
    echo ""
    echo "Options:"
    echo "  -h, --help     Show this help message"
    exit 0
fi

TARGET_IP=$1
TARGET_DIR="/home/root/E10_auto_menu_demo"
INSTALL_DIR="$TARGET_DIR/install"

echo "Target IP: $TARGET_IP"
echo "Target Directory: $TARGET_DIR"

# Check if can connect to target IP
if ! ssh root@$TARGET_IP "echo 'Connection test successful'" 2>/dev/null; then
    echo "Error: Cannot connect to target IP $TARGET_IP"
    echo "Please check:"
    echo "1. Network connection"
    echo "2. SSH service is running"
    echo "3. Firewall settings"
    exit 1
fi

echo "Connection test successful!"

# Step 1: Create directory structure
echo "Step 1: Create directory structure..."
ssh root@$TARGET_IP "mkdir -p $TARGET_DIR && mkdir -p $INSTALL_DIR"

# Step 2: Transfer installation files
echo "Step 2: Transfer installation files..."

# Transfer auto_menu.service
base64 ../install/auto_menu.service | ssh root@$TARGET_IP "base64 -d > $INSTALL_DIR/auto_menu.service"

# Transfer install.sh
base64 ../install/install.sh | ssh root@$TARGET_IP "base64 -d > $INSTALL_DIR/install.sh"

# Transfer install_service.sh
base64 ../install/install_service.sh | ssh root@$TARGET_IP "base64 -d > $INSTALL_DIR/install_service.sh"

# Step 3: Stop service and transfer compiled program
if [ -f "../out/E10_auto_menu_demo" ]; then
    echo "Step 3: Stop service..."
    ssh root@$TARGET_IP "systemctl stop auto_menu.service"
    
    echo "Step 4: Transfer program files..."
    base64 ../out/E10_auto_menu_demo | ssh root@$TARGET_IP "base64 -d > $TARGET_DIR/E10_auto_menu_demo"
    
    echo "Step 5: Set execute permissions..."
    ssh root@$TARGET_IP "chmod +x $TARGET_DIR/E10_auto_menu_demo"
    
    echo "Step 6: Start service..."
    ssh root@$TARGET_IP "systemctl start auto_menu.service"
    
    echo "Step 7: Check service status..."
    ssh root@$TARGET_IP "systemctl status auto_menu.service | head -5"
else
    echo "Warning: Program file does not exist, please compile first"
    echo "Run: cd .. && ./build.sh"
    exit 1
fi

# Step 4: Set execute permissions
echo "Step 4: Set execute permissions..."
ssh root@$TARGET_IP "chmod +x $TARGET_DIR/E10_auto_menu_demo $INSTALL_DIR/*.sh"

# Step 5: Install service
echo "Step 5: Install service..."
ssh root@$TARGET_IP "cd $INSTALL_DIR && ./install.sh"

# Step 6: Verify installation
echo "Step 6: Verify installation..."
ssh root@$TARGET_IP "systemctl status auto_menu.service | head -10"

echo "=========================================="
echo "Transfer completed!"
echo "=========================================="
echo "If installation is successful, the system will automatically start the menu on next boot"
echo "You can check the status with the following commands:"
echo "  ssh root@$TARGET_IP \"systemctl status auto_menu.service\""
echo "  ssh root@$TARGET_IP \"ps aux | grep E10_auto_menu_demo | grep -v grep\""