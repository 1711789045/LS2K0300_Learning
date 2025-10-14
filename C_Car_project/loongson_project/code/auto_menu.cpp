#include "zf_common_headfile.h"
#include "auto_menu.h"
#include "key.h"
#include "servo.h"
#include "control.h"
#include "config_flash.h"  // 包含配置库头文件

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

void dad_name_init(){
	char* p = NULL;
	p = (char*)malloc(STR_LEN_MAX);
	memset(p,0,STR_LEN_MAX);
	strcpy(p,"Page ");
	DAD_NUM = IND1+1;
	for(uint8 i=0;i<DAD_NUM;i++){
		if(i<10){
			p[5] = '0'+ i;
			strcpy(P_dad_head->name,p);
		}else{
			p[5] = '0'+ i/10;
			p[6] = '0'+ i%10;
			strcpy(P_dad_head->name,p);
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
//-------------------------------------------------------------------------------------------------------------------
void unit_param_set(void* p_param,type_value t,float delta,uint8 num,uint8 point_num,unit_type t1,const char _name[STR_LEN_MAX]){
	static menu_unit *p_middle = NULL;
	menu_unit *p1 = NULL,*p2 = NULL;
	param_set *p1_par = NULL,*p2_par = NULL;
	static menu_unit *dad;
	
#ifdef USE_STATIC_MENU
	p1 = my_menu_unit+static_cnt;
	p1_par = my_param_set+static_cnt;
    static_cnt++;
	p2 = my_menu_unit+static_cnt;
	p2_par = my_param_set+static_cnt;
    static_cnt++;
#else
	p1 = malloc(sizeof(menu_unit));
	p1_par = malloc(sizeof(param_set));

	p2 = malloc(sizeof(menu_unit));
	p2_par = malloc(sizeof(param_set));
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
		unit_index_init(p2,0,1);
		dad_link(dad,dad,p1);		
		son_link(p1,p2,dad);
		son_link(p2,p1,dad);
	}else{
		if(IND2>=SON_NUM-2){
#ifdef USE_STATIC_MENU
            dad = my_menu_unit+static_cnt;
            static_cnt++;
#else
			dad = malloc(sizeof(menu_unit));
#endif
			unit_default(dad,IND1+1);
			unit_index_init(p1,IND1+1,0);
			unit_index_init(p2,IND1+1,1);
			dad_link(P_dad_head->down,dad,NULL);		
			dad_link(dad,P_dad_head,p1);		
			son_link(p1,p2,dad);
			son_link(p2,p1,dad);
		}else{
			unit_index_init(p1,IND1,IND2+1);
			unit_index_init(p2,IND1,IND2+2);		
			son_link(p_middle,p1,dad);
			son_link(p1,p2,dad);
			son_link(p2,SON_BEGIN_UNIT,dad);
		}
	}
	p_middle = p2;
	p1->par_set = p1_par;
	p1->par_set->p_par=p_param;
	p1->par_set->par_type=t;
	p1->par_set->delta=delta;
	p1->par_set->num=num;
	p1->par_set->point_num=point_num;
	p1->type_t=t1;
	memset(p1->name,0,STR_LEN_MAX);
	strcpy(p1->name, _name);
	p1->name[strlen(_name)]='+';

	p2->par_set = p2_par;
	p2->par_set->p_par=p_param;
	p2->par_set->par_type=t;
	p2->par_set->delta=-delta;
	p2->par_set->num=num;
	p2->par_set->point_num=point_num;
	p2->type_t=t1;
	memset(p2->name,0,STR_LEN_MAX);
	strcpy(p2->name, _name);
	p2->name[strlen(_name)]='-';

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
		menu_unit* p = NULL;
		p = p_unit;
		for(uint8 i=0;i<DAD_NUM;i++){
			showstr(DAD_INDEX(p->m_index[0],0)+MOUSE_DIS,DAD_INDEX(p->m_index[0],1),p->name);
			p = p->up;
		}
		begin_menu_flag = 0;
	}
}

void assist_menu()
{
	uint8 index = p_unit->m_index[1];
	if(first_in_page_flag)
		showstr(SON_INDEX(index,0),SON_INDEX(index,1),MOUSE_LOOK);
	
	if(button3||button4){
		if(index==0){
			showstr(SON_INDEX(p_unit->down->m_index[1],0),SON_INDEX(p_unit->down->m_index[1],1)," ");
			showstr(SON_INDEX(0,0) ,SON_INDEX(0,1),MOUSE_LOOK);
			showstr(SON_INDEX(1,0) ,SON_INDEX(1,1)," ");
		}else if(index==p_unit->back->enter->down->m_index[1]){
			showstr(SON_INDEX(index-1,0),SON_INDEX(index-1,1)," ");
			showstr(SON_INDEX(index,0),SON_INDEX(index,1),MOUSE_LOOK);
			showstr(SON_INDEX(0,0) ,SON_INDEX(0,1) ," ");			
		}else{
			showstr(SON_INDEX(index-1,0),SON_INDEX(index-1,1)," ");
			showstr(SON_INDEX(index  ,0),SON_INDEX(index  ,1),MOUSE_LOOK);
			showstr(SON_INDEX(index+1,0),SON_INDEX(index+1,1)," ");					
		}
	}else if(is_clear_flag==1&&(button2)){
		menu_unit* p = NULL;
		p = p_unit;
		for(uint8 i=0;i<SON_NUM;i++){
			showstr(SON_INDEX(p->m_index[1],0)+MOUSE_DIS,SON_INDEX(p->m_index[1],1),p->name);
			p = p->up;
		}
	}				 
}
	
//-------------------------------------------------------------------------------------------------------------------
// @return	void
//-------------------------------------------------------------------------------------------------------------------
void change_value(param_set* param)
{
    uint8 type=param->par_type;float delta_x=param->delta;void* value=param->p_par;
    uint8 num=param->num;uint8 point_num=param->point_num;
	//static uint8 last_index = 0;
	uint8 is_show_num = (p_unit_last->par_set==NULL?1:p_unit_last->par_set->p_par!=p_unit->par_set->p_par);
	if(p_unit->par_set->p_par!=NULL){
		if(type==TYPE_FLOAT){
			float *p_value;
			p_value = (float*)(value);
			if(IS_OK)
			{
				*p_value +=	delta_x;
				showfloat(0,(SON_NUM+1)*DIS_Y,*p_value,num,point_num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"            ");
				showfloat(0,(SON_NUM+1)*DIS_Y,*p_value,num,point_num);
			}
		}else if(type==TYPE_DOUBLE){
			double *p_value;
			p_value = (double*)(value);
			if(IS_OK)
			{
				*p_value +=	(double)delta_x;
				showfloat(0,(SON_NUM+1)*DIS_Y,*p_value,num,point_num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"            ");
				showfloat(0,(SON_NUM+1)*DIS_Y,*p_value,num,point_num);
			}
		}else if(type==TYPE_INT){
			int *p_value;
			p_value = (int*)(value);
			if(IS_OK)
			{
				*p_value +=	(int)delta_x;
				showint32(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"        ");
				showint32(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}			
		}else if(type==TYPE_UINT16){
			uint16 *p_value;
			p_value = (uint16*)(value);
			if(IS_OK)
			{
				*p_value +=	(int)delta_x;
				showuint16(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"         ");
				showuint16(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}			
		}else if(type==TYPE_UINT32){
			uint32 *p_value;
			p_value = (uint32*)(value);
			if(IS_OK)
			{
				*p_value +=	(int)delta_x;
				showuint32(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}
			if(is_show_num){
				showstr(0,(SON_NUM+1)*DIS_Y,"         ");
				showuint32(0,(SON_NUM+1)*DIS_Y,*p_value,num);
			}			
		}
	}
	//last_index = p_unit->m_index[1];	
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
void show_process(void *parameter)
{
    #ifdef  MENU_USE_RTT
    while(1)
	{	
		button1=(RT_EOK==rt_sem_take(key1_sem,RT_WAITING_NO));
		button2=(RT_EOK==rt_sem_take(key2_sem,RT_WAITING_NO));
		button3=(RT_EOK==rt_sem_take(key3_sem,RT_WAITING_NO));
		button4=(RT_EOK==rt_sem_take(key4_sem,RT_WAITING_NO));

		is_clear_flag = is_menu_clear();
		
		if(button1||button2||button3||button4){
			rt_sem_release(button_feedback_sem);
		}
			
		if(is_clear_flag)
			clear_hhh(0,0,SCREEN_W,SON_NUM*16,IPS200_BGCOLOR);

		if			(button1==1)
			p_unit=p_unit->back;
		else	if(button2==1)
			p_unit=p_unit->enter;
		else	if(button3==1)
			p_unit=p_unit->up;
		else	if(button4==1)
			p_unit=p_unit->down;
	
		is_first_in_page();
		
		show_menu();
		
		fun_menu();
		
	    p_unit_last=p_unit;
		rt_thread_mdelay(10);
	}
    #else
    button_entry(NULL);

    is_clear_flag = is_menu_clear();

    if(is_clear_flag)
        clear();

    if        (button1==1)
        p_unit=p_unit->back;
    else    if(button2==1)
        p_unit=p_unit->enter;
    else    if(button3==1)
        p_unit=p_unit->up;
    else    if(button4==1)
        p_unit=p_unit->down;

    is_first_in_page();

    show_menu();

    fun_menu();

    p_unit_last=p_unit;
    #endif
}

void menu_init()
{
    screen_init("/dev/fb0");

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

void UNIT_SET(){
    // 速度控制参数
    unit_param_set(&speed, TYPE_INT, 100, 6, 0, NORMAL_PAR, "speed");

    // 舵机PID参数（现在从 config_flash.cpp 中声明）
    unit_param_set(&servo_pid_kp,  TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_kp");
    unit_param_set(&servo_pid_ki,  TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_ki");
    unit_param_set(&servo_pid_kd1, TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_kd1");
    unit_param_set(&servo_pid_kd2, TYPE_FLOAT, 0.01, 3, 3, NORMAL_PAR, "servo_kd2");
}

void FUN_INIT(){
	fun_init(car_start, "START");         // 启动小车
	fun_init(servo_manual_adjust, "SERVO_ADJ");  // 舵机手动调整
	fun_init(image_display, "IMG_VIEW");  // 实时图像显示
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

        // 清屏
        ips200_full(IPS200_DEFAULT_BGCOLOR);

        // 进入实时显示主循环
        while(1)
        {
            // 更新按键状态(必须在每次循环开始)
            button_entry(NULL);

            // 检测返回键(button1): 退出到子菜单
            if(button1)
            {
                printf("Exiting image display mode\r\n");
                // 清屏并退出
                ips200_full(IPS200_DEFAULT_BGCOLOR);
                break;
            }

            // 调用图像处理并显示(mode=1 表示显示边线)
            image_process(IMAGE_W, IMAGE_H, 1);

            // 主循环延迟,与按键处理一致
            system_delay_ms(20);
        }
    }
}