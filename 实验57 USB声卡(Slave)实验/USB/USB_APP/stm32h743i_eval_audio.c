#include "stm32h743i_eval_audio.h"
#include "usbd_audio_if.h"
#include "usbd_audio.h"
#include "sai.h"
#include "wm8978.h"
#include "sai.h" 
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//USB�����ײ�ӿں��� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/31
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
u8 volume=0;									//��ǰ����   
extern u8 usbd_audio_dataout_en;				//USB�������״̬,��usbd_audio.c���涨��

//��Ƶ����SAI DMA����ص�����
void audio_sai_dma_callback(void) 
{      
	BSP_AUDIO_OUT_TransferComplete_CallBack();	//ִ�лص�����,��������
}   

//������Ƶ�ӿ�
//OutputDevice:����豸ѡ��,δ�õ�.
//Volume:������С,0~100
//AudioFreq:��Ƶ������  
uint8_t BSP_AUDIO_OUT_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{ 
	WM8978_I2S_Cfg(2,0);			//�����ֱ�׼,16λ���ݳ���
	SAIA_Init(0,1,4);				//����SAI,������,16λ����
	SAIA_SampleRate_Set(AudioFreq);	//���ò�����  
	BSP_AUDIO_OUT_SetVolume(Volume);//��������
 	sai_tx_callback=audio_sai_dma_callback;		//�ص�����ָ��audio_sai_dma_callback
	printf("EVAL_AUDIO_Init:%d,%d\r\n",Volume,AudioFreq);
	return 0; 
}

//��ʼ������Ƶ����
//pBuffer:��Ƶ�������׵�ַָ��
//Size:��������С(��λ:�ֽ�)
uint8_t BSP_AUDIO_OUT_Play(uint16_t* pBuffer, uint32_t Size)
{	
	SAIA_TX_DMA_Init((u8*)pBuffer,0,Size/2,1);	//����TX DMA,16λ 
	DMA2_Stream3->CR&=~(1<<8);		//����ģʽ
	DMA2_Stream3->CR&=~(1<<18);		//�ĳɵ�����ģʽ 
	SAI_Play_Start();				//����DMA  
	printf("EVAL_AUDIO_Play:%x,%d\r\n",pBuffer,Size);
	return 0;
}

//�л���Ƶ������buffer,ÿ��DMA�ж���ɻ���øú���
//Addr:��Ƶ�����������׵�ַ
//Size:��Ƶ��������С(��λ:harf word,Ҳ����2���ֽ�)
void BSP_AUDIO_OUT_ChangeBuffer(uint16_t *pData, uint16_t Size)
{ 
	if(usbd_audio_dataout_en==1)		//USB����Ƶ�������
	{
		usbd_audio_dataout_en=0;		//������λ
	}else if(usbd_audio_dataout_en==0)	//û���յ��κ�����,ȫ�����0
	{
		mymemset(pData,0,AUDIO_TOTAL_BUF_SIZE);	
		usbd_audio_dataout_en=2;		//����Ѿ�������buffer,ֹͣ�������
	}
	if(Size!=AUDIO_TOTAL_BUF_SIZE)
	{
		//printf("chgbuf:%x,%d\r\n",pData,Size);//��۲�����,����printf,���ǻ�����΢����������
	}
	DMA2_Stream3->CR&=~(1<<0); 		//�ر�DMA���� 
	while(DMA2_Stream3->CR&0X1);	//ȷ��DMA���Ա�����  
	DMA2_Stream3->NDTR=Size/2;		//DMA �洢��0��ַ 
	DMA2_Stream3->CR|=1<<0;			//����DMA����		
}

//ֹͣ����
//Option:���Ʋ���,1/2,���:CODEC_PDWN_HW����
//����ֵ:0,�ɹ�
//    ����,����ʧ��
uint8_t BSP_AUDIO_OUT_Stop(uint32_t Option)
{
	printf("EVAL_AUDIO_Stop:%d\r\n",Option); 
	return 0;
}

//�������� 
//Volume:0~100
//����ֵ:0,�ɹ�
//    ����,����ʧ��
uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume)
{ 
	volume=Volume; 
	WM8978_HPvol_Set(volume*0.63,volume*0.63);
	WM8978_SPKvol_Set(volume*0.63);
	return 0;
}

//��������
//cmd:0,������; 1,����
//����ֵ:0,�ɹ�
//    ����,����ʧ��
uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd)
{
	return 0;
}























