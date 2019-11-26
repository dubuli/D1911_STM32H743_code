#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"    
#include "lcd.h" 
#include "sdram.h"  
#include "usmart.h" 
#include "rtc.h" 
//ALIENTEK ������STM32H7������ ʵ��17
//RTC ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
  
int main(void)
{  
 	u8 led0sta=1; 
	u8 hour,min,sec,ampm;
	u8 year,month,date,week;
	u8 tbuf[40];
	u8 t=0;  
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200 
	usmart_dev.init(200); 			//��ʼ��USMART	
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
	RTC_Init();		 				//��ʼ��RTC
	RTC_Set_WakeUp(4,0);			//����WAKE UP�ж�,1�����ж�һ�� 
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	
  	while(1) 
	{		
		t++;
		if((t%10)==0)	//ÿ100ms����һ����ʾ����
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
		if((t%20)==0)LED0(led0sta^=1);//ÿ200ms,��תһ��LED0 
		delay_ms(10);
	}	
}


















