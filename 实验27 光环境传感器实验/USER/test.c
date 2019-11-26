#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "key.h"
#include "usmart.h"
#include "ap3216c.h"
//ALIENTEK ������STM32H7������ ʵ��27
//AP3216C ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
   
int main(void)
{   
 	u8 led0sta=1;
 	u16 ir,als,ps;			
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200  
	usmart_dev.init(200);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
	KEY_Init(); 					//������ʼ�� 
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"AP3216C TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/14");	 		
	while(AP3216C_Init())		//��ⲻ��AP3216C
	{
		LCD_ShowString(30,130,200,16,16,"AP3216C Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,130,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);		//DS0��˸
	}
	LCD_ShowString(30,130,200,16,16,"AP3216C Ready!");  
    LCD_ShowString(30,160,200,16,16," IR:");	 
 	LCD_ShowString(30,180,200,16,16," PS:");	
 	LCD_ShowString(30,200,200,16,16,"ALS:");	 
 	POINT_COLOR=BLUE;			//��������Ϊ��ɫ		  
    while(1)
    {
        AP3216C_ReadData(&ir,&ps,&als);	//��ȡ���� 
        LCD_ShowNum(30+32,160,ir,5,16);	//��ʾIR����
        LCD_ShowNum(30+32,180,ps,5,16);	//��ʾPS����
        LCD_ShowNum(30+32,200,als,5,16);//��ʾALS���� 
		LED0(led0sta^=1);				//��ʾϵͳ��������	
        delay_ms(120); 
	}	  
}













