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
#include "nand.h"    
#include "ftl.h" 
#include "w25qxx.h" 
#include "pcf8574.h"       
#include "sdmmc_sdcard.h"  
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h" 
//ALIENTEK 阿波罗STM32H7开发板 实验56
//USB读卡器(Slave) 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  


USBD_HandleTypeDef USBD_Device;		//USB Device处理结构体 
extern vu8 USB_STATUS_REG;			//USB状态
extern vu8 bDeviceState;			//USB连接 情况
 
int main(void)
{   
	u8 led0sta=1;
	u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	KEY_Init();						//初始化按键 
	W25QXX_Init();					//初始化W25Q256 
	PCF8574_Init();					//初始化PCF8574 
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"USB Card Reader TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/8/22");	
	if(SD_Init())LCD_ShowString(30,130,200,16,16,"SD Card Error!");	//检测SD卡错误
	else //SD 卡正常
	{   															  
		LCD_ShowString(30,130,200,16,16,"SD Card Size:     MB"); 
 		LCD_ShowNum(134,130,SDCardInfo.CardCapacity>>20,5,16);	//显示SD卡容量
 	}
	if(W25QXX_ReadID()!=W25Q256)LCD_ShowString(30,130,200,16,16,"W25Q128 Error!");	//检测W25Q128错误
	else //SPI FLASH 正常
	{   														 
		LCD_ShowString(30,150,200,16,16,"SPI FLASH Size:25MB");	 
	} 
	if(FTL_Init())LCD_ShowString(30,170,200,16,16,"NAND Error!");	//检测NAND错误
	else //NAND FLASH 正常
	{   														 
		LCD_ShowString(30,170,200,16,16,"NAND Flash Size:    MB"); 
 		LCD_ShowNum(158,170,nand_dev.valid_blocknum*nand_dev.block_pagenum*nand_dev.page_mainsize>>20,4,16);	//显示SD卡容量
	}  
 	LCD_ShowString(30,190,200,16,16,"USB Connecting...");	//提示正在建立连接 	
    USBD_Init(&USBD_Device,&MSC_Desc,0);                        //初始化USB
    USBD_RegisterClass(&USBD_Device,USBD_MSC_CLASS);            //添加类
    USBD_MSC_RegisterStorage(&USBD_Device,&USBD_DISK_fops);     //为MSC类添加回调函数
    USBD_Start(&USBD_Device);                                   //开启USB
	delay_ms(1800);	
	while(1)
	{	
		delay_ms(1);				  
		if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   
			LCD_Fill(30,210,240,210+16,WHITE);//清除显示			  	   
			if(USB_STATUS_REG&0x01)//正在写		  
			{
				LED1(0);
				LCD_ShowString(30,210,200,16,16,"USB Writing...");//提示USB正在写入数据	 
			}
			if(USB_STATUS_REG&0x02)//正在读
			{
				LED1(0);
				LCD_ShowString(30,210,200,16,16,"USB Reading...");//提示USB正在读出数据  		 
			}	 										  
			if(USB_STATUS_REG&0x04)LCD_ShowString(30,230,200,16,16,"USB Write Err ");//提示写入错误
			else LCD_Fill(30,230,240,230+16,WHITE);//清除显示	  
			if(USB_STATUS_REG&0x08)LCD_ShowString(30,250,200,16,16,"USB Read  Err ");//提示读出错误
			else LCD_Fill(30,250,240,250+16,WHITE);//清除显示    
			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)LCD_ShowString(30,190,200,16,16,"USB Connected    ");//提示USB连接已经建立
			else LCD_ShowString(30,190,200,16,16,"USB DisConnected ");//提示USB被拔出了
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED1(1);
			LED0(led0sta^=1);//提示系统在运行
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=1;
			}else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG=0;
		}
	} 
}










