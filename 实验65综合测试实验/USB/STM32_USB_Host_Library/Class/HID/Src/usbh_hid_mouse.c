#include "usbh_hid_mouse.h"
#include "usbh_hid_parser.h"
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

HID_MOUSE_Info_TypeDef    mouse_info;		//鼠标信息(坐标+按钮状态)
u8	mouse_report_data[HID_QUEUE_SIZE];		//鼠标上报数据长度,最多HID_QUEUE_SIZE个字节
 
 
//USBH鼠标初始化
//phost:USBH句柄指针
//返回值:USBH状态  
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

//USBH获取鼠标信息
//phost:USBH句柄指针
//返回值:鼠标信息(HID_MOUSE_Info_TypeDef)
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
 
//USBH鼠标数据解析函数
//phost:USBH句柄指针
//返回值:USBH状态
USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost)
{
	HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
	if(HID_Handle->length==0)return USBH_FAIL;
	if(fifo_read(&HID_Handle->fifo, &mouse_report_data, HID_Handle->length)==HID_Handle->length)	//读取FIFO
	{
		if(HID_Handle->length==5||HID_Handle->length==6||HID_Handle->length==8)	//5/6/8字节长度的USB鼠标数据处理
		{
			mouse_info.button = mouse_report_data[0]; 
			mouse_info.x      = mouse_report_data[1];
			mouse_info.y      = mouse_report_data[3]<<4|mouse_report_data[2]>>4;
			mouse_info.z      = mouse_report_data[4]; 
		}else if(HID_Handle->length==4)	//4字节长度的USB 鼠标数据处理
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

























