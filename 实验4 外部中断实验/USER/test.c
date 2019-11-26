#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "exti.h"  
//ALIENTEK ������STM32H7������ ʵ��4
//�ⲿ�ж�ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{  
	Stm32_Clock_Init(160,5,2,4);//����ʱ��,400Mhz
    delay_init(400);			//��ʱ��ʼ��  
	uart_init(100,115200);		//���ڳ�ʼ��Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�   
	EXTIX_Init();         		//��ʼ���ⲿ�ж����� 
	LED0(0);					//�ȵ������
	while(1)
	{
		printf("OK\r\n");	
		delay_ms(1000);	  
	}
}
















