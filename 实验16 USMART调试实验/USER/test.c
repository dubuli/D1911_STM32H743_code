#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"  
#include "lcd.h" 
#include "sdram.h"  
#include "usmart.h" 
//ALIENTEK 阿波罗STM32H7开发板 实验16
//USMART调试 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
 
//LED状态设置函数
void led_set(u8 sta)
{
	LED1(sta);
} 
//函数参数调用测试函数
void test_fun(void(*ledset)(u8),u8 sta)
{
	ledset(sta);
} 

int main(void)
{  
 	u8 led0sta=1;
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200 
	usmart_dev.init(200); 			//初始化USMART	
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
	POINT_COLOR=RED;				//红色画笔
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"USMART TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	   
  	while(1) 
	{		 	  
		LED0(led0sta^=1);			//LED0闪烁					 
		delay_ms(500);	
	} 
}


















