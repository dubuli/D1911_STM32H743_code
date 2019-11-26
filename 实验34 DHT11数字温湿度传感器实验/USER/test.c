#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "dht11.h"
#include "pcf8574.h"
//ALIENTEK 阿波罗STM32H7开发板 实验34
//DHT11数字温湿度传感器 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  

int main(void)
{  
	u8 led0sta=1;
	u8 t=0;			        
	u8 temperature;  	    
	u8 humidity;  
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	PCF8574_Init();					//初始化PCF8574
	PCF8574_ReadBit(EX_IO);			//读一次,释放INT脚,防止干扰单总线  
 	POINT_COLOR=RED;				//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");
	LCD_ShowString(30,70,200,16,16,"DHT11 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/7/16");	 		 		
 	while(DHT11_Init())				//DHT11初始化	
	{
		LCD_ShowString(30,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(30,130,200,16,16,"DHT11 OK");
	POINT_COLOR=BLUE;//设置字体为蓝色 
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	while(1)
	{	    	    
 		if(t%10==0)//每100ms读取一次
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//读取温湿度值					    
			LCD_ShowNum(30+40,150,temperature,2,16);		//显示温度	   		   
			LCD_ShowNum(30+40,170,humidity,2,16);			//显示湿度	 	   
		}				   
	 	delay_ms(10);
		t++;
		if(t==20)
		{
			t=0;
			LED0(led0sta^=1);
		}
	}
}

















