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
//ALIENTEK 阿波罗STM32H7开发板 实验48
//录音机 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司    

int main(void)
{   
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz 
	delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	KEY_Init();						//初始化按键
	W25QXX_Init();					//初始化W25Q256 
	WM8978_Init();					//初始化WM8978
	WM8978_HPvol_Set(40,40);		//耳机音量设置
	WM8978_SPKvol_Set(50);			//喇叭音量设置
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
	exfuns_init();					//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 			//挂载SD卡  
	POINT_COLOR=RED;      
	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
	POINT_COLOR=RED;      
 	Show_Str(30,40,200,16,"阿波罗STM32F4/F7/H7开发板",16,0);	 				    	 
	Show_Str(30,60,200,16,"录音机实验",16,0);				    	 
	Show_Str(30,80,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(30,100,200,16,"2018年8月17日",16,0); 
	while(1)
	{ 
		wav_recorder(); 
	} 
}













