#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "dht11.h"
#include "pcf8574.h"
//ALIENTEK ������STM32H7������ ʵ��34
//DHT11������ʪ�ȴ����� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  

int main(void)
{  
	u8 led0sta=1;
	u8 t=0;			        
	u8 temperature;  	    
	u8 humidity;  
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
	PCF8574_Init();					//��ʼ��PCF8574
	PCF8574_ReadBit(EX_IO);			//��һ��,�ͷ�INT��,��ֹ���ŵ�����  
 	POINT_COLOR=RED;				//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");
	LCD_ShowString(30,70,200,16,16,"DHT11 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/7/16");	 		 		
 	while(DHT11_Init())				//DHT11��ʼ��	
	{
		LCD_ShowString(30,130,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}								   
	LCD_ShowString(30,130,200,16,16,"DHT11 OK");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
 	LCD_ShowString(30,150,200,16,16,"Temp:  C");	 
 	LCD_ShowString(30,170,200,16,16,"Humi:  %");
	while(1)
	{	    	    
 		if(t%10==0)//ÿ100ms��ȡһ��
		{									  
			DHT11_Read_Data(&temperature,&humidity);		//��ȡ��ʪ��ֵ					    
			LCD_ShowNum(30+40,150,temperature,2,16);		//��ʾ�¶�	   		   
			LCD_ShowNum(30+40,170,humidity,2,16);			//��ʾʪ��	 	   
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

















