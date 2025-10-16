#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "zf_common_headfile.h"


#define IMAGE_W            UVC_WIDTH
#define IMAGE_H            UVC_HEIGHT

// ==================== 图像处理配置参数声明 ====================
// 这些参数在 image.cpp 中定义，可被 config_flash 等模块引用
extern uint8 reference_row;        // 参考点统计行数
extern uint8 cfg_reference_col;    // 参考列统计列数（配置参数）
extern uint8 whitemaxmul;          // 白点最大值相对参考点的倍数 10为倍数单位
extern uint8 whiteminmul;          // 白点最小值相对参考点的倍数 10为倍数单位
extern uint8 blackpoint;           // 黑点值
extern uint8 contrastoffset;       // 对比度偏移
extern uint8 stoprow;              // 搜索停止行
extern uint8 searchrange;          // 搜索半径
extern uint16 circle_1_time;       // 环岛状态一延时时间，单位10ms
extern uint16 circle_2_time;       // 环岛状态二延时时间，单位10ms
extern uint16 circle_4_time;       // 环岛状态四延时时间，单位10ms
extern uint16 circle_5_time;       // 环岛状态五延时时间，单位10ms
extern uint8 stop_analyse_line;    // 停止线分析行（从底部数）
extern uint8 stop_threshold;       // 停止线检测阈值
extern uint8 stretch_num;          // 边线延长数

// 向后兼容的宏定义（引用全局变量）
#define REFERENCE_ROW       reference_row
#define REFERENCE_COL       cfg_reference_col
#define WHITEMAXMUL         whitemaxmul
#define WHITEMINMUL         whiteminmul
#define BLACKPOINT          blackpoint
#define CONTRASTOFFSET      contrastoffset
#define STOPROW             stoprow
#define SEARCHRANGE         searchrange
#define CIRCLE_1_TIME       circle_1_time
#define CIRCLE_2_TIME       circle_2_time
#define CIRCLE_4_TIME       circle_4_time
#define CIRCLE_5_TIME       circle_5_time
#define STOP_ANALYSE_LINE   (IMAGE_H-stop_analyse_line)
#define STOP_THRESHOLD      stop_threshold
#define STRETCH_NUM         stretch_num

extern uint8 reference_point;
extern uint8 white_max_point;          //动态白点最大值
extern uint8 white_min_point;          //动态白点最小值
extern uint8 remote_distance[IMAGE_W]; //远距离数组
extern uint8 reference_col;            // 运行时变量（每帧动态更新）
extern uint8 reference_contrast_ratio; //参考点对比度
extern uint16 reference_line[IMAGE_H];      // 存储参考线
extern uint16 left_edge_line[IMAGE_H];      // 存储左边线
extern uint16 right_edge_line[IMAGE_H];      // 存储右边线
extern uint8 final_mid_line;
extern uint8 prospect;                     //前瞻值
extern uint8 single_edge_err[IMAGE_H];
extern uint8 circle_flag;                  //环岛标志位
extern uint16 circle_time;
extern uint8 cross_flag;                   //十字标志位
extern uint8 mid_mode;                    //循迹模式
extern uint8 if_circle;                     //1为进入圆环，0为不进入圆环



void get_image(void);
void get_reference_point(const uint8 image[][IMAGE_W]);
void search_reference_col(const uint8 image[][IMAGE_W]);
void search_line(const uint8 image[][IMAGE_W]);
void image_display_edge_line(const uint8 image[][IMAGE_W],uint16 width,uint16 height);
void image_process(uint16 display_width,uint16 display_height,uint8 mode);

// 图像处理时间测量函数
void image_process_time_start(void);     // 开始计时
float image_process_time_end(void);      // 结束计时，返回处理时间（毫秒）
void image_process_time_print(void);     // 打印统计信息

#endif
