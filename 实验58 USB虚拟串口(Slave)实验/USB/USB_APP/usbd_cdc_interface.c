#include "usbd_cdc_interface.h"
#include "string.h"	
#include "stdarg.h"		 
#include "stdio.h"	
#include "usart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//usb vcp��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


//USB���⴮��������ò���
USBD_CDC_LineCodingTypeDef LineCoding= 
{
    115200,     //������
    0x00,       //ֹͣλ,Ĭ��1λ
    0x00,       //У��λ,Ĭ����
    0x08        //����λ,Ĭ��8λ
};

u8  USART_PRINTF_Buffer[USB_USART_REC_LEN];	//usb_printf���ͻ�����

//�����ƴ���1�������ݵķ���,������USB���⴮�ڽ��յ�������.
u8 USB_USART_RX_BUF[USB_USART_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.
u8 USBRxBuffer[USB_USART_REC_LEN];          //USB���յ����ݻ�����,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USB_USART_RX_STA=0;       				//����״̬���	


extern USBD_HandleTypeDef USBD_Device;
static int8_t CDC_Itf_Init(void);
static int8_t CDC_Itf_DeInit(void);
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t * pbuf, uint16_t length);
static int8_t CDC_Itf_Receive(uint8_t * pbuf, uint32_t * Len);

//���⴮�����ú���(��USB�ں˵���)
USBD_CDC_ItfTypeDef USBD_CDC_fops= 
{
    CDC_Itf_Init,
    CDC_Itf_DeInit,
    CDC_Itf_Control,
    CDC_Itf_Receive
};

//��ʼ��VCP
//����ֵ:USBD_OK
static int8_t CDC_Itf_Init(void)
{
    USBD_CDC_SetRxBuffer(&USBD_Device,USBRxBuffer);
    return (USBD_OK);
}

//��λVCP
//����ֵ:USBD_OK
static int8_t CDC_Itf_DeInit(void)
{
    return (USBD_OK);
}

//����VCP������
//buf:�������ݻ�����/�������滺����
//len:���ݳ���
//����ֵ:USBD_OK
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t * pbuf, uint16_t length)
{
    switch (cmd)
    {
        case CDC_SEND_ENCAPSULATED_COMMAND:break;
        case CDC_GET_ENCAPSULATED_RESPONSE:break;
        case CDC_SET_COMM_FEATURE:break;
        case CDC_GET_COMM_FEATURE:break;
        case CDC_CLEAR_COMM_FEATURE:break;
        case CDC_SET_LINE_CODING:
            LineCoding.bitrate = (uint32_t) (pbuf[0] | (pbuf[1] << 8) |
                                     (pbuf[2] << 16) | (pbuf[3] << 24));
            LineCoding.format = pbuf[4];
            LineCoding.paritytype = pbuf[5];
            LineCoding.datatype = pbuf[6];
			//��ӡ���ò���
			printf("linecoding.format:%d\r\n",LineCoding.format);
			printf("linecoding.paritytype:%d\r\n",LineCoding.paritytype);
			printf("linecoding.datatype:%d\r\n",LineCoding.datatype);
			printf("linecoding.bitrate:%d\r\n",LineCoding.bitrate);
            break;

        case CDC_GET_LINE_CODING:
            pbuf[0] = (uint8_t) (LineCoding.bitrate);
            pbuf[1] = (uint8_t) (LineCoding.bitrate >> 8);
            pbuf[2] = (uint8_t) (LineCoding.bitrate >> 16);
            pbuf[3] = (uint8_t) (LineCoding.bitrate >> 24);
            pbuf[4] = LineCoding.format;
            pbuf[5] = LineCoding.paritytype;
            pbuf[6] = LineCoding.datatype;
            break;

        case CDC_SET_CONTROL_LINE_STATE:break;
        case CDC_SEND_BREAK:break;
        default:break;
    }

    return (USBD_OK);
}

//CDC���ݽ��պ���
//Buf:�������ݻ�����
//Len:���յ������ݳ���
static int8_t CDC_Itf_Receive(uint8_t *Buf,uint32_t *Len)
{
    SCB_CleanDCache_by_Addr((uint32_t *)Buf,*Len);
    USBD_CDC_ReceivePacket(&USBD_Device);
    VCP_DataRx(Buf,*Len);
    return (USBD_OK);
}

//�����USB���⴮�ڽ��յ�������
//databuffer:���ݻ�����
//Nb_bytes:���յ����ֽ���.
//����ֵ:USBD_OK
uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{
	u8 i;
	u8 res;
	for(i=0;i<Len;i++)
	{  
		res=Buf[i]; 
		if((USB_USART_RX_STA&0x8000)==0)		//����δ���
		{
			if(USB_USART_RX_STA&0x4000)			//���յ���0x0d
			{
				if(res!=0x0a)USB_USART_RX_STA=0;//���մ���,���¿�ʼ
				else USB_USART_RX_STA|=0x8000;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USB_USART_RX_STA|=0x4000;
				else
				{
					USB_USART_RX_BUF[USB_USART_RX_STA&0X3FFF]=res;
					USB_USART_RX_STA++;
					if(USB_USART_RX_STA>(USB_USART_REC_LEN-1))USB_USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	
				}					
			}
		}   
	}  
	return USBD_OK;
}

//ͨ��USB��������
//data:Ҫ���͵�����
//length:���ݳ���
void VCP_DataTx(uint8_t *data,uint32_t Len)
{
    USBD_CDC_SetTxBuffer(&USBD_Device,data,Len);
    USBD_CDC_TransmitPacket(&USBD_Device);
    delay_ms(CDC_POLLING_INTERVAL);
}

//usb���⴮��,printf ����
//ȷ��һ�η������ݲ���USB_USART_REC_LEN�ֽ�
void usb_printf(char* fmt,...)  
{  
	u16 i;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART_PRINTF_Buffer,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART_PRINTF_Buffer);     //�˴η������ݵĳ���
    VCP_DataTx(USART_PRINTF_Buffer,i);              //��������
    SCB_CleanDCache_by_Addr((uint32_t *)USART_PRINTF_Buffer,i);
} 











