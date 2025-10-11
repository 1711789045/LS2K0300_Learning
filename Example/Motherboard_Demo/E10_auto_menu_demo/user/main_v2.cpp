/*********************************************************************************************************************
* LS2K0300 Auto Menu System V2
* Author: Kilo Code
* Date: 2025-10-08
* 
* This is an improved version of the auto menu system that addresses the following issues:
* 1. Menu can be exited to terminal using KEY_3+KEY_2
* 2. Projects run in background, allowing menu to continue
* 3. Better process management
* 4. Terminal can be used to start projects directly
**********************************************************************************************************************/

#include "zf_common_headfile.h"

// Hardware definitions
#define KEY_0       "/dev/zf_driver_gpio_key_0"
#define KEY_1       "/dev/zf_driver_gpio_key_1"
#define KEY_2       "/dev/zf_driver_gpio_key_2"
#define KEY_3       "/dev/zf_driver_gpio_key_3"

// Menu definitions
#define MENU_ITEMS  4
const char* menu_texts[MENU_ITEMS] = {
    "1. project",
    "2. Single Motor Control", 
    "3. Servo Control",
    "4. Encoder Test"
};

// Project paths
const char* project_paths[MENU_ITEMS] = {
    "/home/root/project",
    "/home/root/E01_01_button_switch_buzzer_demo",
    "/home/root/E02_04_drv8701e_double_motor_control_demo", 
    "/home/root/E06_04_ips200_display_demo"
};

// Global variables
int current_menu_item = 0;
uint8_t key_0_prev = 1;
uint8_t key_1_prev = 1;
uint8_t key_2_prev = 1;
uint8_t key_3_prev = 1;
#define DEBOUNCE_TIME 300

// Function prototypes
void run_selected_project(void);
void display_menu(void);
void check_keys(void);
void sigint_handler(int signum);

// Display menu
void display_menu() {
    ips200_clear();
    ips200_full(RGB565_WHITE);
    
    // Display title
    ips200_show_string(0, 16*3, "Select Program:");
    
    // Display menu items
    for(int i = 0; i < MENU_ITEMS; i++) {
        if(i == current_menu_item) {
            // Selected item in red with ">" prefix
            char selected_text[32];
            sprintf(selected_text, "> %s", menu_texts[i]);
            ips200_show_string(0, 16*(5+i), selected_text);
        } else {
            // Unselected items in black
            ips200_show_string(0, 16*(5+i), menu_texts[i]);
        }
    }
    
    // Display instructions
    ips200_show_string(0, 16*10, "KEY_3:Up KEY_2:Down");
    ips200_show_string(0, 16*11, "KEY_1:Enter KEY_0:Cancel");
    ips200_show_string(0, 16*12, "KEY_3+KEY_2:Exit Menu");
}

// Check keys
void check_keys() {
    uint8_t key_0_curr = gpio_get_level(KEY_0);
    uint8_t key_1_curr = gpio_get_level(KEY_1);
    uint8_t key_2_curr = gpio_get_level(KEY_2);
    uint8_t key_3_curr = gpio_get_level(KEY_3);
    
    // Check for KEY_3+KEY_2 combination (exit menu)
    if(key_3_curr == 0 && key_3_prev == 1 && key_2_curr == 0 && key_2_prev == 1) {
        printf("Exiting menu to terminal...\n");
        ips200_clear();
        ips200_full(RGB565_WHITE);
        ips200_show_string(0, 16*3, "Menu exited");
        ips200_show_string(0, 16*4, "You can now use terminal");
        exit(0);
    }
    
    // Check for KEY_3 press (move up)
    if(key_3_curr == 0 && key_3_prev == 1) {
        current_menu_item--;
        if(current_menu_item < 0) {
            current_menu_item = MENU_ITEMS - 1;
        }
        display_menu();
        system_delay_ms(DEBOUNCE_TIME);
    }
    
    // Check for KEY_2 press (move down)
    if(key_2_curr == 0 && key_2_prev == 1) {
        current_menu_item++;
        if(current_menu_item >= MENU_ITEMS) {
            current_menu_item = 0;
        }
        display_menu();
        system_delay_ms(DEBOUNCE_TIME);
    }
    
    // Check for KEY_1 press (run project)
    if(key_1_curr == 0 && key_1_prev == 1) {
        run_selected_project();
        system_delay_ms(DEBOUNCE_TIME);
    }
    
    // Check for KEY_0 press (reset selection)
    if(key_0_curr == 0 && key_0_prev == 1) {
        current_menu_item = 0;
        display_menu();
        system_delay_ms(DEBOUNCE_TIME);
    }
    
    // Update key states
    key_0_prev = key_0_curr;
    key_1_prev = key_1_curr;
    key_2_prev = key_2_curr;
    key_3_prev = key_3_curr;
}

