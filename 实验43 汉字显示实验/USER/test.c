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
//ALIENTEK 阿波罗STM32H7开发板 实验43
//汉字显示 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  

 
int main(void)
{  
	u8 led0sta=1;
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];//gbk码
	u8 key,t;
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
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
	exfuns_init();					//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 			//挂载SD卡 
 	f_mount(fs[1],"1:",1); 			//挂载FLASH. 
	while(font_init()) 				//检查字库
	{
UPD:    
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7/H7");
		while(SD_Init())			//检测SD卡
		{
			LCD_ShowString(30,50,200,16,16,"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,50,200+30,50+16,WHITE);
			delay_ms(200);		    
		}								 						    
		LCD_ShowString(30,50,200,16,16,"SD Card OK");
		LCD_ShowString(30,70,200,16,16,"Font Updating...");
		key=update_font(20,90,16,"0:");//更新字库
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(30,90,200,16,16,"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,90,200+20,90+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,90,200,16,16,"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(WHITE);//清屏	       
	}  
	POINT_COLOR=RED;       
	Show_Str(30,30,200,16,"阿波罗STM32F4/F7/H7开发板",16,0);				    	 
	Show_Str(30,50,200,16,"GBK字库测试程序",16,0);				    	 
	Show_Str(30,70,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(30,90,200,16,"2018年8月2日",16,0);
	Show_Str(30,110,200,16,"按KEY0,更新字库",16,0);
 	POINT_COLOR=BLUE;  
	Show_Str(30,130,200,16,"内码高字节:",16,0);				    	 
	Show_Str(30,150,200,16,"内码低字节:",16,0);				    	 
	Show_Str(30,170,200,16,"汉字计数器:",16,0);

	Show_Str(30,200,200,32,"对应汉字为:",32,0); 
	Show_Str(30,232,200,24,"对应汉字为:",24,0); 
	Show_Str(30,256,200,16,"对应汉字(16*16)为:",16,0);			 
	Show_Str(30,272,200,12,"对应汉字(12*12)为:",12,0);			 
	while(1)
	{
		fontcnt=0;
		for(i=0x81;i<0xff;i++)
		{		
			fontx[0]=i;
			LCD_ShowNum(118,130,i,3,16);		//显示内码高字节    
			for(j=0x40;j<0xfe;j++)
			{
				if(j==0x7f)continue;
				fontcnt++;
				LCD_ShowNum(118,150,j,3,16);	//显示内码低字节	 
				LCD_ShowNum(118,170,fontcnt,5,16);//汉字计数显示	 
			 	fontx[1]=j;
				Show_Font(30+176,200,fontx,32,0);
				Show_Font(30+132,232,fontx,24,0);	  
				Show_Font(30+144,256,fontx,16,0);	  		 		 
				Show_Font(30+108,272,fontx,12,0);	  		 		 
				t=200;
				while(t--)//延时,同时扫描按键
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






