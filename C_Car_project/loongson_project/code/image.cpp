#include "zf_common_headfile.h"

#include <opencv2/imgproc/imgproc.hpp>  // for cv::cvtColor
#include <opencv2/highgui/highgui.hpp> // for cv::VideoCapture
#include <opencv2/opencv.hpp>

#include <iostream> // for std::cerr
#include <fstream>  // for std::ofstream
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <atomic>

#include "image.h"
#include "beep.h"
#include "auto_menu.h"
#include "control.h"
//#include "motor.h"

uint8 reference_point = 0;
uint8 white_max_point = 0;             //��̬�׵�����?
uint8 white_min_point = 0;             //��̬�׵���Сֵ
uint8 remote_distance[IMAGE_W] = {0};   //�����
uint8 reference_col = 0;
uint8 reference_contrast_ratio = 0.1*200; //�ο��Աȶ�
uint16 reference_line[IMAGE_H] = {0};      // �洢�ο���
uint16 left_edge_line[IMAGE_H] = {0};      // �洢�����?
uint16 right_edge_line[IMAGE_H] = {0};      // �洢�ұ���
uint8 user_image[IMAGE_H][IMAGE_W];  //�洢ͼ��

uint8 mid_line[IMAGE_H] = {0};         //��������λ��
uint8 mid_weight_1[IMAGE_H] = {           //��������Ȩ��
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	7 ,8 ,9 ,10,11,12,13,14,15,16,
	17,18,19,20,20,20,20,19,18,17,
	16,15,14,13,12,11,10,9 ,8 ,7 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1  
};

uint8 mid_weight_2[IMAGE_H] = {           //��������Ȩ��
	
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	7 ,8 ,9 ,10,11,12,13,14,15,16,
	17,18,19,20,20,20,20,19,18,17,
	16,15,14,13,12,11,10,9 ,8 ,7 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 
};

uint8 mid_weight_3[IMAGE_H] = {           //��������Ȩ��
	
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	7 ,8 ,9 ,10,11,12,13,14,15,16,
	17,18,19,20,20,20,20,19,18,17,
	16,15,14,13,12,11,10,9 ,8 ,7 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 

	  
};

uint8 mid_weight_4[IMAGE_H] = {           //��������Ȩ��
	
	
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	7 ,8 ,9 ,10,11,12,13,14,15,16,
	17,18,19,20,20,20,20,19,18,17,
	16,15,14,13,12,11,10,9 ,8 ,7 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1  

	  
};

uint8 mid_weight_5[IMAGE_H] = {           //��������Ȩ��
	
	
	
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	7 ,8 ,9 ,10,11,12,13,14,15,16,
	17,18,19,20,20,20,20,19,18,17,
	16,15,14,13,12,11,10,9 ,8 ,7 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 

	  
};

uint8 mid_weight[IMAGE_H] = {           //��������Ȩ��
	
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	7 ,8 ,9 ,10,11,12,13,14,15,16,
	17,18,19,20,20,20,20,19,18,17,
	16,15,14,13,12,11,10,9 ,8 ,7 ,
	6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,
	1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 
};

uint8 single_edge_err[IMAGE_H] = {           //��������Ȩ��
	11,11,12,13,13,14,15,15,16,17,
	17,18,19,19,20,20,21,22,23,23, 
	24,25,25,27,27,28,28,29,30,30,
	31,32,32,33,34,34,35,35,36,37,
	37,38,39,40,40,42,42,42,43,44,
	45,45,46,47,47,48,49,49,50,50,
	51,52,52,53,54,54,55,56,57,57,
	58,59,60,60,61,62,62,63,64,64,
	65,65,66,67,67,68,69,69,70,72,
	71,72,73,74,74,75,76,76,77,78,
	78,79,79,80,80,81,82,82,83,84,
	85,85,86,87,87,88,89,89,90,91 
};
uint8 final_mid_line = 0;                 //��Ȩ����ֵ
uint8 last_final_mid_line = 0;            //�ϴμ�Ȩ����ֵ
uint8 prospect = 100;                     //ǰհֵ
uint8 cross_flag = 0;                    //ʮ�ֱ�־λ
uint8 mid_mode = 0;                      //ѭ��ģʽ��0��ʾѭ�����ߣ�1ѭ����ߣ�?2ѭ�ұ���
uint8 circle_flag = 0;
uint16 circle_time = 0;
uint8 if_circle = 0;                     //1Ϊ����Բ����0Ϊ�ر�Բ��
uint8 stop_search_row = 0;

