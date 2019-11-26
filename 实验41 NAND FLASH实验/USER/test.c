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
//ALIENTEK 阿波罗STM32H7开发板 实验41
//NAND FLASH 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  

 
int main(void)
{  
	u8 led0sta=1;
	u8 key,t=0;
	u16 i;
	u8 *buf;
	u8 *backbuf;  
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	KEY_Init();						//初始化按键 
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
   	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"NAND TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/8/1");	 	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 2");
	LCD_ShowString(30,150,200,16,16,"KEY1:Write Sector 2");
	LCD_ShowString(30,170,200,16,16,"KEY2:Recover Sector 2");		   
 	while(FTL_Init())			//检测NAND FLASH,并初始化FTL
	{
		LCD_ShowString(30,190,200,16,16,"NAND Error!");
		delay_ms(500);				 
		LCD_ShowString(30,190,200,16,16,"Please Check");
		delay_ms(500);	 
		LED0(led0sta^=1);//DS0闪烁
	}
	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);	//申请一个扇区的缓存
	buf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		//申请一个扇区的缓存
 	POINT_COLOR=BLUE;								//设置字体为蓝色 
	sprintf((char*)buf,"NAND Size:%dMB",(nand_dev.block_totalnum/1024)*(nand_dev.page_mainsize/1024)*nand_dev.block_pagenum);
	LCD_ShowString(30,190,200,16,16,buf);			//显示NAND容量  
	FTL_ReadSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//预先读取扇区2到备份区域,防止乱写导致文件系统损坏.
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://KEY0按下,读取sector
				key=FTL_ReadSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);//读取扇区
				if(key==0)//读取成功
				{
					LCD_ShowString(30,210,200,16,16,"USART1 Sending Data...  ");
					printf("Sector 2 data is:\r\n");
					for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)
					{
						printf("%x ",buf[i]);//输出数据
					}
					printf("\r\ndata end.\r\n");
					LCD_ShowString(30,210,200,16,16,"USART1 Send Data Over!  "); 
				}
				break;
			case KEY1_PRES://KEY1按下,写入sector
				for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)buf[i]=i+t;	//填充数据(随机的,根据t的值来确定) 
				LCD_ShowString(30,210,210,16,16,"Writing data to sector..");
				key=FTL_WriteSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);//写入扇区
				if(key==0)LCD_ShowString(30,210,200,16,16,"Write data successed    ");//写入成功
				else LCD_ShowString(30,210,200,16,16,"Write data failed       ");//写入失败
				break;
			case KEY2_PRES://KEY2按下,恢复sector的数据
				LCD_ShowString(30,210,210,16,16,"Recovering data...      ");
				key=FTL_WriteSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//写入扇区
				if(key==0)LCD_ShowString(30,210,200,16,16,"Recovering data OK      ");//恢复成功
				else LCD_ShowString(30,210,200,16,16,"Recovering data failed  ");//恢复失败
				break;
		}
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);
			t=0;
		}
	} 
}











