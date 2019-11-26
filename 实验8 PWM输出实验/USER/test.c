#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "timer.h"  
//ALIENTEK ������STM32H7������ ʵ��8
//PWM���ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{  
	u16 led0pwmval=0;    
	u8 dir=1; 
	Stm32_Clock_Init(160,5,2,4);//����ʱ��,400Mhz
    delay_init(400);			//��ʱ��ʼ��  
	uart_init(100,115200);		//���ڳ�ʼ��Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�   
 	TIM3_PWM_Init(500-1,200-1);	//1Mhz�ļ���Ƶ��,2Khz��PWM.     
   	while(1)
	{
 		delay_ms(10);	 
		if(dir)led0pwmval++;
		else led0pwmval--;	 
 		if(led0pwmval>300)dir=0;
		if(led0pwmval==0)dir=1;	   					 
		LED0_PWM_VAL=led0pwmval;	   
	}
}


















