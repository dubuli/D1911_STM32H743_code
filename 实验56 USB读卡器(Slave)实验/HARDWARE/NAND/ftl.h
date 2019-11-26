#ifndef __FTL_H
#define __FTL_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//NAND FLASH FTL层算法代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//升级说明
//无
////////////////////////////////////////////////////////////////////////////////// 	



//坏块搜索控制
//如果设置为1,将在FTL_Format的时候,搜寻坏块,耗时久(512M,3分钟以上),且会导致RGB屏乱闪
#define FTL_USE_BAD_BLOCK_SEARCH		0		//定义是否使用坏块搜索



u8 FTL_Init(void); 
void FTL_BadBlockMark(u32 blocknum);
u8 FTL_CheckBadBlock(u32 blocknum); 
u8 FTL_UsedBlockMark(u32 blocknum);
u32 FTL_FindUnusedBlock(u32 sblock,u8 flag);
u32 FTL_FindSamePlaneUnusedBlock(u32 sblock);
u8 FTL_CopyAndWriteToBlock(u32 Source_PageNum,u16 ColNum,u8 *pBuffer,u32 NumByteToWrite);
u16 FTL_LBNToPBN(u32 LBNNum); 
u8 FTL_WriteSectors(u8 *pBuffer,u32 SectorNo,u16 SectorSize,u32 SectorCount);
u8 FTL_ReadSectors(u8 *pBuffer,u32 SectorNo,u16 SectorSize,u32 SectorCount);
u8 FTL_CreateLUT(u8 mode);
u8 FTL_BlockCompare(u32 blockx,u32 cmpval);
u32 FTL_SearchBadBlock(void);
u8 FTL_Format(void); 
#endif

