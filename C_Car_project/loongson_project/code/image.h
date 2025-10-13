#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "zf_common_headfile.h"


#define IMAGE_W            UVC_WIDTH
#define IMAGE_H            UVC_HEIGHT

#define REFERENCE_ROW       5          //参考点统计行数
#define REFERENCE_COL       60         //参考列统计列数
#define WHITEMAXMUL        13         //白点最大值相对参考点的倍数 10为倍数单位
#define WHITEMINMUL         7          //白点最小值相对参考点的倍数 10为倍数单位
#define BLACKPOINT          50         //黑点值

#define CONTRASTOFFSET      3          //对比度偏移
#define STOPROW            0           //搜索停止行
#define SEARCHRANGE        10          //搜索半径
#define CIRCLE_1_TIME      15        //环岛状态一延时时间，单位10ms
#define CIRCLE_2_TIME      50         //环岛状态二延时时间，单位10ms
#define CIRCLE_4_TIME      25           //环岛状态四延时时间，单位10ms
#define CIRCLE_5_TIME      25           //环岛状态五延时时间，单位10ms
#define STOP_ANALYSE_LINE  (IMAGE_H-40)          //停止线分析行
#define STOP_THRESHOLD    30			 //停止线检测阈值
#define STRETCH_NUM       80          //边线延长数

extern uint8 reference_point;
extern uint8 white_max_point;          //动态白点最大值
extern uint8 white_min_point;          //动态白点最小值
extern uint8 remote_distance[IMAGE_W]; //远距离数组
extern uint8 reference_col;
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
extern uint8 mid_weight_1[IMAGE_H];
extern uint8 mid_weight_2[IMAGE_H];
extern uint8 mid_weight_3[IMAGE_H];
extern uint8 mid_weight_4[IMAGE_H];
extern uint8 mid_weight_5[IMAGE_H];


extern uint8 mid_weight[IMAGE_H];



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
