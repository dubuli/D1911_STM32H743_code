#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H
#include "usbd_cdc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//usb vcp驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/7/19
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define USB_USART_REC_LEN	 	200     //USB串口接收缓冲区最大字节数
//轮询周期，最大65ms，最小1ms
#define CDC_POLLING_INTERVAL    1       //轮询周期，最大65ms，最小1ms

extern u8  USB_USART_RX_BUF[USB_USART_REC_LEN]; //接收缓冲,最大USB_USART_REC_LEN个字节.末字节为换行符 
extern u16 USB_USART_RX_STA;   					//接收状态标记	
extern USBD_CDC_ItfTypeDef  USBD_CDC_fops;

void VCP_DataTx(uint8_t *data,uint32_t Len);
uint16_t VCP_DataRx(uint8_t* Buf, uint32_t Len);
void usb_printf(char* fmt,...); 

#endif 

