#ifndef __FDCAN_H
#define __FDCAN_H	 
#include "sys.h"	    
#include "stm32h7xx_hal_fdcan.h"	 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//FDCAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/7/15
//版本：V1.0 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 
//CAN1接收RX0中断使能
#define FDCAN1_RX0_INT_ENABLE	0			//0,不使能;1,使能.	

	
//防止加入HAL库 FDCAN驱动后,编译报错,必须实现以下3个宏
#define HAL_FDCAN_MODULE_ENABLED			//使能FDCAN模块
#define assert_param(expr) 		((void)0)	//assert_param定义
#define HAL_GetTick()			10			//返回SysTick计数器值,这里没用到SysTick

										 							 				    
u8 FDCAN1_Mode_Init(u8 tnsjw,u16 tseg1,u8 tseg2,u16 tnbrp,u8 mode);	//初始化FDCAN
u8 FDCAN1_Send_Msg(u8* msg,u32 len);		//发送数据 
u8 FDCAN1_Receive_Msg(u8 *buf);				//接收数据  
#endif

















