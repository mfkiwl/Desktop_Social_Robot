#include "sys.h"
/**************************** 開發版全域變數 *************************************/    
float Voltage;      // 電池電壓
u8    key = 0;      // 按鍵值
u8    KEY_MODE = 0; // 按鍵模式
/***************************** 其他全域變數 **************************************/
float pitch, roll, yaw;               // 歐拉角(姿態)
short aacx, aacy, aacz;               // 加速度數據 
short gyrox, gyroy, gyroz;            // 陀螺儀數據
float UltrasonicWave_Distance;        // 超音波模組距離數據
int   Encoder_Left, Encoder_Right;    // 馬達編碼器測量值
int   Moto1 = 0, Moto2 = 0;           // 計算出的馬達 PWM
int   Flag_Forward, Flag_Backward, Flag_Left, Flag_Right; // 藍牙遙控
u8    flag_UltrasonicWave = 0;        // 超音波模組檢測是否有障礙物
u8    flag_fall = 0;                  // 是否跌倒
/********************************************************************************/
int main(void)	
{ 
/**************************** 開發版函式 ******************************************/ 
    LED_Init();         // 開發版 LED 指示燈初始化
    USB_Init();         // 開發版 USB 初始化
    KEY_Init();         // 開發版按鍵初始化
    delay_init();       // 延時函式初始化	
    uart1_init(128000); // UART 1 初始化 (CH340)
/**************************** 藍牙設定 ********************************************/ 	
    uart3_init(9600);                    // UART 3 初始化與藍芽通訊 Baud Rate (藍芽模組預設：9600)
    delay_ms(100);
    Uart3SendStr("AT\r\n");             
    Uart3SendStr("AT+NAMEDesktopRobot\r\n");
    delay_ms(100);
    Uart3SendStr("AT+BAUD8\r\n");        // 藍芽通訊 Baud Rate 改設為 115200
    delay_ms(100);
    uart3_init(115200);                  // UART 3 Baud Rate 改設為 115200	
/**************************** 開發版函式 ******************************************/ 
    NVIC_Configuration(); // NVIC 中斷優先順序設定
    Adc_Init();           // ADC 初始化
/********************************************************************************/
    Encoder_Init_TIM2();           // 馬達編碼器2初始化
    Encoder_Init_TIM4();           // 馬達編碼器4初始化
    Timer3_Init(5000,7199);        // 超音波計算距離用計時器初始化
    MPU_Init();                    // MPU6050 陀螺儀/加速度模組初始化
    mpu_dmp_init();                // MPU6050 DMP 功能初始化
    UltrasonicWave_Configuration();       // 超音波 GPIO 設定初始化
    TIM1_PWM_Init(7199,0);                // 初始化 PWM 10KHZ 驅動馬達
    delay_ms(1000);                       // 延時 2s  
    delay_ms(1000);                       // 避免開機輪子亂轉 
    Motor_Init();                         // 馬達 GPIO 設定初始化
    MPU6050_EXTI_Init();                  // MPU6050 5ms 定時中斷初始化
}