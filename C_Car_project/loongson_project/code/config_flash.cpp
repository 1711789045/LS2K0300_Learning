/*********************************************************************************************************************
* LS2K0300 Opensourec Library 即（LS2K0300 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是LS2K0300 开源库的一部分
*
* LS2K0300 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          config_flash
* 公司名称          成都逐飞科技有限公司
* 适用平台          LS2K0300
* 店铺链接          https://seekfree.taobao.com/
********************************************************************************************************************/

#include "config_flash.h"
#include "servo.h"          // 引入舵机模块（包含舵机参数声明）
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 未来可以添加更多模块头文件：
// #include "motor.h"      // 电机模块
// #include "image.h"      // 图像处理模块
// ... 等

// ==================== 内部数据结构 ====================

// 配置项类型枚举
typedef enum {
    CONFIG_TYPE_FLOAT = 0,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_INT16,
    CONFIG_TYPE_UINT8,
    CONFIG_TYPE_UINT16,
    CONFIG_TYPE_UINT32
} config_type_e;

// 配置项结构体
typedef struct {
    char key[32];           // 参数名
    void *var_ptr;          // 变量指针
    config_type_e type;     // 变量类型
    union {
        float f;
        int i;
        int16 i16;
        uint8 u8;
        uint16 u16;
        uint32 u32;
    } default_val;          // 默认值
    char comment[64];       // 注释
} config_item_t;

// 配置管理器
static config_item_t config_items[CONFIG_MAX_ITEMS];
static uint8 config_item_count = 0;

// ==================== 内部函数实现 ====================

/**
 * @brief  添加 float 类型配置项
 */
