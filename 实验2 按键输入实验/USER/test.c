#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "key.h"
//ALIENTEK 阿波罗STM32H7开发板 实验2
//按键输入实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{ 
	u8 key;
	u8 led0sta=1,led1sta=1;		//LED0,LED1的当前状态
	Stm32_Clock_Init(160,5,2,4);//设置时钟,400Mhz
    delay_init(400);			//延时初始化  
	LED_Init();					//初始化LED时钟  
	KEY_Init();         		//初始化与按键连接的硬件接口
	LED0(0);					//先点亮红灯  
	while(1)
	{
		key=KEY_Scan(0); 		//得到键值
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//控制LED0,LED1互斥点亮
					led1sta=!led1sta;
					led0sta=!led1sta;
					break;
				case KEY2_PRES:	//控制LED0翻转
	 				led0sta=!led0sta;
					break;
				case KEY1_PRES:	//控制LED1翻转	 
					led1sta=!led1sta;
					break;
				case KEY0_PRES:	//同时控制LED0,LED1翻转 
					led0sta=!led0sta;
					led1sta=!led1sta;
					break;
			}
			LED0(led0sta);		//控制LED0状态
			LED1(led1sta);		//控制LED1状态
		}else delay_ms(10); 
	}
}

















