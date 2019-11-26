/*------------------------------------------------------------------------*/
/* Sample Code of OS Dependent Functions for FatFs                        */
/* (C)ChaN, 2017                                                          */
/*------------------------------------------------------------------------*/
#include "ff.h"
#include "sys.h"
#include "malloc.h"
#include "usart.h"
#include "ucos_ii.h"
#include "calendar.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//FATFS底层(ffsystem) 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

vu8 cnt0=0;
vu8 cnt1=0;
OS_CPU_SR cpu_sr=0; 
//进入临界区
void ff_enter(FATFS *fs)
{   
	if(cnt0)
	{
		printf("in shit:%d\r\n",cnt0);
	}
	if(fs->pdrv!=3)
	{
		OS_ENTER_CRITICAL();	//进入临界区(无法被中断打断)     
		cnt0++;
	}else
	{  
		OSSchedLock();			//阻止ucos调度
		cnt1++;
	}  
}
//退出临界区
void ff_leave(FATFS* fs)
{ 
	if(cnt0)
	{
		cnt0--; 
		OS_EXIT_CRITICAL();	//退出临界区(可以被中断打断) 
	}
	if(cnt1)
	{ 
		cnt1--;
		OSSchedUnlock();	//开启ucos调度 	
	} 
}  

//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	u32 time=0;
	calendar_get_date(&calendar);
	calendar_get_time(&calendar);
	if(calendar.w_year<1980)calendar.w_year=1980;
	time=(calendar.w_year-1980)<<25;//年份
	time|=(calendar.w_month)<<21;	//月份
	time|=(calendar.w_date)<<16;	//日期
	time|=(calendar.hour)<<11;		//时
	time|=(calendar.min)<<5;		//分
	time|=(calendar.sec/2);			//秒	
	return time;
}

//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}

//释放内存
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}











