#include "spdif.h"
#include "delay.h"	 
#include "ucos_ii.h" 
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


spdif_rx_dev spdif_dev;				//SPDIF���ƽṹ��

//SPDIF RX��ʼ�� 
void SPDIF_RX_Init(void)
{	
	u32 tempreg;
	RCC->APB1LENR|=1<<16;			//ʹ��SPDIF RXʱ�� 
	RCC->AHB4ENR|=1<<6;				//ʹ��PORTGʱ�� 
	GPIO_Set(GPIOG,PIN12,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);	//PG12���ù��� 	
   	GPIO_AF_Set(GPIOG,12,8);		//PG12,AF8
	RCC->D2CCIP1R&=~(3<<20);		//SPDIFSEL[1:0]����
	RCC->D2CCIP1R|=0<<20;			//SPDIFSEL[1:0]=0,ѡ��pll1_q_ck,��ΪSPDIFʱ��Դ,Ƶ��Ϊ200Mhz
	spdif_dev.clock=200000000;		//����SPDIF CLK��Ƶ��,Ϊ200Mhz,Ҫ֧��192Khz�����ʱ��뱣֤clock��135.2Mhz
	
	SPDIF_RX_Mode(SPDIF_RX_IDLE);	//�Ƚ���IDLEģʽ
	tempreg=1<<16;					//ѡ��SPDIF RXͨ��1(��ѡ��0,1,2,3)
	tempreg|=1<<14;					//�ȴ�SPDIF RX��·�ϵĻ
	tempreg|=2<<12;					//ͬ���׶�,�����������15��
	tempreg|=0<<11;					//��ͨ��A��ȡͨ��״̬
	tempreg|=0<<10;					//SPDIF����ͨ��״̬���û���Ϣ��ʹ��DMA����
	tempreg|=1<<9;					//��ͷ���Ͳ�д��DR
	tempreg|=1<<8;					//ͨ��״̬���û�λ��д��DR
	tempreg|=1<<7;					//��Ч��λ��д��DR
	tempreg|=1<<6;					//��żУ�����λ��д��DR
	tempreg|=0<<4;					//���ݲ����Ҷ���(LSB),���֧��24λ��Ƶ������
	tempreg|=1<<3;					//������ģʽ  
	tempreg|=1<<2;					//SPDIF��Ƶ����ʹ��DMA������ 
	SPDIFRX->CR=tempreg;
	SPDIFRX->IMR|=(1<<6)|(1<<2);	//ʹ����żУ������жϺʹ��нӿڴ����ж�
	MY_NVIC_Init(1,0,SPDIF_RX_IRQn,2);//����SPDIF RX�ж�,��ռ1�������ȼ�0����2  
}    

//����SPDIF�Ĺ���ģʽ
//mode:0,IDLEģʽ
//     1,RXͬ��ģʽ
//     2,����
//     3,��������ģʽ
void SPDIF_RX_Mode(u8 mode)
{
	u32 tempreg=0;
	tempreg=SPDIFRX->CR;
	tempreg&=~(0X03);
	tempreg|=mode&0X03;	//����ģʽ
	SPDIFRX->CR=tempreg;
}

//SPDIF RX����DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
//width:λ��(�洢��������,ͬʱ����),0,8λ;1,16λ;2,32λ;
void SPDIF_RXDATA_DMA_Init(u32* buf0,u32 *buf1,u16 num,u8 width)
{
	
	RCC->AHB1ENR|=1<<0;			//DMA1ʱ��ʹ��  
	RCC->D3AMR|=1<<0;			//DMAMUXʱ��ʹ�� 
	while(DMA1_Stream1->CR&0X01);//�ȴ�DMA1_Stream1������
	DMAMUX1_Channel1->CCR=93;	//DMA1_Stream1��ͨ��ѡ��: 93,��SPDIFRX_DT��Ӧ��ͨ��
								//���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 116 
	DMA1->LIFCR|=0X3D<<6*0;		//���ͨ��1�������жϱ�־
	DMA1_Stream1->FCR=0X0000021;//����ΪĬ��ֵ	
	
	DMA1_Stream1->PAR=(u32)&SPDIFRX->DR;//�����ַΪ:SPDIFRX->DR
	DMA1_Stream1->M0AR=(u32)buf0;//�ڴ�1��ַ
	DMA1_Stream1->M1AR=(u32)buf1;//�ڴ�2��ַ
	DMA1_Stream1->NDTR=num;		//���ó���
	DMA1_Stream1->CR=0;			//��ȫ����λCR�Ĵ���ֵ  
	DMA1_Stream1->CR|=0<<6;		//���赽�洢��ģʽ 
	DMA1_Stream1->CR|=1<<8;		//ѭ��ģʽ
	DMA1_Stream1->CR|=0<<9;		//���������ģʽ
	DMA1_Stream1->CR|=1<<10;	//�洢������ģʽ
	DMA1_Stream1->CR|=(u16)width<<11;//�������ݳ���:16λ/32λ
	DMA1_Stream1->CR|=(u16)width<<13;//�洢�����ݳ���:16λ/32λ
	DMA1_Stream1->CR|=2<<16;	//�����ȼ�
	DMA1_Stream1->CR|=1<<18;	//˫����ģʽ
	DMA1_Stream1->CR|=0<<21;	//����ͻ�����δ���
	DMA1_Stream1->CR|=0<<23;	//�洢��ͻ�����δ���
	DMA1_Stream1->CR|=0<<25;	//ѡ��ͨ��0 SPDIF RX DRͨ�� 

	DMA1_Stream1->FCR&=~(1<<2);	//��ʹ��FIFOģʽ
	DMA1_Stream1->FCR&=~(3<<0);	//��FIFO ���� 	
}
 
