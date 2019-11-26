#ifndef __USB_APP_H
#define __USB_APP_H	 
#include "sys.h"
#include "usbh_core.h"
#include "usbh_msc.h" 
#include "usbh_hid.h" 
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"  
#include "usbh_hid.h" 
#include "usbh_msc.h"  
#include "string.h" 
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


#define USBH_MSC_MODE	0		//USB HOST MSCģʽ
#define USBH_HID_MODE	1		//USB HOST HIDģʽ
#define USBD_MSC_MODE	2		//USB DEVICE MSCģʽ

//USB APP���ƽṹ��
typedef struct 
{
	u8 bDeviceState; 	//USB״̬���
						//bit0:��ʾ����������SD��д������
						//bit1:��ʾ��������SD����������
						//bit2:SD��д���ݴ����־λ
						//bit3:SD�������ݴ����־λ
						//bit4:1,��ʾ��������ѯ����(�������ӻ�������)
						//bit5:����.
						//bit6:1,��ʾUSB���豸����;0,��ʾû���豸����
						//bit7:1,��ʾUSB�Ѿ�����;0,��ʾUSBû������.
	u8 hdevclass; 		//USB HOST�豸����
						//1,U��
						//2,���
						//3,����
						//4,��Ϸ�ֱ�
	u8 mode; 			//USB����ģʽ:0,USB HOST MSCģʽ(Ĭ��ģʽ,��U��)
						//			  1,USB HOST HIDģʽ(���������̵�)
						//			  2,USB Device MSCģʽ(USB������)
}_usb_app;
extern _usb_app usbx;

extern USBH_HandleTypeDef  hUSBHost; 	//USB Host����ṹ�� 
extern USBD_HandleTypeDef  hUSBDevice;	//USB Device����ṹ�� 

extern HCD_HandleTypeDef hhcd;			//HCD���,��usbh_conf.c���涨��
extern PCD_HandleTypeDef hpcd;			//PCD���,��usbd_conf.c���涨��
extern USBH_StatusTypeDef USBH_MSC_InterfaceDeInit  (USBH_HandleTypeDef *phost);	//��usbh_msc.c���涨��
extern USBH_StatusTypeDef USBH_HID_InterfaceDeInit (USBH_HandleTypeDef *phost );	//��usbh_hid.c���涨��
extern uint8_t  USBD_MSC_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);			//��usbd_msc.c���涨��	



void usbapp_init(void);			//��ʼ��USB
void usbapp_pulling(void);
void usbapp_mode_stop(void);
void usbapp_mode_set(u8 mode);

#endif

