void get_image(void){
	// ����ʽ�ȴ���ͼ��ˢ��
	if(wait_image_refresh() < 0)
	{
		// ����ͷδ�ɼ���ͼ��������Ҫ�رյ�����رյ���ȡ�
		exit(0); 
	}
	memcpy(user_image,rgay_image,IMAGE_H*IMAGE_W);
}




void get_reference_point(const uint8 image[][IMAGE_W]){
	uint16 temp = 0;                 //���ڱ���ͳ�Ƶ�������
	uint32 temp1 = 0;               //���ڱ���ͳ�Ƶ�����ֵ
	uint16 temp_j1 = (IMAGE_W-REFERENCE_COL)/2;    
	uint16 temp_j2 = (IMAGE_W+REFERENCE_COL)/2;	
	
	temp = REFERENCE_ROW * REFERENCE_COL;
	for(int i = IMAGE_H-REFERENCE_ROW;i <= IMAGE_H-1;i++){
		for(int j = temp_j1;j <= temp_j2;j++){
			temp1 += image[i][j];				//ͳ�Ƶ����?
		}
	}
	reference_point = (uint8)func_limit_ab((temp1/temp),BLACKPOINT,255);
	white_max_point = (uint8)func_limit_ab((int32)reference_point * WHITEMAXMUL/10,BLACKPOINT,255);
	white_min_point = (uint8)func_limit_ab((int32)reference_point * WHITEMINMUL/10,BLACKPOINT,255);
	
//	ips200_show_int(96,240,reference_point,4);
//	ips200_show_int(96,256,white_max_point,4);
//	ips200_show_int(96,272,white_min_point,4);
}

void search_reference_col(const uint8 image[][IMAGE_W]){
	int col,row;
	int16 temp1 = 0,temp2 = 0,temp3 = 0;           //��ǰ�� �Աȵ� �Աȶ�
	for(col = 0;col < IMAGE_W;col++){
		remote_distance[col] = IMAGE_H-1;
	}
	for(col = 0;col < IMAGE_W;col+=CONTRASTOFFSET){                  //���У���������һ��
		for	(row = IMAGE_H-1;row >= STOPROW+CONTRASTOFFSET;row-=CONTRASTOFFSET){    //���У��������ж�һ��
			temp1 = image[row][col];
			temp2 = image[row-CONTRASTOFFSET][col];
			
			if(row == 5){   //����Աȶ�?
				remote_distance[col] = (uint8)row;
				break;
			}
			
			if(temp2 > white_max_point){           //�Աȵ��ǰ׵�
				continue;
			}
			if(temp1 < white_min_point){           //��ǰ���Ǻڵ�
				remote_distance[col] = (uint8)row;
				break;
 			}
			
			temp3 = (temp1 - temp2)*200/(temp1 + temp2);
			
			if(temp3 >reference_contrast_ratio ){   //����Աȶ�?
				remote_distance[col] = (uint8)row;
				break;
			}
		}
	}
	
	for(col = 0;col < IMAGE_W;col+=CONTRASTOFFSET){    //Ѱ�������
		if(remote_distance[reference_col] > remote_distance[col])
			reference_col = col;
	}
	
	
	for(row = 0;row < IMAGE_H;row++){
		reference_line[row] =reference_col;
	}
}


