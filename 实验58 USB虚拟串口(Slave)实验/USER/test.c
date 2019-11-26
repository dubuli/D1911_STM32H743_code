#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h" 
#include "malloc.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h" 
#include "usbd_cdc_interface.h"
//ALIENTEK ������STM32H7������ ʵ��58
//USB���⴮��(Slave) ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  

USBD_HandleTypeDef USBD_Device;	//USB Device����ṹ��  
extern vu8 bDeviceState;		//USB���� ���


int main(void)
{  
 	u8 led0sta=1;
	u16 len;	
	u16 times=0;    
	u8 usbstatus=0;	
	
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	usmart_dev.init(100);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"USB Virtual USART TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/9/2");	 
 	LCD_ShowString(30,130,200,16,16,"USB Connecting...");//��ʾUSB��ʼ����
    
    USBD_Init(&USBD_Device,&VCP_Desc,0);
    USBD_RegisterClass(&USBD_Device,USBD_CDC_CLASS);
    USBD_CDC_RegisterInterface(&USBD_Device,&USBD_CDC_fops);
    USBD_Start(&USBD_Device);
    
 	while(1)
	{
		if(usbstatus!=bDeviceState)//USB����״̬�����˸ı�.
		{
			usbstatus=bDeviceState;//��¼�µ�״̬
			if(usbstatus==1)
			{
				POINT_COLOR=BLUE;
				LCD_ShowString(30,130,200,16,16,"USB Connected    ");//��ʾUSB���ӳɹ�
				LED1(0);//DS1��
			}else
			{
				POINT_COLOR=RED;
				LCD_ShowString(30,130,200,16,16,"USB disConnected ");//��ʾUSB�Ͽ�
				LED1(1);//DS1��
			}
		}
		if(USB_USART_RX_STA&0x8000)
		{					   
			len=USB_USART_RX_STA&0x3FFF;//�õ��˴ν��յ������ݳ���
			usb_printf("\r\n�����͵���Ϣ����Ϊ:%d\r\n\r\n",len);
            VCP_DataTx(USB_USART_RX_BUF,len);;
			usb_printf("\r\n\r\n");//���뻻��
			USB_USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				usb_printf("\r\n������STM32F4/F7/H7������USB���⴮��ʵ��\r\n");
				usb_printf("����ԭ��@ALIENTEK\r\n\r\n");
			}
			if(times%200==0)usb_printf("����������,�Իس�������\r\n");  
			if(times%30==0)LED0(led0sta^=1);//��˸LED,��ʾϵͳ��������.
			delay_ms(10);    
		}
	}  	
}




