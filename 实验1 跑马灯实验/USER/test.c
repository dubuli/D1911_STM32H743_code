#include "sys.h"
#include "delay.h" 
#include "led.h" 
#include "usart.h"
//ALIENTEK 阿波罗STM32H7开发板 实验1
//跑马灯实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{ 
	Stm32_Clock_Init(160,5,2,4);//设置时钟,400Mhz
    delay_init(400);			//延时初始化  
	LED_Init();					//初始化LED时钟  
	while(1)
	{
		LED0(0);				//DS0亮
		LED1(1);				//DS1灭
		delay_ms(500);
		LED0(1);				//DS0灭
		LED1(0);				//DS1亮
		delay_ms(500);
	}
}

