void search_line(const uint8 image[][IMAGE_W]){
    uint8 row_max = IMAGE_H - 1;                        // ������?
    uint8 row_min = STOPROW;                                    // ����Сֵ
    uint8 col_max = IMAGE_W - CONTRASTOFFSET-1;           // ������?
    uint8 col_min = CONTRASTOFFSET;                    // ����Сֵ
    int16 leftstartcol  = reference_col;                // ��������ʼ��
    int16 rightstartcol = reference_col;                // ��������ʼ��
    int16 leftendcol    = col_min;                           // ��������ֹ��
    int16 rightendcol   = col_max;                   // ��������ֹ��
    uint8 search_time   = 0;                                    // �������ߴ���
    uint8 temp1 = 0, temp2 = 0;                                 // ��ʱ����  ���ڴ洢ͼ���� 
    int  temp3 = 0;                                             // ��ʱ����  ���ڴ洢�Աȶ�
    int  leftstop = 0, rightstop = 0, stoppoint = 0;            // ������������

    int col, row;

    for(row = row_max; row >= row_min; row --){                //���߹��㵽����
		left_edge_line[row]  = col_min - CONTRASTOFFSET;
        right_edge_line[row] = col_max + CONTRASTOFFSET;
    }
	stop_search_row = 0;

	for(row = row_max;row >= row_min;row--){                   
		if(!leftstop){
			search_time = 2-cross_flag;
			do{
				if(search_time == 1){                         //��һ��û�ѵ�
					leftstartcol = reference_col;
					leftendcol = col_min;
				}
				
				search_time--;
				
				for(col = leftstartcol; col >= leftendcol ;col--){
					if(col - CONTRASTOFFSET <0)
						continue;
					temp1 = image[row][col];
					temp2 = image[row][col - CONTRASTOFFSET];
				
					if(temp1 < white_min_point && col == leftstartcol && leftstartcol == reference_col){
						//�жϲο����Ƿ�Ϊ�ڵ㣬���������в������������?
						leftstop = 1;
						stop_search_row = row;
						
						for(stoppoint = row;stoppoint >= 0;stoppoint--){
							left_edge_line[stoppoint ] = col_min;		//�����������ϵı��߹���
						}
						search_time = 0;
						break;
					}
					
					if(temp1 < white_min_point){           //��ǰ��Ϊ�ڵ�
						left_edge_line[row] = col;
						break;
					}
					if(temp2 > white_max_point){           //�ο���Ϊ�׵�
						continue;
					}
					temp3 = (temp1 - temp2)*200/(temp1 + temp2); //����Աȶ�?
					
					if(temp3 >reference_contrast_ratio || col == col_min){   
						left_edge_line[row] = func_limit_ab(col - CONTRASTOFFSET, 0, IMAGE_W-1);		//������߽�?
						//��һ���������߰뾶������
						leftstartcol = (uint8)func_limit_ab(col+SEARCHRANGE,col,col_max);  
						leftendcol = (uint8)func_limit_ab(col-SEARCHRANGE,col_min,col);
						search_time = 0;
						break;
					}
				}
			}while(search_time);
		}
		
		
		if(!rightstop){
			search_time = 2-cross_flag;
			do{
				if(search_time == 1){                         //��һ��û�ѵ�
					rightstartcol = reference_col;
					rightendcol = col_max;
				}
				
				search_time--;
				
				for(col = rightstartcol; col <= rightendcol ;col++){
					if(col + CONTRASTOFFSET >= IMAGE_W)
						continue;
					temp1 = image[row][col];
					temp2 = image[row][col + CONTRASTOFFSET];
				
					if(temp1 < white_min_point && col == rightstartcol && rightstartcol == reference_col){
						//�жϲο����Ƿ�Ϊ�ڵ㣬���������в��������ұ���
						rightstop = 1;
						stop_search_row = row;

						for(stoppoint = row;stoppoint >= 0;stoppoint--){
							right_edge_line[stoppoint ] = col_max;		//�����������ϵı��߹���
						}
						search_time = 0;
						break;
					}
					
					if(temp1 < white_min_point){           //��ǰ��Ϊ�ڵ�
						right_edge_line[row] = col;
						break;
					}
					if(temp2 > white_max_point){           //�ο���Ϊ�׵�
						continue;
					}
					temp3 = (temp1 - temp2)*200/(temp1 + temp2); //����Աȶ�?
					
					if(temp3 >reference_contrast_ratio || col == col_max){   
						right_edge_line[row] = func_limit_ab(col + CONTRASTOFFSET, 0, IMAGE_W-1);		//�����ұ߽�
						//��һ���������߰뾶������
						rightstartcol = (uint8)func_limit_ab(col-SEARCHRANGE,col_min,col);  
						rightendcol = (uint8)func_limit_ab(col+SEARCHRANGE,col,col_max);
						search_time = 0;
						break;
					}
				}
			}while(search_time);
		}
	}
}

