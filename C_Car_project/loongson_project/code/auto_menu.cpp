#include "zf_common_headfile.h"
#include "auto_menu.h"
#include "key.h"
#include "servo.h"
#include "control.h"
#include "motor.h"          // 包含电机头文件（用于差速参数）
#include "config_flash.h"   // 包含配置库头文件
#include "image.h"          // 包含图像处理头文件（用于动态前瞻参数）

#ifdef  MENU_USE_RTT
extern rt_sem_t key1_sem;
extern rt_sem_t key2_sem;
extern rt_sem_t key3_sem;
extern rt_sem_t key4_sem;
extern rt_sem_t button_feedback_sem;
#endif

// Button definitions
uint8 button1=0,button2=0,button3=0,button4=0;
uint8 first_in_page_flag = 0;
uint8 is_clear_flag=0;

// 编辑模式相关变量
uint8 edit_mode = 0;        // 0=普通模式，1=编辑模式
menu_unit* edit_unit = NULL;
uint16 key_press_time = 0;  // 长按计数器

// 页面名称数组（用于自定义分页）
static const char** custom_page_names = NULL;
static uint8 custom_page_count = 0;

uint8* p_index_xy_dad,*p_index_xy_son;

static menu_unit *p_unit	 	=NULL;
static menu_unit *p_unit_last 	=NULL;

static menu_unit *P_dad_head 	= NULL;

uint8 DAD_NUM=1;

#ifdef USE_STATIC_MENU
menu_unit   my_menu_unit[MEM_SIZE];
param_set   my_param_set[MEM_SIZE];
uint8       my_index[MEM_SIZE*2];
static int  static_cnt=0;
#endif

