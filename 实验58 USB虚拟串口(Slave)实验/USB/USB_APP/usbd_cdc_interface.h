#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H
#include "usbd_cdc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//usb vcp��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/7/19
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

#define USB_USART_REC_LEN	 	200     //USB���ڽ��ջ���������ֽ���
//��ѯ���ڣ����65ms����С1ms
#define CDC_POLLING_INTERVAL    1       //��ѯ���ڣ����65ms����С1ms

extern u8  USB_USART_RX_BUF[USB_USART_REC_LEN]; //���ջ���,���USB_USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USB_USART_RX_STA;   					//����״̬���	
extern USBD_CDC_ItfTypeDef  USBD_CDC_fops;

void VCP_DataTx(uint8_t *data,uint32_t Len);
uint16_t VCP_DataRx(uint8_t* Buf, uint32_t Len);
void usb_printf(char* fmt,...); 

#endif 

