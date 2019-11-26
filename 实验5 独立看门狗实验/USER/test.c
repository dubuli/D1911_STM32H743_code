#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "key.h"  
#include "wdg.h"  
//ALIENTEK ������STM32H7������ ʵ��5
//�������Ź�ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{  
	Stm32_Clock_Init(160,5,2,4);//����ʱ��,400Mhz
    delay_init(400);			//��ʱ��ʼ��  
	uart_init(100,115200);		//���ڳ�ʼ��Ϊ115200 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�   
	KEY_Init();		  			//��ʼ������
 	delay_ms(100);				//��ʱ100ms�ٳ�ʼ�����Ź�,LED0�ı仯"�ɼ�"
	IWDG_Init(4,500);    		//Ԥ��Ƶ��Ϊ64,����ֵΪ500,���ʱ��Ϊ1s	   
	LED0(0);			 		//����LED0
	while(1)
	{ 
		if(KEY_Scan(0)==WKUP_PRES)//���WK_UP����,��ι��
		{
			IWDG_Feed();//ι��
		}
		delay_ms(10);
	} 
}


















