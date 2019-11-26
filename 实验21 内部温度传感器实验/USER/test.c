#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "adc.h"
//ALIENTEK 阿波罗STM32H7开发板 实验21
//内部温度传感器 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
int main(void)
{  
 	u8 led0sta=1;
	short temp; 
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200  
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
 	Adc3_Init();					//初始化ADC3,温度传感器通道
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"Temperature TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/11");	  
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(30,140,200,16,16,"TEMPERATE: 00.00C");	      
	while(1)
	{
		temp=Get_Temprate();	//得到温度值 
		if(temp<0)
		{
			temp=-temp;
			LCD_ShowString(30+10*8,140,16,16,16,"-");	//显示负号
		}else LCD_ShowString(30+10*8,140,16,16,16," ");	//无符号
		
		LCD_ShowxNum(30+11*8,140,temp/100,2,16,0);		//显示整数部分
		LCD_ShowxNum(30+14*8,140,temp%100,2,16,0X80);	//显示小数部分 
		LED0(led0sta^=1);
		delay_ms(250);	
	}
}


















