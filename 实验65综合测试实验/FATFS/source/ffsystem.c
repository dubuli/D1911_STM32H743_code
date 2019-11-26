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
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//FATFS�ײ�(ffsystem) ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

vu8 cnt0=0;
vu8 cnt1=0;
OS_CPU_SR cpu_sr=0; 
//�����ٽ���
void ff_enter(FATFS *fs)
{   
	if(cnt0)
	{
		printf("in shit:%d\r\n",cnt0);
	}
	if(fs->pdrv!=3)
	{
		OS_ENTER_CRITICAL();	//�����ٽ���(�޷����жϴ��)     
		cnt0++;
	}else
	{  
		OSSchedLock();			//��ֹucos����
		cnt1++;
	}  
}
//�˳��ٽ���
void ff_leave(FATFS* fs)
{ 
	if(cnt0)
	{
		cnt0--; 
		OS_EXIT_CRITICAL();	//�˳��ٽ���(���Ա��жϴ��) 
	}
	if(cnt1)
	{ 
		cnt1--;
		OSSchedUnlock();	//����ucos���� 	
	} 
}  

//���ʱ��
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	u32 time=0;
	calendar_get_date(&calendar);
	calendar_get_time(&calendar);
	if(calendar.w_year<1980)calendar.w_year=1980;
	time=(calendar.w_year-1980)<<25;//���
	time|=(calendar.w_month)<<21;	//�·�
	time|=(calendar.w_date)<<16;	//����
	time|=(calendar.hour)<<11;		//ʱ
	time|=(calendar.min)<<5;		//��
	time|=(calendar.sec/2);			//��	
	return time;
}

//��̬�����ڴ�
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}

//�ͷ��ڴ�
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}











