#include "UltrasonicWave.h"
#define    TRIG_PIN GPIO_Pin_3 // TRIG       
#define    ECHO_PIN GPIO_Pin_2 // ECHO   

void UltrasonicWave_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;    
    EXTI_InitTypeDef EXTI_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;        // PA.3 接 TRIG
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 端口工作模式設定
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;        // PA.2 接 ECHO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;    // 端口工作模式設定
    GPIO_Init(GPIOA, &GPIO_InitStructure);     

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);

    EXTI_InitStructure.EXTI_Line = EXTI_Line2;             // 設定 EXTI_Line2 外部中斷  
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // 設定中斷模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              // 啟用外部中斷
    EXTI_Init(&EXTI_InitStructure);                        // 初始化外部中斷
}

void EXTI2_IRQHandler(void)
{
    delay_us(10);
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
        TIM_SetCounter(TIM3, 0);    
        TIM_Cmd(TIM3, ENABLE);                                // 啟動計時器3
        while(GPIO_ReadInputDataBit(GPIOA, ECHO_PIN));        // 等待低電位
        TIM_Cmd(TIM3, DISABLE);                               // 關閉計時器3
        UltrasonicWave_Distance = TIM_GetCounter(TIM3) * 1.7; // 計算距離
        EXTI_ClearITPendingBit(EXTI_Line2);                   // 清除 EXTI_Line2 外部中斷
    }
}

// 發送一個 > 10us 的方波 測量返回高電位時間
void UltrasonicWave_StartMeasure(void)
{
    GPIO_SetBits(GPIOA, TRIG_PIN);
    delay_us(20);
    GPIO_ResetBits(GPIOA, TRIG_PIN);    
}
