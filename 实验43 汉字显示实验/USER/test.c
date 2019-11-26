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
//ALIENTEK ������STM32H7������ ʵ��43
//������ʾ ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  

 
int main(void)
{  
	u8 led0sta=1;
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];//gbk��
	u8 key,t;
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
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
	exfuns_init();					//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 			//����SD�� 
 	f_mount(fs[1],"1:",1); 			//����FLASH. 
	while(font_init()) 				//����ֿ�
	{
UPD:    
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7/H7");
		while(SD_Init())			//���SD��
		{
			LCD_ShowString(30,50,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,50,200+30,50+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,50,200,16,16,"SD Card OK");
		LCD_ShowString(30,70,200,16,16,"Font Updating...");
		key=update_font(20,90,16,"0:");//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(30,90,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,90,200+20,90+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,90,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//����	       
	}  
	POINT_COLOR=RED;       
	Show_Str(30,30,200,16,"������STM32F4/F7/H7������",16,0);				    	 
	Show_Str(30,50,200,16,"GBK�ֿ���Գ���",16,0);				    	 
	Show_Str(30,70,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(30,90,200,16,"2018��8��2��",16,0);
	Show_Str(30,110,200,16,"��KEY0,�����ֿ�",16,0);
 	POINT_COLOR=BLUE;  
	Show_Str(30,130,200,16,"������ֽ�:",16,0);				    	 
	Show_Str(30,150,200,16,"������ֽ�:",16,0);				    	 
	Show_Str(30,170,200,16,"���ּ�����:",16,0);

	Show_Str(30,200,200,32,"��Ӧ����Ϊ:",32,0); 
	Show_Str(30,232,200,24,"��Ӧ����Ϊ:",24,0); 
	Show_Str(30,256,200,16,"��Ӧ����(16*16)Ϊ:",16,0);			 
	Show_Str(30,272,200,12,"��Ӧ����(12*12)Ϊ:",12,0);			 
	while(1)
	{
		fontcnt=0;
		for(i=0x81;i<0xff;i++)
		{		
			fontx[0]=i;
			LCD_ShowNum(118,130,i,3,16);		//��ʾ������ֽ�    
			for(j=0x40;j<0xfe;j++)
			{
				if(j==0x7f)continue;
				fontcnt++;
				LCD_ShowNum(118,150,j,3,16);	//��ʾ������ֽ�	 
				LCD_ShowNum(118,170,fontcnt,5,16);//���ּ�����ʾ	 
			 	fontx[1]=j;
				Show_Font(30+176,200,fontx,32,0);
				Show_Font(30+132,232,fontx,24,0);	  
				Show_Font(30+144,256,fontx,16,0);	  		 		 
				Show_Font(30+108,272,fontx,12,0);	  		 		 
				t=200;
				while(t--)//��ʱ,ͬʱɨ�谴��
				{
					delay_ms(1);
					key=KEY_Scan(0);
					if(key==KEY0_PRES)goto UPD;
				}
				LED0(led0sta^=1);
			}   
		}	
	} 
}