uint8 image_find_jump_point(uint16 *edge_line,uint8 down_num,uint8 up_num,uint8 jump_num,uint8 model){
	//down_num ���¶���ʼ�� up_num ���϶���ֹ��,modelΪ1ʱ���µ��� Ϊ0ʱ���ϵ���
	uint8 temp_jump_point = 0;
	uint8 temp_data;
	
	if(model){
		temp_jump_point = down_num;
		for(int i = 0;i <down_num-up_num;i++){
			temp_data = func_abs(edge_line[down_num-i]-edge_line[down_num-i-1]);
			if(temp_data>jump_num){
				temp_jump_point = (uint8)(down_num-i);
				return temp_jump_point;
			}
		}
	}
	else{
		temp_jump_point = up_num;
		for(int i = 0;i <down_num-up_num;i++){
			temp_data = func_abs(edge_line[up_num+i]-edge_line[up_num+i+1]);
			if(temp_data>jump_num){
				temp_jump_point = (uint8)(up_num+i);
				return temp_jump_point;
			}
		}
	}
	return 0;
}

uint8 image_find_left_jump_point(uint8 down_num,uint8 up_num,uint8 model){
	//down_num ���¶���ʼ�� up_num ���϶���ֹ��,modelΪ1ʱ���µ��� Ϊ0ʱ���ϵ���
	uint8 temp_jump_point = 0;
	uint8 temp_data;
	
	if(model){
		temp_jump_point = down_num;
		for(int i = 0;i <down_num-up_num;i++){
			if(
				left_edge_line[down_num-i]-left_edge_line[down_num-i-5]>=8&&
				left_edge_line[down_num-i]-left_edge_line[down_num-i-6]>=8&&
				left_edge_line[down_num-i]-left_edge_line[down_num-i-7]>=8
			&&
				func_abs(left_edge_line[down_num-i]-left_edge_line[down_num-i+1])<=7&&
				func_abs(left_edge_line[down_num-i]-left_edge_line[down_num-i+2])<=10&&
				func_abs(left_edge_line[down_num-i]-left_edge_line[down_num-i+3])<=15
			){
				temp_jump_point = (uint8)(down_num-i)+3;
				return temp_jump_point;
			}
		}
	}
	else{
		temp_jump_point = up_num;
		for(int i = 0;i <down_num-up_num;i++){
			if(
				left_edge_line[up_num+i]-left_edge_line[up_num+i+5]>=8&&
				left_edge_line[up_num+i]-left_edge_line[up_num+i+6]>=8&&
				left_edge_line[up_num+i]-left_edge_line[up_num+i+7]>=8
			&&
				func_abs(left_edge_line[up_num+i]-left_edge_line[up_num+i+1])<=7&&
				func_abs(left_edge_line[up_num+i]-left_edge_line[up_num+i+2])<=10&&
				func_abs(left_edge_line[up_num+i]-left_edge_line[up_num+i+3])<=15
			){
				temp_jump_point = (uint8)(up_num+i)-3;
				return temp_jump_point;
			}
		}
	}
	return 0;
}

uint8 image_find_right_jump_point(uint8 down_num,uint8 up_num,uint8 model){
	//down_num ���¶���ʼ�� up_num ���϶���ֹ��,modelΪ1ʱ���µ��� Ϊ0ʱ���ϵ���
	uint8 temp_jump_point = 0;
	uint8 temp_data;
	
	if(model){
		temp_jump_point = down_num;
		for(int i = 0;i <down_num-up_num;i++){
			if(
				right_edge_line[down_num-i]-right_edge_line[down_num-i-5]<=-8&&
				right_edge_line[down_num-i]-right_edge_line[down_num-i-6]<=-8&&
				right_edge_line[down_num-i]-right_edge_line[down_num-i-7]<=-8
			&&
				func_abs(right_edge_line[down_num-i]-right_edge_line[down_num-i+1])<=7&&
				func_abs(right_edge_line[down_num-i]-right_edge_line[down_num-i+2])<=10&&
				func_abs(right_edge_line[down_num-i]-right_edge_line[down_num-i+3])<=15
			){
				temp_jump_point = (uint8)(down_num-i)+3;
				return temp_jump_point;
			}
		}
	}
	else{
		temp_jump_point = up_num;
		for(int i = 0;i <down_num-up_num;i++){
			if(
				right_edge_line[up_num+i]-right_edge_line[up_num+i+5]<=-8&&
				right_edge_line[up_num+i]-right_edge_line[up_num+i+6]<=-8&&
				right_edge_line[up_num+i]-right_edge_line[up_num+i+7]<=-8
			&&
				func_abs(right_edge_line[up_num+i]-right_edge_line[up_num+i+1])<=7&&
				func_abs(right_edge_line[up_num+i]-right_edge_line[up_num+i+2])<=10&&
				func_abs(right_edge_line[up_num+i]-right_edge_line[up_num+i+3])<=15
			){
				temp_jump_point = (uint8)(up_num+i)-3;
				return temp_jump_point;
			}
		}
	}
	return 0;
}