//�ȴ�����ͬ��״̬,ͬ������Ժ��Զ��������״̬
//����ֵ:0,δͬ��;1,��ͬ��
u8 SPDIF_RX_WaitSync(void)
{
    u8 res=0;
    u8 timeout=0;
	SPDIF_RX_Mode(SPDIF_RX_SYNC);	//����Ϊͬ��ģʽ
	while(1)
	{
        timeout++;
        delay_ms(2);
		if(timeout>100)break;
		if(SPDIFRX->SR&(1<<5))	//ͬ�����?
		{
			res=1;				//���ͬ�����
			SPDIF_RX_Mode(SPDIF_RX_RCV);//�������ģʽ
			break;	  
		}
	}
	return res;
}

//��ȡSPDIF RX�յ�����Ƶ������
//����ֵ:0,����Ĳ�����
//  ����ֵ,��Ƶ������
u32 SPDIF_RX_GetSampleRate(void)
{
    u16 spdif_w5;
    u32 samplerate; 
    spdif_w5=SPDIFRX->SR>>16;
    samplerate=(spdif_dev.clock*5)/(spdif_w5&0X7FFF);
    samplerate>>=6;     									//����64 
    if((8000-1500<=samplerate)&&(samplerate<=8000+1500))samplerate=8000;			//8K�Ĳ�����
    else if((11025-1500<=samplerate)&&(samplerate<=11025+1500))samplerate=11025;	//11.025K�Ĳ�����
    else if((16000-1500<=samplerate)&&(samplerate<=16000+1500))samplerate=16000;	//16K�Ĳ�����
    else if((22050-1500<=samplerate)&&(samplerate<=22050+1500))samplerate=22050;	//22.05K�Ĳ�����
    else if((32000-1500<=samplerate)&&(samplerate<=32000+1500))samplerate=32000;	//32K�Ĳ�����
    else if((44100-1500<=samplerate)&&(samplerate<=44100+1500))samplerate=44100;	//44.1K�Ĳ�����
    else if((48000-1500<=samplerate)&&(samplerate<=48000+1500))samplerate=48000;	//48K�Ĳ�����
    else if((88200-1500<=samplerate)&&(samplerate<=88200+1500))samplerate=88200;	//88.2K�Ĳ�����
    else if((96000-1500<=samplerate)&&(samplerate<=96000+1500))samplerate=96000;	//96K�Ĳ�����
    else if((176400-6000<=samplerate)&&(samplerate<=176400+6000))samplerate=176400;	//176.4K�Ĳ�
    else if((192000-6000<=samplerate)&&(samplerate<=192000+6000))samplerate=192000;	//192K�Ĳ�
    else samplerate=0; 
	return samplerate;
}
//SAI DMA�ص�����ָ��
void (*spdif_rx_stop_callback)(void);	//�ص����� 

//SPDIF�����жϷ�����
void SPDIF_RX_IRQHandler(void)
{
    //������ʱ��ͬ����֡�����ж�,�������ж�һ��Ҫ����
	u32 sr=SPDIFRX->SR;
	OSIntEnter();   
	if(sr&((1<<8)|(1<<7)|(1<<6)))		//��ʱ����/ͬ������/֡����
	{  
        SPDIF_RX_Stop();				//�������󣬹ر�SPDIF����
		spdif_rx_stop_callback();		//���ûص�����
		SPDIF_RX_Mode(SPDIF_RX_IDLE);	//��������ʱ��ͬ����֡�����ʱ��Ҫ��SPDIFRXENд0������ж�
	}
	if(sr&(1<<3))						//�������������
	{
		SPDIFRX->IFCR|=1<<3;			//����������
	}
	if(sr&(1<<2))						//��������żУ�����
	{
		SPDIFRX->IFCR|=1<<2;			//�����żУ�����
	} 
	OSIntExit(); 
} 

//SPDIF��ʼ����
void SPDIF_RX_Start(void)
{ 
    spdif_dev.consta=1;				//����Ѿ���SPDIF
	DMA1_Stream1->CR|=1<<0;			//����SPDIF DR RX����   
}

//SPDIF�ر�
void SPDIF_RX_Stop(void)
{
    spdif_dev.consta=0;				//����Ѿ��ر�SPDIF  
	DMA1_Stream1->CR&=~(1<<0);		//����SPDIF DMA����  
} 











