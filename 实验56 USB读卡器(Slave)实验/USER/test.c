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
#include "nand.h"    
#include "ftl.h" 
#include "w25qxx.h" 
#include "pcf8574.h"       
#include "sdmmc_sdcard.h"  
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h" 
//ALIENTEK ������STM32H7������ ʵ��56
//USB������(Slave) ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  


USBD_HandleTypeDef USBD_Device;		//USB Device����ṹ�� 
extern vu8 USB_STATUS_REG;			//USB״̬
extern vu8 bDeviceState;			//USB���� ���
 
int main(void)
{   
	u8 led0sta=1;
	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	usmart_dev.init(100);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
	KEY_Init();						//��ʼ������ 
	W25QXX_Init();					//��ʼ��W25Q256 
	PCF8574_Init();					//��ʼ��PCF8574 
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"USB Card Reader TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/8/22");	
	if(SD_Init())LCD_ShowString(30,130,200,16,16,"SD Card Error!");	//���SD������
	else //SD ������
	{   															  
		LCD_ShowString(30,130,200,16,16,"SD Card Size:     MB"); 
 		LCD_ShowNum(134,130,SDCardInfo.CardCapacity>>20,5,16);	//��ʾSD������
 	}
	if(W25QXX_ReadID()!=W25Q256)LCD_ShowString(30,130,200,16,16,"W25Q128 Error!");	//���W25Q128����
	else //SPI FLASH ����
	{   														 
		LCD_ShowString(30,150,200,16,16,"SPI FLASH Size:25MB");	 
	} 
	if(FTL_Init())LCD_ShowString(30,170,200,16,16,"NAND Error!");	//���NAND����
	else //NAND FLASH ����
	{   														 
		LCD_ShowString(30,170,200,16,16,"NAND Flash Size:    MB"); 
 		LCD_ShowNum(158,170,nand_dev.valid_blocknum*nand_dev.block_pagenum*nand_dev.page_mainsize>>20,4,16);	//��ʾSD������
	}  
 	LCD_ShowString(30,190,200,16,16,"USB Connecting...");	//��ʾ���ڽ������� 	
    USBD_Init(&USBD_Device,&MSC_Desc,0);                        //��ʼ��USB
    USBD_RegisterClass(&USBD_Device,USBD_MSC_CLASS);            //�����
    USBD_MSC_RegisterStorage(&USBD_Device,&USBD_DISK_fops);     //ΪMSC����ӻص�����
    USBD_Start(&USBD_Device);                                   //����USB
	delay_ms(1800);	
	while(1)
	{	
		delay_ms(1);				  
		if(USB_STA!=USB_STATUS_REG)//״̬�ı��� 
		{	 						   
			LCD_Fill(30,210,240,210+16,WHITE);//�����ʾ			  	   
			if(USB_STATUS_REG&0x01)//����д		  
			{
				LED1(0);
				LCD_ShowString(30,210,200,16,16,"USB Writing...");//��ʾUSB����д������	 
			}
			if(USB_STATUS_REG&0x02)//���ڶ�
			{
				LED1(0);
				LCD_ShowString(30,210,200,16,16,"USB Reading...");//��ʾUSB���ڶ�������  		 
			}	 										  
			if(USB_STATUS_REG&0x04)LCD_ShowString(30,230,200,16,16,"USB Write Err ");//��ʾд�����
			else LCD_Fill(30,230,240,230+16,WHITE);//�����ʾ	  
			if(USB_STATUS_REG&0x08)LCD_ShowString(30,250,200,16,16,"USB Read  Err ");//��ʾ��������
			else LCD_Fill(30,250,240,250+16,WHITE);//�����ʾ    
			USB_STA=USB_STATUS_REG;//��¼����״̬
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)LCD_ShowString(30,190,200,16,16,"USB Connected    ");//��ʾUSB�����Ѿ�����
			else LCD_ShowString(30,190,200,16,16,"USB DisConnected ");//��ʾUSB���γ���
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED1(1);
			LED0(led0sta^=1);//��ʾϵͳ������
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB������,�����offline������
				bDeviceState=1;
			}else//û�еõ���ѯ 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=0;//2s��û�յ����߱��,����USB���γ���
			}
			USB_STATUS_REG=0;
		}
	} 
}










