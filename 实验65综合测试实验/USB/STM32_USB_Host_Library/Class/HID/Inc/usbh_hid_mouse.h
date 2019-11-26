#ifndef __USBH_HID_MOUSE_H
#define __USBH_HID_MOUSE_H
#include "usbh_hid.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//usb鼠标 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/9/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   


//鼠标信息结构体
typedef struct _HID_MOUSE_Info
{
  uint8_t              x; 
  uint8_t              y;  
  uint8_t              z;  			//添加变量z，否则部分鼠标无法支持
  uint8_t              button;		//将buttons修改为button,存储按键状态
}
HID_MOUSE_Info_TypeDef;



USBH_StatusTypeDef USBH_HID_MouseInit(USBH_HandleTypeDef *phost);
HID_MOUSE_Info_TypeDef *USBH_HID_GetMouseInfo(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost);

#endif














