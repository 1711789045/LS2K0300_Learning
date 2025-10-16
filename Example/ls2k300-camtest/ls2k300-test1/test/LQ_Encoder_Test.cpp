#include "main.hpp"

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
 * @�������ƣ�void EncoderTest()
 * @����˵��������������(�Ĵ�����PWM ������ʵ��)
 * @����˵������
 * @�������أ���
 * @���÷�����EncoderTest();
 * @��ע˵�������Դ�������Ҳ����ת�鿴����������ֵ
 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void EncoderTest()
{
    // ���PWM
    SetPWM LMotorPWM(PWM1, 100000, 32000, "inversed");
    SetPWM RMotorPWM(PWM2, 100000, 32000, "inversed");
    // �������GPIO
    HWGpio LMotoDir(74, GPIO_Mode_Out);
    HWGpio RMotoDir(75, GPIO_Mode_Out);
    // ��ʼ������������ͨ��
    LS_PwmEncoder EncodersL(0, 73);
    LS_PwmEncoder EncodersR(3, 72);
    // ��ʼ�����
    LMotorPWM.Enable();
    RMotorPWM.Enable();
    while(1)
    {
        cout << "left = " << EncodersL.Update() << ", right = " << EncodersR.Update() << endl;
        usleep(1000);
    }
}
