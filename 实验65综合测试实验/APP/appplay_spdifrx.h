#ifndef __APPPLAY_SPDIFRX_H
#define __APPPLAY_SPDIFRX_H 	
#include "common.h"  
#include "spdif.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-SPDIF RX测试 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/9/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	  

#define	SPDIF_DBUF_SIZE		1024			//定义SPDIF数据接收缓冲区的大小,1K*4字节

extern u8*const SPDIF_RECORD_PIC[3];

void spdif_show_samplerate(u16 y,u32 samplerate,u8 fsr);
void sai_dma_tx_callback(void);
void spdif_rx_stopplay_callback(void);
u8 spdif_vu_get(u16 signallevel);
void spdif_vu_meter(u8 width,u16 x,u16 y,u8 level); 
u8 appplay_spdifrx(u8* caption);
#endif



