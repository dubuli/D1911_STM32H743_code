#ifndef __SPDIF_H
#define __SPDIF_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//SPDIF ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/17
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//����˵��
//20181127 V1.1
//1,ȥ��spdif_rx_dev��saisync��Ա,�ò���
////////////////////////////////////////////////////////////////////////////////// 	 

//SPDIF���ƽṹ��
typedef struct
{
    u8 consta;		//����״̬,0,δ����;1,�������� 
    u32 samplerate;	//SPDIF������
    u32 clock;		//SPDIFʱ��Ƶ�ʡ�
}spdif_rx_dev;

extern spdif_rx_dev spdif_dev;
extern void (*spdif_rx_stop_callback)(void);	//SPDIF RXֹͣʱ�Ļص����� 


//SPDIF RX������ģʽ����
#define SPDIF_RX_IDLE	0	//IDLEģʽ
#define SPDIF_RX_SYNC	1	//ͬ��ģʽ
#define SPDIF_RX_RCV	3	//��������ģʽ


void SPDIF_RX_Init(void); 
void SPDIF_RX_Mode(u8 mode);
void SPDIF_RXDATA_DMA_Init(u32* buf0,u32 *buf1,u16 num,u8 width);
u8 SPDIF_RX_WaitSync(void);
u32 SPDIF_RX_GetSampleRate(void);
void SPDIF_RX_Start(void);
void SPDIF_RX_Stop(void); 
#endif

