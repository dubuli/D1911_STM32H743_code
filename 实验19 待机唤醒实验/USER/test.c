#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "wkup.h"
//ALIENTEK 阿波罗STM32H7开发板 实验19
//待机唤醒 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
int main(void)
{  
 	u8 led0sta=1;  
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200  
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
 	WKUP_Init();					//待机唤醒初始化
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"WKUP TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	
	LCD_ShowString(30,130,200,16,16,"WK_UP:Stanby/WK_UP");	 
	while(1)
	{
		LED0(led0sta^=1);
		delay_ms(250);   
	}	
}


















