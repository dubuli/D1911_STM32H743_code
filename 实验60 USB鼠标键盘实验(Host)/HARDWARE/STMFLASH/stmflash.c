#include "stmflash.h"
#include "delay.h"
#include "usart.h" 
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

//解锁STM32的FLASH
void STMFLASH_Unlock(void)
{
	FLASH->KEYR1=FLASH_KEY1;	//Bank1,写入解锁序列.
	FLASH->KEYR1=FLASH_KEY2; 
	
	FLASH->KEYR2=FLASH_KEY1;	//Bank2,写入解锁序列.
	FLASH->KEYR2=FLASH_KEY2; 
}
//flash上锁
void STMFLASH_Lock(void)
{
	FLASH->CR1|=1<<0;	//Bank1,上锁
	FLASH->CR2|=1<<0;	//Bank2,上锁
}
//得到FLASH的错误状态
//bankx:0,获取bank1的状态
//      1,获取bank2的状态
//返回值:
//0,无错误
//其他,错误编号
u8 STMFLASH_GetErrorStatus(u8 bankx)
{	
	u32 res=0;	
	if(bankx==0)res=FLASH->SR1;
	else res=FLASH->SR2; 
	if(res&(1<<17))return 1;   		//WRPERR=1,写保护错误
	else if(res&(1<<18))return 2;	//PGSERR=1,编程序列错误
	else if(res&(1<<19))return 3;	//STRBERR=1,复写错误 
	else if(res&(1<<21))return 4;	//INCERR=1,数据一致性错误
	else if(res&(1<<22))return 5;	//OPERR=1,写/擦除错误 
	else if(res&(1<<23))return 6;	//RDPERR=1,读保护错误
	else if(res&(1<<24))return 7;	//RDSERR=1,非法访问加密区错误 
	else if(res&(1<<25))return 8;	//SNECCERR=1,1bit ecc校正错误 
	else if(res&(1<<26))return 9;	//DBECCERR=1,2bit ecc错误
	return 0;						//没有任何状态/操作完成.
} 
//等待操作完成
//bankx:0,bank1; 1,bank2
//time:要延时的长短(单位:10us)
//返回值:
//0,完成      
//1~9,错误代码.
//0XFF,超时
u8 STMFLASH_WaitDone(u8 bankx,u32 time)
{
	u8 res=0;
	u32 tempreg=0; 
	while(1)
	{
		if(bankx==0)tempreg=FLASH->SR1;
		else tempreg=FLASH->SR2;
		if((tempreg&0X07)==0)break;			//BSY=0,WBNE=0,QW=0,则操作完成
		delay_us(10);
		time--;
		if(time==0)return 0XFF;
	}
	res=STMFLASH_GetErrorStatus(bankx);
	if(res)
	{
		if(bankx==0)FLASH->CCR1=0X07EE0000;	//清所有错误标志
		else FLASH->CCR2=0X07EE0000; 		//清所有错误标志
		
	}
	return res;
}
//擦除扇区
//sectoraddr:扇区地址,范围是:0~15.
//0~7,addr所在的bank1扇区编号
//8~15,addr所在的bank2扇区编号+8,需要减去8,才得bank2扇区编号
//返回值:执行情况
u8 STMFLASH_EraseSector(u8 sectornum)
{
	u8 res=0;
	res=STMFLASH_WaitDone(sectornum/8,200000);	//等待上次操作结束,最大2s    
	if(res==0)
	{  
		if(sectornum<8)	//BANK1	擦除
		{
			FLASH->CR1&=~(7<<8);	//SNB1[2:0]=0,清除原来的设置
			FLASH->CR1&=~(3<<4);	//PSIZE1[1:0]=0,清除原来的设置
			FLASH->CR1|=(u32)sectornum<<8;	//设置要擦除的扇区编号,0~7
			FLASH->CR1|=2<<4;		//设置为32bit宽
			FLASH->CR1|=1<<2;		//SER1=1,扇区擦除 
			FLASH->CR1|=1<<7;		//START1=1,开始擦除
		}else			//BANK2 擦除
		{
			FLASH->CR2&=~(7<<8);	//SNB2[2:0]=0,清除原来的设置
			FLASH->CR2&=~(3<<4);	//PSIZE2[1:0]=0,清除原来的设置
			FLASH->CR2|=(u32)(sectornum-8)<<8;//设置要擦除的扇区编号,0~7
			FLASH->CR2|=2<<4;		//设置为32bit宽
			FLASH->CR2|=1<<2;		//SER2=1,扇区擦除 
			FLASH->CR2|=1<<7;		//START2=1,开始擦除
		} 
		res=STMFLASH_WaitDone(sectornum/8,200000);	//等待操作结束,最大2s  
		if(sectornum<8)FLASH->CR1&=~(1<<2);	//SER1=0,清除扇区擦除标志
		else FLASH->CR2&=~(1<<2);			//SER2=0,清除扇区擦除标志 
	}
	return res;
}
//在FLASH指定地址写8个字,即256bit
//必须以256bit为单位编程!!
//faddr:指定地址(此地址必须为4的倍数!!)
//dat:要写入的数据
//返回值:0,写入成功
//    其他,写入失败
u8 STMFLASH_Write8Word(u32 faddr, u32* pdata)
{
	u8 nword=8;	//每次写8个字,256bit
	u8 res;
	u8 bankx=0;
	if(faddr<BANK2_FLASH_SECTOR_0)bankx=0;	//判断地址是在bank0,还是在bank1
	else bankx=1;
	res=STMFLASH_WaitDone(bankx,0XFF);	 
	if(res==0)	//OK
	{
		if(bankx==0)	//BANK1	编程
		{ 
			FLASH->CR1&=~(3<<4);	//PSIZE1[1:0]=0,清除原来的设置 
			FLASH->CR1|=2<<4;		//设置为32bit宽
			FLASH->CR1|=1<<1;		//PG1=1,编程使能 
		}else			//BANK2 编程
		{
			FLASH->CR2&=~(3<<4);	//PSIZE2[1:0]=0,清除原来的设置 
			FLASH->CR2|=2<<4;		//设置为32bit宽
			FLASH->CR2|=1<<1;		//PG2=1,编程使能 
		} 
		while(nword)
		{
			*(vu32*)faddr=*pdata;	//写入数据
			faddr+=4;				//写地址+4
			pdata++;				//偏移到下一个数据首地址
			nword--;
		}
		__DSB();					//写操作完成后,屏蔽数据同步,使CPU重新执行指令序列
		res=STMFLASH_WaitDone(bankx,0XFF);	//等待操作完成,一个字编程,最多100us.

		if(bankx==0)FLASH->CR1&=~(1<<1);//PG1=0,清除扇区擦除标志
		else FLASH->CR2&=~(1<<1);		//PG2=0,清除扇区擦除标志
	} 
	return res;
} 
//读取指定地址的一个字(32位数据) 
//faddr:读地址 
//返回值:对应数据.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~7,addr所在的bank1扇区编号
//      8~15,addr所在的bank2扇区编号+8,需要减去8,才得bank2扇区编号
u8 STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<BANK1_FLASH_SECTOR_1)return 0;
	else if(addr<BANK1_FLASH_SECTOR_2)return 1;
	else if(addr<BANK1_FLASH_SECTOR_3)return 2;
	else if(addr<BANK1_FLASH_SECTOR_4)return 3;
	else if(addr<BANK1_FLASH_SECTOR_5)return 4;
	else if(addr<BANK1_FLASH_SECTOR_6)return 5;
	else if(addr<BANK1_FLASH_SECTOR_7)return 6; 
	else if(addr<BANK2_FLASH_SECTOR_0)return 7;
	else if(addr<BANK2_FLASH_SECTOR_1)return 8;
	else if(addr<BANK2_FLASH_SECTOR_2)return 9;
	else if(addr<BANK2_FLASH_SECTOR_3)return 10; 
	else if(addr<BANK2_FLASH_SECTOR_4)return 11;
	else if(addr<BANK2_FLASH_SECTOR_5)return 12;
	else if(addr<BANK2_FLASH_SECTOR_6)return 13; 
	else if(addr<BANK2_FLASH_SECTOR_7)return 14;    
	return 15;	
}
//从指定地址开始写入指定长度的数据
//特别注意:因为STM32H7的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//WriteAddr:起始地址(此地址必须为32的倍数!!,否则写入出错!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数,一次至少写入32字节,即8个字) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
	u8 status=0;
	u32 addrx=0;
	u32 endaddr=0;	
  	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%32)return;	//非法地址
	STMFLASH_Unlock();									//解锁 
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
	if(addrx<0X1FF00000)			//只有主存储区,才需要执行擦除操作!!
	{
		while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
				status=STMFLASH_EraseSector(STMFLASH_GetFlashSector(addrx));
				if(status)break;	//发生错误了 
                SCB_CleanInvalidateDCache();	//清除无效的D-Cache
			}else addrx+=4;
		} 
	}
	if(status==0)
	{
		while(WriteAddr<endaddr)//写数据
		{
			if(STMFLASH_Write8Word(WriteAddr,pBuffer))//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=32;
			pBuffer+=8;
		} 
	}
	STMFLASH_Lock();//上锁
} 

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(32位)数
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}

//////////////////////////////////////////测试用///////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr,u32 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}
 













