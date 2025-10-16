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

// ==================== 图像处理配置参数（在本模块中定义）====================
uint8 cfg_reference_row = 5;        // 参考点统计行数
uint8 cfg_reference_col = 60;       // 参考列统计列数
uint8 cfg_whitemaxmul = 13;         // 白点最大值相对参考点的倍数 10为倍数单位
uint8 cfg_whiteminmul = 7;          // 白点最小值相对参考点的倍数 10为倍数单位
uint8 cfg_blackpoint = 50;          // 黑点值
uint8 cfg_contrastoffset = 3;       // 对比度偏移
uint8 cfg_stoprow = 0;              // 搜索停止行
uint8 cfg_searchrange = 10;         // 搜索半径
uint16 cfg_circle_1_time = 15;      // 环岛状态一延时时间，单位10ms
uint16 cfg_circle_2_time = 50;      // 环岛状态二延时时间，单位10ms
uint16 cfg_circle_4_time = 25;      // 环岛状态四延时时间，单位10ms
uint16 cfg_circle_5_time = 25;      // 环岛状态五延时时间，单位10ms
uint8 cfg_stop_analyse_line = 40;   // 停止线分析行（从底部数）
uint8 cfg_stop_threshold = 30;      // 停止线检测阈值
uint8 cfg_stretch_num = 80;         // 边线延长数
uint8 cfg_mid_calc_center_row = 90; // 中线计算中心行（从底部数）

uint8 reference_point = 0;
uint8 white_max_point = 0;
uint8 white_min_point = 0;
uint8 remote_distance[IMAGE_W] = {0};
uint8 reference_col = 0;
uint8 reference_contrast_ratio = 0.1*200;
uint16 reference_line[IMAGE_H] = {0};
uint16 left_edge_line[IMAGE_H] = {0};
uint16 right_edge_line[IMAGE_H] = {0};
uint8 user_image[IMAGE_H][IMAGE_W];

