#include "stm32h743i_eval_audio.h"
#include "usbd_audio_if.h"
#include "usbd_audio.h"
#include "sai.h"
#include "wm8978.h"
#include "sai.h" 
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//USB声卡底层接口函数 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/31
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
u8 volume=0;									//当前音量   
extern u8 usbd_audio_dataout_en;				//USB数据输出状态,在usbd_audio.c里面定义

//音频数据SAI DMA传输回调函数
void audio_sai_dma_callback(void) 
{      
	BSP_AUDIO_OUT_TransferComplete_CallBack();	//执行回调函数,更新数据
}   

//配置音频接口
//OutputDevice:输出设备选择,未用到.
//Volume:音量大小,0~100
//AudioFreq:音频采样率  
uint8_t BSP_AUDIO_OUT_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq)
{ 
	WM8978_I2S_Cfg(2,0);			//飞利浦标准,16位数据长度
	SAIA_Init(0,1,4);				//设置SAI,主发送,16位数据
	SAIA_SampleRate_Set(AudioFreq);	//设置采样率  
	BSP_AUDIO_OUT_SetVolume(Volume);//设置音量
 	sai_tx_callback=audio_sai_dma_callback;		//回调函数指向audio_sai_dma_callback
	printf("EVAL_AUDIO_Init:%d,%d\r\n",Volume,AudioFreq);
	return 0; 
}

//开始播放音频数据
//pBuffer:音频数据流首地址指针
//Size:数据流大小(单位:字节)
uint8_t BSP_AUDIO_OUT_Play(uint16_t* pBuffer, uint32_t Size)
{	
	SAIA_TX_DMA_Init((u8*)pBuffer,0,Size/2,1);	//配置TX DMA,16位 
	DMA2_Stream3->CR&=~(1<<8);		//单次模式
	DMA2_Stream3->CR&=~(1<<18);		//改成单缓冲模式 
	SAI_Play_Start();				//开启DMA  
	printf("EVAL_AUDIO_Play:%x,%d\r\n",pBuffer,Size);
	return 0;
}

//切换音频数据流buffer,每次DMA中断完成会调用该函数
//Addr:音频数据流缓存首地址
//Size:音频数据流大小(单位:harf word,也就是2个字节)
void BSP_AUDIO_OUT_ChangeBuffer(uint16_t *pData, uint16_t Size)
{ 
	if(usbd_audio_dataout_en==1)		//USB有音频数据输出
	{
		usbd_audio_dataout_en=0;		//清除标记位
	}else if(usbd_audio_dataout_en==0)	//没有收到任何数据,全部填充0
	{
		mymemset(pData,0,AUDIO_TOTAL_BUF_SIZE);	
		usbd_audio_dataout_en=2;		//标记已经清零了buffer,停止数据输出
	}
	if(Size!=AUDIO_TOTAL_BUF_SIZE)
	{
		//printf("chgbuf:%x,%d\r\n",pData,Size);//想观察数据,可以printf,但是会有轻微的音箱音质
	}
	DMA2_Stream3->CR&=~(1<<0); 		//关闭DMA传输 
	while(DMA2_Stream3->CR&0X1);	//确保DMA可以被设置  
	DMA2_Stream3->NDTR=Size/2;		//DMA 存储器0地址 
	DMA2_Stream3->CR|=1<<0;			//开启DMA传输		
}

//停止播放
//Option:控制参数,1/2,详见:CODEC_PDWN_HW定义
//返回值:0,成功
//    其他,设置失败
uint8_t BSP_AUDIO_OUT_Stop(uint32_t Option)
{
	printf("EVAL_AUDIO_Stop:%d\r\n",Option); 
	return 0;
}

//音量设置 
//Volume:0~100
//返回值:0,成功
//    其他,设置失败
uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume)
{ 
	volume=Volume; 
	WM8978_HPvol_Set(volume*0.63,volume*0.63);
	WM8978_SPKvol_Set(volume*0.63);
	return 0;
}

//静音设置
//cmd:0,不静音; 1,静音
//返回值:0,成功
//    其他,设置失败
uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd)
{
	return 0;
}