void image_connect_point(uint16 *array_value, uint8 num0, uint8 num1)
{
    float point_1 = (float)array_value[num0];
    float point_2 = (float)array_value[num1];
    float temp_slope = (point_2 - point_1) / (num0 - num1);

    for (int i = 0; i < (num0 - num1); i++)
    {
        array_value[num0 - i] = (int8)(temp_slope * i) + array_value[num0];
    }
}

void image_stretch_point(uint16 *array_value, uint8 num ,uint8 direction){
	
	//directionΪ1���������죬0����������
	if((num+5>=IMAGE_H) ||(num-5<=0))
		return;
	
	float temp_slope = 0;
    float point_1 = (float)array_value[num];
	
	if(direction){
		float point_2 = (float)array_value[num+5];
		temp_slope = (point_1 - point_2) / 5;
		for (int i = 0; i < STRETCH_NUM&& num-i>=5; i++)
		{
			array_value[num - i] = func_limit_ab((int8)(temp_slope * i) + array_value[num],0,IMAGE_W-1);
		}
	}
	else{
		float point_2 = (float)array_value[num-5];
		temp_slope = (point_1 - point_2) / 5;
		for (int i = 0; i < STRETCH_NUM&& num+i<=IMAGE_H-1; i++)
		{
			array_value[num + i] = func_limit_ab((int8)(temp_slope * i) + array_value[num],0,IMAGE_W-1);
		}
	}
	

    
}

void image_cross_analysis(void){
	uint32 track_width = 0;
	uint8 start_point = 0,end_point = 0;
	for(int i = (IMAGE_H * 2 / 3);i >  (IMAGE_H / 3);i--){
		track_width += (right_edge_line[i] - left_edge_line[i]);
	}
	
	if(!cross_flag && track_width > (IMAGE_W * (IMAGE_H * 4 / 15))){
		cross_flag = 1;
	}
	
	if(cross_flag == 1){       //��Ƭ��ɫ
		start_point = 0;
		end_point = 0;
		start_point = image_find_left_jump_point(IMAGE_H - 5,IMAGE_H/4,0);
		end_point = image_find_left_jump_point(IMAGE_H - 5,IMAGE_H/3,1);
		if(end_point && start_point){
			image_connect_point(left_edge_line,end_point,start_point);
		}
		if(end_point && !start_point){
			image_stretch_point(left_edge_line,end_point,1);
		}
		if(!end_point && start_point){
			image_stretch_point(left_edge_line,start_point,0);
		}
		
		
		ips200_show_int(96,224,start_point,4);
		ips200_show_int(128,224,end_point,4);
		
		start_point = 0;
		end_point = 0;
		start_point = image_find_right_jump_point(IMAGE_H - 5,IMAGE_H/4,0);
		end_point = image_find_right_jump_point(IMAGE_H - 5,IMAGE_H/3,1);
		
		if(end_point && start_point){
			image_connect_point(right_edge_line,end_point,start_point);
		}
		if(end_point && !start_point){
			image_stretch_point(right_edge_line,end_point,1);
		}
		if(!end_point && start_point){
			image_stretch_point(right_edge_line,start_point,0);
		}
		
		ips200_show_int(96,240,start_point,4);
		ips200_show_int(128,240,end_point,4);

		
		if(track_width < (IMAGE_W * (IMAGE_H * 1 / 5))){
			cross_flag = 0;
		}
		
		
		

	}
}

uint8 image_find_circle_point(uint16 *edge_line,uint8 down_num,uint8 up_num){
	uint8 temp_jump_point = 0;
	temp_jump_point = down_num;
	for(int i = 0;i <down_num-up_num;i++){
		if(edge_line[down_num-i]<edge_line[down_num-i-1]&&
			edge_line[down_num-i]<edge_line[down_num-i-2]&&
			edge_line[down_num-i]<edge_line[down_num-i-3]&&
//			edge_line[down_num-i]<edge_line[down_num-i-4]&&
//			edge_line[down_num-i]<edge_line[down_num-i-5]&&
			edge_line[down_num-i]<edge_line[down_num-i+1]&&
			edge_line[down_num-i]<edge_line[down_num-i+2]&&
			edge_line[down_num-i]<edge_line[down_num-i+3]
//		&&
//			edge_line[down_num-i]<edge_line[down_num-i+4]&&
//			edge_line[down_num-i]<edge_line[down_num-i+5]
		){
			temp_jump_point = (uint8)(down_num-i);
			
			return temp_jump_point;
		}
	}
	return 0;
}


