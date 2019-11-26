#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "key.h"
//ALIENTEK ������STM32H7������ ʵ��2
//��������ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{ 
	u8 key;
	u8 led0sta=1,led1sta=1;		//LED0,LED1�ĵ�ǰ״̬
	Stm32_Clock_Init(160,5,2,4);//����ʱ��,400Mhz
    delay_init(400);			//��ʱ��ʼ��  
	LED_Init();					//��ʼ��LEDʱ��  
	KEY_Init();         		//��ʼ���밴�����ӵ�Ӳ���ӿ�
	LED0(0);					//�ȵ������  
	while(1)
	{
		key=KEY_Scan(0); 		//�õ���ֵ
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//����LED0,LED1�������
					led1sta=!led1sta;
					led0sta=!led1sta;
					break;
				case KEY2_PRES:	//����LED0��ת
	 				led0sta=!led0sta;
					break;
				case KEY1_PRES:	//����LED1��ת	 
					led1sta=!led1sta;
					break;
				case KEY0_PRES:	//ͬʱ����LED0,LED1��ת 
					led0sta=!led0sta;
					led1sta=!led1sta;
					break;
			}
			LED0(led0sta);		//����LED0״̬
			LED1(led1sta);		//����LED1״̬
		}else delay_ms(10); 
	}
}

















