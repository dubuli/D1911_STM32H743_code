#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "tpad.h"  
//ALIENTEK 阿波罗STM32H7开发板 实验10
//电容触摸按键实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
   
int main(void)
{  
	u8 t=0,led0sta=1,led1sta=1;
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200 
	LED_Init();						//初始化与LED连接的硬件接口   
 	TPAD_Init(2);					//初始化触摸按键,以200/2=100Mhz频率计数
   	while(1)
	{					  						  		 
 		if(TPAD_Scan(0))			//成功捕获到了一次上升沿(此函数执行时间至少15ms)
		{
			LED1(led1sta^=1);		//LED1取反
		}
		t++;
		if(t==15)		 
		{
			t=0;
			LED0(led0sta^=1);		//LED0取反,提示程序运行
		}
		delay_ms(10);
	}
}


















