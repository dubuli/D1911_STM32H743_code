#include "mjpeg.h" 
#include "malloc.h"
#include "ff.h"  
#include "lcd.h" 
#include "ltdc.h" 
#include "usart.h" 
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

jpeg_codec_typedef mjpeg;  		//JPEGӲ������ṹ��
u16 imgoffx,imgoffy;			//ͼ����LCD��Ļ��x,y�����ƫ����
u16* rgb565buf;					//������RGBͼ���Դ��ַ
vu32 mjpeg_remain_size;			//MJPEGһ֡ͼ���ʣ���С(�ֽ���)
vu8 mjpeg_fileover=0;			//MJPEGͼƬ�ļ���ȡ��ɱ�־
	
u16 *mjpeg_rgb_framebuf;		//RGB��֡�����ַ
extern u32 *ltdc_framebuf[2];	//LTDC LCD֡��������ָ��,��ltdc.c���涨��


//��Ƶ����汾��JPEG_Core_Init����
//��Ƶ���벢����Ҫ�õ����JPEG_DMA_INBUF_NB
//��ʼ��Ӳ��JPEG�ں�
//tjpeg:jpeg�������ƽṹ��
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 mjpeg_jpeg_core_init(jpeg_codec_typedef *tjpeg)
{  
	RCC->AHB3ENR|=1<<5;				//ʹ��Ӳ��jpegʱ�� 	 
	RCC->AHB3RSTR|=1<<5;			//��λӲ��jpeg������	 
	RCC->AHB3RSTR&=~(1<<5);			//������λ 	 
	JPEG->CR=0;						//������
	JPEG->CR|=1<<0;					//ʹ��Ӳ��JPEG
	JPEG->CONFR0&=~(1<<0);			//ֹͣJPEG��������
	JPEG->CR|=1<<13;				//�������fifo
	JPEG->CR|=1<<14;				//������fifo
	JPEG->CFR=3<<5;					//��ձ�־ 
	MY_NVIC_Init(1,3,JPEG_IRQn,2);	//��ռ1�������ȼ�3����2  
	JPEG->CONFR1|=1<<8;				//ʹ��header����
	return 0;
}
 
//��Ƶ����汾��JPEG_Core_Destroy���� 
//�ر�Ӳ��JPEG�ں�,���ͷ��ڴ�
//tjpeg:jpeg�������ƽṹ��
void mjpeg_jpeg_core_destroy(jpeg_codec_typedef *tjpeg)
{
	u8 i; 
	JPEG_DMA_Stop();//ֹͣMDMA���� 
	for(i=0;i<JPEG_DMA_OUTBUF_NB;i++)myfree(SRAMIN,tjpeg->outbuf[i].buf);	//�ͷ��ڴ�
}

