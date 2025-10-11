#!/bin/bash

# LS2K0300 Auto Menu System Start Script
# Author: Kilo Code
# Date: 2025-10-08

echo "=========================================="
echo "LS2K0300 Auto Menu System Start Options"
echo "=========================================="

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root"
    exit 1
fi

# Check if menu program exists
if [ ! -f "/home/root/E10_auto_menu_demo/E10_auto_menu_demo" ]; then
    echo "Error: Menu program not found"
    echo "Please compile and install first"
    exit 1
fi

echo "Select start option:"
echo "1. Start Auto Menu (recommended)"
echo "2. Start project directly"
echo "3. Start Single Motor Control directly"
echo "4. Start Motor Control directly"
echo "5. Start Encoder Test directly"
echo "6. Exit to terminal (no menu)"
echo ""
read -p "Enter your choice (1-6): " choice

case $choice in
    1)
        echo "Starting Auto Menu..."
        /home/root/E10_auto_menu_demo/E10_auto_menu_demo
        ;;
    2)
        echo "Starting project directly..."
        cd /home/root/project
        ./project
        ;;
    3)
        echo "Starting Single Motor Control directly..."
        cd /home/root/E01_01_button_switch_buzzer_demo
        ./E01_01_button_switch_buzzer_demo
        ;;
    4)
        echo "Starting Motor Control directly..."
        cd /home/root/E02_04_drv8701e_double_motor_control_demo
        ./E02_04_drv8701e_double_motor_control_demo
        ;;
    5)
        echo "Starting Encoder Test directly..."
        cd /home/root/E06_04_ips200_display_demo
        ./E06_04_ips200_display_demo
        ;;
    6)
        echo "Exiting to terminal..."
        echo "You can now run programs directly from terminal"
        exit 0
        ;;
    *)
        echo "Invalid choice"
        exit 1
        ;;
esac