#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "ltdc.h" 
#include "stmflash.h" 
#include "iap.h"  
#include "key.h"      
//ALIENTEK ������STM32H7������ ʵ��55
//����IAP ʵ��-IAP Bootloader V1.0 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
  
int main(void)
{  
 	u8 led0sta=1;
	u8 t;
	u8 key;
	u32 oldcount=0;				//�ϵĴ��ڽ�������ֵ
	u32 applenth=0;				//���յ���app���볤��
	u8 clearflag=0; 		
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200 
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	KEY_Init();						//��ʼ��LED
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"FLASH EEPROM TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/8/21");	 		
	LCD_ShowString(30,130,200,16,16,"KEY_UP:Copy APP2FLASH");
	LCD_ShowString(30,150,200,16,16,"KEY1:Erase SRAM APP");
	LCD_ShowString(30,170,200,16,16,"KEY0:Run SRAM APP");
	LCD_ShowString(30,190,200,16,16,"KEY2:Run FLASH APP");
	POINT_COLOR=BLUE;
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	while(1)
	{
	 	if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",applenth);
			}else oldcount=USART_RX_CNT;			
		}
		t++;
		delay_ms(10);
		if(t==30)
		{
			LED0(led0sta^=1);
			t=0; 
			if(clearflag)
			{
				clearflag--;
				if(clearflag==0)LCD_Fill(30,210,240,210+16,WHITE);//�����ʾ
			}
		}	  	 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)	//WK_UP��������
		{
			if(applenth)
			{
				printf("��ʼ���¹̼�...\r\n");	
				LCD_ShowString(30,210,200,16,16,"Copying APP2FLASH...");
 				if(((*(vu32*)(0x24001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//����FLASH����   
					LCD_ShowString(30,210,200,16,16,"Copy APP Successed!!");
					printf("�̼��������!\r\n");	
				}else 
				{
					LCD_ShowString(30,210,200,16,16,"Illegal FLASH APP!  ");	   
					printf("��FLASHӦ�ó���!\r\n");
				}
 			}else 
			{
				printf("û�п��Ը��µĹ̼�!\r\n");
				LCD_ShowString(30,210,200,16,16,"No APP!");
			}
			clearflag=7;//��־��������ʾ,��������7*300ms�������ʾ									 
		}
		if(key==KEY1_PRES)	//KEY1����
		{
			if(applenth)
			{																	 
				printf("�̼�������!\r\n");    
				LCD_ShowString(30,210,200,16,16,"APP Erase Successed!");
				applenth=0;
			}else  
			{
				printf("û�п�������Ĺ̼�!\r\n");
				LCD_ShowString(30,210,200,16,16,"No APP!");
			}
			clearflag=7;//��־��������ʾ,��������7*300ms�������ʾ									 
		}
		if(key==KEY2_PRES)	//KEY2����
		{
			printf("��ʼִ��FLASH�û�����!!\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			}else 
			{
				printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");
				LCD_ShowString(30,210,200,16,16,"Illegal FLASH APP!");	   
			}									 
			clearflag=7;//��־��������ʾ,��������7*300ms�������ʾ	  
		}
		if(key==KEY0_PRES)	//KEY0����
		{
			printf("��ʼִ��SRAM�û�����!!\r\n");
			if(((*(vu32*)(0x24001000+4))&0xFF000000)==0x24000000)//�ж��Ƿ�Ϊ0X24XXXXXX.
			{	 
				iap_load_app(0x24001000);//SRAM��ַ
			}else 
			{
				printf("��SRAMӦ�ó���,�޷�ִ��!\r\n");
				LCD_ShowString(30,210,200,16,16,"Illegal SRAM APP!");	   
			}									 
			clearflag=7;//��־��������ʾ,��������7*300ms�������ʾ	 
		}				   
		 
	}  
}
















