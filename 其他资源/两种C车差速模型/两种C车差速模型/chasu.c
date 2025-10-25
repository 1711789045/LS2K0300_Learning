#include "chasu.h"
#include "math.h"
//定义差速模型宏及变量
#define LDISTANCE 200 //车身的前后轮中心距 mm
#define BORDWIDTH 150 //两后轮中心距     mm
#define A 1.05 //差速的大小系数
#define K 1.06 //差速的预知系数  K越大差速越提前
#define AK 1   

//差速函数
#define steering_PWM sevro_ctl
#define steeringPWM_MID sevro_mid

float g_Control=0,g_Control_last=0,g_Control_pre=0,casu_weight=0.38;//1.29;//原0.6

extern uint16 sevro_mid;
extern uint16 sevro_ctl;


float k_white_40col=0;
float casu_weight_base=0.4;
extern uint8 white_num_40col;//第40列白点数
extern float  white_num_40col_min;
extern float  white_num_40col_max;
/*********************************************/
阿克曼转向模型
float Get_Dire(uint8 i)
{
  g_Control=-(steering_PWM-steeringPWM_MID);
  
  float tn=tan(g_Control/54.95*casu_weight); //54.95=70*0.785
  if(i) ///左轮
  {      
    return (A*(K-0.5*BORDWIDTH*tn/LDISTANCE*AK));
    
   // return 1;
  }
  
  //右轮
    return (A*(K+0.5*BORDWIDTH*tn/LDISTANCE*AK));
  //return 0;
}

/**********************被动差速lsd限滑********************/
float lsd_p=0.1,lsd_pl=0.6,lsd_d=0.8;
float lsd_p_base=0.15;
int16 speed_set_l,speed_set_r;
extern int16 speed_set;     //57大概为1m/s
extern float speed_p;
extern float speed_i;
extern float speed_d;

extern float lsd_show;
void LSD(int16 encoder_l,int16 encoder_r)
{
  static int16 encoder_err;
  static int16 encoder_det;
  static int16 encoder_err_last;
  static float lsd;
  int turn_err_lsd;
  
  turn_err_lsd=(int)center-39;
  
  encoder_err=encoder_l-encoder_r;
//  if((encoder_err>10)||(encoder_err<-10))
//  {
    encoder_det=encoder_err-encoder_err_last;
    
    //lsd_p=lsd_p_base+turn_err_lsd*turn_err_lsd/7000;

    //if(lsd_p>0.25) lsd_p=0.25;
    lsd=encoder_err*lsd_p
       +encoder_err_last*lsd_pl
       +encoder_det*lsd_d;
  
    //在这里加lsd的限幅
    
    if(lsd>120) lsd=120;
    if(lsd<-120) lsd=-120;
    lsd_show=lsd;
    
    encoder_err_last=encoder_err;
    speed_set_l=speed_set+(int16)lsd;
    speed_set_r=speed_set-(int16)lsd;


}

int16 Speed_Ctl_l(int16 encoder_l)
{
  static int16 speed_err;
  static int16 speed;
  static int16 speed_err_last1,speed_err_last2;
  static int16 speed_bb;
  speed_err = speed_set_l - encoder_l;    //速度偏差=目标速度-当前速度
  
  if(speed_err>80) speed_bb=9800;
  else if(speed_err<-80) speed_bb=-9800;
  else speed_bb=0;
  /**********************************/
  speed+=  speed_p*(speed_err-speed_err_last1)
                +speed_i* speed_err
                +speed_d*(speed_err-2*speed_err_last1+speed_err_last2)
                +speed_bb;
  
  speed_err_last1=speed_err;
  speed_err_last2=speed_err_last1;
  
  if(speed> 9800) speed= 9800;
  if(speed<-9800) speed=-9800;
  
  return speed;
}

int16 Speed_Ctl_r(int16 encoder_r)
{
  static int16 speed_err;
  static int16 speed;
  static int16 speed_err_last1,speed_err_last2;
  static int16 speed_bb;
  speed_err = speed_set_r - encoder_r;    //速度偏差=目标速度-当前速度
 
  if(speed_err>80) speed_bb=9800;
  else if(speed_err<-80) speed_bb=-9800;
  else speed_bb=0;
  /**********************************/
  speed+=  speed_p*(speed_err-speed_err_last1)
                +speed_i* speed_err
                +speed_d*(speed_err-2*speed_err_last1+speed_err_last2)
                +speed_bb;
  
  speed_err_last1=speed_err;
  speed_err_last2=speed_err_last1;
  
  if(speed> 9800) speed= 9800;
  if(speed<-9800) speed=-9800;
  
  return speed;
}