#ifndef _SDRAM_H
#define _SDRAM_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//SDRAM ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/8
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//�޸�˵��
//V1.1 20180806
//1,Ĭ��FMCƵ��Ϊ220M,SDRAMƵ��Ϊ110M,����SDRAM��س�ʼ������.
//2,CAS��Ϊ3,��֧��MCU��,��������MCU����SDRAM����������,���ܻ����������쳣.
//3,����SDRAM_RGBLCD_Set����,���RGB��,��CAS����Ϊ2,������ٶ�.
//////////////////////////////////////////////////////////////////////////////////  


#define Bank5_SDRAM_ADDR    ((u32)(0XC0000000)) 			//SDRAM��ʼ��ַ
 
u8 SDRAM_Send_Cmd(u8 bankx,u8 cmd,u8 refresh,u16 regval);
void SDRAM_RGBLCD_Set(void);	
void SDRAM_Init(void);
void FMC_SDRAM_WriteBuffer(u8 *pBuffer,u32 WriteAddr,u32 n);
void FMC_SDRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n);
#endif



































