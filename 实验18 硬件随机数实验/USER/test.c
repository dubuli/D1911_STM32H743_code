#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"  
#include "usmart.h" 
#include "key.h" 
#include "rng.h"
//ALIENTEK 阿波罗STM32H7开发板 实验18
//硬件随机数(RNG) 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
int main(void)
{  
 	u8 led0sta=1; 
	u32 random;
	u8 t=0,key;
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200 
	usmart_dev.init(200); 			//初始化USMART	
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	KEY_Init();						//按键初始化
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"RNG TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	 
	while(RNG_Init())	 		//初始化随机数发生器
	{
		LCD_ShowString(30,130,200,16,16,"  RNG Error! ");	 
		delay_ms(200);
		LCD_ShowString(30,130,200,16,16,"RNG Trying...");	 
	}                                 
	LCD_ShowString(30,130,200,16,16,"RNG Ready!   ");	 
	LCD_ShowString(30,150,200,16,16,"KEY0:Get Random Num");	 
	LCD_ShowString(30,180,200,16,16,"Random Num:");	 
 	LCD_ShowString(30,210,200,16,16,"Random Num[0-9]:");
 	POINT_COLOR=BLUE;
	while(1) 
	{		
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			random=RNG_Get_RandomNum(); 
			LCD_ShowNum(30+8*11,180,random,10,16); 
		} 
		if((t%20)==0)
		{
			LED0(led0sta^=1);						//每200ms,翻转一次LED0 
			random=RNG_Get_RandomRange(0,9);		//获取[0,9]区间的随机数
			LCD_ShowNum(30+8*16,210,random,1,16); 	//显示随机数
		}
		delay_ms(10);
		t++;
	}		
}


















