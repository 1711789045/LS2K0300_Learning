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

#ifndef __CONFIG_FLASH_H__
#define __CONFIG_FLASH_H__

#include "zf_common_typedef.h"

// ==================== 移植配置 ====================
// 配置文件路径(绝对路径,程序在设备上运行时从 /home/root/ 启动)
// 使用绝对路径确保在任何工作目录下都能正确读写
#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH    "/home/root/ls2k0300_config.txt"
#endif

// 最大支持的配置项数量
#ifndef CONFIG_MAX_ITEMS
#define CONFIG_MAX_ITEMS    50
#endif
// ==================== 移植配置结束 ====================

// ==================== 引入各模块头文件 ====================
// config_flash 通过包含各模块头文件来使用模块中定义的变量
// 各模块的变量在各自的 .cpp 中定义，在各自的 .h 中用 extern 声明

#include "motor.h"      // 电机模块
#include "control.h"    // 控制模块
#include "image.h"      // 图像处理模块

// 未来可以添加更多模块：
// ... 等

// ==================== API 函数 ====================

/**
 * @brief  初始化配置系统
 * @param  无
 * @return 无
 * @note   在 main.cpp 中程序启动时调用一次
 *         会自动注册所有变量并加载配置文件
 *         如果配置文件不存在，会创建默认配置
 */
void config_init(void);

/**
 * @brief  保存所有配置到文件
 * @param  无
 * @return 无
 * @note   保存所有已注册的变量到配置文件
 *         在任何模块中修改配置变量后，调用此函数保存
 */
void config_save(void);

/**
 * @brief  恢复所有配置为默认值
 * @param  无
 * @return 无
 * @note   将所有变量恢复为默认值，并保存到文件
 */
void config_reset(void);

// ==================== 内部函数（不需要外部调用）====================
void config_register_all(void);  // 内部函数：注册所有变量
void config_add_float(const char *key, float *var, float default_val, const char *comment);
void config_add_int(const char *key, int *var, int default_val, const char *comment);
void config_add_int16(const char *key, int16 *var, int16 default_val, const char *comment);
void config_add_uint8(const char *key, uint8 *var, uint8 default_val, const char *comment);
void config_add_uint16(const char *key, uint16 *var, uint16 default_val, const char *comment);
void config_add_uint32(const char *key, uint32 *var, uint32 default_val, const char *comment);
bool config_load(void);  // 内部函数：加载配置文件

#endif