// Run selected project
void run_selected_project() {
    ips200_clear();
    ips200_full(RGB565_WHITE);
    
    switch(current_menu_item) {
        case 0:
            ips200_show_string(0, 16*3, "Starting project...");
            ips200_show_string(0, 16*4, "Please wait...");
            system_delay_ms(1000);
            // Run project in background
            system("cd /home/root/project && ./project &");
            break;
            
        case 1:
            ips200_show_string(0, 16*3, "Starting Single Motor...");
            ips200_show_string(0, 16*4, "Please wait...");
            system_delay_ms(1000);
            // Run motor demo in background
            system("cd /home/root/E01_01_button_switch_buzzer_demo && ./E01_01_button_switch_buzzer_demo &");
            break;
            
        case 2:
            ips200_show_string(0, 16*3, "Starting Motor Control...");
            ips200_show_string(0, 16*4, "Please wait...");
            system_delay_ms(1000);
            // Run motor control in background
            system("cd /home/root/E02_04_drv8701e_double_motor_control_demo && ./E02_04_drv8701e_double_motor_control_demo &");
            break;
            
        case 3:
            ips200_show_string(0, 16*3, "Starting Encoder Test...");
            ips200_show_string(0, 16*4, "Please wait...");
            system_delay_ms(1000);
            // Run encoder test in background
            system("cd /home/root/E06_04_ips200_display_demo && ./E06_04_ips200_display_demo &");
            break;
    }
    
    // Show return instructions
    ips200_show_string(0, 16*6, "Press KEY_0 to return to menu");
    ips200_show_string(0, 16*7, "Press KEY_3+KEY_2 to exit menu");
    
    // Wait for user input
    while(1) {
        uint8_t key_0_curr = gpio_get_level(KEY_0);
        uint8_t key_3_curr = gpio_get_level(KEY_3);
        uint8_t key_2_curr = gpio_get_level(KEY_2);
        
        if(key_0_curr == 0 && key_0_prev == 1) {
            // KEY_0 pressed, return to menu
            display_menu();
            break;
        }
        
        if(key_3_curr == 0 && key_3_prev == 1 && key_2_curr == 0 && key_2_prev == 1) {
            // KEY_3+KEY_2 pressed, exit to terminal
            printf("Exiting menu to terminal...\n");
            ips200_clear();
            ips200_full(RGB565_WHITE);
            ips200_show_string(0, 16*3, "Menu exited");
            ips200_show_string(0, 16*4, "You can now use terminal");
            exit(0);
        }
        
        key_0_prev = key_0_curr;
        key_3_prev = key_3_curr;
        key_2_prev = key_2_curr;
        system_delay_ms(50);
    }
}

// Signal handler
void sigint_handler(int signum) {
    printf("Received Ctrl+C, program will exit\n");
    exit(0);
}

int main(int, char**) {
    // Initialize IPS200 display
    ips200_init("/dev/fb0");
    
    // Register signal handler
    signal(SIGINT, sigint_handler);
    
    // Display menu
    display_menu();
    
    printf("Auto Menu System V2 Started\n");
    printf("Use KEY_3/KEY_2 to select, KEY_1 to enter, KEY_0 to cancel\n");
    printf("Press KEY_3+KEY_2 to exit to terminal\n");
    
    while(1) {
        // Check keys
        check_keys();
        
        // Prevent CPU overuse
        system_delay_ms(50);
    }
}