uint8 mid_line[IMAGE_H] = {0};
uint8 single_edge_err[IMAGE_H] = {
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
uint8 final_mid_line = 0;
uint8 last_final_mid_line = 0;
uint8 prospect = 100;                     //前瞻值
uint8 cross_flag = 0;
uint8 mid_mode = 0;
uint8 circle_flag = 0;
uint16 circle_time = 0;
uint8 if_circle = 0;
uint8 stop_search_row = 0;

void get_image(void){
	if(wait_image_refresh() < 0)
	{
		exit(0); 
	}
	memcpy(user_image,rgay_image,IMAGE_H*IMAGE_W);
}




void get_reference_point(const uint8 image[][IMAGE_W]){
	uint16 temp = 0;
	uint32 temp1 = 0;
	uint16 temp_j1 = (IMAGE_W-REFERENCE_COL)/2;    
	uint16 temp_j2 = (IMAGE_W+REFERENCE_COL)/2;	
	
	temp = REFERENCE_ROW * REFERENCE_COL;
	for(int i = IMAGE_H-REFERENCE_ROW;i <= IMAGE_H-1;i++){
		for(int j = temp_j1;j <= temp_j2;j++){
			temp1 += image[i][j];
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
	int16 temp1 = 0,temp2 = 0,temp3 = 0;
	for(col = 0;col < IMAGE_W;col++){
		remote_distance[col] = IMAGE_H-1;
	}
	for(col = 0;col < IMAGE_W;col+=CONTRASTOFFSET){
		for	(row = IMAGE_H-1;row >= STOPROW+CONTRASTOFFSET;row-=CONTRASTOFFSET){
			temp1 = image[row][col];
			temp2 = image[row-CONTRASTOFFSET][col];
			
			if(row == 5){
				remote_distance[col] = (uint8)row;
				break;
			}
			
			if(temp2 > white_max_point){
				continue;
			}
			if(temp1 < white_min_point){
				remote_distance[col] = (uint8)row;
				break;
 			}
			
			temp3 = (temp1 - temp2)*200/(temp1 + temp2);
			
			if(temp3 >reference_contrast_ratio ){
				remote_distance[col] = (uint8)row;
				break;
			}
		}
	}
	
	for(col = 0;col < IMAGE_W;col+=CONTRASTOFFSET){
		if(remote_distance[reference_col] > remote_distance[col])
			reference_col = col;
	}
	
	
	for(row = 0;row < IMAGE_H;row++){
		reference_line[row] =reference_col;
	}
}


void search_line(const uint8 image[][IMAGE_W]){
    uint8 row_max = IMAGE_H - 1;
    uint8 row_min = STOPROW;
    uint8 col_max = IMAGE_W - CONTRASTOFFSET-1;
    uint8 col_min = CONTRASTOFFSET;
    int16 leftstartcol  = reference_col;
    int16 rightstartcol = reference_col;
    int16 leftendcol    = col_min;
    int16 rightendcol   = col_max;
    uint8 search_time   = 0;
    uint8 temp1 = 0, temp2 = 0;
    int  temp3 = 0;
    int  leftstop = 0, rightstop = 0, stoppoint = 0;

    int col, row;

    for(row = row_max; row >= row_min; row --){
		left_edge_line[row]  = col_min - CONTRASTOFFSET;
        right_edge_line[row] = col_max + CONTRASTOFFSET;
    }
	stop_search_row = 0;

	for(row = row_max;row >= row_min;row--){                   
		if(!leftstop){
			search_time = 2-cross_flag;
			do{
				if(search_time == 1){
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
						leftstop = 1;
						stop_search_row = row;
						
						for(stoppoint = row;stoppoint >= 0;stoppoint--){
							left_edge_line[stoppoint ] = col_min;
						}
						search_time = 0;
						break;
					}
					
					if(temp1 < white_min_point){
						left_edge_line[row] = col;
						break;
					}
					if(temp2 > white_max_point){
						continue;
					}
					temp3 = (temp1 - temp2)*200/(temp1 + temp2);
					
					if(temp3 >reference_contrast_ratio || col == col_min){   
						left_edge_line[row] = func_limit_ab(col - CONTRASTOFFSET, 0, IMAGE_W-1);
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
				if(search_time == 1){
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
						rightstop = 1;
						stop_search_row = row;

						for(stoppoint = row;stoppoint >= 0;stoppoint--){
							right_edge_line[stoppoint ] = col_max;
						}
						search_time = 0;
						break;
					}
					
					if(temp1 < white_min_point){
						right_edge_line[row] = col;
						break;
					}
					if(temp2 > white_max_point){
						continue;
					}
					temp3 = (temp1 - temp2)*200/(temp1 + temp2);
					
					if(temp3 >reference_contrast_ratio || col == col_max){   
						right_edge_line[row] = func_limit_ab(col + CONTRASTOFFSET, 0, IMAGE_W-1);
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
	
	if(cross_flag == 1){
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
	
	if(circle_flag == 0){
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
				circle_time = 0;
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
				circle_time = 5;
				beep_flag = 1;
			}
		}
		ips200_show_int(96,240,start_point,4);
		ips200_show_int(128,240,end_point,4);
	}
	else if (circle_flag == 1){
		mid_mode = 1;
		if(circle_time >= CIRCLE_1_TIME){
			circle_time = 0;
			circle_flag = 2;
			beep_flag = 1;
		}
	}
	else if (circle_flag == 2){
		mid_mode = 2;
		if(circle_time >= CIRCLE_2_TIME){
			circle_time = 0;
			circle_flag = 3;
			beep_flag = 1;

		}
	}
	else if (circle_flag == 3){
		mid_mode = 0;
//			mid_mode = 2;
//			circle_time = 0;
//			circle_flag = 4;
//		}
		if(left_edge_line[IMAGE_H/2]<3 && left_edge_line[IMAGE_H/2-1]<3 &&left_edge_line[IMAGE_H/2+1]<3){
//		uint8 start_point = 0,end_point = 0;
//		start_point = image_find_jump_point(left_edge_line,IMAGE_H-5,5,10,0);
//		if(start_point){
			mid_mode = 2;
			circle_time = 0;
			circle_flag = 4;
			beep_flag = 1;

		}
	}
	else if(circle_flag == 4){
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
	else if(circle_flag == 5){
		mid_mode = 1;
		if(circle_time >= CIRCLE_5_TIME){
			circle_time = 0;
			circle_flag = 0;
		}
	}
	
	
	
}


void image_calculate_mid(uint8 mode){
	uint32 mid_sum = 0;
	uint8 valid_count = 0;
	uint8 temp = 0;
	uint8 center_row = 0;

	// 计算中线（根据模式）
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

	// 新的中线计算方法：取连续5行的平均值
	// 中心行从底部数（IMAGE_H - MID_CALC_CENTER_ROW）
	center_row = IMAGE_H - MID_CALC_CENTER_ROW;

	// 取中心行前后各2行，共5行
	for(int offset = -2; offset <= 2; offset++){
		int row = center_row + offset;
		// 边界检查
		if(row >= 0 && row < IMAGE_H && row >= stop_search_row){
			mid_sum += mid_line[row];
			valid_count++;
		}
	}

	// 计算平均值
	if(valid_count > 0){
		temp = (uint8)(mid_sum / valid_count);
	}
	else{
		// 如果没有有效行，使用图像中心
		temp = IMAGE_W / 2;
	}

	// 低通滤波
	if(!last_final_mid_line)
		final_mid_line = temp;
	else
		final_mid_line = temp*0.8+last_final_mid_line*0.2;

	last_final_mid_line = final_mid_line;
}

void image_calculate_prospect(const uint8 image[][IMAGE_W]){
	int col = IMAGE_W/2;
	int16 temp1 = 0,temp2 = 0,temp3 = 0;
	for	(int row = IMAGE_H-1;row > CONTRASTOFFSET;row--){
		temp1 = image[row][col];
		temp2 = image[row-CONTRASTOFFSET][col];
		
		if(row == 4){
			prospect = IMAGE_H-(uint8)row;
			break;
		}
		
		if(temp1 < white_min_point){
			prospect = IMAGE_H-(uint8)row;
			break;
		}
		
		if(temp2 > white_max_point){
			continue;
		}
		
		
		temp3 = (temp1 - temp2)*200/(temp1 + temp2);
		
		if(temp3 >reference_contrast_ratio ){
			prospect = IMAGE_H-(uint8)row;
			break;
		}
	}
//	ips200_show_int(96,224,prospect,4);

}


void image_display_edge_line(const uint8 image[][IMAGE_W], uint16 display_width, uint16 display_height) {
    ips200_show_gray_image(0, 0, rgay_image, UVC_WIDTH, UVC_HEIGHT);	
    
    for(uint16 i = 0; i < IMAGE_H; i++) {
        if(left_edge_line[i] < display_width && i < display_height) {
            ips200_draw_point((uint16)left_edge_line[i], i, RGB565_RED);
        }
        
        if(right_edge_line[i] < display_width && i < display_height) {
            ips200_draw_point((uint16)right_edge_line[i], i, RGB565_BLUE);
        }
        
        if(reference_line[i] < display_width && i < display_height) {
            ips200_draw_point((uint16)reference_line[i], i, RGB565_YELLOW);
        }
        
        if(mid_line[i] < display_width && i < display_height) {
            ips200_draw_point((uint16)mid_line[i], i, RGB565_GREEN);
        }
    }
    
    if(final_mid_line < display_width) {
//        ips200_show_int(96, 208, final_mid_line, 4);
    }
}

void image_get_left_err(void){
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
			if(temp3 >reference_contrast_ratio ){
				stop_count++;
			}
		}
	}
//	ips200_show_int(96,288,stop_count,4);

	if(stop_count> 35 /* && start_time > 30 */){
		stop_flag = 1;
		beep_flag = 1;
	}
}


void image_process(uint16 display_width,uint16 display_height,uint8 mode){
	get_image();
	reference_point = 0; white_max_point = 0;white_min_point = 0;reference_col = 0;
	
	get_reference_point(user_image);
	search_reference_col(user_image);
	search_line(user_image);
	
//	image_get_left_err();
	
	if(if_circle){
		image_circle_analysis();
	}
	
	if(!circle_flag){
		image_cross_analysis();
	}
	
	if(go_flag)
		stop_analysis(user_image);
	

	image_calculate_mid(mid_mode);
	image_calculate_prospect(user_image);
	
	if(mode)
		image_display_edge_line(user_image,display_width,display_height);
}


// ==================== 图像处理时间测量功能 ====================

#include <time.h>

// 静态变量用于存储时间测量数据
static struct timespec img_start_time, img_end_time;
static float last_process_time = 0.0f;       // 上一次处理时间(ms)
static float total_process_time = 0.0f;      // 总处理时间(ms)
static uint32 frame_count = 0;               // 帧计数器
static float max_process_time = 0.0f;        // 最大处理时间(ms)
static float min_process_time = 999.0f;      // 最小处理时间(ms)

/**
 * @brief 开始图像处理计时
 *
 */
void image_process_time_start(void)
{
    clock_gettime(CLOCK_MONOTONIC, &img_start_time);
}

/**
 *
 *
 * @return float 本次图像处理所用时间(毫秒)
 */
float image_process_time_end(void)
{
    clock_gettime(CLOCK_MONOTONIC, &img_end_time);

    // 计算时间差(纳秒级)
    long seconds = img_end_time.tv_sec - img_start_time.tv_sec;
    long nanoseconds = img_end_time.tv_nsec - img_start_time.tv_nsec;

    // 转换为毫秒
    float elapsed_ms = seconds * 1000.0f + nanoseconds / 1000000.0f;

    last_process_time = elapsed_ms;
    total_process_time += elapsed_ms;
    frame_count++;

    // 更新最大/最小值
    if(elapsed_ms > max_process_time)
        max_process_time = elapsed_ms;

    if(elapsed_ms < min_process_time)
        min_process_time = elapsed_ms;

    return elapsed_ms;
}

/**
 *
 * 可以在 main() 中的循环里定期调用
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
    printf("[ 帧数统计 ] 已处理帧数: %u 帧\n", frame_count);
    printf("[ 本次时间 ] 上次处理: %.2f ms\n", last_process_time);
    printf("[ 平均时间 ] %.2f ms (理论FPS: %.1f)\n", avg_time, fps);
    printf("[ 最大时间 ] %.2f ms\n", max_process_time);
    printf("[ 最小时间 ] %.2f ms\n", min_process_time);
    printf("[ 总计时间 ] %.2f ms\n", total_process_time);
    printf("=====================================\n\n");

    // 如果平均处理时间 > 10ms, 给出警告
    if(avg_time > 10.0f)
    {
        printf("⚠️  警告: 图像处理时间超过 10ms, 可能无法满足 100Hz 控制周期!\n");
        printf("   建议: 降低分辨率或优化算法\n\n");
    }
    else if(avg_time < 8.0f)
    {
        printf("✅ 良好: 图像处理时间 < 8ms, 可以满足 100Hz 控制周期\n\n");
    }
}
