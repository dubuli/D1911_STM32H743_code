#ifndef __MJPEG_H
#define __MJPEG_H  
#include "sys.h"
#include "jpegcodec.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//MJPEG��Ƶ���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/19
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   


extern jpeg_codec_typedef mjpeg;  

u8 mjpeg_jpeg_core_init(jpeg_codec_typedef *tjpeg);
void mjpeg_jpeg_core_destroy(jpeg_codec_typedef *tjpeg); 
void mjpeg_dma_in_callback(void);
void mjpeg_dma_out_callback(void);
void mjpeg_endofcovert_callback(void);
void mjpeg_hdrover_callback(void);
u8 mjpeg_init(u16 offx,u16 offy,u32 width,u32 height);
void mjpeg_free(void);
void mjpeg_ltdc_dma2d_yuv2rgb_fill(u16 sx,u16 sy,jpeg_codec_typedef *tjpeg);
void mjpeg_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color);
u8 mjpeg_decode(u8* buf,u32 bsize);
#endif
