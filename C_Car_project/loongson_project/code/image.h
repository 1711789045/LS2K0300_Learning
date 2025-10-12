#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "zf_common_headfile.h"


#define IMAGE_W            UVC_WIDTH
#define IMAGE_H            UVC_HEIGHT

#define REFERENCE_ROW       5          //�ο���ͳ������
#define REFERENCE_COL       60         //�ο���ͳ������
#define WHITEMAXMUL        13         //�׵����ֵ���ڲο���ı��� 10Ϊ���Ŵ�
#define WHITEMINMUL         7          //�׵���Сֵ���ڲο���ı���? 10Ϊ���Ŵ�
#define BLACKPOINT          50         //�ڵ�ֵ

#define CONTRASTOFFSET      3          //���߶Աȶ�ƫ��
#define STOPROW            0           //����ֹͣ��
#define SEARCHRANGE        10          //���߰뾶
#define CIRCLE_1_TIME      15        //����״̬һ��ʱʱ�䣬��λ10ms
#define CIRCLE_2_TIME      50         //����״̬����ʱʱ�䣬��λ10ms
#define CIRCLE_4_TIME      25           //����״̬�ļ�ʱʱ�䣬��λ10ms
#define CIRCLE_5_TIME      25           //����״̬���ʱʱ��?��λ10ms
#define STOP_ANALYSE_LINE  (IMAGE_H-40)          //�����߼����?
#define STOP_THRESHOLD    30			 //������������������?
#define STRETCH_NUM       80          //�������쳤��

extern uint8 reference_point;
extern uint8 white_max_point;          //��̬�׵�����?
extern uint8 white_min_point;          //��̬�׵���Сֵ
extern uint8 remote_distance[IMAGE_W]; //�����
extern uint8 reference_col;
extern uint8 reference_contrast_ratio; //�ο��Աȶ�
extern uint16 reference_line[IMAGE_H];      // �洢�ο���
extern uint16 left_edge_line[IMAGE_H];      // �洢�����?
extern uint16 right_edge_line[IMAGE_H];      // �洢�ұ���
extern uint8 final_mid_line;
extern uint8 prospect;                     //ǰհֵ
extern uint8 single_edge_err[IMAGE_H];
extern uint8 circle_flag;                  //������־λ
extern uint16 circle_time;
extern uint8 cross_flag;                   //十字标志�?
extern uint8 mid_mode;                    //ѭ��ģʽ
extern uint8 if_circle;                     //1Ϊ����Բ����0Ϊ�ر�Բ��
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
void image_process_time_start(void);     // 开始�?�时
float image_process_time_end(void);      // 结束计时，返回�?�理时间（�??秒）
void image_process_time_print(void);     // 打印统�?�信�?

#endif
