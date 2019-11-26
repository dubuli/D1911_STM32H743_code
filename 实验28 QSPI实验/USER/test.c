#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "key.h"
#include "usmart.h"
#include "w25qxx.h" 
//ALIENTEK 阿波罗STM32H7开发板 实验28
//QSPI 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
   
//要写入到W25Q256的字符串数组
const u8 TEXT_Buffer[]={"Apollo STM32H7 QSPI TEST"};
#define SIZE sizeof(TEXT_Buffer)	 
	
int main(void)
{   
 	u8 led0sta=1; 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	u32 flassize;			
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200  
	usmart_dev.init(200);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
	KEY_Init(); 					//按键初始化 
	W25QXX_Init();					//W25QXX初始化
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"QSPI TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/14");	 		
	LCD_ShowString(30,130,200,16,16,"KEY1:Write  KEY0:Read");	//显示提示信息		
	while(W25QXX_ReadID()!=W25Q256)								//检测不到W25Q256
	{
		LCD_ShowString(30,150,200,16,16,"W25Q256 Check Failed!");
		delay_ms(500);
		LCD_ShowString(30,150,200,16,16,"Please Check!        ");
		delay_ms(500);
		LED0(led0sta^=1);		//DS0闪烁
	}
	LCD_ShowString(30,150,200,16,16,"W25Q256 Ready!"); 
	flassize=32*1024*1024;		//FLASH 大小为32M字节
  	POINT_COLOR=BLUE;			//设置字体为蓝色	  
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按下,写入W25Q256
		{
			LCD_Fill(0,170,239,319,WHITE);//清除半屏    
 			LCD_ShowString(30,170,200,16,16,"Start Write W25Q256....");
			W25QXX_Write((u8*)TEXT_Buffer,flassize-100,SIZE);			//从倒数第100个地址处开始,写入SIZE长度的数据
			LCD_ShowString(30,170,200,16,16,"W25Q256 Write Finished!");	//提示传送完成
		}
		if(key==KEY0_PRES)//KEY0按下,读取字符串并显示
		{
 			LCD_ShowString(30,170,200,16,16,"Start Read W25Q256.... ");
			W25QXX_Read(datatemp,flassize-100,SIZE);					//从倒数第100个地址处开始,读出SIZE个字节
			LCD_ShowString(30,170,200,16,16,"The Data Readed Is:   ");	//提示传送完成
			LCD_ShowString(30,190,200,16,16,datatemp);					//显示读到的字符串
		} 
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);	//DS0闪烁
			i=0;
		}		   
	}	  
}













