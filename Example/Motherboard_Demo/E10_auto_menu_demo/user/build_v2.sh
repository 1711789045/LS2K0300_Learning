#!/bin/bash

# LS2K0300 Auto Menu System V2 Build Script
# Author: Kilo Code
# Date: 2025-10-08

echo "=========================================="
echo "LS2K0300 Auto Menu System V2 Build Script"
echo "=========================================="

# Check if in correct directory
if [ ! -f "main_v2.cpp" ] || [ ! -f "CMakeLists_v2.txt" ]; then
    echo "Error: Please run this script in the correct directory"
    exit 1
fi

# Create output directory
mkdir -p out

# Step 1: Clean previous build
echo "Step 1: Cleaning previous build..."
make clean 2>/dev/null || true

# Step 2: Configure CMake
echo "Step 2: Configuring CMake..."
cmake -B out -DCMAKE_BUILD_TYPE=Release -C cross.cmake

if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

# Step 3: Build program
echo "Step 3: Building program..."
cmake --build out

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

# Step 4: Check build result
if [ ! -f "out/E10_auto_menu_demo_v2" ]; then
    echo "Error: Build failed, executable not generated"
    exit 1
fi

echo "=========================================="
echo "Build successful!"
echo "=========================================="
echo "Executable: out/E10_auto_menu_demo_v2"
echo "File size: $(ls -lh out/E10_auto_menu_demo_v2 | awk '{print $5}')"

# Step 5: Transfer to development board (optional)
if [ "$1" = "--transfer" ] || [ "$1" = "-t" ]; then
    echo "Step 5: Transferring program to development board..."
    
    # Check if can access development board
    if ! ssh root@172.20.10.7 "echo 'Connection test successful'" 2>/dev/null; then
        echo "Warning: Cannot connect to development board, please check network connection"
        echo "You can manually transfer later:"
        echo "base64 out/E10_auto_menu_demo_v2 | ssh root@172.20.10.7 \"base64 -d > /tmp/E10_auto_menu_demo_v2_new && chmod +x /tmp/E10_auto_menu_demo_v2_new\""
    else
        # Stop service
        echo "Stopping service..."
        ssh root@172.20.10.7 "systemctl stop auto_menu.service"
        
        # Transfer file
        echo "Transferring file..."
        base64 out/E10_auto_menu_demo_v2 | ssh root@172.20.10.7 "base64 -d > /home/root/E10_auto_menu_demo/E10_auto_menu_demo_v2 && chmod +x /home/root/E10_auto_menu_demo/E10_auto_menu_demo_v2"
        
        if [ $? -eq 0 ]; then
            echo "Transfer successful!"
            echo "File updated to: /home/root/E10_auto_menu_demo/E10_auto_menu_demo_v2"
            
            # Start service
            echo "Starting service..."
            ssh root@172.20.10.7 "systemctl start auto_menu.service"
            
            # Check service status
            echo "Checking service status..."
            ssh root@172.20.10.7 "systemctl status auto_menu.service | head -5"
        else
            echo "Transfer failed"
            # Try to restart service if transfer failed
            ssh root@172.20.10.7 "systemctl start auto_menu.service"
        fi
    fi
fi

echo "=========================================="
echo "Build completed!"
echo "=========================================="