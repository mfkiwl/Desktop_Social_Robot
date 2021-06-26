#include "control.h"

int Balance_Pwm, Velocity_Pwm, Turn_Pwm;
float Mechanical_balance = -6; // 機械平衡角度

float balance_UP_KP = 280; 	   // 直立迴路 KP
float balance_UP_KD = 0.8;

float velocity_KP = 90;
float velocity_KI = 0.45;

u8 UltrasonicWave_Voltage_Counter = 0;

void EXTI9_5_IRQHandler(void) 
{    
	if(PBin(5) == 0)		
	{		
		EXTI->PR=1<<5;                             // 清除 EXTI_Line5 外部中斷
		mpu_dmp_get_data(&pitch, &roll, &yaw);     // 歐拉角(姿態)數據
		MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz); // 陀螺儀數據
		Encoder_Left = Read_Encoder(2);            // 馬達編碼器測量值
		Encoder_Right = -Read_Encoder(4);          // 加負號調整方向一致
		UltrasonicWave_Voltage_Counter++;
		if(UltrasonicWave_Voltage_Counter == 10)   // 100ms 讀取一次超音波距離數據與電壓
		{
			UltrasonicWave_Voltage_Counter = 0;
			Voltage = Get_battery_volt();          // 讀取電池電壓		
			UltrasonicWave_StartMeasure();         // 超音波距離數據
		}
		Balance_Pwm = balance_UP(pitch, Mechanical_balance, gyroy); // 直立PD迴路
		Velocity_Pwm = velocity(Encoder_Left, Encoder_Right);       // 速度PI迴路	 
  		if(Flag_Left || Flag_Right)    
			Turn_Pwm = turn(Encoder_Left, Encoder_Right, gyroz);    // 轉向PD迴路
		else Turn_Pwm = (-0.5) * gyroz;
		Moto1 = Balance_Pwm - Velocity_Pwm - Turn_Pwm;              // 左側馬達PWM計算結果
		Moto2 = Balance_Pwm - Velocity_Pwm + Turn_Pwm;              // 右側馬達PWM計算結果
		Pwm_limiting();                                             // 限制PWM範圍 避免電壓過高 
		USB_TEST();                                                 // 檢查是否充電中
		Turn_Off(pitch, Voltage);                                   // 檢查電壓 傾角
		Set_Pwm(Moto1, Moto2);                                      // 設定馬達最終PWM
	}       	
} 

// 利用歐拉角與對應角速度平衡
int balance_UP(float Angle, float Mechanical_balance, float Gyro)
{  
	float Bias;
	int balance;
	Bias = Angle - Mechanical_balance; // 求出與機械平衡角度差值
	// 直立PD迴路
	balance = balance_UP_KP * Bias + balance_UP_KD * Gyro;
	return balance;
}

// 利用馬達編碼器測量值平衡
int velocity(int encoder_left, int encoder_right)
{  
	static float Velocity, Encoder_Least, Encoder, Movement;
	static float Encoder_Integral;
	if(1 == Flag_Forward)
	{
		flag_UltrasonicWave = 0; // 藍芽遙控優先於障礙物檢測
		Movement = 20;
	}
    else if(1 == Flag_Backward)
	{
		flag_UltrasonicWave = 0;
		Movement = -20;
	}
	// 障礙物檢測距離低於 10cm 後退 
	else if(UltrasonicWave_Distance < 10 && UltrasonicWave_Distance > 3)
	{
		flag_UltrasonicWave = 1;
		Movement = UltrasonicWave_Distance * (-2);		
	}
	else
	{	
		flag_UltrasonicWave = 0;
		Movement = 0;
	}
	// 速度PI迴路
	Encoder_Least = (Encoder_Left + Encoder_Right) - 0;      // 最新速度差值 ＝ 測量速度(左右編碼器之和) - 目標速度(0) 
	Encoder *= 0.8;		                                     // LPF       
	Encoder += Encoder_Least * 0.2;	                         // LPF 降低速度差值對直立迴路影響
	Encoder_Integral += Encoder;                             // 積分出位移 t = 10ms
	Encoder_Integral = Encoder_Integral - Movement;          // 藍芽數據 控制前進後退
	if(Encoder_Integral > 10000) Encoder_Integral = 10000;   // 限制速度最大上限
	if(Encoder_Integral < -10000) Encoder_Integral = -10000; // 限制速度最大上限
		Velocity = Encoder * velocity_KP + Encoder_Integral * velocity_KI; 
	if(pitch < -40 || pitch > 40) Encoder_Integral = 0;     			    
	return Velocity;
}

// 保持轉向時平衡
int turn(int encoder_left, int encoder_right, float gyro)
{
	static float Turn_Target, Turn, Encoder_temp, Turn_Convert = 0.9, Turn_Count;
	float Turn_Amplitude = 44, Kp = 20, Kd = 0;     
	// 根據轉向前速度調整起始速度
  	if(1 == Flag_Left || 1 == Flag_Right)                      
	{
		if(++Turn_Count == 1)
		Encoder_temp = myabs(encoder_left + encoder_right);      
		Turn_Convert = 55 / Encoder_temp;
		if(Turn_Convert < 0.6) Turn_Convert = 0.6;
		if(Turn_Convert > 3) Turn_Convert = 3;
	}	
	else
	{
		Turn_Convert = 0.9;
		Turn_Count = 0;
		Encoder_temp = 0;
	}			
	if(1 == Flag_Left) Turn_Target -= Turn_Convert;
	else if(1 == Flag_Right) Turn_Target += Turn_Convert; 
	else Turn_Target = 0;
    if(Turn_Target > Turn_Amplitude) Turn_Target = Turn_Amplitude;   // 轉向速度限制
	if(Turn_Target < -Turn_Amplitude) Turn_Target = -Turn_Amplitude;
	if(Flag_Forward == 1 || Flag_Backward == 1) Kd = 0.5;        
	else Kd = 0; // 轉向時取消根據陀螺儀數據的修正
	// 轉向PD迴路
	Turn = - Turn_Target * Kp - gyro * Kd; // 結合陀螺儀Z軸數據計算
	return Turn;
}