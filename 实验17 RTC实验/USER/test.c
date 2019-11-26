#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"    
#include "lcd.h" 
#include "sdram.h"  
#include "usmart.h" 
#include "rtc.h" 
//ALIENTEK 阿波罗STM32H7开发板 实验17
//RTC 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
int main(void)
{  
 	u8 led0sta=1; 
	u8 hour,min,sec,ampm;
	u8 year,month,date,week;
	u8 tbuf[40];
	u8 t=0;  
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200 
	usmart_dev.init(200); 			//初始化USMART	
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
	RTC_Init();		 				//初始化RTC
	RTC_Set_WakeUp(4,0);			//配置WAKE UP中断,1秒钟中断一次 
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	
  	while(1) 
	{		
		t++;
		if((t%10)==0)	//每100ms更新一次显示数据
		{
			RTC_Get_Time(&hour,&min,&sec,&ampm);
			sprintf((char*)tbuf,"Time:%02d:%02d:%02d",hour,min,sec); 
			LCD_ShowString(30,140,210,16,16,tbuf);	
			RTC_Get_Date(&year,&month,&date,&week);
			sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",year,month,date); 
			LCD_ShowString(30,160,210,16,16,tbuf);	
			sprintf((char*)tbuf,"Week:%d",week); 
			LCD_ShowString(30,180,210,16,16,tbuf);
		} 
		if((t%20)==0)LED0(led0sta^=1);//每200ms,翻转一次LED0 
		delay_ms(10);
	}	
}


