void config_add_float(const char *key, float *var, float default_val, const char *comment)
{
    if (config_item_count >= CONFIG_MAX_ITEMS)
    {
        printf("[CONFIG ERROR] Config items limit exceeded!\r\n");
        return;
    }

    config_item_t *item = &config_items[config_item_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    item->var_ptr = var;
    item->type = CONFIG_TYPE_FLOAT;
    item->default_val.f = default_val;
    strncpy(item->comment, comment, sizeof(item->comment) - 1);

    config_item_count++;
}

/**
 * @brief  添加 int 类型配置项
 */
void config_add_int(const char *key, int *var, int default_val, const char *comment)
{
    if (config_item_count >= CONFIG_MAX_ITEMS)
    {
        printf("[CONFIG ERROR] Config items limit exceeded!\r\n");
        return;
    }

    config_item_t *item = &config_items[config_item_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    item->var_ptr = var;
    item->type = CONFIG_TYPE_INT;
    item->default_val.i = default_val;
    strncpy(item->comment, comment, sizeof(item->comment) - 1);

    config_item_count++;
}

/**
 * @brief  添加 int16 类型配置项
 */
void config_add_int16(const char *key, int16 *var, int16 default_val, const char *comment)
{
    if (config_item_count >= CONFIG_MAX_ITEMS)
    {
        printf("[CONFIG ERROR] Config items limit exceeded!\r\n");
        return;
    }

    config_item_t *item = &config_items[config_item_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    item->var_ptr = var;
    item->type = CONFIG_TYPE_INT16;
    item->default_val.i16 = default_val;
    strncpy(item->comment, comment, sizeof(item->comment) - 1);

    config_item_count++;
}

/**
 * @brief  添加 uint8 类型配置项
 */
void config_add_uint8(const char *key, uint8 *var, uint8 default_val, const char *comment)
{
    if (config_item_count >= CONFIG_MAX_ITEMS)
    {
        printf("[CONFIG ERROR] Config items limit exceeded!\r\n");
        return;
    }

    config_item_t *item = &config_items[config_item_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    item->var_ptr = var;
    item->type = CONFIG_TYPE_UINT8;
    item->default_val.u8 = default_val;
    strncpy(item->comment, comment, sizeof(item->comment) - 1);

    config_item_count++;
}

/**
 * @brief  添加 uint16 类型配置项
 */
void config_add_uint16(const char *key, uint16 *var, uint16 default_val, const char *comment)
{
    if (config_item_count >= CONFIG_MAX_ITEMS)
    {
        printf("[CONFIG ERROR] Config items limit exceeded!\r\n");
        return;
    }

    config_item_t *item = &config_items[config_item_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    item->var_ptr = var;
    item->type = CONFIG_TYPE_UINT16;
    item->default_val.u16 = default_val;
    strncpy(item->comment, comment, sizeof(item->comment) - 1);

    config_item_count++;
}

/**
 * @brief  添加 uint32 类型配置项
 */
void config_add_uint32(const char *key, uint32 *var, uint32 default_val, const char *comment)
{
    if (config_item_count >= CONFIG_MAX_ITEMS)
    {
        printf("[CONFIG ERROR] Config items limit exceeded!\r\n");
        return;
    }

    config_item_t *item = &config_items[config_item_count];
    strncpy(item->key, key, sizeof(item->key) - 1);
    item->var_ptr = var;
    item->type = CONFIG_TYPE_UINT32;
    item->default_val.u32 = default_val;
    strncpy(item->comment, comment, sizeof(item->comment) - 1);

    config_item_count++;
}

/**
 * @brief  注册所有配置变量
 * @note   所有需要保存的变量都在这里注册
 *         添加新变量只需要在这里添加一行 config_add_xxx()
 */
void config_register_all(void)
{
    // 舵机参数
    config_add_float("servo_mid",  &g_servo_mid,   69.7,  "Servo motor middle position (degree)");
    config_add_float("servo_kp0",  &servo_pid_kp0, 0.1,   "Servo PID Kp0 parameter (constant term)");
    config_add_float("servo_kp2",  &servo_pid_kp2, 0.35,  "Servo PID Kp2 parameter (quadratic coefficient)");
    config_add_float("servo_ki",   &servo_pid_ki,  0.0,   "Servo PID Ki parameter");
    config_add_float("servo_kd1",  &servo_pid_kd1, 0.56,  "Servo PID Kd1 parameter");
    config_add_float("servo_kd2",  &servo_pid_kd2, 0.0,   "Servo PID Kd2 parameter");

    // 电机参数
    config_add_float("motor_kp",   &motor_pid_kp,  8.0,   "Motor PID Kp parameter");
    config_add_float("motor_ki",   &motor_pid_ki,  2.0,   "Motor PID Ki parameter");
    config_add_float("motor_kd",   &motor_pid_kd,  4.0,   "Motor PID Kd parameter");

    // 控制参数
    config_add_int16("speed",            &speed,             2000, "Target speed");
    config_add_float("dif_speed_reduce", &dif_speed_reduce, 0.3,  "Differential speed reduce coefficient");
    config_add_float("dif_speed_plus",   &dif_speed_plus,   0.3,  "Differential speed plus coefficient");

    // 图像处理参数
    config_add_uint16("img_mid_weight",   &mid_weight_select,   2,  "Mid-line weight array selection (1-5)");

    printf("[CONFIG] Registered %d configuration items.\r\n", config_item_count);
}

/**
 * @brief  加载配置文件
 * @return true=成功, false=失败
 */
bool config_load(void)
{
    FILE *fp = fopen(CONFIG_FILE_PATH, "r");
    if (fp == NULL)
    {
        printf("[CONFIG] Config file not found, creating default configuration...\r\n");

        // 文件不存在，使用默认值并创建配置文件
        for (uint8 i = 0; i < config_item_count; i++)
        {
            config_item_t *item = &config_items[i];

            // 将默认值赋给变量
            switch (item->type)
            {
                case CONFIG_TYPE_FLOAT:
                    *(float *)item->var_ptr = item->default_val.f;
                    break;
                case CONFIG_TYPE_INT:
                    *(int *)item->var_ptr = item->default_val.i;
                    break;
                case CONFIG_TYPE_INT16:
                    *(int16 *)item->var_ptr = item->default_val.i16;
                    break;
                case CONFIG_TYPE_UINT8:
                    *(uint8 *)item->var_ptr = item->default_val.u8;
                    break;
                case CONFIG_TYPE_UINT16:
                    *(uint16 *)item->var_ptr = item->default_val.u16;
                    break;
                case CONFIG_TYPE_UINT32:
                    *(uint32 *)item->var_ptr = item->default_val.u32;
                    break;
            }
        }

        // 创建默认配置文件
        config_save();
        return true;
    }

    // 读取配置文件
    char line[128];
    uint16 loaded_count = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // 跳过注释和空行
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;

        // 解析 key = value
        char key[32] = {0};
        char value[64] = {0};

        if (sscanf(line, "%s = %s", key, value) != 2)
            continue;

        // 查找对应的配置项
        for (uint8 i = 0; i < config_item_count; i++)
        {
            config_item_t *item = &config_items[i];

            if (strcmp(key, item->key) == 0)
            {
                // 找到匹配的配置项，根据类型解析值
                switch (item->type)
                {
                    case CONFIG_TYPE_FLOAT:
                        *(float *)item->var_ptr = atof(value);
                        break;
                    case CONFIG_TYPE_INT:
                        *(int *)item->var_ptr = atoi(value);
                        break;
                    case CONFIG_TYPE_INT16:
                        *(int16 *)item->var_ptr = (int16)atoi(value);
                        break;
                    case CONFIG_TYPE_UINT8:
                        *(uint8 *)item->var_ptr = (uint8)atoi(value);
                        break;
                    case CONFIG_TYPE_UINT16:
                        *(uint16 *)item->var_ptr = (uint16)atoi(value);
                        break;
                    case CONFIG_TYPE_UINT32:
                        *(uint32 *)item->var_ptr = (uint32)atol(value);
                        break;
                }

                loaded_count++;
                break;
            }
        }
    }

    fclose(fp);
    printf("[CONFIG] Loaded %d configuration items from file.\r\n", loaded_count);
    return true;
}

/**
 * @brief  保存配置文件
 */
void config_save(void)
{
    FILE *fp = fopen(CONFIG_FILE_PATH, "w");
    if (fp == NULL)
    {
        printf("[CONFIG ERROR] Failed to open config file for writing!\r\n");
        return;
    }

    // 写入文件头
    fprintf(fp, "# LS2K0300 Configuration File\n");
    fprintf(fp, "# Auto-generated by config_flash library\n");

    // 获取当前时间
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "# Generated on %04d-%02d-%02d %02d:%02d:%02d\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);

    fprintf(fp, "# Format: key = value  # comment\n");
    fprintf(fp, "# You can edit values manually and reboot to apply\n\n");

    // 写入所有配置项
    for (uint8 i = 0; i < config_item_count; i++)
    {
        config_item_t *item = &config_items[i];

        // 写入注释
        fprintf(fp, "# %s\n", item->comment);

        // 写入 key = value
        fprintf(fp, "%s = ", item->key);

        // 根据类型写入值
        switch (item->type)
        {
            case CONFIG_TYPE_FLOAT:
                fprintf(fp, "%.2f\n", *(float *)item->var_ptr);
                break;
            case CONFIG_TYPE_INT:
                fprintf(fp, "%d\n", *(int *)item->var_ptr);
                break;
            case CONFIG_TYPE_INT16:
                fprintf(fp, "%d\n", *(int16 *)item->var_ptr);
                break;
            case CONFIG_TYPE_UINT8:
                fprintf(fp, "%u\n", *(uint8 *)item->var_ptr);
                break;
            case CONFIG_TYPE_UINT16:
                fprintf(fp, "%u\n", *(uint16 *)item->var_ptr);
                break;
            case CONFIG_TYPE_UINT32:
                fprintf(fp, "%u\n", *(uint32 *)item->var_ptr);
                break;
        }

        fprintf(fp, "\n");  // 空行分隔
    }

    fclose(fp);
    printf("[CONFIG] Configuration saved to %s\r\n", CONFIG_FILE_PATH);
}

// ==================== 对外API ====================

/**
 * @brief  初始化配置系统
 */
void config_init(void)
{
    printf("[CONFIG] Initializing configuration system...\r\n");

    // 1. 注册所有变量
    config_register_all();

    // 2. 加载配置文件
    config_load();

    printf("[CONFIG] Configuration system initialized.\r\n");
}