//JPEG����������,�ص�����,���ڻ�ȡJPEG�ļ�ԭʼ����
//ÿ��JPEG DMA IN BUFΪ�յ�ʱ��,���øú���
void mjpeg_dma_in_callback(void)
{ 
	if(mjpeg_remain_size)							//����ʣ��������Ҫ����
	{
		mjpeg.inbuf[0].buf+=JPEG_DMA_INBUF_LEN;		//ƫ�Ƶ���һ��λ��
		if(mjpeg_remain_size<JPEG_DMA_INBUF_LEN)	//ʣ�����ݱȽ���,һ�ξͿ��Դ������
		{
			mjpeg.inbuf[0].size=mjpeg_remain_size;	//�����С����ʣ���ܴ�С
			mjpeg_remain_size=0;					//һ�δ���Ϳ��Ը���
		}else										//ͼƬ�Ƚϴ�,��Ҫ�ֶ�δ���
		{
			mjpeg.inbuf[0].size=JPEG_DMA_INBUF_LEN;	//������䳤��,�����δ���
			mjpeg_remain_size-=JPEG_DMA_INBUF_LEN;	//ʣ�೤�ȵݼ�
		}
		JPEG_IN_DMA_Resume((u32)mjpeg.inbuf[0].buf,mjpeg.inbuf[0].size);	//������һ��DMA����		
	}else mjpeg_fileover=1;							//�ļ���ȡ��� 
}
//JPEG���������(YCBCR)�ص�����,�������YCbCr������
void mjpeg_dma_out_callback(void)
{	  
	u32 *pdata=0; 
	mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta=1;	//��buf����
	mjpeg.outbuf[mjpeg.outbuf_write_ptr].size=mjpeg.yuvblk_size-(MDMA_Channel6->CBNDTR&0X1FFFF);	//��buf�������ݵĳ���
	if(mjpeg.state==JPEG_STATE_FINISHED)		//����ļ��Ѿ��������,��Ҫ��ȡDOR��������(<=32�ֽ�)
	{
		pdata=(u32*)(mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf+mjpeg.outbuf[mjpeg.outbuf_write_ptr].size);
		while(JPEG->SR&(1<<4))
		{
			*pdata=JPEG->DOR;
			pdata++;
			mjpeg.outbuf[mjpeg.outbuf_write_ptr].size+=4; 
		}
	}  
	mjpeg.outbuf_write_ptr++;					//ָ����һ��buf
	if(mjpeg.outbuf_write_ptr>=JPEG_DMA_OUTBUF_NB)mjpeg.outbuf_write_ptr=0;//����
	if(mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta==1)//����Чbuf
	{ 
 		mjpeg.outdma_pause=1;					//�����ͣ
	}else										//��Ч��buf
	{
		JPEG_OUT_DMA_Resume((u32)mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf,mjpeg.yuvblk_size);//������һ��DMA����
	}
}
//JPEG�����ļ�������ɻص�����
void mjpeg_endofcovert_callback(void)
{ 
	mjpeg.state=JPEG_STATE_FINISHED;			//���JPEG�������
}
//JPEG header�����ɹ��ص�����
void mjpeg_hdrover_callback(void)
{ 
	mjpeg.state=JPEG_STATE_HEADEROK;			//HEADER��ȡ�ɹ�
	JPEG_Get_Info(&mjpeg);						//��ȡJPEG�����Ϣ,������С,ɫ�ʿռ�,������ 
	//��Ҫ��ȡJPEG������Ϣ�Ժ�,���ܸ���jpeg�����С�Ͳ�����ʽ,��������������С,���������MDMA
	switch(mjpeg.Conf.ChromaSubsampling)
	{
		case JPEG_420_SUBSAMPLING: 
			mjpeg.yuvblk_size=24*mjpeg.Conf.ImageWidth;	//YUV420,ÿ��YUV����ռ1.5���ֽ�.ÿ�����16��.16*1.5=24
			mjpeg.yuvblk_height=16;						//ÿ�����16��
			break;
		case JPEG_422_SUBSAMPLING:
			mjpeg.yuvblk_size=16*mjpeg.Conf.ImageWidth;	//YUV420,ÿ��YUV����ռ2���ֽ�.ÿ�����8��.8*2=16
			mjpeg.yuvblk_height=8;						//ÿ�����8��
			break;
		case JPEG_444_SUBSAMPLING:
			mjpeg.yuvblk_size=24*mjpeg.Conf.ImageWidth;	//YUV420,ÿ��YUV����ռ3���ֽ�.ÿ�����8��.8*3=24
			mjpeg.yuvblk_height=8;						//ÿ�����8��	
			break;
	}  
	mjpeg.yuvblk_curheight=0;	//��ǰ�м��������� 	
	if(mjpeg.outbuf[1].buf!=NULL)		//����buf������OK
	{
 		JPEG_OUT_DMA_Init((u32)mjpeg.outbuf[0].buf,mjpeg.yuvblk_size);	//�������DMA
		JPEG_OUT_DMA_Start();			//����DMA OUT����,��ʼ����JPEG���������� 
	} 
} 
//��ʼ��MJPEG
//offx,offy:��Ƶ��LCD����,X,Y�����ƫ����
//width,height:ͼ��Ŀ�Ⱥ͸߶�
//����ֵ:0,�ɹ�;����,ʧ��.
u8 mjpeg_init(u16 offx,u16 offy,u32 width,u32 height)
{
	u8 i;
	u8 res;
	res=mjpeg_jpeg_core_init(&mjpeg);		//��ʼ��JPEG�ں�,������IN BUF
	if(res)return 1;
	for(i=0;i<JPEG_DMA_OUTBUF_NB;i++)
	{
		mjpeg.outbuf[i].buf=mymalloc(SRAMIN,width*24+32);	//�����ͼƬ��ȵ�24��,���⻹���ܻ����Ҫ32�ֽ��ڴ�  
		if(mjpeg.outbuf[i].buf==NULL)return 2; 
	}
	rgb565buf=0;							//RGB565 BUFָ������
	if(lcdltdc.pwidth==0||lcddev.dir==0)	//�������RGB����,����Ҫrgb565buf����
	{
		rgb565buf=mymalloc(SRAMEX,width*height*2);	//����RGB����
		if(rgb565buf==NULL)return  3;
	}
	imgoffx=offx;
	imgoffy=offy;
	mjpeg_rgb_framebuf=(u16*)ltdc_framebuf[lcdltdc.activelayer];//ָ��RGBLCD��ǰ�Դ� 
	return 0; 
}
//�ͷ��ڴ�
void mjpeg_free(void)
{ 
	mjpeg_jpeg_core_destroy(&mjpeg); 
	myfree(SRAMEX,rgb565buf);	
} 

