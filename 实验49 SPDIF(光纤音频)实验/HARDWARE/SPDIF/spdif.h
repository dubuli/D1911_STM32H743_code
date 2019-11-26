#ifndef __SPDIF_H
#define __SPDIF_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//SPDIF 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/17
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//升级说明
//20181127 V1.1
//1,去掉spdif_rx_dev的saisync成员,用不到
////////////////////////////////////////////////////////////////////////////////// 	 

//SPDIF控制结构体
typedef struct
{
    u8 consta;		//连接状态,0,未连接;1,连接上了 
    u32 samplerate;	//SPDIF采样率
    u32 clock;		//SPDIF时钟频率　
}spdif_rx_dev;

extern spdif_rx_dev spdif_dev;
extern void (*spdif_rx_stop_callback)(void);	//SPDIF RX停止时的回调函数 


//SPDIF RX的三种模式设置
#define SPDIF_RX_IDLE	0	//IDLE模式
#define SPDIF_RX_SYNC	1	//同步模式
#define SPDIF_RX_RCV	3	//正常工作模式


void SPDIF_RX_Init(void); 
void SPDIF_RX_Mode(u8 mode);
void SPDIF_RXDATA_DMA_Init(u32* buf0,u32 *buf1,u16 num,u8 width);
u8 SPDIF_RX_WaitSync(void);
u32 SPDIF_RX_GetSampleRate(void);
void SPDIF_RX_Start(void);
void SPDIF_RX_Stop(void); 
#endif

