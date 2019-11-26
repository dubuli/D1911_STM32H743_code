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
#include "sdmmc_sdcard.h" 
#include "ff.h"  
#include "exfuns.h" 
#include "text.h"	
#include "piclib.h"	 
#include "usbh_core.h"
#include "usbh_msc.h"
//ALIENTEK 阿波罗STM32H7开发板 实验59
//U盘(Host) 实验
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  

USBH_HandleTypeDef hUSBHost;	//USB Host处理结构体 
 

static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
{
    u32 total,free;
	u8 res=0;
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_DISCONNECTION:
            f_mount(0,"3:",1); 	//卸载U盘
            POINT_COLOR=RED;    //设置字体为红色	   
            Show_Str(30,140,200,16,"设备连接中...",16,0);
            LCD_Fill(30,160,239,220,WHITE); 
            break;
        case HOST_USER_CLASS_ACTIVE:
            Show_Str(30,140,200,16,"设备连接成功!.",16,0);	
            f_mount(fs[3],"3:",1); 	//重新挂载U盘
            res=exf_getfree("3:",&total,&free);
        	if(res==0)
            {
                POINT_COLOR=BLUE;//设置字体为蓝色	   
                LCD_ShowString(30,160,200,16,16,"FATFS OK!");	
                LCD_ShowString(30,180,200,16,16,"U Disk Total Size:     MB");	 
                LCD_ShowString(30,200,200,16,16,"U Disk  Free Size:     MB"); 	    
                LCD_ShowNum(174,180,total>>10,5,16);//显示U盘总容量 MB
                LCD_ShowNum(174,200,free>>10,5,16);	
            }
            else
            {
                printf("U盘存储空间获取失败\r\n");
            }
            break;
        case HOST_USER_CONNECTION:
            break;
        default:
            break;
    }
}


int main(void)
{  
	u8 led0sta=1;
	u8 t=0;	
	
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
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
	exfuns_init();					//为fatfs相关变量申请内存  
	piclib_init();					//初始化画图 
 	f_mount(fs[0],"0:",1); 			//挂载SD卡 
 	f_mount(fs[1],"1:",1); 			//挂载FLASH.
 	f_mount(fs[2],"2:",1); 			//挂载NAND FLASH.
	POINT_COLOR=RED;      
 	while(font_init())				//检查字库
	{	    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}
	Show_Str(30,50,200,16,"阿波罗STM32F4/F7/H7开发板",16,0);				    	 
	Show_Str(30,70,200,16,"USB U盘实验",16,0);					    	 
	Show_Str(30,90,200,16,"2018年9月3日",16,0);	    	 
	Show_Str(30,110,200,16,"正点原子@ALIENTEK",16,0); 
	Show_Str(30,140,200,16,"设备连接中...",16,0);	

    USBH_Init(&hUSBHost, USBH_UserProcess, 0);
    USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
    USBH_Start(&hUSBHost);
	
    while (1)
    {
        USBH_Process(&hUSBHost);
        delay_ms(10);
		t++; 
		if(t==50)
        { 
            t=0;
            LED0(led0sta^=1);
        }
    }	 
}







