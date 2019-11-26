#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "wdg.h"  
//ALIENTEK ������STM32H7������ ʵ��6
//���ڿ��Ź�ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{  
	Stm32_Clock_Init(160,5,2,4);//����ʱ��,400Mhz
    delay_init(400);			//��ʱ��ʼ��  
	uart_init(100,115200);		//���ڳ�ʼ��Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�   
 	LED0(0);					//����LED0
	delay_ms(300);				//��ʱ300ms�ٳ�ʼ�����Ź�,LED0�ı仯"�ɼ�"
	WWDG_Init(0X7F,0X5F,4);		//������ֵΪ7f,���ڼĴ���Ϊ5f,��Ƶ��Ϊ16	   
	while(1)
	{
		LED0(1);				//�ر�LED0
	}
}


















