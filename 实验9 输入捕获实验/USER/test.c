#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "timer.h"  
//ALIENTEK 阿波罗STM32H7开发板 实验9
//输入捕获实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值
int main(void)
{  
	long long temp=0;  
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200 
	LED_Init();						//初始化与LED连接的硬件接口   
 	TIM3_PWM_Init(500-1,200-1);		//1Mhz的计数频率,2Khz的PWM.     
	TIM5_CH1_Cap_Init(0XFFFFFFFF,200-1);//以1Mhz的频率计数 
   	while(1)
	{
 		delay_ms(10);
		LED0_PWM_VAL++;
		if(LED0_PWM_VAL==300)LED0_PWM_VAL=0;	 		 
 		if(TIM5CH1_CAPTURE_STA&0X80)		//成功捕获到了一次高电平
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F; 
			temp*=0XFFFFFFFF;		 		//溢出时间总和
			temp+=TIM5CH1_CAPTURE_VAL;		//得到总的高电平时间
			printf("HIGH:%lld us\r\n",temp);//打印总的高点平时间
			TIM5CH1_CAPTURE_STA=0;			//开启下一次捕获
		}
	}
}


















