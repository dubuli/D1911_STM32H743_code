#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h"   
#include "pcf8574.h" 
#include "malloc.h"     
#include "wm8978.h"	 
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_audio.h"
#include "usbd_audio_if.h"	 
//ALIENTEK 阿波罗STM32H7开发板 实验57
//USB声卡(Slave) 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司    


USBD_HandleTypeDef USBD_Device;	//USB Device处理结构体  

extern vu8 bDeviceState;		//USB连接 情况
extern u8 volume;				//音量(可通过按键设置)

int main(void)
{   
	u8 led0sta=1;
	u8 key;
	u8 t=0;
	u8 Divece_STA=0XFF;
	
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz 
	delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	KEY_Init();						//初始化按键
	PCF8574_Init();					//初始化PCF8574
	WM8978_Init();					//初始化WM8978
	WM8978_ADDA_Cfg(1,0);			//开启DAC
	WM8978_Input_Cfg(0,0,0);		//关闭输入通道
	WM8978_Output_Cfg(1,0);			//开启DAC输出  
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
	POINT_COLOR=RED;      
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");	
	LCD_ShowString(30,70,200,16,16,"USB Sound Card TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/8/31");	
	LCD_ShowString(30,130,200,16,16,"KEY2:Vol-  KEY0:vol+");	
 	POINT_COLOR=BLUE;//设置字体为蓝色 
 	LCD_ShowString(30,160,200,16,16,"VOLUME:");		 //音量显示 	  
	LCD_ShowxNum(30+56,160,AUDIO_DEFAULT_VOLUME,3,16,0X80);//显示音量
	LCD_ShowString(30,180,200,16,16,"USB Connecting...");//提示正在建立连接 

    USBD_Init(&USBD_Device,&AUDIO_Desc,0);
    USBD_RegisterClass(&USBD_Device,USBD_AUDIO_CLASS);
    USBD_AUDIO_RegisterInterface(&USBD_Device,&USBD_AUDIO_fops);
    USBD_Start(&USBD_Device);
	
	while(1)
	{
		key=KEY_Scan(1);	//支持连按
		if(key)
		{
			if(key==KEY0_PRES)		//KEY0按下,音量增加
			{
				volume++;
				if(volume>100)volume=100; 
			}else if(key==KEY2_PRES)//KEY2按下,音量减少
			{ 
				if(volume)volume--;
				else volume=0;
			} 
            WM8978_HPvol_Set(volume*0.63,volume*0.63);
            WM8978_SPKvol_Set(volume*0.63);
			LCD_ShowxNum(30+56,160,volume,3,16,0X80);//显示音量 
			delay_ms(20);
		} 
		if(Divece_STA!=bDeviceState)//状态改变了
		{
			if(bDeviceState==1)
			{
				LED1(0);
				LCD_ShowString(30,180,200,16,16,"USB Connected    ");//提示USB连接已经建立
			}else 
			{
				LED1(1);
				LCD_ShowString(30,180,200,16,16,"USB DisConnected ");//提示USB连接失败
			}
			Divece_STA=bDeviceState;
		}	
		delay_ms(20);
		t++;
		if(t>10)
		{
			t=0;
			LED0(led0sta^=1);//提示系统在运行
            //printf("USBD_Device.dev_connection_status=%d\r\n",USBD_Device.dev_connection_status);
		}
	}
}












