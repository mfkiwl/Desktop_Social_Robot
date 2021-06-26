#include "motor.h"
void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	AIN1 = 0, AIN2 = 0;
	BIN1 = 0, BIN1 = 0;
}

// 設定馬達PWM值 左輪PWM 右輪PWM
void Set_Pwm(int moto1,int moto2)
{
	if(moto1 < 0) AIN2 = 1, AIN1 = 0;
	else  AIN2 = 0, AIN1 = 1;
	PWMA = myabs(moto1);
	if(moto2 < 0) BIN1 = 0, BIN2 = 1;
	else BIN1 = 1, BIN2 = 0;
	PWMB = myabs(moto2);	
}

// 將PWM計算結果調整為恆正
int myabs(int a)
{ 		   
	int temp;
	if(a < 0) temp = -a;  
	else temp = a;
	return temp;
}

// 限制PWM範圍 避免電壓過高 
void Pwm_limiting(void)
{
    if(Moto1 < -7000) Moto1 = -7000;
	if(Moto1 >  7000) Moto1 = 7000;
	if(Moto2 < -7000) Moto2 = -7000;
	if(Moto2 >  7000) Moto2 = 7000;
}

// 電壓過低 傾角過大 關閉馬達
void Turn_Off(float angle, float voltage)
{
	if(angle < -40 || angle > 40 || voltage < 3.6)
	{																	 
		Moto1 = 0;
		Moto2 = 0;
		flag_fall = 1;
	}		
	else 				
		flag_fall = 0;
}
