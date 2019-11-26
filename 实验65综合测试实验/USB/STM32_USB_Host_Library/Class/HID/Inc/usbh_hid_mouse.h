#ifndef __USBH_HID_MOUSE_H
#define __USBH_HID_MOUSE_H
#include "usbh_hid.h"
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


//�����Ϣ�ṹ��
typedef struct _HID_MOUSE_Info
{
  uint8_t              x; 
  uint8_t              y;  
  uint8_t              z;  			//��ӱ���z�����򲿷�����޷�֧��
  uint8_t              button;		//��buttons�޸�Ϊbutton,�洢����״̬
}
HID_MOUSE_Info_TypeDef;



USBH_StatusTypeDef USBH_HID_MouseInit(USBH_HandleTypeDef *phost);
HID_MOUSE_Info_TypeDef *USBH_HID_GetMouseInfo(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost);

#endif














