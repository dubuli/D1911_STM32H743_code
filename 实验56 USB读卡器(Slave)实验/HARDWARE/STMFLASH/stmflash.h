#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//STM32内部FLASH读写 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/7/18
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved				
//********************************************************************************
//修改说明 
//20180816
//1,修正部分注释.
//2,改为写入地址必须为32的倍数!否则写入将出错
////////////////////////////////////////////////////////////////////////////////// 

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
//FLASH解锁键值
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB

//FLASH 扇区的起始地址,分2个bank,每个bank 1MB
#define BANK1_FLASH_SECTOR_0     ((u32)0x08000000) 	//Bank1扇区0起始地址, 128 Kbytes  
#define BANK1_FLASH_SECTOR_1     ((u32)0x08020000) 	//Bank1扇区1起始地址, 128 Kbytes  
#define BANK1_FLASH_SECTOR_2     ((u32)0x08040000) 	//Bank1扇区2起始地址, 128 Kbytes  
#define BANK1_FLASH_SECTOR_3     ((u32)0x08060000) 	//Bank1扇区3起始地址, 128 Kbytes  
#define BANK1_FLASH_SECTOR_4     ((u32)0x08080000) 	//Bank1扇区4起始地址, 128 Kbytes  
#define BANK1_FLASH_SECTOR_5     ((u32)0x080A0000) 	//Bank1扇区5起始地址, 128 Kbytes  
#define BANK1_FLASH_SECTOR_6     ((u32)0x080C0000) 	//Bank1扇区6起始地址, 128 Kbytes  
#define BANK1_FLASH_SECTOR_7     ((u32)0x080E0000) 	//Bank1扇区7起始地址, 128 Kbytes 
#define BANK2_FLASH_SECTOR_0     ((u32)0x08100000) 	//Bank2扇区0起始地址, 128 Kbytes  
#define BANK2_FLASH_SECTOR_1     ((u32)0x08120000) 	//Bank2扇区1起始地址, 128 Kbytes  
#define BANK2_FLASH_SECTOR_2     ((u32)0x08140000) 	//Bank2扇区2起始地址, 128 Kbytes  
#define BANK2_FLASH_SECTOR_3     ((u32)0x08160000) 	//Bank2扇区3起始地址, 128 Kbytes  
#define BANK2_FLASH_SECTOR_4     ((u32)0x08180000) 	//Bank2扇区4起始地址, 128 Kbytes  
#define BANK2_FLASH_SECTOR_5     ((u32)0x081A0000) 	//Bank2扇区5起始地址, 128 Kbytes  
#define BANK2_FLASH_SECTOR_6     ((u32)0x081C0000) 	//Bank2扇区6起始地址, 128 Kbytes  
#define BANK2_FLASH_SECTOR_7     ((u32)0x081E0000)  //Bank2扇区7起始地址, 128 Kbytes    

 
 
void STMFLASH_Unlock(void);						//FLASH解锁
void STMFLASH_Lock(void);				 		//FLASH上锁
u8 STMFLASH_GetStatus(u8 bankx);				//获得状态
u8 STMFLASH_WaitDone(u8 bankx,u32 time); 		//等待操作结束
u8 STMFLASH_EraseSector(u8 sectornum);	 		//擦除页
u8 STMFLASH_Write8Word(u32 faddr, u32* pdata);	//一次写入8个字
u32 STMFLASH_ReadWord(u32 faddr);				//读一个字
u8 STMFLASH_GetFlashSector(u32 addr);			//获取扇区编号 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);	//指定地址开始写入指定长度的数据 
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);	//从指定地址开始读出指定长度的数据

//测试写入
void Test_Write(u32 WriteAddr,u32 WriteData);								   
#endif

















