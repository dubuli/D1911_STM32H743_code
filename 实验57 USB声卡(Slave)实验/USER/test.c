#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h"   
#include "pcf8574.h" 
#include "malloc.h"     
#include "wm8978.h"	 
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usbd_audio_if.h"	 
//ALIENTEK ������STM32H7������ ʵ��57
//USB����(Slave) ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾    


USBD_HandleTypeDef USBD_Device;	//USB Device����ṹ��  

extern vu8 bDeviceState;		//USB���� ���
extern u8 volume;				//����(��ͨ����������)

int main(void)
{   
	u8 led0sta=1;
	u8 key;
	u8 t=0;
	u8 Divece_STA=0XFF;
	
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz 
	delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	usmart_dev.init(100);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
	KEY_Init();						//��ʼ������
	PCF8574_Init();					//��ʼ��PCF8574
	WM8978_Init();					//��ʼ��WM8978
	WM8978_ADDA_Cfg(1,0);			//����DAC
	WM8978_Input_Cfg(0,0,0);		//�ر�����ͨ��
	WM8978_Output_Cfg(1,0);			//����DAC���  
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
	POINT_COLOR=RED;      
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");	
	LCD_ShowString(30,70,200,16,16,"USB Sound Card TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/8/31");	
	LCD_ShowString(30,130,200,16,16,"KEY2:Vol-  KEY0:vol+");	
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
 	LCD_ShowString(30,160,200,16,16,"VOLUME:");		 //������ʾ 	  
	LCD_ShowxNum(30+56,160,AUDIO_DEFAULT_VOLUME,3,16,0X80);//��ʾ����
	LCD_ShowString(30,180,200,16,16,"USB Connecting...");//��ʾ���ڽ������� 

    USBD_Init(&USBD_Device,&AUDIO_Desc,0);
    USBD_RegisterClass(&USBD_Device,USBD_AUDIO_CLASS);
    USBD_AUDIO_RegisterInterface(&USBD_Device,&USBD_AUDIO_fops);
    USBD_Start(&USBD_Device);
	
	while(1)
	{
		key=KEY_Scan(1);	//֧������
		if(key)
		{
			if(key==KEY0_PRES)		//KEY0����,��������
			{
				volume++;
				if(volume>100)volume=100; 
			}else if(key==KEY2_PRES)//KEY2����,��������
			{ 
				if(volume)volume--;
				else volume=0;
			} 
            WM8978_HPvol_Set(volume*0.63,volume*0.63);
            WM8978_SPKvol_Set(volume*0.63);
			LCD_ShowxNum(30+56,160,volume,3,16,0X80);//��ʾ���� 
			delay_ms(20);
		} 
		if(Divece_STA!=bDeviceState)//״̬�ı���
		{
			if(bDeviceState==1)
			{
				LED1(0);
				LCD_ShowString(30,180,200,16,16,"USB Connected    ");//��ʾUSB�����Ѿ�����
			}else 
			{
				LED1(1);
				LCD_ShowString(30,180,200,16,16,"USB DisConnected ");//��ʾUSB����ʧ��
			}
			Divece_STA=bDeviceState;
		}	
		delay_ms(20);
		t++;
		if(t>10)
		{
			t=0;
			LED0(led0sta^=1);//��ʾϵͳ������
            //printf("USBD_Device.dev_connection_status=%d\r\n",USBD_Device.dev_connection_status);
		}
	}
}












