#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "key.h"
#include "usmart.h"
#include "pcf8574.h"
//ALIENTEK ������STM32H7������ ʵ��26
//PCF8574 IO��չ ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
   
int main(void)
{  
 	u8 led0sta=1,led1sta=1;
	u8 key;
	u16 i=0; 
	u8 beepsta=1;		
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
	LCD_ShowString(30,70,200,16,16,"PCF8574 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/14");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:BEEP ON/OFF");	//��ʾ��ʾ��Ϣ	
	LCD_ShowString(30,150,200,16,16,"EXIO:DS1 ON/OFF");		//��ʾ��ʾ��Ϣ		
	while(PCF8574_Init())		//��ⲻ��PCF8574
	{
		LCD_ShowString(30,170,200,16,16,"PCF8574 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,170,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);//DS0��˸
	}
	LCD_ShowString(30,170,200,16,16,"PCF8574 Ready!");    
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	while(1)
	{
		key=KEY_Scan(0); 
		if(key==KEY0_PRES)//KEY0����,��ȡ�ַ�������ʾ
		{ 
			beepsta=!beepsta;					//������״̬ȡ��
			PCF8574_WriteBit(BEEP_IO,beepsta);	//���Ʒ�����
		}
		if(PCF8574_INT==0)				//PCF8574���жϵ͵�ƽ��Ч
		{
			key=PCF8574_ReadBit(EX_IO);	//��ȡEXIO״̬,ͬʱ���PCF8574���ж����(INT�ָ��ߵ�ƽ)
			if(key==0)LED1(led1sta^=1); //LED1״̬ȡ�� 
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//DS0��˸
			i=0;
		}		   
	} 	    
}