//����RGB��,���Ǻ���ģʽ,���������DMA2D,ֱ�ӽ�JPEG�����YUV����ת����RGB����,
//����䵽��Ӧ��GRAM����,���в���ȫӲ�����,�ٶȷǳ���.
//sx,sy:��ʼ����
//tjpeg:jpeg����ṹ�� 
void mjpeg_ltdc_dma2d_yuv2rgb_fill(u16 sx,u16 sy,jpeg_codec_typedef *tjpeg)
{
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	u32 regval=0;
	u32 cm=0;						//������ʽ 
	if(tjpeg->Conf.ChromaSubsampling==JPEG_420_SUBSAMPLING)cm=DMA2D_CSS_420;	//YUV420תRGB
	if(tjpeg->Conf.ChromaSubsampling==JPEG_422_SUBSAMPLING)cm=DMA2D_CSS_422;	//YUV422תRGB
	else if(tjpeg->Conf.ChromaSubsampling==JPEG_444_SUBSAMPLING)cm=DMA2D_NO_CSS;//YUV444תRGB	 
	offline=lcdltdc.pwidth-tjpeg->Conf.ImageWidth;
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*sy+sx)); 
	RCC->AHB3ENR|=1<<4;				//ʹ��DMA2Dʱ��	
	RCC->AHB3RSTR|=1<<4;			//��λDMA2D
	RCC->AHB3RSTR&=~(1<<4);			//������λ
	DMA2D->CR&=~(1<<0);				//��ֹͣDMA2D
	DMA2D->CR=1<<16;				//MODE[1:0]=01,�洢�����洢��,��PFCģʽ
	DMA2D->OPFCCR=2<<0;				//CM[2:0]=010,���ΪRGB565��ʽ
	DMA2D->OOR=offline;				//������ƫ�� 
	DMA2D->IFCR|=1<<1;				//���������ɱ�־  	
	regval=11<<0;					//CM[3:0]=1011,��������ΪYCbCr��ʽ
	regval|=cm<<18;					//CSS[1:0]=cm,Chroma Sub-Sampling:0,4:4:4;1,4:2:2;2,4:2:0 
	DMA2D->FGPFCCR=regval;			//����FGPCCR�Ĵ���
	DMA2D->FGOR=0;					//ǰ������ƫ��Ϊ0
	DMA2D->NLR=tjpeg->yuvblk_height|(tjpeg->Conf.ImageWidth<<16);	//�趨�����Ĵ��� 
	DMA2D->OMAR=addr;				//����洢����ַ 
	DMA2D->FGMAR=(u32)tjpeg->outbuf[tjpeg->outbuf_read_ptr].buf;	//Դ��ַ
	DMA2D->CR|=1<<0;				//����DMA2D
	while((DMA2D->ISR&(1<<1))==0)	//�ȴ��������
	{
		timeout++;
		if(timeout>0X1FFFFFF)break;	//��ʱ�˳�
	} 
	tjpeg->yuvblk_curheight+=tjpeg->yuvblk_height;	//ƫ�Ƶ���һ���ڴ��ַ 
	//YUV2RGBת�������,�ٸ�λһ��DMA2D
	RCC->AHB3RSTR|=1<<4;			//��λDMA2D
	RCC->AHB3RSTR&=~(1<<4);			//������λ 
} 
 
