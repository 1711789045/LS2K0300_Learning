#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "zf_common_headfile.h"


#define IMAGE_W            UVC_WIDTH
#define IMAGE_H            UVC_HEIGHT

#define REFERENCE_ROW       5          //锟轿匡拷锟斤拷统锟斤拷锟斤拷锟斤拷
#define REFERENCE_COL       60         //锟轿匡拷锟斤拷统锟斤拷锟斤拷锟斤拷
#define WHITEMAXMUL        13         //锟阶碉拷锟斤拷锟街碉拷锟斤拷诓慰锟斤拷锟侥憋拷锟斤拷 10为锟斤拷锟脚达拷
#define WHITEMINMUL         7          //锟阶碉拷锟斤拷小值锟斤拷锟节参匡拷锟斤拷谋锟斤拷锟? 10为锟斤拷锟脚达拷
#define BLACKPOINT          50         //锟节碉拷值

#define CONTRASTOFFSET      3          //锟斤拷锟竭对比讹拷偏锟斤拷
#define STOPROW            0           //锟斤拷锟斤拷停止锟斤拷
#define SEARCHRANGE        10          //锟斤拷锟竭半径
#define CIRCLE_1_TIME      15        //锟斤拷锟斤拷状态一锟斤拷时时锟戒，锟斤拷位10ms
#define CIRCLE_2_TIME      50         //锟斤拷锟斤拷状态锟斤拷锟斤拷时时锟戒，锟斤拷位10ms
#define CIRCLE_4_TIME      25           //锟斤拷锟斤拷状态锟侥硷拷时时锟戒，锟斤拷位10ms
#define CIRCLE_5_TIME      25           //锟斤拷锟斤拷状态锟斤拷锟绞笔憋拷洌?锟斤拷位10ms
#define STOP_ANALYSE_LINE  (IMAGE_H-40)          //锟斤拷锟斤拷锟竭硷拷锟斤拷锟?
#define STOP_THRESHOLD    30			 //锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街?
#define STRETCH_NUM       80          //锟斤拷锟斤拷锟斤拷锟届长锟斤拷

extern uint8 reference_point;
extern uint8 white_max_point;          //锟斤拷态锟阶碉拷锟斤拷锟街?
extern uint8 white_min_point;          //锟斤拷态锟阶碉拷锟斤拷小值
extern uint8 remote_distance[IMAGE_W]; //锟筋长锟斤拷锟斤拷
extern uint8 reference_col;
extern uint8 reference_contrast_ratio; //锟轿匡拷锟皆比讹拷
extern uint16 reference_line[IMAGE_H];      // 锟芥储锟轿匡拷锟斤拷
extern uint16 left_edge_line[IMAGE_H];      // 锟芥储锟斤拷锟斤拷锟?
extern uint16 right_edge_line[IMAGE_H];      // 锟芥储锟揭憋拷锟斤拷
extern uint8 final_mid_line;
extern uint8 prospect;                     //前瞻值
extern uint8 single_edge_err[IMAGE_H];
extern uint8 circle_flag;                  //锟斤拷锟斤拷锟斤拷志位
extern uint16 circle_time;
extern uint8 cross_flag;                   //鍗佸瓧鏍囧織浣?
extern uint8 mid_mode;                    //循锟斤拷模式
extern uint8 if_circle;                     //1为锟斤拷锟斤拷圆锟斤拷锟斤拷0为锟截憋拷圆锟斤拷
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

// 鍥惧儚澶勭悊鏃堕棿娴嬮噺鍑芥暟
void image_process_time_start(void);     // 寮€濮嬭?℃椂
float image_process_time_end(void);      // 缁撴潫璁℃椂锛岃繑鍥炲?勭悊鏃堕棿锛堟??绉掞級
void image_process_time_print(void);     // 鎵撳嵃缁熻?′俊鎭?

#endif
