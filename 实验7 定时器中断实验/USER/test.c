#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "timer.h"  
//ALIENTEK 阿波罗STM32H7开发板 实验7
//定时器中断实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{  
	u8 led0sta=1;
	Stm32_Clock_Init(160,5,2,4);//设置时钟,400Mhz
    delay_init(400);			//延时初始化  
	uart_init(100,115200);		//串口初始化为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口   
 	TIM3_Int_Init(5000-1,20000-1);	//10Khz的计数频率，计数5K次为500ms     
	while(1)
	{
		LED0(led0sta^=1);
		delay_ms(200);
	}
}


