//�����ɫ
//x,y:��ʼ����
//width��height����Ⱥ͸߶ȡ�
//*color����ɫ����
void mjpeg_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color)
{  
	u16 i,j;
	u32 param1;	
	u32 param2;
	u32 param3;
	u16* pdata; 
	if(lcdltdc.pwidth!=0&&lcddev.dir==0)//�����RGB��,������,����亯������ֱ����
	{ 
		param1=lcdltdc.pixsize*lcdltdc.pwidth*(lcdltdc.pheight-x-1)+lcdltdc.pixsize*y;	//������������,����ٶ�
		param2=lcdltdc.pixsize*lcdltdc.pwidth;
		for(i=0;i<height;i++)
		{
			param3=i*lcdltdc.pixsize+param1;
			pdata=color+i*width;
			for(j=0;j<width;j++)
			{ 
				*(u16*)((u32)mjpeg_rgb_framebuf+param3-param2*j)=pdata[j];  
			}
		}
	}else if(lcdltdc.pwidth==0)LCD_Color_Fill(x,y,x+width-1,y+height-1,color);			//��MCU��(RGB�����������!!,��YUVת����ʱ��,ֱ�Ӿ������)
}

//����һ��JPEGͼƬ
//ע��:
//1,������ͼƬ�ķֱ���,����С�ڵ�����Ļ�ķֱ���!
//2,�뱣֤ͼƬ�Ŀ����16�ı���,���������ֻ���.
//buf:jpeg����������
//bsize:�����С
//����ֵ:0,�ɹ�
//    ����,����
u8 mjpeg_decode(u8* buf,u32 bsize) 
{ 
	vu32 timecnt=0;
	if(bsize==0)return 0;  
	JPEG_Decode_Init(&mjpeg);						//��ʼ��Ӳ��JPEG������
	mjpeg_remain_size=bsize;						//��¼��ǰͼƬ�Ĵ�С(�ֽ���)
	mjpeg.inbuf[0].buf=buf;							//ָ��jpeg���������׵�ַ  
	mjpeg_fileover=0;								//���δ����
	if(mjpeg_remain_size<JPEG_DMA_INBUF_LEN)		//ͼƬ�Ƚ�С,һ�ξͿ��Դ������
	{
		//mjpeg.inbuf[0].size=mjpeg_remain_size;	//�����С�����ܴ�С
		//mjpeg_remain_size=0;						//һ�δ���Ϳ��Ը���
		return 0;									//ͼƬ�ߴ�Ƚ�С��ֱ�Ӳ�����
	}else											//ͼƬ�Ƚϴ�,��Ҫ�ֶ�δ���
	{
		mjpeg.inbuf[0].size=JPEG_DMA_INBUF_LEN;		//������䳤��,�����δ���
		mjpeg_remain_size-=JPEG_DMA_INBUF_LEN;		//ʣ�೤��
	} 
	JPEG_IN_DMA_Init((u32)mjpeg.inbuf[0].buf,mjpeg.inbuf[0].size);	//��������DMA 
	jpeg_in_callback=mjpeg_dma_in_callback;			//JPEG DMA��ȡ���ݻص�����
	jpeg_out_callback=mjpeg_dma_out_callback; 		//JPEG DMA������ݻص�����
	jpeg_eoc_callback=mjpeg_endofcovert_callback;	//JPEG ��������ص�����
	jpeg_hdp_callback=mjpeg_hdrover_callback; 		//JPEG Header������ɻص�����
 	JPEG_IN_DMA_Start();							//����DMA IN����,��ʼ����JPEGͼƬ 
	while(1)
	{ 
		if(mjpeg.outbuf[mjpeg.outbuf_read_ptr].sta==1)	//buf����������Ҫ����
		{
			//SCB_CleanInvalidateDCache();				//���D catch
			if(lcdltdc.pwidth==0||lcddev.dir==0)		//��RGB����,��Ҫ�Ƚ�YUV���ݽ��뵽rgb565buf,Ȼ���ٴ�rgb565buf������LCD��GRAM
			{
				JPEG_DMA2D_YUV2RGB_Conversion(&mjpeg,(u32*)rgb565buf);	//����DMA2D,��YUVͼ��ת��RGB565ͼ�� 
			}else										//RGB����,ֱ�ӽ�YUV���ݽ��뵽LCD��GRAM,ʡȥ�˿�������,���Դﵽ�������
			{
				mjpeg_ltdc_dma2d_yuv2rgb_fill(imgoffx,imgoffy+mjpeg.yuvblk_curheight,&mjpeg);	//DMA2D��YUV���뵽LCD GRAM,�ٶ����
			}
			//SCB_CleanInvalidateDCache();				//���D catch
			mjpeg.outbuf[mjpeg.outbuf_read_ptr].sta=0;	//���bufΪ��
			mjpeg.outbuf[mjpeg.outbuf_read_ptr].size=0;	//���������
			mjpeg.outbuf_read_ptr++;
			if(mjpeg.outbuf_read_ptr>=JPEG_DMA_OUTBUF_NB)mjpeg.outbuf_read_ptr=0;	//���Ʒ�Χ
			if(mjpeg.yuvblk_curheight>=mjpeg.Conf.ImageHeight)break;				//��ǰ�߶ȵ��ڻ��߳���ͼƬ�ֱ��ʵĸ߶�,��˵�����������,ֱ���˳�.
		}else if(mjpeg.outdma_pause==1&&mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta==0)//out��ͣ,�ҵ�ǰwritebuf�Ѿ�Ϊ����,��ָ�out���
		{
 			JPEG_OUT_DMA_Resume((u32)mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf,mjpeg.yuvblk_size);//������һ��DMA����
 			mjpeg.outdma_pause=0;
		}
		if(mjpeg.state==JPEG_STATE_ERROR)		//�������,ֱ���˳�
		{ 
			break;
		} 
		if(mjpeg.state==JPEG_STATE_FINISHED)	//���������,����Ƿ��쳣����
		{ 
			if(mjpeg.yuvblk_curheight<mjpeg.Conf.ImageHeight)
			{
				if(mjpeg.Conf.ImageHeight>(mjpeg.yuvblk_curheight+16))	//�����쳣,ֱ���˳�
				{
					mjpeg.state=JPEG_STATE_ERROR;	//��Ǵ���
					printf("early finished!\r\n");
					break;
				}
			}
		}
		if(mjpeg_fileover)	//�ļ�������,��ʱ�˳�,��ֹ��ѭ��
		{
			timecnt++;   
			if(mjpeg.state==JPEG_STATE_NOHEADER)break;	//����JPEGͷʧ����
			if(timecnt>0X3FFFF)break;					//��ʱ�˳�
		}
	}    
	if(mjpeg.state==JPEG_STATE_FINISHED)	//���������
	{
		mjpeg_fill_color(imgoffx,imgoffy,mjpeg.Conf.ImageWidth,mjpeg.Conf.ImageHeight,rgb565buf);  
	} 
	return 0;
}

