void (*current_operation_menu)(void);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      设置自定义页面名称数组
//  @param      names   页面名称数组指针
//  @param      count   页面数量
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void set_page_names(const char** names, uint8 count){
	custom_page_names = names;
	custom_page_count = count;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      强制创建新页面（用于手动分页）
//  @param      page_name   页面名称
//  @return     void
//  @note       在 UNIT_SET() 中调用，用于按功能分类参数
//-------------------------------------------------------------------------------------------------------------------
void force_new_page(const char* page_name){
	// 如果已经有页面存在，强制创建新页
	if(P_dad_head != NULL){
		// 通过设置 IND2 为 SON_NUM-1 触发自动创建新页逻辑
		// 下一个 unit_param_set 或 fun_init 调用会自动创建新页
		menu_unit* current_son_end = SON_END_UNIT;
		current_son_end->m_index[1] = SON_NUM - 1;
	}

	// 保存页面名称（将在 dad_name_init 中使用）
	// 注意：此函数仅标记需要创建新页，实际创建在下一次 unit_param_set/fun_init 调用时
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化父菜单（页面）名称
//  @return     void
//  @note       修改版本：支持自定义页面名称
//-------------------------------------------------------------------------------------------------------------------
void dad_name_init(){
	char* p = NULL;
	p = (char*)malloc(STR_LEN_MAX);
	memset(p,0,STR_LEN_MAX);

	DAD_NUM = IND1+1;

	for(uint8 i=0;i<DAD_NUM;i++){
		// 如果设置了自定义名称数组，使用自定义名称
		if(custom_page_names != NULL && i < custom_page_count){
			strcpy(P_dad_head->name, custom_page_names[i]);
		}else{
			// 否则使用默认的 "Page X" 格式
			strcpy(p,"Page ");
			if(i<10){
				p[5] = '0'+ i;
				strcpy(P_dad_head->name,p);
			}else{
				p[5] = '0'+ i/10;
				p[6] = '0'+ i%10;
				strcpy(P_dad_head->name,p);
			}
		}
		P_dad_head = P_dad_head->up;
	}
	free(p);
}

void son_link(menu_unit *p_1,menu_unit *p_2,menu_unit *dad)
{
		p_1->up 	=   p_2;
		p_2->down  	=   p_1;
		p_1->enter	=	p_1;
		p_2->enter	=	p_2;
		p_1->back	=	dad;
		p_2->back	=	dad;
}

void dad_link(menu_unit *p_1,menu_unit *p_2,menu_unit *son)
{	
	p_1-> up	= p_2;
	p_2-> down  = p_1;
	p_1-> back	= p_1;
	p_2-> back	= p_2;
	if(son!=NULL)
		p_1->enter	=	son;
}
void unit_default(menu_unit *p,uint8 ind)
{
	p->type_t = USE_FUN;
	p->current_operation = NULL_FUN;
	p->m_index[0]=ind;
	p->m_index[1]=255;
}	
void unit_index_init(menu_unit *_p1,uint8 ind_0,uint8 ind_1){
	_p1->m_index[0]=ind_0;
	_p1->m_index[1]=ind_1;	
}
//-------------------------------------------------------------------------------------------------------------------
//  @return     void
//  Sample usage:               			unit_param_set(&param_test,TYPE_FLOAT,0.001,1,4,"par_test");
//  @note       修改版本：只创建一个菜单项（不再创建+/-两行）
//-------------------------------------------------------------------------------------------------------------------
void unit_param_set(void* p_param,type_value t,float delta,uint8 num,uint8 point_num,unit_type t1,const char _name[STR_LEN_MAX]){
	static menu_unit *p_middle = NULL;
	menu_unit *p1 = NULL;
	param_set *p1_par = NULL;
	static menu_unit *dad;

#ifdef USE_STATIC_MENU
	p1 = my_menu_unit+static_cnt;
	p1_par = my_param_set+static_cnt;
    static_cnt++;
#else
	p1 = malloc(sizeof(menu_unit));
	p1_par = malloc(sizeof(param_set));
#endif

	if(P_dad_head==NULL){
#ifdef USE_STATIC_MENU
	    dad = my_menu_unit+static_cnt;
	    static_cnt++;
#else
		dad = malloc(sizeof(menu_unit));
#endif
		P_dad_head = dad;
		unit_default(dad,0);
		p_unit = dad;
		p_unit_last = NULL;
		unit_index_init(p1,0,0);
		dad_link(dad,dad,p1);
		son_link(p1,p1,dad);  // 单个参数自己连接自己
	}else{
		if(IND2>=SON_NUM-1){  // 修改为 SON_NUM-1（8个参数满了才创建新页）
#ifdef USE_STATIC_MENU
            dad = my_menu_unit+static_cnt;
            static_cnt++;
#else
			dad = malloc(sizeof(menu_unit));
#endif
			unit_default(dad,IND1+1);
			unit_index_init(p1,IND1+1,0);
			dad_link(P_dad_head->down,dad,NULL);
			dad_link(dad,P_dad_head,p1);
			son_link(p1,p1,dad);  // 单个参数自己连接自己
		}else{
			unit_index_init(p1,IND1,IND2+1);
			son_link(p_middle,p1,dad);
			son_link(p1,SON_BEGIN_UNIT,dad);
		}
	}
	p_middle = p1;  // 修改为 p1
	p1->par_set = p1_par;
	p1->par_set->p_par=p_param;
	p1->par_set->par_type=t;
	p1->par_set->delta=delta;
	p1->par_set->num=num;
	p1->par_set->point_num=point_num;
	p1->type_t=t1;
	memset(p1->name,0,STR_LEN_MAX);
	strcpy(p1->name, _name);
	// 不再添加 '+' 后缀
}

//-------------------------------------------------------------------------------------------------------------------
//  @return     void
//  Sample usage:               		fun_init(Flash_Read,"flash_read");
//-------------------------------------------------------------------------------------------------------------------
void fun_init(void (*fun)(),const char* _name)
{
	menu_unit *p2 = NULL;
	menu_unit *p1 = NULL;
	
#ifdef USE_STATIC_MENU
    p1 = my_menu_unit+static_cnt;
	static_cnt++;
#else
	p1 = malloc(sizeof(menu_unit));
#endif
	p1->par_set = NULL;
	
	if(IND2>=SON_NUM-1){
#ifdef USE_STATIC_MENU
	    p2 = my_menu_unit+static_cnt;
	    static_cnt++;
#else
		p2 = malloc(sizeof(menu_unit));
#endif

		unit_default(p2,IND1+1);
		unit_index_init(p1,IND1+1,0);
		dad_link(P_dad_head->down,p2,NULL);	
		dad_link(p2,P_dad_head,p1);
		son_link(p1,p1,p2);
	}
	else{
		unit_index_init(p1,IND1,IND2+1);
		son_link(SON_END_UNIT,p1,P_dad_head->down);
		son_link(p1,SON_BEGIN_UNIT,P_dad_head->down);
	}
	
	p1->current_operation = fun;

	for(uint8 i=0;i<STR_LEN_MAX;i++)p1->name[i]=0;
	strcpy(p1->name, _name);
	p1->type_t = USE_FUN;
}

void index_xy_init()
{
#ifdef USE_STATIC_MENU
    p_index_xy_dad = my_index;
    p_index_xy_son = my_index+DAD_NUM*2;
#else
	p_index_xy_dad =(uint8*)malloc(sizeof(uint8)*DAD_NUM*2);
	p_index_xy_son =(uint8*)malloc(sizeof(uint8)*SON_NUM*2);
#endif
	uint8 half;
	half = (DAD_NUM+1)/2;
	for(uint8 i=0;i<half;i++){
		DAD_INDEX(i,0)=DIS_X*0;
		DAD_INDEX(i,1)=DIS_Y*i;
	}
	for(uint8 i=half;i<DAD_NUM;i++){
		DAD_INDEX(i,0)=DIS_X*1;
		DAD_INDEX(i,1)=DIS_Y*(i-half);
	}
	for(uint8 i=0;i<SON_NUM;i++){
		SON_INDEX(i,0)=DIS_X*0;
		SON_INDEX(i,1)=DIS_Y*i;
	}
}


void flash_init_wz()
{
	#if AUTO_READ
	menu_unit* p;
	p = P_dad_head->enter;
	while(1){
		if(p->m_index[1]<p->up->m_index[1]){
			flash_unit_init(p->par_set.p_par,p->par_set.par_type);
			p = p->up;
		}else{
			flash_unit_init(p->par_set.p_par,p->par_set.par_type);
			p = p->back->up->enter;
		}
		if(p->par_set.type_t==USE_FUN)
			break;
	}
	#endif
	#if AUTO_READ
	flash_index_init();
	flash_read();
	#endif
}

uint8 is_menu_clear()
{
	return (((p_unit->back)!=(p_unit))&&button1)||(((p_unit->enter)!=(p_unit))&&button2);
}
uint8 begin_menu_flag=1;
void center_menu()
{
	uint8 index = p_unit->m_index[0];

	if(first_in_page_flag)
		showstr(DAD_INDEX(index,0),DAD_INDEX(index,1),MOUSE_LOOK);

	if(button3||button4){
		if(index==0){
			showstr(DAD_INDEX(DAD_NUM-1,0),DAD_INDEX(DAD_NUM-1,1)," ");
			showstr(DAD_INDEX(0,0) ,DAD_INDEX(0,1),MOUSE_LOOK);
			showstr(DAD_INDEX(1,0) ,DAD_INDEX(1,1)," ");
		}else if(index==DAD_NUM-1){
			showstr(DAD_INDEX(DAD_NUM-2,0),DAD_INDEX(DAD_NUM-2,1)," ");
			showstr(DAD_INDEX(DAD_NUM-1,0),DAD_INDEX(DAD_NUM-1,1),MOUSE_LOOK);
			showstr(DAD_INDEX(0,0) ,DAD_INDEX(0,1) ," ");
		}else{
			showstr(DAD_INDEX(index-1,0),DAD_INDEX(index-1,1)," ");
			showstr(DAD_INDEX(index  ,0),DAD_INDEX(index  ,1),MOUSE_LOOK);
			showstr(DAD_INDEX(index+1,0),DAD_INDEX(index+1,1)," ");
		}
	}else if((is_clear_flag==1&&(button1))||(begin_menu_flag)){
		// 首次显示或返回父菜单时，确保整个屏幕背景为黑色
		ips200_full(RGB565_BLACK);
		ips200_set_color(RGB565_WHITE, RGB565_BLACK);

		menu_unit* p = NULL;
		p = p_unit;
		for(uint8 i=0;i<DAD_NUM;i++){
			showstr(DAD_INDEX(p->m_index[0],0)+MOUSE_DIS,DAD_INDEX(p->m_index[0],1),p->name);
			p = p->up;
		}
		// 显示当前选中项的光标
		showstr(DAD_INDEX(index,0),DAD_INDEX(index,1),MOUSE_LOOK);
		begin_menu_flag = 0;
	}
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取参数在当前页的实际显示位置（从0开始连续编号）
//  @param      target  目标参数指针
//  @return     实际显示位置（0-7）
//-------------------------------------------------------------------------------------------------------------------
uint8 get_param_display_position(menu_unit* target)
{
	menu_unit* p = target;

	// 先找到索引为0的参数（页面起始参数）
	while(p->m_index[1] != 0){
		p = p->down;
	}

	// 从索引0开始计数，找到 target 的位置
	uint8 position = 0;
	menu_unit* start = p;
	do {
		if(p == target){
			return position;  // 找到目标，返回位置
		}
		p = p->up;
		position++;
	} while(p != start && position < SON_NUM);

	return 0;  // 默认返回0（理论上不会到这里）
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      刷新整页参数显示（用于进入页面和退出编辑模式）
//  @param      selected_index  当前选中参数的索引（用绿色高亮）
//  @return     void
//-------------------------------------------------------------------------------------------------------------------
void refresh_all_params(uint8 selected_index)
{
	// 先全屏填充黑色（确保整个屏幕背景为黑色）
	ips200_full(RGB565_BLACK);
	ips200_set_color(RGB565_WHITE, RGB565_BLACK);

	menu_unit* p = p_unit;

	// 先找到索引为0的参数（页面起始参数）
	while(p->m_index[1] != 0){
		p = p->down;
	}

	// 从索引0开始，依次显示每个参数
	uint8 param_count = 0;
	menu_unit* start = p;
	do {
		uint16 y_pos = SON_INDEX(param_count, 1);  // 使用连续的Y坐标

		// 如果是当前选中的参数，参数名用绿色显示
		if(p->m_index[1] == selected_index){
			ips200_set_color(RGB565_GREEN, IPS200_BGCOLOR);
		}else{
			ips200_set_color(IPS200_DEFAULT_PENCOLOR, IPS200_BGCOLOR);
		}

		// 显示参数名（左侧，占100像素）
		showstr(SON_INDEX(param_count, 0), y_pos, p->name);

		// 恢复默认颜色
		ips200_set_color(IPS200_DEFAULT_PENCOLOR, IPS200_BGCOLOR);

		// 显示参数值（右侧，从 x=105 开始）
		if(p->par_set != NULL && p->par_set->p_par != NULL){
			uint8 type = p->par_set->par_type;
			void* value = p->par_set->p_par;
			uint8 num = p->par_set->num;
			uint8 point_num = p->par_set->point_num;

			if(type==TYPE_FLOAT){
				float *p_value = (float*)(value);
				showfloat(105, y_pos, *p_value, num, point_num);
			}else if(type==TYPE_DOUBLE){
				double *p_value = (double*)(value);
				showfloat(105, y_pos, *p_value, num, point_num);
			}else if(type==TYPE_INT){
				int *p_value = (int*)(value);
				showint32(105, y_pos, *p_value, num);
			}else if(type==TYPE_UINT16){
				uint16 *p_value = (uint16*)(value);
				showuint16(105, y_pos, *p_value, num);
			}else if(type==TYPE_UINT32){
				uint32 *p_value = (uint32*)(value);
				showuint32(105, y_pos, *p_value, num);
			}
		}

		p = p->up;
		param_count++;

		// 如果回到起始参数或达到最大参数数，退出
	} while(p != start && param_count < SON_NUM);

	// 清空剩余行（如果参数不足8个）
	for(uint8 i = param_count; i < SON_NUM; i++){
		uint16 y_pos = SON_INDEX(i, 1);
		showstr(0, y_pos, "                ");  // 清空整行
	}
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      子菜单显示函数（参数列表）
//  @return     void
//  @note       修改版本：一行显示参数名+值，选中的参数名用绿色高亮
//-------------------------------------------------------------------------------------------------------------------
void assist_menu()
{
	uint8 index = p_unit->m_index[1];

	// 进入新页面时，全屏刷新显示所有参数
	if(is_clear_flag==1&&(button2)){
		refresh_all_params(index);
	}
	// 切换参数时，只刷新相关行的参数名颜色（编辑模式下不处理，保持黄色）
	else if((button3||button4) && edit_mode == 0){
		menu_unit* p_old = (button3 ? p_unit->down : p_unit->up);

		// 获取实际显示位置（连续的0-7位置）
		uint8 old_pos = get_param_display_position(p_old);
		uint8 cur_pos = get_param_display_position(p_unit);

		// 恢复上一个参数名的颜色为默认白色
		ips200_set_color(IPS200_DEFAULT_PENCOLOR, IPS200_BGCOLOR);
		showstr(SON_INDEX(old_pos,0), SON_INDEX(old_pos,1), p_old->name);

		// 当前参数名改为绿色
		ips200_set_color(RGB565_GREEN, IPS200_BGCOLOR);
		showstr(SON_INDEX(cur_pos,0), SON_INDEX(cur_pos,1), p_unit->name);

		// 恢复默认颜色
		ips200_set_color(IPS200_DEFAULT_PENCOLOR, IPS200_BGCOLOR);
	}
	// 首次进入该参数时，高亮显示
	else if(first_in_page_flag){
		uint8 cur_pos = get_param_display_position(p_unit);
		ips200_set_color(RGB565_GREEN, IPS200_BGCOLOR);
		showstr(SON_INDEX(cur_pos,0), SON_INDEX(cur_pos,1), p_unit->name);
		ips200_set_color(IPS200_DEFAULT_PENCOLOR, IPS200_BGCOLOR);
	}
}
	
//-------------------------------------------------------------------------------------------------------------------
// @brief      修改参数值函数
// @return     void
// @note       修改版本：只在编辑模式下响应按键修改参数，实时刷新参数值显示
//-------------------------------------------------------------------------------------------------------------------
void change_value(param_set* param)
{
	// 只有在编辑模式下才允许修改参数值
	if(edit_mode == 0 || p_unit->par_set->p_par == NULL){
		return;
	}

	uint8 type = param->par_type;
	float delta_x = param->delta;
	void* value = param->p_par;
	uint8 num = param->num;
	uint8 point_num = param->point_num;

	// 获取实际显示位置（连续的0-7位置）
	uint8 cur_pos = get_param_display_position(p_unit);
	uint16 y_pos = SON_INDEX(cur_pos, 1);

	// 长按加速：如果按键持续时间超过阈值，增大步进
	float speed_multiplier = 1.0f;
	if(key_press_time > 20){  // 按下超过200ms（20*10ms）
		speed_multiplier = 5.0f;  // 步进加速5倍
	}

	uint8 value_changed = 0;

	if(type==TYPE_FLOAT){
		float *p_value = (float*)(value);
		if(button3){  // UP键减少
			*p_value -= delta_x * speed_multiplier;
			value_changed = 1;
		}else if(button4){  // DOWN键增加
			*p_value += delta_x * speed_multiplier;
			value_changed = 1;
		}
		if(value_changed){
			// 清除旧值
			showstr(105, y_pos, "        ");
			// 显示新值
			showfloat(105, y_pos, *p_value, num, point_num);
			config_save();  // 自动保存
		}
	}else if(type==TYPE_DOUBLE){
		double *p_value = (double*)(value);
		if(button3){  // UP键减少
			*p_value -= (double)(delta_x * speed_multiplier);
			value_changed = 1;
		}else if(button4){  // DOWN键增加
			*p_value += (double)(delta_x * speed_multiplier);
			value_changed = 1;
		}
		if(value_changed){
			showstr(105, y_pos, "        ");
			showfloat(105, y_pos, *p_value, num, point_num);
			config_save();
		}
	}else if(type==TYPE_INT){
		int *p_value = (int*)(value);
		if(button3){  // UP键减少
			*p_value -= (int)(delta_x * speed_multiplier);
			value_changed = 1;
		}else if(button4){  // DOWN键增加
			*p_value += (int)(delta_x * speed_multiplier);
			value_changed = 1;
		}
		if(value_changed){
			showstr(105, y_pos, "        ");
			showint32(105, y_pos, *p_value, num);
			config_save();
		}
	}else if(type==TYPE_UINT16){
		uint16 *p_value = (uint16*)(value);
		if(button3){  // UP键减少
			if(*p_value >= (int)(delta_x * speed_multiplier)){  // 防止负溢出
				*p_value -= (int)(delta_x * speed_multiplier);
				value_changed = 1;
			}
		}else if(button4){  // DOWN键增加
			*p_value += (int)(delta_x * speed_multiplier);
			value_changed = 1;
		}
		if(value_changed){
			showstr(105, y_pos, "        ");
			showuint16(105, y_pos, *p_value, num);

			// 如果修改的是 mid_weight_select，调用切换函数
			if(p_value == &mid_weight_select){
				select_mid_weight(mid_weight_select);
			}
			config_save();
		}
	}else if(type==TYPE_UINT32){
		uint32 *p_value = (uint32*)(value);
		if(button3){  // UP键减少
			if(*p_value >= (int)(delta_x * speed_multiplier)){  // 防止负溢出
				*p_value -= (int)(delta_x * speed_multiplier);
				value_changed = 1;
			}
		}else if(button4){  // DOWN键增加
			*p_value += (int)(delta_x * speed_multiplier);
			value_changed = 1;
		}
		if(value_changed){
			showstr(105, y_pos, "        ");
			showuint32(105, y_pos, *p_value, num);
			config_save();
		}
	}
}

void is_first_in_page()
{
	first_in_page_flag = (p_unit_last!=p_unit)&&(button1||button2);
}

void show_menu()
{
	if(p_unit->m_index[1]==255)
		center_menu();
	else
		assist_menu();
}

void fun_menu()
{
	if(p_unit->type_t==NORMAL_PAR||p_unit->type_t==PID_PAR){
		change_value(p_unit->par_set);
	}else{
		current_operation_menu = p_unit->current_operation;
		(*current_operation_menu)();
	}
}
//-------------------------------------------------------------------------------------------------------------------
// @brief      菜单主处理函数
// @param      parameter   参数（RTT线程用）
// @return     void
// @note       修改版本：实现编辑模式逻辑
//              普通模式：button3/4切换参数，button2进入编辑，button1返回
//              编辑模式：button3/4修改参数值，button1退出编辑
//-------------------------------------------------------------------------------------------------------------------
void show_process(void *parameter)
{
    #ifdef  MENU_USE_RTT
    while(1)
	{
		button1=(RT_EOK==rt_sem_take(key1_sem,RT_WAITING_NO));
		button2=(RT_EOK==rt_sem_take(key2_sem,RT_WAITING_NO));
		button3=(RT_EOK==rt_sem_take(key3_sem,RT_WAITING_NO));
		button4=(RT_EOK==rt_sem_take(key4_sem,RT_WAITING_NO));

		// 长按计数器（用于加速修改参数）
		if(button3 || button4){
			key_press_time++;
		}else{
			key_press_time = 0;
		}

		is_clear_flag = is_menu_clear();

		if(button1||button2||button3||button4){
			rt_sem_release(button_feedback_sem);
		}

		if(is_clear_flag){
			// 清屏时全屏填充黑色（不仅仅是部分区域）
			ips200_full(RGB565_BLACK);
			ips200_set_color(RGB565_WHITE, RGB565_BLACK);
		}

		// ========== 编辑模式按键处理 ==========
		if(edit_mode == 1){
			// 编辑模式：只响应 button1(退出) 和 button3/4(修改值)
			if(button1){  // 退出编辑模式
				edit_mode = 0;
				edit_unit = NULL;
				// 刷新整页参数显示（解决退出编辑模式后只显示一个参数名的bug）
				uint8 index = p_unit->m_index[1];
				refresh_all_params(index);
				button1 = 0;  // 消耗掉 button1，防止触发返回父菜单
			}
			// button3/4 在 change_value() 中处理
		}
		// ========== 普通模式按键处理 ==========
		else{
			// 在父菜单层（页面选择）
			if(p_unit->m_index[1] == 255){
				if(button1==1)
					p_unit=p_unit->back;
				else if(button2==1)
					p_unit=p_unit->enter;
				else if(button3==1)
					p_unit=p_unit->up;
				else if(button4==1)
					p_unit=p_unit->down;
			}
			// 在子菜单层（参数列表）
			else{
				// 如果是参数项
				if(p_unit->type_t==NORMAL_PAR || p_unit->type_t==PID_PAR){
					if(button1==1){
						p_unit=p_unit->back;  // 返回父菜单
					}else if(button2==1 && !first_in_page_flag){
						// 进入编辑模式
						edit_mode = 1;
						edit_unit = p_unit;
						// 参数名变为黄色表示进入编辑模式
						uint8 cur_pos = get_param_display_position(p_unit);
						ips200_set_color(RGB565_YELLOW, IPS200_BGCOLOR);
						showstr(SON_INDEX(cur_pos,0), SON_INDEX(cur_pos,1), p_unit->name);
						ips200_set_color(IPS200_DEFAULT_PENCOLOR, IPS200_BGCOLOR);
					}else if(button3==1){
						p_unit=p_unit->up;  // 上一个参数
					}else if(button4==1){
						p_unit=p_unit->down;  // 下一个参数
					}
				}
				// 如果是功能菜单项
				else{
					if(button1==1)
						p_unit=p_unit->back;
					else if(button2==1)
						p_unit=p_unit->enter;
					else if(button3==1)
						p_unit=p_unit->up;
					else if(button4==1)
						p_unit=p_unit->down;
				}
			}
		}

		is_first_in_page();

		show_menu();

		fun_menu();

	    p_unit_last=p_unit;
		rt_thread_mdelay(10);
	}
    #else
    button_entry(NULL);

	// 长按计数器
	if(button3 || button4){
		key_press_time++;
	}else{
		key_press_time = 0;
	}

    is_clear_flag = is_menu_clear();

    if(is_clear_flag){
        // 清屏时全屏填充黑色（不仅仅是部分区域）
        ips200_full(RGB565_BLACK);
        ips200_set_color(RGB565_WHITE, RGB565_BLACK);
    }

	// ========== 编辑模式按键处理 ==========
	if(edit_mode == 1){
		if(button1){
			edit_mode = 0;
			edit_unit = NULL;
			// 刷新整页参数显示（解决退出编辑模式后只显示一个参数名的bug）
			uint8 index = p_unit->m_index[1];
			refresh_all_params(index);
			button1 = 0;  // 消耗掉 button1，防止触发返回父菜单
		}
	}
	// ========== 普通模式按键处理 ==========
	else{
		if(p_unit->m_index[1] == 255){
			if(button1==1)
				p_unit=p_unit->back;
			else if(button2==1)
				p_unit=p_unit->enter;
			else if(button3==1)
				p_unit=p_unit->up;
			else if(button4==1)
				p_unit=p_unit->down;
		}else{
			if(p_unit->type_t==NORMAL_PAR || p_unit->type_t==PID_PAR){
				if(button1==1){
					p_unit=p_unit->back;
				}else if(button2==1 && !first_in_page_flag){
					edit_mode = 1;
					edit_unit = p_unit;
					uint8 cur_pos = get_param_display_position(p_unit);
					ips200_set_color(RGB565_YELLOW, IPS200_BGCOLOR);
					showstr(SON_INDEX(cur_pos,0), SON_INDEX(cur_pos,1), p_unit->name);
					ips200_set_color(IPS200_DEFAULT_PENCOLOR, IPS200_BGCOLOR);
				}else if(button3==1){
					p_unit=p_unit->up;
				}else if(button4==1){
					p_unit=p_unit->down;
				}
			}else{
				if(button1==1)
					p_unit=p_unit->back;
				else if(button2==1)
					p_unit=p_unit->enter;
				else if(button3==1)
					p_unit=p_unit->up;
				else if(button4==1)
					p_unit=p_unit->down;
			}
		}
	}

    is_first_in_page();

    show_menu();

    fun_menu();

    p_unit_last=p_unit;
    #endif
}

void menu_init()
{
    screen_init("/dev/fb0");

    // 初始化时将整个屏幕背景设为黑色（必须使用 RGB565_BLACK 而不是宏，确保生效）
    ips200_full(RGB565_BLACK);

    // 设置默认颜色为黑底白字
    ips200_set_color(RGB565_WHITE, RGB565_BLACK);

    key_into();

    UNIT_SET();

    FUN_INIT();

    dad_name_init();

    index_xy_init();

    #ifdef USE_FLASH
    flash_init_wz();
    #endif

    #ifdef  MENU_USE_RTT
    rt_thread_t tid;
    tid = rt_thread_create("display", show_process, RT_NULL, 1024*2, 11, 5);
    if(RT_NULL != tid)
    {
        rt_thread_startup(tid);
    }
    #endif
}

void NULL_FUN(){

}

//-------------------------------------------------------------------------------------------------------------------
// @brief      参数设置函数
// @return     void
// @note       修改版本：按功能分类参数到不同页面，每页最多8个参数
//-------------------------------------------------------------------------------------------------------------------
void UNIT_SET(){
	// 定义页面名称数组
	static const char* page_names[] = {
		"SERVO",     // 页面0: 舵机参数
		"MOTOR",     // 页面1: 电机参数
		"DIFFER",    // 页面2: 差速参数
		"IMAGE",     // 页面3: 图像参数
		"DYNAMIC",   // 页面4: 动态前瞻
		"FUNC"       // 页面5: 功能菜单
	};
	set_page_names(page_names, 6);

	// ==================== Page 0: SERVO (舵机参数) ====================
	force_new_page("SERVO");
	unit_param_set(&g_servo_mid,   TYPE_FLOAT, 0.1,  3, 2, NORMAL_PAR, "servo_mid");
	unit_param_set(&servo_pid_kp0, TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_kp0");
	unit_param_set(&servo_pid_kp2, TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_kp2");
	unit_param_set(&servo_pid_ki,  TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_ki");
	unit_param_set(&servo_pid_kd1, TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_kd1");
	unit_param_set(&servo_pid_kd2, TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_kd2");

	// ==================== Page 2: MOTOR (电机参数) ====================
	force_new_page("MOTOR");
	unit_param_set(&speed, TYPE_INT, 10, 5, 0, NORMAL_PAR, "speed");
	unit_param_set(&motor_pid_kp, TYPE_FLOAT, 0.5, 3, 2, NORMAL_PAR, "motor_kp");
	unit_param_set(&motor_pid_ki, TYPE_FLOAT, 0.5, 3, 2, NORMAL_PAR, "motor_ki");
	unit_param_set(&motor_pid_kd, TYPE_FLOAT, 0.5, 3, 2, NORMAL_PAR, "motor_kd");

	// ==================== Page 3: DIFFER (阿克曼差速参数) ====================
	force_new_page("DIFFER");
	unit_param_set(&differential_enable, TYPE_UINT16, 1, 1, 0, NORMAL_PAR, "dif_switch");
	unit_param_set(&inner_wheel_coef, TYPE_FLOAT, 0.05, 3, 2, NORMAL_PAR, "inner_coef");
	unit_param_set(&outer_wheel_coef, TYPE_FLOAT, 0.05, 3, 2, NORMAL_PAR, "outer_coef");

	// ==================== Page 4: IMAGE (图像处理参数) ====================
	force_new_page("IMAGE");
	unit_param_set(&mid_weight_select, TYPE_UINT16, 1, 1, 0, NORMAL_PAR, "mid_weight");
	unit_param_set(&cross_enable, TYPE_UINT16, 1, 1, 0, NORMAL_PAR, "cross_switch");

	// ==================== Page 5: DYNAMIC (动态前瞻权重参数) ====================
	force_new_page("DYNAMIC");
	unit_param_set(&dynamic_weight_enable, TYPE_UINT16, 1, 1, 0, NORMAL_PAR, "dyn_enable");
	unit_param_set(&curvature_threshold_low, TYPE_UINT16, 1, 2, 0, NORMAL_PAR, "curv_low");
	unit_param_set(&curvature_threshold_high, TYPE_UINT16, 1, 2, 0, NORMAL_PAR, "curv_high");
	unit_param_set(&weight_shift_speed, TYPE_UINT16, 1, 2, 0, NORMAL_PAR, "shift_spd");
	unit_param_set(&curvature_filter_ratio, TYPE_FLOAT, 0.05, 3, 2, NORMAL_PAR, "curv_filter");
}

//-------------------------------------------------------------------------------------------------------------------
// @brief      功能菜单初始化
// @return     void
// @note       所有功能菜单项都在 Page 6: FUNC
//-------------------------------------------------------------------------------------------------------------------
void FUN_INIT(){
	// ==================== Page 6: FUNC (功能菜单) ====================
	force_new_page("FUNC");
	fun_init(car_start, "START");              // 启动小车
	fun_init(servo_manual_adjust, "SERVO_ADJ");// 舵机手动调整
	fun_init(image_display, "IMG_VIEW");       // 实时图像显示
	fun_init(dynamic_weight_debug, "DYN_DEBUG");// 动态权重调试
	fun_init(config_save, "CFG_SAVE");         // 手动保存配置
	fun_init(NULL_FUN,   "NULL_FUN");
}

/**
 * @brief  实时图像显示函数(用于菜单调试)
 * @param  无
 * @return 无
 * @note   按确认键(IS_OK)进入实时图像显示模式
 *         显示摄像头图像和赛道识别边线
 *         按返回键(button1)退出到子菜单
 */
void image_display(void)
{
    if(IS_OK)  // 必须按下确认键才进入显示模式
    {
        printf("Entering image display mode...\r\n");

        // 进入实时显示主循环
        while(1)
        {
            // 更新按键状态(必须在每次循环开始)
            button_entry(NULL);

            // 检测返回键(button1): 退出到子菜单
            if(button1)
            {
                printf("Exiting image display mode\r\n");
                // 清屏并退出（使用黑色背景）
                ips200_full(RGB565_BLACK);
                ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                break;
            }

            // 调用图像处理并显示(mode=1 表示显示边线)
            image_process(IMAGE_W, IMAGE_H, 1);

            // 主循环延迟,与按键处理一致
            system_delay_ms(20);
        }
    }
}

/**
 * @brief  动态权重调试函数(用于菜单调试)
 * @param  无
 * @return 无
 * @note   按确认键(IS_OK)进入动态权重调试模式
 *         显示摄像头图像、边线识别、曲率值和所有动态权重参数
 *         按返回键(button1)退出到子菜单
 */
void dynamic_weight_debug(void)
{
    if(IS_OK)  // 必须按下确认键才进入调试模式
    {
        printf("Entering dynamic weight debug mode...\r\n");

        // 进入实时调试主循环
        while(1)
        {
            // 更新按键状态(必须在每次循环开始)
            button_entry(NULL);

            // 检测返回键(button1): 退出到子菜单
            if(button1)
            {
                printf("Exiting dynamic weight debug mode\r\n");
                // 清屏并退出（使用黑色背景）
                ips200_full(RGB565_BLACK);
                ips200_set_color(RGB565_WHITE, RGB565_BLACK);
                break;
            }

            // 调用图像处理并显示(mode=1 表示显示边线)
            image_process(IMAGE_W, IMAGE_H, 1);

            // ==================== 动态权重调试信息显示 ====================
            // 清除image_display_edge_line()函数显示的文字和下方调试区域
            // image_display_edge_line()会在图像右侧(X=185, Y=144/160/176)显示"DynWgt:ON"、"Curv:"等信息

            // 1. 清除图像右侧区域（X >= IMAGE_W，Y < IMAGE_H）
            for(uint16 y = 0; y < IMAGE_H; y++) {
                ips200_draw_line(IMAGE_W, y, 239, y, RGB565_BLACK);
            }

            // 2. 清除图像下方区域（Y >= IMAGE_H）
            for(uint16 y = IMAGE_H; y < 320; y++) {
                ips200_draw_line(0, y, 239, y, RGB565_BLACK);
            }

            // 显示在图像下方区域（图像高度180，屏幕高度320）
            // 屏幕宽度240像素，需要合理布局避免超出屏幕
            uint16 y_offset = 185;  // 显示起始Y坐标（图像下方）
            uint16 line_h = 16;     // 行高
            uint16 col1_x = 0;      // 第一列X坐标
            uint16 col2_x = 110;    // 第二列X坐标

            // 标题
            ips200_show_string(col1_x, y_offset, "Dynamic Weight");

            // 第一行：开关状态 | 当前状态
            ips200_show_string(col1_x, y_offset + line_h * 1, "Enab:");
            if(dynamic_weight_enable) {
                ips200_show_string(col1_x + 40, y_offset + line_h * 1, "ON ");
            } else {
                ips200_show_string(col1_x + 40, y_offset + line_h * 1, "OFF");
            }

            // 状态说明（第一行右侧）
            ips200_show_string(col2_x, y_offset + line_h * 1, "Mode:");
            if(dynamic_weight_enable) {
                if(dynamic_weight_status == 0) {
                    ips200_show_string(col2_x + 40, y_offset + line_h * 1, "FAR ");  // 远前瞻
                } else if(dynamic_weight_status == 1) {
                    ips200_show_string(col2_x + 40, y_offset + line_h * 1, "MID ");  // 中前瞻
                } else {
                    ips200_show_string(col2_x + 40, y_offset + line_h * 1, "NEAR");  // 近前瞻
                }
            } else {
                // 关闭时显示固定权重编号
                ips200_show_string(col2_x + 40, y_offset + line_h * 1, "FIX");
                ips200_show_int(col2_x + 72, y_offset + line_h * 1, mid_weight_select, 1);
            }

            // 第二行：原始曲率 | 滤波曲率
            ips200_show_string(col1_x, y_offset + line_h * 2, "RawC:");
            ips200_show_float(col1_x + 40, y_offset + line_h * 2, raw_curvature, 2, 1);

            ips200_show_string(col2_x, y_offset + line_h * 2, "Curv:");
            ips200_show_float(col2_x + 40, y_offset + line_h * 2, current_curvature, 2, 1);

            // 第三行：低阈值 | 高阈值
            ips200_show_string(col1_x, y_offset + line_h * 3, "ThrL:");
            ips200_show_int(col1_x + 40, y_offset + line_h * 3, curvature_threshold_low, 2);

            ips200_show_string(col2_x, y_offset + line_h * 3, "ThrH:");
            ips200_show_int(col2_x + 40, y_offset + line_h * 3, curvature_threshold_high, 2);

            // 第四行：切换速度 | 滤波系数
            ips200_show_string(col1_x, y_offset + line_h * 4, "Shft:");
            ips200_show_int(col1_x + 40, y_offset + line_h * 4, weight_shift_speed, 2);

            ips200_show_string(col2_x, y_offset + line_h * 4, "Filt:");
            ips200_show_float(col2_x + 40, y_offset + line_h * 4, curvature_filter_ratio, 1, 2);

            // 第五行：中线值 | 截止行
            ips200_show_string(col1_x, y_offset + line_h * 5, "MidL:");
            ips200_show_int(col1_x + 40, y_offset + line_h * 5, final_mid_line, 3);

            ips200_show_string(col2_x, y_offset + line_h * 5, "Stop:");
            ips200_show_int(col2_x + 40, y_offset + line_h * 5, stop_search_row, 3);

            // 提示信息（最后一行）
            ips200_show_string(col1_x, y_offset + line_h * 7, "KEY1:EXIT");

            // 主循环延迟,与按键处理一致
            system_delay_ms(10);
        }
    }
}