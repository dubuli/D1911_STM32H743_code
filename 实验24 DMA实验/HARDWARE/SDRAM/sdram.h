#ifndef _SDRAM_H
#define _SDRAM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//SDRAM 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/8
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//修改说明
//V1.1 20180806
//1,默认FMC频率为220M,SDRAM频率为110M,设置SDRAM相关初始化参数.
//2,CAS改为3,以支持MCU屏,否则由于MCU屏和SDRAM共用数据线,可能会引起数据异常.
//3,新增SDRAM_RGBLCD_Set函数,针对RGB屏,将CAS设置为2,以提高速度.
//////////////////////////////////////////////////////////////////////////////////  


#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) 			//SDRAM开始地址
 
u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval);
void SDRAM_RGBLCD_Set(void);	
void SDRAM_Init(void);
void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);
#endif



































