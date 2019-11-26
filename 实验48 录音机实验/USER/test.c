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
#include "wm8978.h"	 
#include "audioplay.h"	
#include "recorder.h"		 
//ALIENTEK ������STM32H7������ ʵ��48
//¼���� ʵ�� 
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
	WM8978_HPvol_Set(40,40);		//������������
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
	POINT_COLOR=RED;      
 	Show_Str(30,40,200,16,"������STM32F4/F7/H7������",16,0);	 				    	 
	Show_Str(30,60,200,16,"¼����ʵ��",16,0);				    	 
	Show_Str(30,80,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(30,100,200,16,"2018��8��17��",16,0); 
	while(1)
	{ 
		wav_recorder(); 
	} 
}













