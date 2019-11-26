#include "usb_app.h" 
#include "delay.h"   
#include "malloc.h"    
#include "exfuns.h"      
#include "includes.h"      
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//USB-APP ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   


USBH_HandleTypeDef  hUSBHost; 	//USB Host����ṹ�� 
USBD_HandleTypeDef  hUSBDevice;	//USB Device����ṹ�� 

_usb_app usbx;//USB APP������ 


//USB OTG �жϷ�����
//��������USB�ж�
void OTG_FS_IRQHandler(void)
{ 
	OSIntEnter();    		    
	if(usbx.mode==2) 	//USB�ӻ�ģʽ��USB��������
	{  
		HAL_PCD_IRQHandler(&hpcd);	
	}else				//USB����ģʽ
	{
		HAL_HCD_IRQHandler(&hhcd);
	}
	OSIntExit();  											 
}  

//USB����ͨ�Ŵ�����
//phost:USBH���ָ��
//id:USBH��ǰ���û�״̬
void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
{ 
	u8 dev_type=0XFF;	//�豸����,1,����;2,���;����,��֧�ֵ��豸.
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_DISCONNECTION: 
            break;
        case HOST_USER_CLASS_ACTIVE:		//��ִ�е���һ����˵��USB HOST�Բ����豸�Ѿ�ʶ��ɹ�
			usbx.bDeviceState|=0X80;		//���������
			if(usbx.mode==USBH_HID_MODE)	//HIDģʽ��,���Խ���:���/����/�ֱ�
			{
				dev_type=phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bInterfaceProtocol;
				if(dev_type==HID_KEYBRD_BOOT_CODE)		//�����豸
				{  
					usbx.hdevclass=3;		//��⵽���̲���
				}else if(dev_type==HID_MOUSE_BOOT_CODE)	//����豸
				{ 
					usbx.hdevclass=2;		//��⵽������
				}else if(dev_type==0)		//�ֱ��豸
				{ 
					usbx.hdevclass=4;		//��⵽�ֱ�����
				}else usbx.hdevclass=0;		//��֧�ֵ��豸
			}else usbx.hdevclass=1;			//��⵽U�̲���
            break;
        case HOST_USER_CONNECTION:
            break;
        default:
            break;
    }
}

//USB����/�����ʾdemo���ݴ���
//phost:USBH���ָ��
void USB_Demo(USBH_HandleTypeDef * phost)
{  
	if(usbx.hdevclass==3)				//�����豸
	{  
		USBH_HID_GetKeybdInfo(phost);	//ת����ASCII�� 
	}else if(usbx.hdevclass==4)			//USB��Ϸ�ֱ��豸
	{  
		USBH_HID_GetGamePadInfo(phost);	//��ȡ�ֱ���Ϣ  
	} 
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��ʼ��USB
void usbapp_init(void)
{
	usbx.bDeviceState=0;
	usbx.hdevclass=0;
	usbx.mode=0XFF; 	//����Ϊһ���Ƿ���ģʽ,�����ȵ���usbapp_mode_set����ģʽ,������������
} 
 
//USB��ѯ����,���������Եı�����.
void usbapp_pulling(void)
{
	switch(usbx.mode)
	{
		case USBH_MSC_MODE: 			//USB HOST U��������
			USBH_Process(&hUSBHost); 	
			if(hUSBHost.gState==HOST_ABORT_STATE)	//��ֹ�����ֱ�/����,����usbapp_pulling�Ͳ�ѯ
			{
				usbx.bDeviceState=0;
				usbx.hdevclass=0; 
			}
			break;
		case USBH_HID_MODE: 			//USB HOST HID������
			USBH_Process(&hUSBHost); 	
			USB_Demo(&hUSBHost);
			break;  	
		case USBD_MSC_MODE:
			break;	  
	}	
}

//USB������ǰ����ģʽ
void usbapp_mode_stop(void)
{
	switch(usbx.mode)
	{
		case USBH_MSC_MODE:  
			USBH_MSC_InterfaceDeInit(&hUSBHost);//��λUSB HOST MSC��
			USBH_DeInit(&hUSBHost);				//��λUSB HOST 
			break;
		case USBH_HID_MODE:  
			USBH_HID_InterfaceDeInit(&hUSBHost);//��λUSB HOST HID��
			USBH_DeInit(&hUSBHost);				//��λUSB HOST   
			break;		
		case USBD_MSC_MODE:
			USBD_MSC_DeInit(&hUSBDevice,0);		//��λUSB SLAVE MSC��
			USBD_DeInit(&hUSBDevice);			//��λUSB SLAVE 
			break;	 
	} 
	RCC->AHB1RSTR|=1<<27;		//USB2 OTG FS ��λ
	delay_ms(5);
	RCC->AHB1RSTR&=~(1<<27);	//��λ����   
	memset(&hUSBHost,0,sizeof(USBH_HandleTypeDef));
	memset(&hUSBDevice,0,sizeof(USBD_HandleTypeDef));
	usbx.mode=0XFF;
	usbx.bDeviceState=0;
	usbx.hdevclass=0; 
}

//����USB����ģʽ
//mode:0,USB HOST MSCģʽ(Ĭ��ģʽ,��U��)
//	   1,USB HOST HIDģʽ(���������̵�)
//	   2,USB Device MSCģʽ(USB������) 
void usbapp_mode_set(u8 mode)
{
	usbapp_mode_stop();//��ֹͣ��ǰUSB����ģʽ
	usbx.mode=mode;
	switch(usbx.mode)
	{
		case USBH_MSC_MODE: 
			USBH_Init(&hUSBHost, USBH_UserProcess, 0);				//��ʼ��USB HOST
			USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);			//���USB HOST MSC��
			USBH_Start(&hUSBHost);  								//����USB HOST	
			break;
		case USBH_HID_MODE:  
			USBH_Init(&hUSBHost, USBH_UserProcess, 0);				//��ʼ��USB HOST
			USBH_RegisterClass(&hUSBHost,USBH_HID_CLASS);			//���USB HOST HID��
			USBH_Start(&hUSBHost); 									//����USB HOST		
			break;		
		case USBD_MSC_MODE:
			USBD_Init(&hUSBDevice,&MSC_Desc,0);						//��ʼ��USB SLAVE
			USBD_RegisterClass(&hUSBDevice,USBD_MSC_CLASS);			//���USB SLAVE MSC��
			USBD_MSC_RegisterStorage(&hUSBDevice,&USBD_DISK_fops);	//ΪUSB SLAVE MSC����ӻص�����
			USBD_Start(&hUSBDevice);								//����USB SLAVE
			break;	  
	}
}







