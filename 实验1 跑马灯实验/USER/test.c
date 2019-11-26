#include "sys.h"
#include "delay.h" 
#include "led.h" 
#include "usart.h"
//ALIENTEK ������STM32H7������ ʵ��1
//�����ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{ 
	Stm32_Clock_Init(160,5,2,4);//����ʱ��,400Mhz
    delay_init(400);			//��ʱ��ʼ��  
	LED_Init();					//��ʼ��LEDʱ��  
	while(1)
	{
		LED0(0);				//DS0��
		LED1(1);				//DS1��
		delay_ms(500);
		LED0(1);				//DS0��
		LED1(0);				//DS1��
		delay_ms(500);
	}
}

















