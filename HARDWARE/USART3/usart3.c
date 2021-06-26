#include "usart3.h"
void uart3_init(u32 bound)
{  	 
    // GPIO 端口設定
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // Enable GPIOB Clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // Enable USART3 Clock

    // USART3_TX  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        // PB.10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 端口工作模式設定
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3_RX	  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;            // PB.11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 端口工作模式設定
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART 初始化設定
    USART_InitStructure.USART_BaudRate = bound; 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;     // 字符長度8位元
    USART_InitStructure.USART_StopBits = USART_StopBits_1;          // 一個停止位元
    USART_InitStructure.USART_Parity = USART_Parity_No;             // 無奇偶校驗位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 關閉 Ｍodem Ｍodes
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;   // 收發模式
    USART_Init(USART3, &USART_InitStructure);      // 初始化 USART
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 接收到一個位元組的數據就中斷
    USART_Cmd(USART3, ENABLE);                     // Enable USART
}

// UART 3 接收中斷函式
void USART3_IRQHandler(void)
{	
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收到數據
	{	  
		static int uart_receive = 0; // 儲存藍芽接收數據
		uart_receive = USART_ReceiveData(USART3); 
		if(uart_receive == 0x00)                                                      // 停
		    Flag_Forward = 0, Flag_Backward = 0, Flag_Left = 0, Flag_Right = 0;
		else if(uart_receive == 0x01)                                                 // 前
		    Flag_Forward = 1, Flag_Backward = 0, Flag_Left = 0, Flag_Right = 0;
		else if(uart_receive == 0x05)                                                 // 後	
		    Flag_Forward = 0, Flag_Backward = 1, Flag_Left = 0, Flag_Right = 0;
		else if(uart_receive == 0x02 || uart_receive==0x03 || uart_receive == 0x04)   // 右
		    Flag_Forward = 0, Flag_Backward = 0, Flag_Left = 0, Flag_Right = 1;  
		else if(uart_receive == 0x06 || uart_receive == 0x07 || uart_receive == 0x08) // 左
		    Flag_Forward = 0, Flag_Backward = 0, Flag_Left = 1, Flag_Right = 0; 
		else Flag_Forward = 0, Flag_Backward = 0, Flag_Left = 0, Flag_Right =0 ;      // 停
	}  											 
} 

// Uart 3 發送位元組
void Uart3SendByte(char byte)   
{
	USART_SendData(USART3, byte); // 發送數據(stm32函式庫)
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC) != SET);  
	// 等待發送完成 檢測 USART_FLAG_TC 
 }

void Uart3SendBuf(char *buf, u16 len)
{
	u16 i;
	for(i = 0; i < len; i++) Uart3SendByte(*buf++);
}

void Uart3SendStr(char *str)
{
	u16 i,len;
	len = strlen(str);
	for(i = 0; i < len; i++) Uart3SendByte(*str++);
}