#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "adc.h"
//ALIENTEK ������STM32H7������ ʵ��21
//�ڲ��¶ȴ����� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
  
int main(void)
{  
 	u8 led0sta=1;
	short temp; 
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200  
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
 	Adc3_Init();					//��ʼ��ADC3,�¶ȴ�����ͨ��
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"Temperature TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/11");	  
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(30,140,200,16,16,"TEMPERATE: 00.00C");	      
	while(1)
	{
		temp=Get_Temprate();	//�õ��¶�ֵ 
		if(temp<0)
		{
			temp=-temp;
			LCD_ShowString(30+10*8,140,16,16,16,"-");	//��ʾ����
		}else LCD_ShowString(30+10*8,140,16,16,16," ");	//�޷���
		
		LCD_ShowxNum(30+11*8,140,temp/100,2,16,0);		//��ʾ��������
		LCD_ShowxNum(30+14*8,140,temp%100,2,16,0X80);	//��ʾС������ 
		LED0(led0sta^=1);
		delay_ms(250);	
	}
}


