void image_circle_analysis(void){
	
	if(circle_flag == 0){           //ʶ�𻷵�
		mid_mode = 0;
		for(int i = IMAGE_H-2;i>0;i--){
			if(func_abs(left_edge_line[i]-left_edge_line[i+1])>3)
				return;
		}
		for(int i = IMAGE_H-40;i>0;i--){
			if(left_edge_line[i]<3)
				return;
		}
		uint8 start_point = 0,end_point = 0;
		start_point = image_find_jump_point(right_edge_line,IMAGE_H - 5,5,10,1);
		if(start_point)
			end_point = image_find_circle_point(right_edge_line,start_point-10,10);
		if(end_point){
			if(start_point - end_point>30){
				circle_flag = 1;
				circle_time = 0;     //��ʼ��ʱ
				beep_flag = 1;
			}
		}

		start_point = 0,end_point = 0;
		start_point = image_find_jump_point(right_edge_line,IMAGE_H - 80,5,10,0);
		if(start_point)
			end_point = image_find_circle_point(right_edge_line,IMAGE_H - 5,start_point-5);
		if(end_point){
			if(end_point- start_point>30){
				circle_flag = 1;
				circle_time = 5;     //��ʼ��ʱ
				beep_flag = 1;
			}
		}
		ips200_show_int(96,240,start_point,4);
		ips200_show_int(128,240,end_point,4);
	}
	else if (circle_flag == 1){      //�����������ڳ��������������ֱ��?
		mid_mode = 1;
		if(circle_time >= CIRCLE_1_TIME){
			circle_time = 0;
			circle_flag = 2;
			beep_flag = 1;
		}
	}
	else if (circle_flag == 2){      //������ں����ұ�����?
		mid_mode = 2;
		if(circle_time >= CIRCLE_2_TIME){
			circle_time = 0;
			circle_flag = 3;
			beep_flag = 1;

		}
	}
	else if (circle_flag == 3){      //�뵺������ѭ��ת��
		mid_mode = 0;
//		if(final_mid_line < IMAGE_W/2+10){//����������תʱ˵���������?
//			mid_mode = 2;
//			circle_time = 0;
//			circle_flag = 4;
//		}
		if(left_edge_line[IMAGE_H/2]<3 && left_edge_line[IMAGE_H/2-1]<3 &&left_edge_line[IMAGE_H/2+1]<3){//����������תʱ˵���������?
//		uint8 start_point = 0,end_point = 0;
//		start_point = image_find_jump_point(left_edge_line,IMAGE_H-5,5,10,0);
//		if(start_point){
			mid_mode = 2;
			circle_time = 0;
			circle_flag = 4;
			beep_flag = 1;

		}
	}
	else if(circle_flag == 4){       //�ڳ��ڴ����ұ�����
		mid_mode = 0;
		for(int i = IMAGE_H-1;i>0;i--){
			left_edge_line[i] = IMAGE_W/3;
		}
		if(circle_time >= CIRCLE_4_TIME){
			circle_time = 0;
			circle_flag = 5;
			beep_flag = 1;

		}
	}
	else if(circle_flag == 5){       //�������ֱ���뿪����?
		mid_mode = 1;
		if(circle_time >= CIRCLE_5_TIME){
			circle_time = 0;
			circle_flag = 0;
		}
	}
	
	
	
}


