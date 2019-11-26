#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "wkup.h"
//ALIENTEK ������STM32H7������ ʵ��19
//�������� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
  
int main(void)
{  
 	u8 led0sta=1;  
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200  
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
 	WKUP_Init();					//�������ѳ�ʼ��
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"WKUP TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	
	LCD_ShowString(30,130,200,16,16,"WK_UP:Stanby/WK_UP");	 
	while(1)
	{
		LED0(led0sta^=1);
		delay_ms(250);   
	}	
}


















