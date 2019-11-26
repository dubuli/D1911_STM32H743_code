#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
#include "tpad.h"  
//ALIENTEK ������STM32H7������ ʵ��10
//���ݴ�������ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
   
int main(void)
{  
	u8 t=0,led0sta=1,led1sta=1;
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200 
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�   
 	TPAD_Init(2);					//��ʼ����������,��200/2=100MhzƵ�ʼ���
   	while(1)
	{					  						  		 
 		if(TPAD_Scan(0))			//�ɹ�������һ��������(�˺���ִ��ʱ������15ms)
		{
			LED1(led1sta^=1);		//LED1ȡ��
		}
		t++;
		if(t==15)		 
		{
			t=0;
			LED0(led0sta^=1);		//LED0ȡ��,��ʾ��������
		}
		delay_ms(10);
	}
}


















