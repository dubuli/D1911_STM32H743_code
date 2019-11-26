#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "ltdc.h"  
//ALIENTEK 阿波罗STM32H7开发板 实验15
//LTDC LCD（RGB屏）实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
int main(void)
{  
 	u8 x=0,led0sta=1;
	u8 lcd_id[12];	
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200 
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
	POINT_COLOR=RED;     
	sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id);//将LCD ID打印到lcd_id数组。				 	
  	while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break; 
			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(10,40,240,32,32,"Apollo STM32"); 	
		LCD_ShowString(10,80,240,24,24,"LTDC LCD TEST");
		LCD_ShowString(10,110,240,16,16,"ATOM@ALIENTEK");
 		LCD_ShowString(10,130,240,16,16,lcd_id);		//显示LCD ID	      					 
		LCD_ShowString(10,150,240,12,12,"2018/06/08");	      					 
	    x++;
		if(x==12)x=0;
		LED0(led0sta^=1);	//LED0闪烁	 
		delay_ms(1000);	
	}
}


















