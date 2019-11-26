#include "usbd_cdc_interface.h"
#include "string.h"	
#include "stdarg.h"		 
#include "stdio.h"	
#include "usart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//usb vcp驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


//USB虚拟串口相关配置参数
USBD_CDC_LineCodingTypeDef LineCoding= 
{
    115200,     //波特率
    0x00,       //停止位,默认1位
    0x00,       //校验位,默认无
    0x08        //数据位,默认8位
};

u8  USART_PRINTF_Buffer[USB_USART_REC_LEN];	//usb_printf发送缓冲区

//用类似串口1接收数据的方法,来处理USB虚拟串口接收到的数据.
u8 USB_USART_RX_BUF[USB_USART_REC_LEN]; 	//接收缓冲,最大USART_REC_LEN个字节.
u8 USBRxBuffer[USB_USART_REC_LEN];          //USB接收的数据缓冲区,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USB_USART_RX_STA=0;       				//接收状态标记	


extern USBD_HandleTypeDef USBD_Device;
static int8_t CDC_Itf_Init(void);
static int8_t CDC_Itf_DeInit(void);
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t * pbuf, uint16_t length);
static int8_t CDC_Itf_Receive(uint8_t * pbuf, uint32_t * Len);

//虚拟串口配置函数(供USB内核调用)
USBD_CDC_ItfTypeDef USBD_CDC_fops= 
{
    CDC_Itf_Init,
    CDC_Itf_DeInit,
    CDC_Itf_Control,
    CDC_Itf_Receive
};

//初始化VCP
//返回值:USBD_OK
static int8_t CDC_Itf_Init(void)
{
    USBD_CDC_SetRxBuffer(&USBD_Device,USBRxBuffer);
    return (USBD_OK);
}

//复位VCP
//返回值:USBD_OK
static int8_t CDC_Itf_DeInit(void)
{
    return (USBD_OK);
}

//控制VCP的设置
//buf:命令数据缓冲区/参数保存缓冲区
//len:数据长度
//返回值:USBD_OK
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
			//打印配置参数
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

//CDC数据接收函数
//Buf:接收数据缓冲区
//Len:接收到的数据长度
static int8_t CDC_Itf_Receive(uint8_t *Buf,uint32_t *Len)
{
    SCB_CleanDCache_by_Addr((uint32_t *)Buf,*Len);
    USBD_CDC_ReceivePacket(&USBD_Device);
    VCP_DataRx(Buf,*Len);
    return (USBD_OK);
}

//处理从USB虚拟串口接收到的数据
//databuffer:数据缓存区
//Nb_bytes:接收到的字节数.
//返回值:USBD_OK
uint16_t VCP_DataRx (uint8_t* Buf, uint32_t Len)
{
	u8 i;
	u8 res;
	for(i=0;i<Len;i++)
	{  
		res=Buf[i]; 
		if((USB_USART_RX_STA&0x8000)==0)		//接收未完成
		{
			if(USB_USART_RX_STA&0x4000)			//接收到了0x0d
			{
				if(res!=0x0a)USB_USART_RX_STA=0;//接收错误,重新开始
				else USB_USART_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USB_USART_RX_STA|=0x4000;
				else
				{
					USB_USART_RX_BUF[USB_USART_RX_STA&0X3FFF]=res;
					USB_USART_RX_STA++;
					if(USB_USART_RX_STA>(USB_USART_REC_LEN-1))USB_USART_RX_STA=0;//接收数据错误,重新开始接收	
				}					
			}
		}   
	}  
	return USBD_OK;
}

//通过USB发送数据
//data:要发送的数据
//length:数据长度
void VCP_DataTx(uint8_t *data,uint32_t Len)
{
    USBD_CDC_SetTxBuffer(&USBD_Device,data,Len);
    USBD_CDC_TransmitPacket(&USBD_Device);
    delay_ms(CDC_POLLING_INTERVAL);
}

//usb虚拟串口,printf 函数
//确保一次发送数据不超USB_USART_REC_LEN字节
void usb_printf(char* fmt,...)  
{  
	u16 i;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART_PRINTF_Buffer,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART_PRINTF_Buffer);     //此次发送数据的长度
    VCP_DataTx(USART_PRINTF_Buffer,i);              //发送数据
    SCB_CleanDCache_by_Addr((uint32_t *)USART_PRINTF_Buffer,i);
} 