void image_calculate_mid(uint8 mode){
	uint32 mid_weight_sum = 0;        //��Ȩ�����ۼ�ֵ
	uint32 weight_sum = 0;            //Ȩ���ۼ�ֵ
	uint8 temp = 0;                   //��ʱ�洢����
	if(mode == 0){
		for(int i = 0;i<IMAGE_H;i++){
			mid_line[i] = (left_edge_line[i] + right_edge_line[i])/2;
		}
	}
	if(mode == 1){
		for(int i = 0;i<IMAGE_H;i++){
			mid_line[i] = func_limit_ab(left_edge_line[i] + single_edge_err[i],0,IMAGE_W);
		}
	}
	if(mode == 2){
		for(int i = 0;i<IMAGE_H;i++){
			mid_line[i] = func_limit_ab(right_edge_line[i] - single_edge_err[i],0,IMAGE_W);
		}
	}
	
	
	for(int i = 0;i<IMAGE_H;i++){
		if(i >= stop_search_row){
			weight_sum += mid_weight[i];
			mid_weight_sum += mid_line[i]*mid_weight[i];
		}
	
	}
	temp = (uint8)(mid_weight_sum/weight_sum);
	if(!last_final_mid_line)
		final_mid_line = temp;
	else
		final_mid_line = temp*0.8+last_final_mid_line*0.2;  //�����˲�

	last_final_mid_line = final_mid_line;
	
}

void image_calculate_prospect(const uint8 image[][IMAGE_W]){
	int col = IMAGE_W/2;
	int16 temp1 = 0,temp2 = 0,temp3 = 0;
	for	(int row = IMAGE_H-1;row > CONTRASTOFFSET;row--){    //���У��������ж�һ��
		temp1 = image[row][col];
		temp2 = image[row-CONTRASTOFFSET][col];
		
		if(row == 4){   //����Աȶ�?
			prospect = IMAGE_H-(uint8)row;
			break;
		}
		
		if(temp1 < white_min_point){           //��ǰ���Ǻڵ�
			prospect = IMAGE_H-(uint8)row;
			break;
		}
		
		if(temp2 > white_max_point){           //�Աȵ��ǰ׵�
			continue;
		}
		
		
		temp3 = (temp1 - temp2)*200/(temp1 + temp2);
		
		if(temp3 >reference_contrast_ratio ){   //����Աȶ�?
			prospect = IMAGE_H-(uint8)row;
			break;
		}
	}
//	ips200_show_int(96,224,prospect,4);

}


void image_display_edge_line(const uint8 image[][IMAGE_W], uint16 display_width, uint16 display_height) {
    // ����ʾ�Ҷ�ͼ��
    ips200_show_gray_image(0, 0, rgay_image, UVC_WIDTH, UVC_HEIGHT);	
    
    // ���ӱ߽���
    for(uint16 i = 0; i < IMAGE_H; i++) {
        // �������������Ƿ�����Ч��Χ��
        if(left_edge_line[i] < display_width && i < display_height) {
            ips200_draw_point((uint16)left_edge_line[i], i, RGB565_RED);
        }
        
        // ����ұ��������Ƿ�����Ч��Χ��?
        if(right_edge_line[i] < display_width && i < display_height) {
            ips200_draw_point((uint16)right_edge_line[i], i, RGB565_BLUE);
        }
        
        // ���ο��������Ƿ�����Ч��Χ��
        if(reference_line[i] < display_width && i < display_height) {
            ips200_draw_point((uint16)reference_line[i], i, RGB565_YELLOW);
        }
        
        // ������������Ƿ�����Ч��Χ��?
        if(mid_line[i] < display_width && i < display_height) {
            ips200_draw_point((uint16)mid_line[i], i, RGB565_GREEN);
        }
    }
    
    // ��ʾ��������ֵ�����ӱ߽��飩
    if(final_mid_line < display_width) {
//        ips200_show_int(96, 208, final_mid_line, 4);
    }
}

void image_get_left_err(void){     //��ȡ����������ߵ�ƫ������?
	for(int i = 0;i <= IMAGE_H-1;i++){
		single_edge_err[i] = mid_line[i] - left_edge_line[i];
	}
}

void stop_analysis(const uint8 image[][IMAGE_W]){
	int16 temp1 = 0,temp2 = 0,temp3 = 0;
	uint16 stop_count = 0;
	for(int row = STOP_ANALYSE_LINE-1;row<=STOP_ANALYSE_LINE+1;row++){
		for(int col = 0;col<IMAGE_W-CONTRASTOFFSET;col+=CONTRASTOFFSET){
			temp1 = image[row][col];
			temp2 = image[row][col+CONTRASTOFFSET];
			temp3 = (temp1 - temp2)*200/(temp1 + temp2);
			if(temp3 >reference_contrast_ratio ){   //����Աȶ�?
				stop_count++;
			}
		}
	}
//	ips200_show_int(96,288,stop_count,4);

	if(stop_count> 35 /* && start_time > 30 */){
		// 统一停车接口：只需设置 stop_flag = 1
		stop_flag = 1;
		beep_flag = 1;
	}
}


