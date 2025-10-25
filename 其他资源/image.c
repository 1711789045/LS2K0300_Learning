#include "search_line.h"
#include "zf_device_mpu6050.h"
#include "auto_menu.h"
#include "zf_common_function.h"

#define STRETCH_NUM       80          //补线延伸长度
 
int circle_flag = 1;
uint8_t element_name = 0;
int16_t test_data1 = 0,test_data2 = 0,test_data3 = 0;
int16_t left_nomal_flag = 0,right_nomal_flag = 0;
int16_t island_temp_flag = 0;

uint8 cross_flag = 0;                    //十字标志位
int8_t obstacle_flag = 0;
uint8 speed_up = 0;
uint8 center_final = 0;
uint8_t center_line_mode = 0;
uint16_t encoder_left_count = 0;
uint16_t encoder_right_count = 0;
uint8_t obstacle_more_flag = 0;

int16 continuity_pointLeft[2]={0,0}; // 左不连续点[0]存某行，[1]存某列
int16 continuity_pointRight[2]={0,0}; // 右不连续点 [0]存某行，[1]存某列
//十字↓↓↓↓
int16 Right_Down_Find=0;    //右下点
int16 Left_Down_Find=0;     //左下点
int16 Right_Up_Find=0;      //右上点
int16 Left_Up_Find=0;       //左上点

uint8_t rightfollowline[MT9V03X_H];
uint8_t leftfollowline[MT9V03X_H];

float dx1[5]={0};
float dx2[5]={0};

uint8_t big_half_line[SEARCH_IMAGE_H]={           
21,21,22,23,23,24,25,25,26,27,
27,28,29,29,30,30,31,32,33,33,
34,35,35,37,37,38,38,39,40,40,
41,42,42,43,44,44,45,45,46,47,
47,48,49,50,50,52,52,52,53,54,
55,55,56,57,57,58,59,59,60,60,
61,62,62,63,64,64,65,66,67,67,
68,69,70,70,71,72,72,73,74,74,
75,75,76,77,77,78,79,79,80,82,
81,82,83,84,84,85,86,86,87,88,
88,89,89,90,90,91,92,92,93,94,
95,95,96,97,97,98,99,99,100,101
};
uint8_t half_line[SEARCH_IMAGE_H]={           
 6, 6, 7, 8,8, 9,10,10,11,12,
12,13,14,14,15,15,16,17,18,18,
19,20,20,22,22,23,23,24,25,25,
26,27,27,28,29,29,30,30,31,32,
32,33,34,35,35,37,37,37,38,39,
40,40,41,42,42,43,44,44,45,45,
46,47,47,48,49,49,50,51,52,52,
53,54,55,55,56,57,57,58,59,59,
60,60,61,62,62,63,64,64,65,67,
66,67,68,69,69,70,71,71,72,73,
73,74,74,75,75,76,77,77,78,79,
80,80,81,82,82,83,84,84,85,86
};
//uint8_t right_half_line[SEARCH_IMAGE_H]={           
//	 1, 1, 2, 3, 3, 4, 5, 5, 6, 7,
//	 7, 8, 9, 9,10,10,11,12,13,13, 
//	14,15,15,17,17,18,18,19,20,20,
//	21,22,22,23,24,24,25,25,26,27,
//	27,28,29,30,30,32,32,32,33,34,
//	35,35,36,37,37,38,39,39,40,40,
//	41,42,42,43,44,44,45,46,47,47,
//	48,49,50,50,51,52,52,53,54,54,
//	55,55,56,57,57,58,59,59,60,62,
//	61,62,63,64,64,65,66,66,67,68,
//	68,69,69,70,70,71,72,72,73,74,
//	75,75,76,77,77,78,79,79,80,81 
//};


uint8_t find_jump_point(uint8_t *arrary_value, uint8_t num0, uint8_t num1, uint8_t jump_num, uint8_t model)
{
	uint8 temp_jump_point = 0;
	uint8 temp_data;
	
	if(model){
		temp_jump_point = num0;
		for(int i = 0;i <num0-num1;i++){
			temp_data = abs(arrary_value[num0-i]-arrary_value[num0-i-2]);
			if(temp_data>jump_num){
				temp_jump_point = (uint8)(num0-i);
				return temp_jump_point;
			}
		}
	}
	else{
		temp_jump_point = num1;
		for(int i = 0;i <num0-num1;i++){
			temp_data = abs(arrary_value[num1+i]-arrary_value[num1+i+2]);
			if(temp_data>jump_num){
				temp_jump_point = (uint8)(num1+i);
				return temp_jump_point;
			}
		}
	}
	return 0;
}

uint8_t find_circle_down_jump_point_right(uint8_t *arrary_value, uint8_t num0, uint8_t num1)
{
	for(int i = num0;i > num1;i--)
	{
		if(
			 (arrary_value[i + 5] > arrary_value[i + 3]) &&
			 (arrary_value[i + 3] > arrary_value[i]) &&
				((arrary_value[i - 3] - arrary_value[i]) > 7) &&
				(arrary_value[i - 4] > 183)
			)		return i;

	}
	return 0;
}

uint8_t find_circle_up_jump_point_right(uint8_t *arrary_value, uint8_t num0, uint8_t num1)
{
	for(int i = num1;i < num0;i++)
	{
		if(
			 arrary_value[i - 5] < arrary_value[i - 3] &&
			 arrary_value[i - 3] < arrary_value[i] &&
			(arrary_value[i + 3] - arrary_value[i] > 7) &&
			(arrary_value[i + 4] > 183)
			)		return i;
	}
	return 0;
}

uint8_t find_circle_up_jump_point_left(uint8_t *arrary_value, uint8_t num0, uint8_t num1)
{
	for(int i = num0;i > num1;i--)
	{
		if(
			 arrary_value[i - 5] > arrary_value[i - 3] &&
			 arrary_value[i - 3] > arrary_value[i] &&
			(arrary_value[i] - arrary_value[i + 3] > 10) 
			)		return i;
	}
	return 0;
}

uint8_t find_circle_down_jump_point_left(uint8_t *arrary_value, uint8_t num0, uint8_t num1)
{
	for(int i = num0;i > num1;i--)
	{
		if(
			 arrary_value[i + 5] < arrary_value[i + 3] &&
			 arrary_value[i + 3] < arrary_value[i] &&
			(arrary_value[i] - arrary_value[i - 3] > 10) 
//			(myabs(arrary_value[i - 1] - arrary_value[i - 2]) < 5)&&
			)		return i;
	}
	return 0;
}

