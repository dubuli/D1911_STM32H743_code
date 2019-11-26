#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "adc.h"    
#include "key.h"
#include "timer.h"
#include "usmart.h"
//ALIENTEK 阿波罗STM32H7开发板 实验23
//PWM DAC 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
//设置输出电压
//vol:0~330,代表0~3.3V
void PWM_DAC_Set(u16 vol)
{
	double temp=vol;
	temp/=100;
	temp=temp*256/3.3;
	PWM_DAC_VAL=temp;	   
} 

int main(void)
{  
 	u8 led0sta=1;
	u16 adcx;
	float temp;
 	u8 t=0;	 
	u16 pwmval=0;
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
	TIM15_CH2_PWM_Init(255,1);		//TIM15 PWM初始化, Fpwm=200M/2/256=390.625Khz.
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"PWM DAC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/13");	  
	LCD_ShowString(30,130,200,16,16,"WK_UP:+  KEY1:-");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	LCD_ShowString(30,150,200,16,16,"DAC VAL:");	      
	LCD_ShowString(30,170,200,16,16,"DAC VOL:0.000V");	      
	LCD_ShowString(30,190,200,16,16,"ADC VOL:0.000V");
	PWM_DAC_VAL=pwmval;//初始值为0	    	      
	while(1)
	{
		t++;
		key=KEY_Scan(0);			  
		if(key==WKUP_PRES)
		{		 
			if(pwmval<250)pwmval+=10;
			PWM_DAC_VAL=pwmval;		//输出	
		}else if(key==KEY1_PRES)	
		{
			if(pwmval>10)pwmval-=10;
			else pwmval=0;
			PWM_DAC_VAL=pwmval; 		//输出
		}	 
		if(t==10||key==KEY1_PRES||key==WKUP_PRES) 	//WKUP/KEY1按下了,或者定时时间到了
		{	  
 			adcx=PWM_DAC_VAL;
			LCD_ShowxNum(94,150,adcx,3,16,0);     	//显示DAC寄存器值
			temp=(float)adcx*(3.3/256);;			//得到DAC电压值
			adcx=temp;
 			LCD_ShowxNum(94,170,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,170,temp,3,16,0x80); 	//显示电压值的小数部分
 			adcx=Get_Adc_Average(ADC1_CH19,20);  	//得到ADC转换值	  
			temp=(float)adcx*(3.3/65536);			//得到ADC电压值
			adcx=temp;
 			LCD_ShowxNum(94,190,temp,1,16,0);     	//显示电压值整数部分
 			temp-=adcx;
			temp*=1000;
			LCD_ShowxNum(110,190,temp,3,16,0x80); 	//显示电压值的小数部分
			t=0;
			LED0(led0sta^=1);	   
		}	    
		delay_ms(10); 
	}	
}


