void image_process(uint16 display_width,uint16 display_height,uint8 mode){
	get_image();
	reference_point = 0; white_max_point = 0;white_min_point = 0;reference_col = 0;
	
	/* get_reference_point(user_image);
	search_reference_col(user_image);
	search_line(user_image); */
	
//	image_get_left_err();
	
	/* if(if_circle){
		image_circle_analysis();
	}
	
	if(!circle_flag){
		image_cross_analysis();
	} */
	
	/* if(go_flag)
		stop_analysis(user_image);
	

	image_calculate_mid(mid_mode);
	image_calculate_prospect(user_image); */
	
	if(mode)
		image_display_edge_line(user_image,display_width,display_height);
}


// ==================== 图像处理时间测量功能 ====================

#include <time.h>

// 静态变量用于存储时间测量数�?
static struct timespec img_start_time, img_end_time;
static float last_process_time = 0.0f;       // 上一次处理时�?(ms)
static float total_process_time = 0.0f;      // 总处理时�?(ms)
static uint32 frame_count = 0;               // 帧计数器
static float max_process_time = 0.0f;        // 最大处理时�?(ms)
static float min_process_time = 999.0f;      // 最小处理时�?(ms)

/**
 * @brief 开始图像处理计�?
 *
 * 使用方法：在 image_process() 函数开始时调用
 */
void image_process_time_start(void)
{
    clock_gettime(CLOCK_MONOTONIC, &img_start_time);
}

/**
 * @brief 结束图像处理计时并返回处理时�?
 *
 * 使用方法：在 image_process() 函数结束时调�?
 *
 * @return float 本次图像处理所用时间（毫秒�?
 */
float image_process_time_end(void)
{
    clock_gettime(CLOCK_MONOTONIC, &img_end_time);

    // 计算时间差（纳秒级）
    long seconds = img_end_time.tv_sec - img_start_time.tv_sec;
    long nanoseconds = img_end_time.tv_nsec - img_start_time.tv_nsec;

    // 转换为毫�?
    float elapsed_ms = seconds * 1000.0f + nanoseconds / 1000000.0f;

    // 更新统计数据
    last_process_time = elapsed_ms;
    total_process_time += elapsed_ms;
    frame_count++;

    // 更新最�?/最小�?
    if(elapsed_ms > max_process_time)
        max_process_time = elapsed_ms;

    if(elapsed_ms < min_process_time)
        min_process_time = elapsed_ms;

    return elapsed_ms;
}

/**
 * @brief 打印图像处理时间统计信息
 *
 * 使用方法：每隔一段时间调用（如每100帧或每秒�?
 * 可以�? main() 中的循环里定期调�?
 */
void image_process_time_print(void)
{
    if(frame_count == 0)
    {
        printf("[ 图像处理时间 ] 尚未开始测量\n");
        return;
    }

    float avg_time = total_process_time / frame_count;
    float fps = 1000.0f / avg_time;  // 基于平均处理时间计算理论FPS

    printf("\n========== 图像处理时间统计 ==========\n");
    printf("[ 帧数统计 ] 已处理帧�?: %u 帧\n", frame_count);
    printf("[ 本次时间 ] 上次处理: %.2f ms\n", last_process_time);
    printf("[ 平均时间 ] %.2f ms (理论FPS: %.1f)\n", avg_time, fps);
    printf("[ 最大时�? ] %.2f ms\n", max_process_time);
    printf("[ 最小时�? ] %.2f ms\n", min_process_time);
    printf("[ 总计时间 ] %.2f ms\n", total_process_time);
    printf("=====================================\n\n");

    // 如果平均处理时间 > 10ms，给出警�?
    if(avg_time > 10.0f)
    {
        printf("⚠️  警告: 图像处理时间超过 10ms，可能无法满�? 100Hz 控制周期！\n");
        printf("   建议: 降低分辨率或优化算法\n\n");
    }
    else if(avg_time < 8.0f)
    {
        printf("�? 良好: 图像处理时间 < 8ms，可以满�? 100Hz 控制周期\n\n");
    }
}