void connect_point(uint8_t *array_value, uint8 num0, uint8 num1)
{
    float point_1 = (float)array_value[num0];
    float point_2 = (float)array_value[num1];
    float temp_slope = (point_2 - point_1) / (num0 - num1);

    for (int i = 0; i < (num0 - num1); i++)
    {
        array_value[num0 - i] = (int8)(temp_slope * i) + array_value[num0];
    }
}
void calculate_half_way()
{
		for(int i = SEARCH_IMAGE_H-40;i>0;i--)
		{
			if(left_edge_line[i]<3)
				return;
		}
		for(int i = SEARCH_IMAGE_H-40;i>0;i--)
		{
			if(right_edge_line[i]>185)
				return;
		}
		if(prospect > 100 && top < 10)
		{
			for(int i = 0;i < SEARCH_IMAGE_H;i++)
			{
				half_line[i] = right_edge_line[i] - center_line[i];
			}
		}
	
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief    寻找右圆环的圆环点
  @param     edge_line默认为right_edge_line
						 down_num 搜索下边界
						 up_num 搜索上边界
						 model == 1从上向下搜 model == 0从下向上搜
  @return    返回左圆环圆环点的高度
  Sample     
  @note     
-------------------------------------------------------------------------------------------------------------------*/	
uint8 find_circle_point_right(uint8_t *edge_line,uint8 down_num,uint8 up_num,uint8 model) //model == 1从小到大 model == 0从大到小
{
	uint8 temp_jump_point = 0;
	if(model)
	{
		for(int i = 0;i < (down_num - up_num);i++)
		{
			if(
//				(edge_line[up_num + i] < edge_line[up_num + i - 2]) &&
				(edge_line[up_num + i] < edge_line[up_num + i - 5]) &&
//				(edge_line[up_num + i] < edge_line[up_num + i + 3]) &&
				(edge_line[up_num + i] < edge_line[up_num + i + 5]) &&
				(edge_line[up_num + i - 2] < 175) &&
				(edge_line[up_num + i] < 175) &&
				(edge_line[up_num + i + 2] < 175)
				)
			{
				temp_jump_point = (uint8)(up_num + i);	
				return temp_jump_point;
			}
		}
	
	}
	else
	{
		for(int i = 0;i < (down_num - up_num);i++)
		{
			if(
//				(edge_line[down_num - i] < edge_line[down_num - i - 2]) &&
				(edge_line[down_num - i] < edge_line[down_num - i - 5]) &&
//				(edge_line[down_num - i] < edge_line[down_num - i + 3]) &&
				(edge_line[down_num - i] < edge_line[down_num - i + 5]) &&
				(edge_line[up_num - i - 2] < 175) &&
				(edge_line[up_num - i] < 175) &&
				(edge_line[up_num - i + 2] < 175)			
				)
			{
				temp_jump_point = (uint8)(down_num - i);	
				return temp_jump_point;
			}
		}
	}

	return 0;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief    寻找左圆环的圆环点
  @param     edge_line默认为left_edge_line
						 down_num 搜索下边界
						 up_num 搜索上边界
						 model == 1从上向下搜 model == 0从下向上搜
  @return    返回左圆环圆环点的高度
  Sample     
  @note     
-------------------------------------------------------------------------------------------------------------------*/	
uint8 find_circle_point_left(uint8_t *edge_line,uint8 down_num,uint8 up_num,uint8 model) //model == 1从小到大 model == 0从大到小
{
	uint8 temp_jump_point = 0;
	if(model)
	{
		for(int i = 0;i < (down_num - up_num);i++)
		{
			if(
				edge_line[up_num + i] > edge_line[up_num + i - 1]&&
				edge_line[up_num + i] > edge_line[up_num + i + 3]&&
				edge_line[up_num + i] > edge_line[up_num + i + 5]
				)
			{
				temp_jump_point = (uint8)(up_num + i);	
				return temp_jump_point;
			}
		}
	
	}
	else
	{
		for(int i = 0;i < (down_num - up_num);i++)
		{
			if(
				edge_line[down_num - i] > edge_line[down_num - i - 1]&&
				edge_line[down_num - i] > edge_line[down_num - i + 3]&&
				edge_line[down_num - i] > edge_line[down_num - i + 5]
				)
			{
				temp_jump_point = (uint8)(down_num - i);	
				return temp_jump_point;
			}
		}
	}

	return 0;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过权重数组获取目标中线值
  @param     
  @return    改变center_final
  Sample     
  @note     
-------------------------------------------------------------------------------------------------------------------*/	

/*
void get_center_weight()
{
			
		memcpy(center_line_weight_buffer,center_line_weight,sizeof(center_line_weight));				
	for(int i = 0;i < 120;i++)
	{	
			if(i < top)
			{
				center_line_weight_buffer[i] = 0;
			}
	}			
	for(int i = 0;i < 120;i++)
	{
		center_line_weight_temp += center_line_weight_buffer[i]*center_line[i];	
		center_line_weight_count += center_line_weight_buffer[i];
	}
		center_final = center_line_weight_temp / center_line_weight_count;	
		center_line_weight_count = 0;
		center_line_weight_temp = 0;


}*/
/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过平均值获取目标中线值
  @param     
  @return    改变center_final
  Sample     
  @note     
-------------------------------------------------------------------------------------------------------------------*/	
void get_center_average()
{ 
	int qianzhan_real = 0;

	if(qianzhan > top)
	{
		qianzhan_real = qianzhan;
		center_final = (center_line[qianzhan_real] + center_line[qianzhan_real + 1] + center_line[qianzhan_real + 2] + center_line[qianzhan_real + 3] + center_line[qianzhan_real + 4]
		+ center_line[qianzhan_real + 5] + center_line[qianzhan_real + 6] + center_line[qianzhan_real + 7] + center_line[qianzhan_real + 8] + center_line[qianzhan_real + 9]) / 10;
	}
	else if(qianzhan == top)
	{
		qianzhan_real = top + 1;
		center_final = (center_line[qianzhan_real] + center_line[qianzhan_real + 1] + center_line[qianzhan_real + 2] + center_line[qianzhan_real + 3] + center_line[qianzhan_real + 4]
		+ center_line[qianzhan_real + 5] + center_line[qianzhan_real + 6] + center_line[qianzhan_real + 7] + center_line[qianzhan_real + 8]) / 9;
	}	
	else if(qianzhan == (top - 1))
	{
		qianzhan_real = top + 1;
		center_final = (center_line[qianzhan_real] + center_line[qianzhan_real + 1] + center_line[qianzhan_real + 2] + center_line[qianzhan_real + 3] + center_line[qianzhan_real + 4]
		+ center_line[qianzhan_real + 5] + center_line[qianzhan_real + 6] + center_line[qianzhan_real + 7]) / 8;
	}	
	else if(qianzhan == (top - 2))
	{
		qianzhan_real = top + 1;
		center_final = (center_line[qianzhan_real] + center_line[qianzhan_real + 1] + center_line[qianzhan_real + 2] + center_line[qianzhan_real + 3] + center_line[qianzhan_real + 4]
		+ center_line[qianzhan_real + 5] + center_line[qianzhan_real + 6]) / 7;
	}		
	else if(qianzhan == (top - 3))
	{
		qianzhan_real = top + 1;
		center_final = (center_line[qianzhan_real] + center_line[qianzhan_real + 1] + center_line[qianzhan_real + 2] + center_line[qianzhan_real + 3] + center_line[qianzhan_real + 4]
		+ center_line[qianzhan_real + 5]) / 6;
	}		
	else if(qianzhan <= (top - 4))
	{
		qianzhan_real = top + 1;
		center_final = (center_line[qianzhan_real] + center_line[qianzhan_real + 1] + center_line[qianzhan_real + 2] + center_line[qianzhan_real + 3] + center_line[qianzhan_real + 4]) / 5;
	}		

/*	if(qianzhan < top)
	{
		qianzhan_real = top;
		center_final = (center_line[qianzhan_real] + center_line[qianzhan_real + 1] + center_line[qianzhan_real + 2] + center_line[qianzhan_real + 3]) / 4;
	}
	else
	{
		qianzhan_real = qianzhan;
		center_final = (center_line[qianzhan_real] + center_line[qianzhan_real + 1] + center_line[qianzhan_real + 2] + center_line[qianzhan_real + 3] + center_line[qianzhan_real + 4]) / 5;
	}*/

}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     环岛入环和出环拉线
  @param     
  @return    改变left_edge_line和right_edge_line
  Sample     
  @note     
-------------------------------------------------------------------------------------------------------------------*/	

void Connect_Circle_In()
{
	if(circle_flag == 1)
	{
		for(int i = 0;i < 120;i++)
		{
			left_edge_line[i] = 188 - 1.5 * i;            //慢速可以再减20
		if(left_edge_line[i] > 188)left_edge_line[i] = 188;
		if(left_edge_line[i] < 0)left_edge_line[i] = 0;
		}
	}
	else if(circle_flag == -1)
	{
		for(int i = 0;i < 120;i++)
		{
			right_edge_line[i] = 1.5 * i;                 //慢速可以再加20
		if(right_edge_line[i] > 188)right_edge_line[i] = 188;
		if(right_edge_line[i] < 0)right_edge_line[i] = 0;
		}		
	}
}




//================================以下新圆环===========================================
void FindIsland_Ready_Right()
{
	if(island_temp_flag == 0)
	{
 		if(top > 11 || prospect < 113)return;  //截至行较远
		
		for(int i = SEARCH_IMAGE_H-40;i>20;i--) //左边线连续
	  {
			if(abs(left_edge_line[i]-left_edge_line[i+1]) > 5)
			{			
//				island_temp_flag = 10;
				return;
			}
		}
		
		for(int i = SEARCH_IMAGE_H-40;i>30;i--)//左边线不丢线
		{
			if(left_edge_line[i]<5)
			{
//				island_temp_flag = 20;
				return;
			}

		}	
//		island_temp_flag = 30;
		
		uint8 up_point = 0,mid_point = 0,down_point = 0;
		
		down_point = find_circle_down_jump_point_right(right_edge_line,SEARCH_IMAGE_H - 5,40);
		test_data1 = down_point;
		
		if(down_point)
		{
			
//			island_temp_flag= 40;
		  mid_point = find_circle_point_right(right_edge_line,down_point - 5,5,0);
			test_data2 = mid_point;		
		}
		else
		{
//			island_temp_flag= 50;
			up_point = find_circle_up_jump_point_right(right_edge_line,SEARCH_IMAGE_H - 40,5);
			test_data3 = up_point;					
		}
		
		if(down_point && mid_point)
		{
					island_temp_flag = 1;
					center_line_mode = 2;
					encoder_left_count = 0;
					return;
		}
		
		if(up_point)
		{
//			island_temp_flag= 60;
			mid_point = find_circle_point_right(right_edge_line,SEARCH_IMAGE_H - 10,up_point + 5,1);	
			test_data2 = mid_point;
		}
		else
		{
//			island_temp_flag= 70;
			mid_point = 0;
			test_data2 = mid_point;			
		}
		
		if(up_point && mid_point)
		{
					island_temp_flag = 2;
					center_line_mode = 2;
					encoder_left_count = 6000;  //7000spe200
		}		
	}

}
void FinfIsland_In_Right()         //island_temp_flag == 1 || island_temp_flag == 2寻左边线  8500spe200
{
	if(island_temp_flag == 1 && encoder_left_count > 6000) 
	{
		island_temp_flag = 3;
		center_line_mode = 0;		
	}
	if(island_temp_flag == 2 && encoder_left_count > 6000) 
	{
		island_temp_flag = 3;
		center_line_mode = 0;		
	}
}

void FindIsland_Ing_Right()         //island_temp_flag == 3拉线入环  20000spe200
{
 if(island_temp_flag == 3 && encoder_left_count > 20000)
 {
	 island_temp_flag = 4;
	 center_line_mode = 0;		 
 }
}

void FindIsland_Outready_Right()       //island_temp_flag == 4正常巡线  38000spe200
{
	if(island_temp_flag == 4 && encoder_left_count > 42000)
	{
		island_temp_flag = 5;
		center_line_mode = 0;	
	}
}

void FindIsland_Out_Right()            //island_temp_flag == 5拉线出环  51000spe200
{
	uint8_t left_not_lianxu = 0,left_diuxian = 0;
	if(island_temp_flag == 5)
	{
		if( encoder_left_count > 53000)
		{
			island_temp_flag = 6;
			center_line_mode = 2;
		}
		for(int i = SEARCH_IMAGE_H-40;i>20;i--) //左边线连续
	  {
			if((left_edge_line[i] - left_edge_line[i+1] > 5) || (left_edge_line[i] < left_edge_line[i+1]))
			{		
				left_not_lianxu ++;
			}
		}
		
		for(int i = SEARCH_IMAGE_H-40;i>30;i--)//左边线不丢线
		{
			if(left_edge_line[i]<5)
			{
				left_diuxian ++;
			}

		}	
		if((left_not_lianxu < 1) && (left_diuxian < 1))
		{
			island_temp_flag = 6;
			center_line_mode = 2;
		}
	} 
}

void FindIsland_Complete_Right()         //island_temp_flag == 6寻左边线  60000spe200
{
	if(island_temp_flag == 6 && encoder_left_count > 60000)
	{
		island_temp_flag = 0;
		center_line_mode = 0;	
	}
}
void circle_state()
{
	if(circle_flag == 1)              //右圆环
	{
		FindIsland_Ready_Right();
		FinfIsland_In_Right();
		FindIsland_Ing_Right();
		FindIsland_Outready_Right();
		FindIsland_Out_Right();
		FindIsland_Complete_Right();
	}
}
/*
void left_ring_state()
{
	if(ring_temp_flag == 0)
	{
 		if(top > 11 || prospect < 113)return;  //截至行较远
		
		for(int i = SEARCH_IMAGE_H-40;i>20;i--) //左边线连续
	  {
			if(myabs(left_edge_line[i]-left_edge_line[i+1]) > 5)
			{			
				return;
			}
		}
		
		for(int i = SEARCH_IMAGE_H-40;i>30;i--)//左边线不丢线
		{
			if(left_edge_line[i]<5)
			{
				return;
			}

		}			
		uint8 up_point = 0,mid_point = 0,down_point = 0;
		
		down_point = find_circle_down_jump_point_right(right_edge_line,SEARCH_IMAGE_H - 5,40);
		test_data1 = down_point;
		
		if(down_point)
		{
		  mid_point = find_circle_point_right(right_edge_line,down_point - 5,5,0);
			test_data2 = mid_point;		
		}
		else
		{
			up_point = find_circle_up_jump_point_right(right_edge_line,SEARCH_IMAGE_H - 40,5);
			test_data3 = up_point;					
		}
		
		if(down_point && mid_point)
		{
					ring_temp_flag = 1;
					center_line_mode = 2;
					encoder_left_count = 0;
					angle_temp = 0;
					beep_flag = 50;
			    qianzhan = 40;//拉近前瞻,前瞻值需要调,拉近前瞻后转向环需要重新调?
					return;
		}
		
		if(up_point)
		{
			mid_point = find_circle_point_right(right_edge_line,SEARCH_IMAGE_H - 10,up_point + 5,1);	
			test_data2 = mid_point;
		}
		else
		{
			mid_point = 0;
			test_data2 = mid_point;			
		}
		
		if(up_point && mid_point)
		{
					ring_temp_flag = 2;
					center_line_mode = 2;
					encoder_left_count = 6000;  //7000spe200
					angle_temp = 0;
					beep_flag = 50;
			    qianzhan = 40;//拉近前瞻,前瞻值需要调,拉近前瞻后转向环需要重新调?

		}		
	}
	if((ring_temp_flag == 1) || (ring_temp_flag == 2))
	{
		if(encoder_left_count > 6000) 
		{
			ring_temp_flag = 3;
			center_line_mode = 1;		 
		}
	}
	if(ring_temp_flag == 3)
	{
			uint8_t left_not_lianxu = 0,left_diuxian = 0;
			if( encoder_left_count > 53000)
			{
				ring_temp_flag = 4;
				center_line_mode = 2;
			}
			for(int i = SEARCH_IMAGE_H-40;i>20;i--) //左边线连续
			{
				if(myabs(left_edge_line[i]-left_edge_line[i+1]) > 5)
				{		
					left_not_lianxu ++;
				}
			}
		
			for(int i = SEARCH_IMAGE_H-40;i>30;i--)//左边线不丢线
			{
				if(left_edge_line[i]<5)
				{
					left_diuxian ++;
				}

			}		
			if((left_not_lianxu < 1) && (left_diuxian < 1))
			{
				ring_temp_flag = 4;
				center_line_mode = 2;
			} 
	}
	if(ring_temp_flag == 4 && encoder_left_count > 60000)
	{
		center_line_mode = 0;
		ring_temp_flag = 0;
	}

}
*/

//=================================以上新圆环==========================================






/*-------------------------------------------------------------------------------------------------------------------
  @brief     判断是否有斑马线
  @param     
  @return    banmaxian_flag
  Sample     
  @note      count用于判断斑马线条数,
						 banmaxian_flag用于判断斑马线长度
-------------------------------------------------------------------------------------------------------------------*/	

/*void banmaxian_stop(const uint8_t *image)
{
	int16 temp1 = 0,temp2 = 0,temp3 = 0,count = 0;
	for(int i = 0; i < SEARCH_IMAGE_W - 3; i+=3)
	{
		for( int j = 79; j < 81; j++)
		{
			temp1 = *(image + j * SEARCH_IMAGE_W + i);
			temp2 = *(image + j * SEARCH_IMAGE_W + i + 3);
			temp3 = (temp1 - temp2)*200/(temp1 + temp2);
			if(temp3 > reference_contrast_ratio)count ++;
		}
	}
	if(count > 20)banmaxian_flag ++;
}*/

/*-------------------------------------------------------------------------------------------------------------------
  @brief     判断是否有障碍
  @param     
  @return    修改obstacle_flag
  Sample     
  @note      obstacle_more_flag == 1为只搜左障碍,obstacle_more_flag == -1为只搜右障碍
						 obstacle_flag == 1为有障碍,obstacle_flag == 0为无障碍
-------------------------------------------------------------------------------------------------------------------*/	
/*
void Find_obstacle()
{
	if(obstacle_more_flag == 1)
	{
		for(int i = SEARCH_IMAGE_H-40;i>20;i--)
	  {
			if((abs(right_edge_line[i + 1]-right_edge_line[i]) > 5) || right_edge_line[i + 1] < right_edge_line[i])
			{			
				obstacle_flag = 10;
				return;
			}
		}
		for(int i = SEARCH_IMAGE_H-40;i>30;i--)
		{
			if(right_edge_line[i] > 183)
			{
				obstacle_flag = 20;
				return;
			}
		}	
//		uint16 temp = 0;
//		for(int i = 0; i < SEARCH_IMAGE_H;i++)
//		{
//			temp += center_line[i];
//		}
//		temp = temp / 120;
		uint16_t up_point = 0,down_point = 0;
		for(int i = top + 5; i < SEARCH_IMAGE_H - 30;i++)
		{
				if(
					(left_edge_line[i - 3] < left_edge_line[i])	&&
					(left_edge_line[i - 5] < left_edge_line[i]) )
				{
				up_point = i;
					break;
				}
		}
		for(int i = up_point + 5; i < SEARCH_IMAGE_H - 30;i++)
		{
			if(
					(left_edge_line[i] -  left_edge_line[i + 3] > 5) &&
					(left_edge_line[i] - left_edge_line[i + 5] > 5) )
			{
				down_point = i;
				break;
			}
		}
		if(up_point == 0 && down_point != 0)obstacle_flag = 30;
		if(up_point !=0 && down_point == 0)obstacle_flag = 40;
		if(up_point && down_point && (down_point - up_point < 20) )
		{
			obstacle_flag = 1;
		}
	}
	
	else if(obstacle_more_flag == -1)
	{
		for(int i = SEARCH_IMAGE_H-40;i>20;i--)
	  {
			if((left_edge_line[i]-left_edge_line[i + 1] > 5) || left_edge_line[i] < left_edge_line[i + 1])
			{			
				obstacle_flag = 0;
				return;
			}
		}
		for(int i = SEARCH_IMAGE_H-40;i>30;i--)
		{
			if(left_edge_line[i] < 5)
			{
				obstacle_flag = 0;
				return;
			}
		}	
//		uint16 temp = 0;
//		for(int i = 0; i < SEARCH_IMAGE_H;i++)
//		{
//			temp += center_line[i];
//		}
//		temp = temp / 120;
		uint16_t up_point = 0,down_point = 0;
		for(int i = 25; i < SEARCH_IMAGE_H - 30;i++)
		{
				if(
					(right_edge_line[i] < right_edge_line[i - 3]) &&
					(right_edge_line[i] < right_edge_line[i - 5]) )
				{
				up_point = i;
					break;
				}
		}
		for(int i = up_point + 5; i < SEARCH_IMAGE_H - 30;i++)
		{
			if(
					(right_edge_line[i + 3] -  right_edge_line[i] > 5) &&
					(right_edge_line[i + 5] - right_edge_line[i] > 5) )
			{
				down_point = i;
				break;
			}
		}
		if(up_point && down_point && (down_point - up_point < 20) )
		{
			obstacle_flag = -1;
		}
	}
	}
*/
/*-------------------------------------------------------------------------------------------------------------------
  @brief     判断是否为长直到,用于加速
  @param     
  @return    修改speed_up
  Sample     
  @note      speed_up == 1加速,speed_up == 0不加速
-------------------------------------------------------------------------------------------------------------------*/	
	
void straight_up()
{
		for(int i = SEARCH_IMAGE_H-20;i>20;i--)
	  {
			if(abs(right_edge_line[i + 1]-right_edge_line[i]) > 3)
			{	
				speed_up = 10;				
				return;
			if(abs(left_edge_line[i]-left_edge_line[i + 1]) > 3)
			{			
				speed_up = 20;						
				return;
			}
			}
		}
		for(int i = SEARCH_IMAGE_H-35;i>15;i--)
		{
			if(right_edge_line[i] > 183)
			{
				speed_up = 30;		
				return;
			}
			if(left_edge_line[i] < 5)
			{
				speed_up = 40;						
				return;
			}
		}
		if(abs(center_final - 95) > 7)
		{
				speed_up = 50;						
				return;			
		}
		if(prospect < 113)
		{
				speed_up = 60;						
				return;			
		}
		speed_up = 1;
		
}



//==========================        以下新十字补线      ======================================


/*-------------------------------------------------------------------------------------------------------------------
  @brief     找下面的两个拐点，供十字使用
  @param     搜索的范围起点，终点
  @return    修改两个全局变量
             Right_Down_Find=0;
             Left_Down_Find=0;
  Sample     Find_Down_Point(int16 start,int16 end)
  @note      运行完之后查看对应的变量，注意，没找到时对应变量将是0
-------------------------------------------------------------------------------------------------------------------*/
void Find_Down_Point(int16 start,int16 end)
{
    int16 i,t;
    Right_Down_Find=0;
    Left_Down_Find=0;
    if(start<end)               //从下往上找，大的索引在图象下面
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-3)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=MT9V03X_H-1-3;
    if(end>=MT9V03X_H-5)
        end=MT9V03X_H-5;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(Left_Down_Find==0&&//只找第一个符合条件的点
            (left_edge_line[i]>0)&&//左边界点不能为0
           abs(left_edge_line[i]-left_edge_line[i+1])<=5&&//角点的阈值可以更改
           abs(left_edge_line[i+1]-left_edge_line[i+2])<=5&&
           abs(left_edge_line[i+2]-left_edge_line[i+3])<=5&&
            ((left_edge_line[i]-left_edge_line[i-2])>=3||left_edge_line[i-2]<=0)&&
            ((left_edge_line[i]-left_edge_line[i-3])>=5||left_edge_line[i-3]<=0)&&
            ((left_edge_line[i]-left_edge_line[i-4])>=5||left_edge_line[i-4]<=0))
        {
            Left_Down_Find=i+2;//获取行数即可
            if(Left_Down_Find==start+2)
            {
                Left_Down_Find=0;//如果是起始行，说明没有找到
            }
        }
        if(Right_Down_Find==0&&//只找第一个符合条件的点
           abs(right_edge_line[i]-right_edge_line[i+1])<=5&&//角点的阈值可以更改
           abs(right_edge_line[i+1]-right_edge_line[i+2])<=5&&
           abs(right_edge_line[i+2]-right_edge_line[i+3])<=5&&
            right_edge_line[i]<MT9V03X_W-1&&//右边界点不能为MT9V03X_W-1
              ((right_edge_line[i]-right_edge_line[i-2])<=-3||right_edge_line[i-2]>MT9V03X_W-2)&&
              ((right_edge_line[i]-right_edge_line[i-3])<=-5||right_edge_line[i-3]>MT9V03X_W-2)&&
              ((right_edge_line[i]-right_edge_line[i-4])<=-5||right_edge_line[i-4]>MT9V03X_W-2))
        {
            Right_Down_Find=i+2;
            if(Right_Down_Find==start+2)
            {
                Right_Down_Find=0;//如果是起始行，说明没有找到
            }
        }
        if(Left_Down_Find!=0&&Right_Down_Find!=0)//两个找到就退出
        {
            break;
        }
    }

}
 

/*-------------------------------------------------------------------------------------------------------------------
  @brief     找上面的两个拐点，供十字使用,从上往下找
  @param     搜索的范围起点，终点
  @return    修改两个全局变量
             Left_Up_Find=0;
             Right_Up_Find=0;
  Sample     Find_Up_Point(int16 start,int16 end)
  @note      运行完之后查看对应的变量，注意，没找到时对应变量将是0
-------------------------------------------------------------------------------------------------------------------*/
void Find_Up_Point(int16 start,int16 end)
{
    int16 i,t;
    Left_Up_Find=0;
    Right_Up_Find=0;
    if(start>end)
    {
        t=start;
        start=end;
        end=t;
    }
    //start<end由上往下
    if(end>=MT9V03X_H-5)
        end=MT9V03X_H-5;
    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start<=5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=5;
    if(start<top+5)
    {
        start =top+5;
    }
    for(i=start;i<=end;i++)
    { 
//        if(Left_Up_Find==0&&//只找第一个符合条件的点
//           abs(leftline[i]-leftline[i-1])<=4&&
//           abs(leftline[i-1]-leftline[i-2])<=4&&
//           abs(leftline[i-2]-leftline[i-3])<=4&&
//              ((leftline[i]-leftline[i+2])>=3||leftline[i+2]<1)&&
//              ((leftline[i]-leftline[i+3])>=5||leftline[i+3]<1)&&
//              ((leftline[i]-leftline[i+4])>=10||leftline[i+4]<1))
//        {

//            Left_Up_Find=i-2;//获取行数即可
//            if(Left_Up_Find==start-2)
//            {
//                Left_Up_Find=0;//如果是起始行，说明没有找到
//            }
//        }
//        if(Right_Up_Find==0&&//只找第一个符合条件的点
//           abs(rightline[i]-rightline[i-1])<=4&&//下面两行位置差不多
//           abs(rightline[i-1]-rightline[i-2])<=4&&
//           abs(rightline[i-2]-rightline[i-3])<=4&&
//              ((rightline[i]-rightline[i+2]<=-3)||rightline[i+2]>MT9V03X_W-2)&&
//              ((rightline[i]-rightline[i+3])<=-5||rightline[i+3]>MT9V03X_W-2)&&
//              ((rightline[i]-rightline[i+4])<=-10||rightline[i+4]>MT9V03X_W-2))
//        {
//            Right_Up_Find=i-2;//获取行数即可
//            if(Right_Up_Find==start-2)
//            {
//                Right_Up_Find=0;//如果是起始行，说明没有找到
//            }
//        }

        if(Left_Up_Find==0&&//只找第一个符合条件的点
//					 (left_edge_line[i - 10] > 5)&&
//					 (left_edge_line[i - 11] > 5)&&
           abs(left_edge_line[i]-left_edge_line[i-1])<=3&&
           abs(left_edge_line[i-1]-left_edge_line[i-2])<=3&&
           abs(left_edge_line[i-2]-left_edge_line[i-3])<=3&&
              ((left_edge_line[i]-left_edge_line[i+2])>=3)&&
              ((left_edge_line[i]-left_edge_line[i+3])>=5)&&
              ((left_edge_line[i]-left_edge_line[i+4])>=10))
        {

            Left_Up_Find=i-2;//获取行数即可
            if(Left_Up_Find==start-2)
            {
                Left_Up_Find=0;//如果是起始行，说明没有找到
            }
        }
        if(Right_Up_Find==0&&//只找第一个符合条件的点
//					 (right_edge_line[i - 10] < 183)&&
//					 (right_edge_line[i - 11] < 183)&&
           abs(right_edge_line[i]-right_edge_line[i-1])<=3&&//下面两行位置差不多
           abs(right_edge_line[i-1]-right_edge_line[i-2])<=3&&
           abs(right_edge_line[i-2]-right_edge_line[i-3])<=3&&
              ((right_edge_line[i]-right_edge_line[i+2]<=-3))&&
              ((right_edge_line[i]-right_edge_line[i+3])<=-5)&&
              ((right_edge_line[i]-right_edge_line[i+4])<=-10))
        {
            Right_Up_Find=i-2;//获取行数即可 
            if(Right_Up_Find==start-2)
            {
                Right_Up_Find=0;//如果是起始行，说明没有找到
            }
        }
        if(Left_Up_Find!=0&&Right_Up_Find!=0)//下面两个找到就出去
        {
            break;
        }
    }
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     确定连续性，用于处理右侧线。
参数说明     uint8起点和终点
返回参数     中点位置
使用示例     
备注信息     
-------------------------------------------------------------------------------------------------------------------
*/
int16 continuity_right(uint8 start,uint8 end)
{
    int16 i;
    int16 continuity_change_flag=0;
    if(start>=MT9V03X_H-2)//数组越界保护
        start=MT9V03X_H-2;
    if(end<=1)
    {
        end=1;
    }
        if(start<end)
    {
        uint8 t=start;
        start=end;
        end=t;
    }
    for(i=start;i>=end;i--)
    {
        if(abs(right_edge_line[i]-right_edge_line[i-1])>=5)//连续性阈值是5，可更改
       {
            continuity_change_flag=i;                                         //在i处不连续了

            break;
       }

    }

    return continuity_change_flag;
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     确定从start到end连续性，用于处理左侧线。
参数说明     uint8起点和终点
返回参数     中点位置
使用示例     
备注信息     
-------------------------------------------------------------------------------------------------------------------
*/

int16 continuity_left(uint8 start,uint8 end)
{
    int16 i;
    int16 continuity_change_flag=0;
    if(start>=MT9V03X_H-2)//数组越界保护
        start=MT9V03X_H-2;
    if(end<=1)
    {
        end=1;
    }
        if(start<end)
    {
        uint8 t=start;
        start=end;
        end=t;
    }
    for(i=start;i>=end;i--)
    {
        if(abs(left_edge_line[i]-left_edge_line[i-1])>=5)//连续性阈值是5，可更改
       {

            continuity_change_flag=i;                                         //在i处不连续了

            break;
       }

    }
    return continuity_change_flag;
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     补线右
参数说明     起点x，y终点y，增长率dx
返回参数     无  
使用示例     
备注信息     增长率dx等于x1-x2/y1-y2
-------------------------------------------------------------------------------------------------------------------
*/
void draw_Rline_k(int16 startx, int16 starty, int16 endy, float dx) {


    int16 step = (starty < endy) ? 1 : -1;

    if (dx == 0) 
    {                
        for (int16 i = starty; i != endy; i += step) 
        {
            rightfollowline[i] = startx;
        }
        return;
    }
    for (int16 i = starty; i != endy; i += step) {
        int16 temp=startx + (int16)((float)(i - starty) * dx );
        if(temp<0) temp=0; //防止越界
        if(temp>MT9V03X_W-1) temp=MT9V03X_W-1; //防止越界    
        rightfollowline[i] = temp;
    }
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     右两点间连线
参数说明     起点xy，终点xy
返回参数     无
使用示例     
备注信息     
-------------------------------------------------------------------------------------------------------------------
*/
void add_Rline_k(int16 startx, int16 starty, int16 endy,int16 endx)
{
    if(endy!=starty)
    {
        float dx=(float)(endx-startx)/(float)(endy-starty);
        draw_Rline_k(startx,starty,endy,dx);
    }
    else
    {
        return;
    }
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     补线左
参数说明     起点x，y终点y，增长率dx
返回参数     无
使用示例     
备注信息     增长率dx等于x1-x2/y1-y2
-------------------------------------------------------------------------------------------------------------------
*/
void draw_Lline_k(int16 startx, int16 starty, int16 endy, float dx) {
    int16 step = (starty < endy) ? 1 : -1;
    if (dx == 0) {

        for (int16 i =starty; i != endy; i += step) {
            leftfollowline[i] = startx;
        }
        return;
    }
    for (int16 i = starty; i != endy; i += step) {

        int16 temp=startx + (int16)((float)(i - starty) * dx );   
        if(temp<0) temp=0; //防止越界
        if(temp>MT9V03X_W-1) temp=MT9V03X_W-1; //防止越界
        leftfollowline[i] = temp;

    }
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     左两点间连线
参数说明     起点xy，终点xy
返回参数     无
使用示例     
备注信息     能除以0那你真牛逼
-------------------------------------------------------------------------------------------------------------------
*/
void add_Lline_k(int16 startx, int16 starty, int16 endy,int16 endx)
{
    if(endy!=starty)
    {
        float dx=(float)(endx-startx)/(float)(endy-starty);
        draw_Lline_k(startx,starty,endy,dx);
    }
    else{
         return;
    }
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     滑动平均滤波左
参数说明     无
返回参数     无
使用示例     
备注信息     
-------------------------------------------------------------------------------------------------------------------
*/
void dx1_left_average(float dx)
{
    for(uint8 i=1;i<5;i++)
    {
        dx1[i-1]=dx1[i];
    }
    dx1[4]=dx;
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     滑动平均滤波右
参数说明     无
返回参数     无
使用示例     
备注信息     
-------------------------------------------------------------------------------------------------------------------
*/
void dx2_right_average(float dx)
{
    for(uint8 i=1;i<5;i++)
    {
        dx2[i-1]=dx2[i];
    }
    dx2[4]=dx;
}

/*
------------------------------------------------------------------------------------------------------------------
函数简介     自上而下补左线
参数说明     起点
返回参数     无
使用示例     
备注信息     
-------------------------------------------------------------------------------------------------------------------
*/
void lenthen_Left_bondarise(int16 start)
{
    if(start<7){start=7;}
    if(start>MT9V03X_H-1){start=MT9V03X_H-1;}
    float dx=(float)(left_edge_line[start]-left_edge_line[start-5])/5;
    dx1_left_average(dx);
    float dx_average=(dx1[0]+dx1[1]+dx1[2]+dx1[3]+dx1[4])/5;
    for(int16 i=start;i<MT9V03X_H-1;i++)
    {
        if((float)left_edge_line[start]+(float)(dx_average*(i-start))<0)
        {
            leftfollowline[i]=0;
        }
        else if((float)left_edge_line[start]+dx_average*(float)(i-start)>(float)MT9V03X_W-2)
        {
            leftfollowline[i]=MT9V03X_W-1;
        }
        else
        {
            leftfollowline[i]=(int16)((float)left_edge_line[start]+dx_average*(float)(i-start));
        }
    }
}


 /*
------------------------------------------------------------------------------------------------------------------
函数简介     自上而下补右线
参数说明     起点
返回参数     无
使用示例      
备注信息     
-------------------------------------------------------------------------------------------------------------------
*/  
void lenthen_Right_bondarise(int16 start)  
{
    if(start<7){start=7;}
    if(start>MT9V03X_H-1){start=MT9V03X_H-1;}
    float dx=(float)(right_edge_line[start]-right_edge_line[start-5])/5;
    dx2_right_average(dx);
    float dx_average=(dx2[0]+dx2[1]+dx2[2]+dx2[3]+dx2[4])/5;
    for(int16 i=start;i<MT9V03X_H-1;i++)
    {
        if((float)right_edge_line[start]+(float)(dx_average*(i-start))>MT9V03X_W-2)
        {
            rightfollowline[i]=MT9V03X_W-1;
        }
        else if((float)right_edge_line[start]+dx_average*(float)(i-start)<0)
        {
            rightfollowline[i]=0;
        }
        else 
        {
            rightfollowline[i]=(int16)((float)right_edge_line[start]+dx_average*(float)(i-start));
        }
    }
}

enum mark {
    straight,    // 直道行驶
    crossroad,   // 十字路口
    crossroadL, // 左斜入十字
    crossroadR, // 右斜入十字
};
enum mark carstatus_now = straight;


 /*
------------------------------------------------------------------------------------------------------------------
函数简介     十字补线应用函数
参数说明     
返回参数     无
使用示例      
备注信息     最前面的两个memcpy如果注释了，在Update_Line函数要有这两个
-------------------------------------------------------------------------------------------------------------------
*/ 
void cross_check()
{

	  memcpy(leftfollowline, left_edge_line, sizeof(left_edge_line));
    memcpy(rightfollowline, right_edge_line, sizeof(right_edge_line));
	
//		get_center_line();
	  Find_Up_Point(MT9V03X_H-1, top); // 查找上半段边界点
    Find_Down_Point(MT9V03X_H-1, top); //查找下半段边界点
    continuity_pointLeft[0]=continuity_left(MT9V03X_H-1,top+5); // 左连续性判断
    continuity_pointRight[0]=continuity_right(MT9V03X_H-1,top+5); // 右连续性判断
    continuity_pointLeft[1]=left_edge_line[continuity_pointLeft[0]]; // 左连续性点列
    continuity_pointRight[1]=right_edge_line[continuity_pointRight[0]]; // 右连续性点列
	  
		if(carstatus_now == straight) {

    if(top < 11){
    if(continuity_pointLeft[0] != 0 && continuity_pointRight[0] != 0 && Right_Up_Find != 0 && Left_Up_Find != 0 && (Right_Up_Find > top - 2 && Left_Up_Find > top - 2))//左不连续点找到 且右不连续点找到，且左上拐点找到且右上拐点找到，此时为正入十字
    {
        carstatus_now = crossroad; // 进入十字路口状态
        return;
    }
    if(continuity_pointLeft[0] != 0 &&continuity_pointRight[0] == 0 && Left_Up_Find != 0 && Left_Up_Find > top - 2)//左不连续点找到 且右不连续点未找到，且左上拐点找到，此时为左斜入十字
    {
        carstatus_now = crossroadL; // 进入十字路口状态
        return;
    }

    if(continuity_pointLeft[0] == 0 && continuity_pointRight[0] != 0 && Right_Up_Find != 0 && Right_Up_Find > top - 2)//左不连续点未找到 右不连续点找到，且右上拐点找到，此时为右斜入十字
    {
        carstatus_now = crossroadR; // 进入十字路口状态
        return; 
    }
    }
//    ips200_show_string(0,300,"straig");		
  }
//   ========================== 十字路口状态处理  ==============================
  if(carstatus_now == crossroad) {
       int start_down_point=5;
       int16 temp1_L=0;//记录第一个左拐点
       int16 temp1_R=0;//记录第一个右拐点 

      //        // 确定下半段边界点（取左右下点
      if(Left_Down_Find <= Left_Up_Find) Left_Down_Find = 0;
      if(Right_Down_Find <= Right_Up_Find) Right_Down_Find = 0;

      /* 边界线拟合策略 */
      if(Left_Down_Find != 0 && Right_Down_Find != 0) {
          // 情况1：左右下点均有效 → 双边界直线拟合
          add_Rline_k(right_edge_line[Right_Down_Find], Right_Down_Find, 
                     Right_Up_Find-2, right_edge_line[Right_Up_Find-2]);        // 右边界拟合
          add_Lline_k(left_edge_line[Left_Down_Find], Left_Down_Find,   
                     Left_Up_Find-2, left_edge_line[Left_Up_Find-2]);           // 左边界拟合
//          ips200_show_string(0,300,"cross1");
        //    printf("cross1");
      }
      else if(Left_Down_Find == 0 && Right_Down_Find != 0) {
          // 情况2：仅右下点有效 → 右边界拟合+左边界延长
          add_Rline_k(right_edge_line[Right_Down_Find], Right_Down_Find,        // 右边界拟合
                     Right_Up_Find, right_edge_line[Right_Up_Find]);
          lenthen_Left_bondarise(Left_Up_Find);                       //
//          ips200_show_string(0,300,"cross2");
      }
      else if(Left_Down_Find != 0 && Right_Down_Find == 0) {
          // 情况3：仅左下点有效 → 左边界拟合+右边界延长
          lenthen_Right_bondarise(Right_Up_Find);
          add_Lline_k(left_edge_line[Left_Down_Find], Left_Down_Find, 
                     Left_Up_Find, left_edge_line[Left_Up_Find]);
        //    printf("cross3");
//            ips200_show_string(0,300,"cross3");
      }
      else {
          // 情况4：无有效下点 → 双边界延长
          lenthen_Right_bondarise(Right_Up_Find);
          lenthen_Left_bondarise(Left_Up_Find);
        //    printf("cross4");
//        ips200_show_string(0,300,"cross4");
      }

      // 异常处理：突变点失效时恢复原始边界
      if(Right_Up_Find == 0) memcpy(rightfollowline, right_edge_line, sizeof(right_edge_line));
      if(Left_Up_Find == 0) memcpy(leftfollowline, left_edge_line, sizeof(left_edge_line));
//      get_center_line();
      if(Right_Up_Find==0||Left_Up_Find==0)
          {
          carstatus_now = straight;
          return;
      }
      
  }
	if(carstatus_now == crossroadL) {


    if(Left_Up_Find&&Right_Up_Find)                                     //如果左上点和右上点都有效
    {
        carstatus_now = crossroad;                                      // 进入十字路口状态
        return;
    }
//		ips200_show_string(0,300,"crossL");
    if(Left_Up_Find!=0&&Left_Down_Find!=0)                              //如果找到了左上点和左下点
    {
        add_Lline_k(left_edge_line[Left_Down_Find], Left_Down_Find,   
        Left_Up_Find, left_edge_line[Left_Up_Find]);           
//        get_center_line();
    }
    if(Left_Up_Find!=0&&Left_Down_Find==0)                              //如果只找到了左上点
    {
        lenthen_Left_bondarise(Left_Up_Find);                           //延长左边界
//        get_center_line();

    }
    if(Left_Up_Find==0)
    {
        carstatus_now = straight;                                      // 如果左上点未找到，返回直道状态
    }
    
  }
  if(carstatus_now == crossroadR)  {


    if(Left_Up_Find&&Right_Up_Find)                                     //如果左上点和右上点都有效
    {
        carstatus_now = crossroad;                                      // 进入十字路口状态
        return;
    }
//		ips200_show_string(0,300,"crossR");
    if(Right_Up_Find!=0&&Right_Down_Find!=0)                            //如果找到了右上点和右下点
    {
        add_Rline_k(right_edge_line[Right_Down_Find], Right_Down_Find, 
        Right_Up_Find, right_edge_line[Right_Up_Find]);
//        get_center_line();
    }
    if(Right_Up_Find!=0&&Right_Down_Find==0)                            //如果只找到了右上点           ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！左转弯会有这个问题
    {
        lenthen_Right_bondarise(Right_Up_Find);                         //延长右边界
//        get_center_line();

    }
    if(Right_Up_Find==0)
    {
        carstatus_now = straight;                                      // 如果右上点未找到，返回直道状态
    }
  }
}