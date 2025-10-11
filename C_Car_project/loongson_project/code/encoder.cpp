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
* 文件名称          encoder
* 公司名称          成都逐飞科技有限公司
* 适用平台          LS2K0300
* 店铺链接          https://seekfree.taobao.com/
********************************************************************************************************************/

#include "zf_common_headfile.h"
#include "encoder.h"

// 编码器数据变量
int16 encoder_data_l = 0;
int16 encoder_data_r = 0;
uint8 encoder_f = 0;

/**
 * @brief  编码器初始化
 * @param  无
 * @return 无
 * @note   LS2K0300平台编码器使用设备路径方式访问，无需额外初始化
 */
void encoder_init(void)
{
    // LS2K0300 编码器通过设备文件访问，驱动已在内核中初始化
    // 这里可以进行一些初始化提示或参数设置
    printf("Encoder initialized.\r\n");
    printf("ENCODER_L: %s\r\n", ENCODER_L);
    printf("ENCODER_R: %s\r\n", ENCODER_R);
}

/**
 * @brief  读取编码器数据
 * @param  无
 * @return 无
 * @note   从编码器设备读取计数值并保存到全局变量
 */
void encoder_read(void)
{
    // 读取左右编码器计数值
    encoder_data_l = encoder_get_count(ENCODER_L);
    encoder_data_r = encoder_get_count(ENCODER_R);
}

/**
 * @brief  编码器处理函数（在定时器中调用）
 * @param  无
 * @return 无
 */
void encoder_process(void)
{
    if(encoder_f)
    {
        encoder_read();
        encoder_f = 0;
    }
}
