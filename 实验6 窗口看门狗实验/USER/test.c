#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "wdg.h"  
//ALIENTEK 阿波罗STM32H7开发板 实验6
//窗口看门狗实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{  
	Stm32_Clock_Init(160,5,2,4);//设置时钟,400Mhz
    delay_init(400);			//延时初始化  
	uart_init(100,115200);		//串口初始化为115200 
	LED_Init();		  			//初始化与LED连接的硬件接口   
 	LED0(0);					//点亮LED0
	delay_ms(300);				//延时300ms再初始化看门狗,LED0的变化"可见"
	WWDG_Init(0X7F,0X5F,4);		//计数器值为7f,窗口寄存器为5f,分频数为16	   
	while(1)
	{
		LED0(1);				//关闭LED0
	}
}


















