#include "usbh_hid_mouse.h"
#include "usbh_hid_parser.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//usb��� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/9/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   

HID_MOUSE_Info_TypeDef    mouse_info;		//�����Ϣ(����+��ť״̬)
u8	mouse_report_data[HID_QUEUE_SIZE];		//����ϱ����ݳ���,���HID_QUEUE_SIZE���ֽ�
 
 
//USBH����ʼ��
//phost:USBH���ָ��
//����ֵ:USBH״̬  
USBH_StatusTypeDef USBH_HID_MouseInit(USBH_HandleTypeDef *phost)
{
	HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData;
	mouse_info.x=0;
	mouse_info.y=0;
	mouse_info.button=0;
	if(HID_Handle->length > sizeof(mouse_report_data))
	{
		HID_Handle->length = sizeof(mouse_report_data);
	}
	HID_Handle->pData = (uint8_t *)mouse_report_data;
	fifo_init(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE);
	return USBH_OK;  
} 

//USBH��ȡ�����Ϣ
//phost:USBH���ָ��
//����ֵ:�����Ϣ(HID_MOUSE_Info_TypeDef)
HID_MOUSE_Info_TypeDef *USBH_HID_GetMouseInfo(USBH_HandleTypeDef *phost)
{
	if(USBH_HID_MouseDecode(phost)== USBH_OK)
	{
		return &mouse_info;
	}else
	{
		return NULL; 
	}
}
 
//USBH������ݽ�������
//phost:USBH���ָ��
//����ֵ:USBH״̬
USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost)
{
	HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
	if(HID_Handle->length==0)return USBH_FAIL;
	if(fifo_read(&HID_Handle->fifo, &mouse_report_data, HID_Handle->length)==HID_Handle->length)	//��ȡFIFO
	{
		if(HID_Handle->length==5||HID_Handle->length==6||HID_Handle->length==8)	//5/6/8�ֽڳ��ȵ�USB������ݴ���
		{
			mouse_info.button = mouse_report_data[0]; 
			mouse_info.x      = mouse_report_data[1];
			mouse_info.y      = mouse_report_data[3]<<4|mouse_report_data[2]>>4;
			mouse_info.z      = mouse_report_data[4]; 
		}else if(HID_Handle->length==4)	//4�ֽڳ��ȵ�USB ������ݴ���
		{ 
			mouse_info.button = mouse_report_data[0]; 
			mouse_info.x      = mouse_report_data[1];
			mouse_info.y      = mouse_report_data[2];
			mouse_info.z      = mouse_report_data[3];
		} 
		return USBH_OK;  
	}
	return   USBH_FAIL;
}

























