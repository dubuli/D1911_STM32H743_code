#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "key.h"
#include "dma.h" 
//ALIENTEK ������STM32H7������ ʵ��24
//DMA ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
  
#define SEND_BUF_SIZE 7800	//�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.

u8 SendBuff[SEND_BUF_SIZE];	//�������ݻ�����
const u8 TEXT_TO_SEND[]={"ALIENTEK Apollo STM32H7 DMA ����ʵ��"};	

int main(void)
{  
 	u8 led0sta=1;
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0;					//����
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
	KEY_Init(); 					//������ʼ��
 	MYDMA_Config(DMA2_Stream7,42,(u32)&USART1->TDR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH42,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/13");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Start");	 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ      	 
	//��ʾ��ʾ��Ϣ	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//���ASCII�ַ�������
    {
		if(t>=j)//���뻻�з�
		{
			if(mask)
			{
				SendBuff[i]=0x0a;
				t=0;
			}else 
			{
				SendBuff[i]=0x0d;
				mask++;
			}	
		}else//����TEXT_TO_SEND���
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
    }		 
	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	i=0;
	while(1)
	{
		t=KEY_Scan(0);
		if(t==KEY0_PRES)//KEY0����
		{
			printf("\r\nDMA DATA:\r\n"); 	    
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %");//��ʾ�ٷֺ�
		    USART1->CR3=1<<7;           //ʹ�ܴ���1��DMA����       
			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);//��ʼһ��DMA���䣡	  
		    //�ȴ�DMA������ɣ���ʱ������������һЩ�£����
		    //ʵ��Ӧ���У����������ڼ䣬����ִ�����������
		    while(1)
		    {
				if(DMA2->HISR&(1<<27))	//�ȴ�DMA2_Steam7�������
				{
					DMA2->HIFCR|=1<<27;	//���DMA2_Steam7������ɱ�־
					break; 
		        }
				pro=DMA2_Stream7->NDTR;	//�õ���ǰ��ʣ����ٸ�����
				pro=1-pro/SEND_BUF_SIZE;//�õ��ٷֱ�	  
				pro*=100;      			//����100��
				LCD_ShowNum(30,170,pro,3,16);	  
		    }			    
			LCD_ShowNum(30,170,100,3,16);//��ʾ100%	  
			LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//��ʾ�������
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//��ʾϵͳ��������	
			i=0;
		}		   
	}		    
}













