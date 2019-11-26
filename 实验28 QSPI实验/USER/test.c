#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "key.h"
#include "usmart.h"
#include "w25qxx.h" 
//ALIENTEK ������STM32H7������ ʵ��28
//QSPI ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
   
//Ҫд�뵽W25Q256���ַ�������
const u8 TEXT_Buffer[]={"Apollo STM32H7 QSPI TEST"};
#define SIZE sizeof(TEXT_Buffer)	 
	
int main(void)
{   
 	u8 led0sta=1; 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 flassize;			
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200  
	usmart_dev.init(200);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
	KEY_Init(); 					//������ʼ�� 
	W25QXX_Init();					//W25QXX��ʼ��
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"QSPI TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/14");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//��ʾ��ʾ��Ϣ		
	while(W25QXX_ReadID()!=W25Q256)								//��ⲻ��W25Q256
	{
		LCD_ShowString(30,150,200,16,16,"W25Q256 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);		//DS0��˸
	}
	LCD_ShowString(30,150,200,16,16,"W25Q256 Ready!"); 
	flassize=32*1024*1024;		//FLASH ��СΪ32M�ֽ�
  	POINT_COLOR=BLUE;			//��������Ϊ��ɫ	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1����,д��W25Q256
		{
			LCD_Fill(0,170,239,319,WHITE);//�������    
 			LCD_ShowString(30,170,200,16,16,"Start Write W25Q256....");
			W25QXX_Write((u8*)TEXT_Buffer,flassize-100,SIZE);			//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
			LCD_ShowString(30,170,200,16,16,"W25Q256 Write Finished!");	//��ʾ�������
		}
		if(key==KEY0_PRES)//KEY0����,��ȡ�ַ�������ʾ
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read W25Q256.... ");
			W25QXX_Read(datatemp,flassize-100,SIZE);					//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:   ");	//��ʾ�������
			LCD_ShowString(30,190,200,16,16,datatemp);					//��ʾ�������ַ���
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);	//DS0��˸
			i=0;
		}		   
	}	  
}













