#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "adc.h"
//ALIENTEK ������STM32H7������ ʵ��20
//ADC ʵ��   
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
  
int main(void)
{  
 	u8 led0sta=1;  
 	u16 adcx;
	float temp;
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200  
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
 	Adc_Init();						//��ʼ��ADC
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"ADC TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/10");	  
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(30,130,200,16,16,"ADC1_CH19_VAL:");	      
	LCD_ShowString(30,150,200,16,16,"ADC1_CH19_VOL:0.000V");//���ڹ̶�λ����ʾС����  	
    while(1)
	{
        adcx=Get_Adc_Average(ADC1_CH19,20);		//��ȡͨ��5��ת��ֵ��20��ȡƽ��
		LCD_ShowxNum(142,130,adcx,5,16,0);		//��ʾADCC�������ԭʼֵ
		temp=(float)adcx*(3.3/65536);			//��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
		adcx=temp;								//��ֵ�������ָ�adcx��������ΪadcxΪu16����
		LCD_ShowxNum(142,150,adcx,1,16,0);		//��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3
		temp-=adcx;								//���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111
		temp*=1000;								//С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС����
		LCD_ShowxNum(158,150,temp,3,16,0X80);	//��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111.
		LED0(led0sta^=1);
		delay_ms(250);	
	}
}


















