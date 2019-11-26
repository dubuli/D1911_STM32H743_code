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
//ALIENTEK 阿波罗STM32H7开发板 实验55
//串口IAP 实验-IAP Bootloader V1.0 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
int main(void)
{  
 	u8 led0sta=1;
	u8 t;
	u8 key;
	u32 oldcount=0;				//老的串口接收数据值
	u32 applenth=0;				//接收到的app代码长度
	u8 clearflag=0; 		
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200 
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	KEY_Init();						//初始化LED
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
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
	//显示提示信息
	POINT_COLOR=BLUE;//设置字体为蓝色	  
	while(1)
	{
	 	if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("用户程序接收完成!\r\n");
				printf("代码长度:%dBytes\r\n",applenth);
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
				if(clearflag==0)LCD_Fill(30,210,240,210+16,WHITE);//清除显示
			}
		}	  	 
		key=KEY_Scan(0);
		if(key==WKUP_PRES)	//WK_UP按键按下
		{
			if(applenth)
			{
				printf("开始更新固件...\r\n");	
				LCD_ShowString(30,210,200,16,16,"Copying APP2FLASH...");
 				if(((*(vu32*)(0x24001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//更新FLASH代码   
					LCD_ShowString(30,210,200,16,16,"Copy APP Successed!!");
					printf("固件更新完成!\r\n");	
				}else 
				{
					LCD_ShowString(30,210,200,16,16,"Illegal FLASH APP!  ");	   
					printf("非FLASH应用程序!\r\n");
				}
 			}else 
			{
				printf("没有可以更新的固件!\r\n");
				LCD_ShowString(30,210,200,16,16,"No APP!");
			}
			clearflag=7;//标志更新了显示,并且设置7*300ms后清除显示									 
		}
		if(key==KEY1_PRES)	//KEY1按下
		{
			if(applenth)
			{																	 
				printf("固件清除完成!\r\n");    
				LCD_ShowString(30,210,200,16,16,"APP Erase Successed!");
				applenth=0;
			}else  
			{
				printf("没有可以清除的固件!\r\n");
				LCD_ShowString(30,210,200,16,16,"No APP!");
			}
			clearflag=7;//标志更新了显示,并且设置7*300ms后清除显示									 
		}
		if(key==KEY2_PRES)	//KEY2按下
		{
			printf("开始执行FLASH用户代码!!\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else 
			{
				printf("非FLASH应用程序,无法执行!\r\n");
				LCD_ShowString(30,210,200,16,16,"Illegal FLASH APP!");	   
			}									 
			clearflag=7;//标志更新了显示,并且设置7*300ms后清除显示	  
		}
		if(key==KEY0_PRES)	//KEY0按下
		{
			printf("开始执行SRAM用户代码!!\r\n");
			if(((*(vu32*)(0x24001000+4))&0xFF000000)==0x24000000)//判断是否为0X24XXXXXX.
			{	 
				iap_load_app(0x24001000);//SRAM地址
			}else 
			{
				printf("非SRAM应用程序,无法执行!\r\n");
				LCD_ShowString(30,210,200,16,16,"Illegal SRAM APP!");	   
			}									 
			clearflag=7;//标志更新了显示,并且设置7*300ms后清除显示	 
		}				   
		 
	}  
}
















