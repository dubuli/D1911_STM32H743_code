#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h" 
#include "malloc.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h" 
#include "usbd_cdc_interface.h"
//ALIENTEK 阿波罗STM32H7开发板 实验58
//USB虚拟串口(Slave) 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  

USBD_HandleTypeDef USBD_Device;	//USB Device处理结构体  
extern vu8 bDeviceState;		//USB连接 情况


int main(void)
{  
 	u8 led0sta=1;
	u16 len;	
	u16 times=0;    
	u8 usbstatus=0;	
	
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"USB Virtual USART TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/9/2");	 
 	LCD_ShowString(30,130,200,16,16,"USB Connecting...");//提示USB开始连接
    
    USBD_Init(&USBD_Device,&VCP_Desc,0);
    USBD_RegisterClass(&USBD_Device,USBD_CDC_CLASS);
    USBD_CDC_RegisterInterface(&USBD_Device,&USBD_CDC_fops);
    USBD_Start(&USBD_Device);
    
 	while(1)
	{
		if(usbstatus!=bDeviceState)//USB连接状态发生了改变.
		{
			usbstatus=bDeviceState;//记录新的状态
			if(usbstatus==1)
			{
				POINT_COLOR=BLUE;
				LCD_ShowString(30,130,200,16,16,"USB Connected    ");//提示USB连接成功
				LED1(0);//DS1亮
			}else
			{
				POINT_COLOR=RED;
				LCD_ShowString(30,130,200,16,16,"USB disConnected ");//提示USB断开
				LED1(1);//DS1灭
			}
		}
		if(USB_USART_RX_STA&0x8000)
		{					   
			len=USB_USART_RX_STA&0x3FFF;//得到此次接收到的数据长度
			usb_printf("\r\n您发送的消息长度为:%d\r\n\r\n",len);
            VCP_DataTx(USB_USART_RX_BUF,len);;
			usb_printf("\r\n\r\n");//插入换行
			USB_USART_RX_STA=0;
		}else
		{
			times++;
			if(times%5000==0)
			{
				usb_printf("\r\n阿波罗STM32F4/F7/H7开发板USB虚拟串口实验\r\n");
				usb_printf("正点原子@ALIENTEK\r\n\r\n");
			}
			if(times%200==0)usb_printf("请输入数据,以回车键结束\r\n");  
			if(times%30==0)LED0(led0sta^=1);//闪烁LED,提示系统正在运行.
			delay_ms(10);    
		}
	}  	
}




