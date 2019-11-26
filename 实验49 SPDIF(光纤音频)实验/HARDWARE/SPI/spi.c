#include "spi.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//SPI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/7/17
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//升级说明
//无
////////////////////////////////////////////////////////////////////////////////// 	 

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI2的初始化
void SPI2_Init(void)
{	 
	u32 tempreg=0;
	RCC->AHB4ENR|=1<<1;			//使能PORTB时钟 
	RCC->APB1LENR|=1<<14;		//SPI2时钟使能 
	GPIO_Set(GPIOB,7<<13,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_FAST,GPIO_PUPD_PU);	//PB13~15复用功能输出	
  	GPIO_AF_Set(GPIOB,13,5);	//PB13,AF5
 	GPIO_AF_Set(GPIOB,14,5);	//PB14,AF5
 	GPIO_AF_Set(GPIOB,15,5);	//PB15,AF5 
	//配置SPI的时钟源
	RCC->D2CCIP1R&=~(7<<12);	//SPI123SEL[2:0]=0,清除原来的设置
	RCC->D2CCIP1R|=0<<12;		//SPI123SEL[2:0]=1,选择pll1_q_ck作为SPI1/2/3的时钟源,一般为200Mhz
								//即:spi_ker_ck=200Mhz 
	//这里只针对SPI口初始化
	RCC->APB1LRSTR|=1<<14;		//复位SPI2
	RCC->APB1LRSTR&=~(1<<14);	//停止复位SPI2
	
	SPI2->CR1|=1<<12;			//SSI=1,设置内部SS信号为高电平	
	SPI2->CFG1=7<<28;			//MBR[2:0]=7,设置spi_ker_ck为256分频.
	SPI2->CFG1|=7<<0;			//DSIZE[4:0]=7,设置SPI帧格式为8位,即字节传输
	tempreg=(u32)1<<31;			//AFCNTR=1,SPI保持对IO口的控制
	tempreg|=0<<29;				//SSOE=0,禁止硬件NSS输出
	tempreg|=1<<26;				//SSM=1,软件管理NSS脚
	tempreg|=1<<25;				//CPOL=1,空闲状态下,SCK为高电平
	tempreg|=1<<24;				//CPHA=1,数据采样从第2个时间边沿开始
	tempreg|=0<<23;				//LSBFRST=0,MSB先传输
	tempreg|=1<<22;				//MASTER=1,主机模式
	tempreg|=0<<19;				//SP[2:0]=0,摩托罗拉格式
	tempreg|=0<<17;				//COMM[1:0]=0,全双工通信
	SPI2->CFG2=tempreg;			//设置CFG2寄存器	
	SPI2->I2SCFGR&=~(1<<0);		//选择SPI模式
	SPI2->CR1|=1<<0;			//SPE=1,使能SPI2
 
	SPI2_ReadWriteByte(0xff);	//启动传输		 
}   

//SPI2速度设置函数
//SpeedSet:0~7
//SPI速度=spi_ker_ck/2^(SpeedSet+1)
//spi_ker_ck我们选择来自pll1_q_ck,为200Mhz
void SPI2_SetSpeed(u8 SpeedSet)
{
	SpeedSet&=0X07;					//限制范围
 	SPI2->CR1&=~(1<<0); 			//SPE=0,SPI设备失能
	SPI2->CFG1&=~(7<<28); 			//MBR[2:0]=0,清除原来的分频设置
	SPI2->CFG1|=(u32)SpeedSet<<28;	//MBR[2:0]=SpeedSet,设置SPI2速度  
	SPI2->CR1|=1<<0; 				//SPE=1,SPI设备使能	 	  
} 

//SPI2 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI2_ReadWriteByte(u8 TxData)
{			 	   
	u8 RxData=0;	
	SPI2->CR1|=1<<0;				//SPE=1,使能SPI2
	SPI2->CR1|=1<<9;  				//CSTART=1,启动传输
	
	while((SPI2->SR&1<<1)==0);		//等待发送区空 
	*(vu8 *)&SPI2->TXDR=TxData;		//发送一个byte,以传输长度访问TXDR寄存器   
	while((SPI2->SR&1<<0)==0);		//等待接收完一个byte  
	RxData=*(vu8 *)&SPI2->RXDR;		//接收一个byte,以传输长度访问RXDR寄存器	
	
	SPI2->IFCR|=3<<3;				//EOTC和TXTFC置1,清除EOT和TXTFC位 
	SPI2->CR1&=~(1<<0);				//SPE=0,关闭SPI2,会执行状态机复位/FIFO重置等操作
	return RxData;					//返回收到的数据
}






