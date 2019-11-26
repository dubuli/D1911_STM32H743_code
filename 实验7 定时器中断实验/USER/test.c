#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "timer.h"  
//ALIENTEK ������STM32H7������ ʵ��7
//��ʱ���ж�ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{  
	u8 led0sta=1;
	Stm32_Clock_Init(160,5,2,4);//����ʱ��,400Mhz
    delay_init(400);			//��ʱ��ʼ��  
	uart_init(100,115200);		//���ڳ�ʼ��Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�   
 	TIM3_Int_Init(5000-1,20000-1);	//10Khz�ļ���Ƶ�ʣ�����5K��Ϊ500ms     
	while(1)
	{
		LED0(led0sta^=1);
		delay_ms(200);
	}
}


















