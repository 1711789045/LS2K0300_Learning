#!/bin/bash

# LS2K0300 Auto Menu System Service Path Fix Script
# Author: Kilo Code
# Date: 2025-10-08

echo "=========================================="
echo "LS2K0300 Auto Menu System Service Path Fix"
echo "=========================================="

# Check if we can connect to the target IP
if [ $# -eq 0 ]; then
    echo "Usage: $0 <target_ip>"
    echo "Example: $0 172.20.10.7"
    exit 1
fi

TARGET_IP=$1

echo "Target IP: $TARGET_IP"

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

# Fix the service path
echo "Fixing service path..."
ssh root@$TARGET_IP "sed -i 's|ExecStart=/root/E10_auto_menu_demo|ExecStart=/home/root/E10_auto_menu_demo/E10_auto_menu_demo|' /etc/systemd/system/auto_menu.service"

# Reload systemd and restart service
echo "Reloading systemd and restarting service..."
ssh root@$TARGET_IP "systemctl daemon-reload && systemctl restart auto_menu.service"

# Check service status
echo "Checking service status..."
ssh root@$TARGET_IP "systemctl status auto_menu.service | head -10"

echo "=========================================="
echo "Service path fix completed!"
echo "=========================================="