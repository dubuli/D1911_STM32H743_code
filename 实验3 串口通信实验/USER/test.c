#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
//ALIENTEK ������STM32H7������ ʵ��3
//����ͨ��ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{ 
	u8 t;
	u8 len;	
	u16 times=0;  
	u8 led0sta=1;				//LED0ǰ״̬
	Stm32_Clock_Init(160,5,2,4);//����ʱ��,400Mhz
    delay_init(400);			//��ʱ��ʼ��  
	uart_init(100,115200);		//���ڳ�ʼ��Ϊ115200
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�   
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->TDR=USART_RX_BUF[t];
				while((USART1->ISR&0X40)==0);//�ȴ����ͽ���
			}
			printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0; 
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nALIENTEK ������STM32F4/F7/H7������ ����ʵ��\r\n");
				printf("����ԭ��@ALIENTEK\r\n\r\n\r\n");
			}
			if(times%200==0)printf("����������,�Իس�������\r\n");  
			if(times%30==0)LED0(led0sta^=1);//��˸LED,��ʾϵͳ��������.
			delay_ms(10);   
		}
	}
}
















