#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h" 
#include "malloc.h"  
#include "nand.h"    
#include "ftl.h"  
#include "w25qxx.h"    
#include "sdmmc_sdcard.h" 
#include "ff.h"  
#include "exfuns.h" 
#include "text.h"
#include "sai.h"	 	
#include "wm8978.h"	  
#include "videoplayer.h" 
#include "timer.h" 
//ALIENTEK ������STM32H7������ ʵ��50
//��Ƶ���� ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾    
 

int main(void)
{   
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz 
	delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	usmart_dev.init(100);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
	KEY_Init();						//��ʼ������
	W25QXX_Init();					//��ʼ��W25Q256 
	WM8978_Init();					//��ʼ��WM8978
	
	WM8978_ADDA_Cfg(1,0);			//����DAC
	WM8978_Input_Cfg(0,0,0);		//�ر�����ͨ��
	WM8978_Output_Cfg(1,0);			//����DAC��� 	
	WM8978_HPvol_Set(50,50);		//������������
	WM8978_SPKvol_Set(50);			//������������
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
	exfuns_init();					//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 			//����SD��  
	POINT_COLOR=RED;      
	while(font_init()) 				//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 
 	Show_Str(60,50,200,16,"������STM32F4/F7/H7������",16,0);			    	 
	Show_Str(60,70,200,16,"��Ƶ������ʵ��",16,0);				    	 
	Show_Str(60,90,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(60,110,200,16,"2018��8��19��",16,0);
	Show_Str(60,130,200,16,"KEY0:NEXT   KEY2:PREV",16,0); 
	Show_Str(60,150,200,16,"KEY_UP:FF   KEY1��REW",16,0);
	delay_ms(1500);
	TIM3_Int_Init(10000-1,20000-1);	//10Khz����,1�����ж�һ��
	while(1)
	{ 
		video_play();
	}
}















