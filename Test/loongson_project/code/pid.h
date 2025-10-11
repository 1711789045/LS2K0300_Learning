#ifndef __PID_H__
#define __PID_H__

typedef struct {
    float last_error;      // ��һ�����
    float prev_error;      // ���ϴ�������΢�֣�
    float output;          // ��ǰ���ֵ
	float last_i;          // ��ǰ���ֵ

} PID_INCREMENT_TypeDef;

typedef struct {
    float last_error;      // ��һ�����
    float error_sum;      // ������
    float output;          // ��ǰ���ֵ
} PID_POSITIONAL_TypeDef;


float pid_increment(PID_INCREMENT_TypeDef *pid, float target, float current, 
                    float limit, float kp, float ki, float kd);
float pid_positional(PID_POSITIONAL_TypeDef *pid, float target, float current, 
                     float limit, float kp, float ki, float kd1,float kd2) ;
#endif
