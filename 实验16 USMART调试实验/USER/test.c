#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"  
#include "lcd.h" 
#include "sdram.h"  
#include "usmart.h" 
//ALIENTEK ������STM32H7������ ʵ��16
//USMART���� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
 
//LED״̬���ú���
void led_set(u8 sta)
{
	LED1(sta);
} 
//�����������ò��Ժ���
void test_fun(void(*ledset)(u8),u8 sta)
{
	ledset(sta);
} 

int main(void)
{  
 	u8 led0sta=1;
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200 
	usmart_dev.init(200); 			//��ʼ��USMART	
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
	POINT_COLOR=RED;				//��ɫ����
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"USMART TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	   
  	while(1) 
	{		 	  
		LED0(led0sta^=1);			//LED0��˸					 
		delay_ms(500);	
	} 
}


















