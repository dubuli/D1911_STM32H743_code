#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "adc.h"    
#include "key.h"
#include "dac.h"
#include "usmart.h"
//ALIENTEK 阿波罗STM32H7开发板 实验22
//DAC 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
int main(void)
{  
 	u8 led0sta=1;
	u16 adcx;
	float temp;
 	u8 t=0;	 
	u16 dacval=0;
	u8 key;	 
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200  
	usmart_dev.init(200);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
	KEY_Init(); 					//按键初始化
	Adc_Init(); 					//初始化ADC
	Dac1_Init();		 			//DAC通道1初始化	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/12");	  
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
	DAC1->DHR12R1=dacval;			//初始值为0	    		
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(dacval<4000)dacval+=200;
			DAC1->DHR12R1=dacval;		//输出	
		}else if(key==KEY1_PRES)	
		{
			if(dacval>200)dacval-=200;
			else dacval=0;
			DAC1->DHR12R1=dacval; 		//输出
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1按下了,或者定时时间到了
		{	  
 			adcx=DAC1->DHR12R1;
			LCD_ShowxNum(94,150,adcx,4,16,0);     	//显示DAC寄存器值
			temp=(float)adcx*(3.3/4096);			//得到DAC电压值
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0X80); 	//显示电压值的小数部分
 			adcx=Get_Adc_Average(ADC1_CH19,20);		//得到ADC转换值	  
			temp=(float)adcx*(3.3/65536);			//得到ADC电压值
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0X80); 	//显示电压值的小数部分
			LED0(led0sta^=1);	   
			t=0;
		}	    
		delay_ms(10);	 
	}
}


















