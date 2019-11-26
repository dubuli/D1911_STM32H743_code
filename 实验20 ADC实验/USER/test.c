#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "adc.h"
//ALIENTEK 阿波罗STM32H7开发板 实验20
//ADC 实验   
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
int main(void)
{  
 	u8 led0sta=1;  
 	u16 adcx;
	float temp;
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200  
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
 	Adc_Init();						//初始化ADC
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	  
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(30,130,200,16,16,"ADC1_CH19_VAL:");	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH19_VOL:0.000V");//先在固定位置显示小数点  	
    while(1)
	{
        adcx=Get_Adc_Average(ADC1_CH19,20);		//获取通道5的转换值，20次取平均
		LCD_ShowxNum(142,130,adcx,5,16,0);		//显示ADCC采样后的原始值
		temp=(float)adcx*(3.3/65536);			//获取计算后的带小数的实际电压值，比如3.1111
		adcx=temp;								//赋值整数部分给adcx变量，因为adcx为u16整形
		LCD_ShowxNum(142,150,adcx,1,16,0);		//显示电压值的整数部分，3.1111的话，这里就是显示3
		temp-=adcx;								//把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111
		temp*=1000;								//小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。
		LCD_ShowxNum(158,150,temp,3,16,0X80);	//显示小数部分（前面转换为了整形显示），这里显示的就是111.
		LED0(led0sta^=1);
		delay_ms(250);	
	}
}


















