#include "mjpeg.h" 
#include "malloc.h"
#include "ff.h"  
#include "lcd.h" 
#include "ltdc.h" 
#include "usart.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//MJPEG视频处理 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/19
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   

jpeg_codec_typedef mjpeg;  		//JPEG硬件解码结构体
u16 imgoffx,imgoffy;			//图像在LCD屏幕上x,y方向的偏移量
u16* rgb565buf;					//解码后的RGB图像显存地址
vu32 mjpeg_remain_size;			//MJPEG一帧图像的剩余大小(字节数)
vu8 mjpeg_fileover=0;			//MJPEG图片文件读取完成标志
	
u16 *mjpeg_rgb_framebuf;		//RGB屏帧缓存地址
extern u32 *ltdc_framebuf[2];	//LTDC LCD帧缓存数组指针,在ltdc.c里面定义


//视频解码版本的JPEG_Core_Init函数
//视频解码并不需要用到多个JPEG_DMA_INBUF_NB
//初始化硬件JPEG内核
//tjpeg:jpeg编解码控制结构体
//返回值:0,成功;
//    其他,失败
u8 mjpeg_jpeg_core_init(jpeg_codec_typedef *tjpeg)
{  
	RCC->AHB3ENR|=1<<5;				//使能硬件jpeg时钟 	 
	RCC->AHB3RSTR|=1<<5;			//复位硬件jpeg解码器	 
	RCC->AHB3RSTR&=~(1<<5);			//结束复位 	 
	JPEG->CR=0;						//先清零
	JPEG->CR|=1<<0;					//使能硬件JPEG
	JPEG->CONFR0&=~(1<<0);			//停止JPEG编解码进程
	JPEG->CR|=1<<13;				//清空输入fifo
	JPEG->CR|=1<<14;				//清空输出fifo
	JPEG->CFR=3<<5;					//清空标志 
	MY_NVIC_Init(1,3,JPEG_IRQn,2);	//抢占1，子优先级3，组2  
	JPEG->CONFR1|=1<<8;				//使能header处理
	return 0;
}
 
//视频解码版本的JPEG_Core_Destroy函数 
//关闭硬件JPEG内核,并释放内存
//tjpeg:jpeg编解码控制结构体
void mjpeg_jpeg_core_destroy(jpeg_codec_typedef *tjpeg)
{
	u8 i; 
	JPEG_DMA_Stop();//停止MDMA传输 
	for(i=0;i<JPEG_DMA_OUTBUF_NB;i++)myfree(SRAMIN,tjpeg->outbuf[i].buf);	//释放内存
}

//JPEG输入数据流,回调函数,用于获取JPEG文件原始数据
//每当JPEG DMA IN BUF为空的时候,调用该函数
void mjpeg_dma_in_callback(void)
{ 
	if(mjpeg_remain_size)							//还有剩余数据需要处理
	{
		mjpeg.inbuf[0].buf+=JPEG_DMA_INBUF_LEN;		//偏移到下一个位置
		if(mjpeg_remain_size<JPEG_DMA_INBUF_LEN)	//剩余数据比较少,一次就可以传输完成
		{
			mjpeg.inbuf[0].size=mjpeg_remain_size;	//传输大小等于剩余总大小
			mjpeg_remain_size=0;					//一次传输就可以搞完
		}else										//图片比较大,需要分多次传输
		{
			mjpeg.inbuf[0].size=JPEG_DMA_INBUF_LEN;	//按最大传输长度,分批次传输
			mjpeg_remain_size-=JPEG_DMA_INBUF_LEN;	//剩余长度递减
		}
		JPEG_IN_DMA_Resume((u32)mjpeg.inbuf[0].buf,mjpeg.inbuf[0].size);	//继续下一次DMA传输		
	}else mjpeg_fileover=1;							//文件读取完成 
}
//JPEG输出数据流(YCBCR)回调函数,用于输出YCbCr数据流
void mjpeg_dma_out_callback(void)
{	  
	u32 *pdata=0; 
	mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta=1;	//此buf已满
	mjpeg.outbuf[mjpeg.outbuf_write_ptr].size=mjpeg.yuvblk_size-(MDMA_Channel6->CBNDTR&0X1FFFF);	//此buf里面数据的长度
	if(mjpeg.state==JPEG_STATE_FINISHED)		//如果文件已经解码完成,需要读取DOR最后的数据(<=32字节)
	{
		pdata=(u32*)(mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf+mjpeg.outbuf[mjpeg.outbuf_write_ptr].size);
		while(JPEG->SR&(1<<4))
		{
			*pdata=JPEG->DOR;
			pdata++;
			mjpeg.outbuf[mjpeg.outbuf_write_ptr].size+=4; 
		}
	}  
	mjpeg.outbuf_write_ptr++;					//指向下一个buf
	if(mjpeg.outbuf_write_ptr>=JPEG_DMA_OUTBUF_NB)mjpeg.outbuf_write_ptr=0;//归零
	if(mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta==1)//无有效buf
	{ 
 		mjpeg.outdma_pause=1;					//标记暂停
	}else										//有效的buf
	{
		JPEG_OUT_DMA_Resume((u32)mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf,mjpeg.yuvblk_size);//继续下一次DMA传输
	}
}
//JPEG整个文件解码完成回调函数
void mjpeg_endofcovert_callback(void)
{ 
	mjpeg.state=JPEG_STATE_FINISHED;			//标记JPEG解码完成
}
//JPEG header解析成功回调函数
void mjpeg_hdrover_callback(void)
{ 
	mjpeg.state=JPEG_STATE_HEADEROK;			//HEADER获取成功
	JPEG_Get_Info(&mjpeg);						//获取JPEG相关信息,包括大小,色彩空间,抽样等 
	//需要获取JPEG基本信息以后,才能根据jpeg输出大小和采样方式,来计算输出缓冲大小,并启动输出MDMA
	switch(mjpeg.Conf.ChromaSubsampling)
	{
		case JPEG_420_SUBSAMPLING: 
			mjpeg.yuvblk_size=24*mjpeg.Conf.ImageWidth;	//YUV420,每个YUV像素占1.5个字节.每次输出16行.16*1.5=24
			mjpeg.yuvblk_height=16;						//每次输出16行
			break;
		case JPEG_422_SUBSAMPLING:
			mjpeg.yuvblk_size=16*mjpeg.Conf.ImageWidth;	//YUV420,每个YUV像素占2个字节.每次输出8行.8*2=16
			mjpeg.yuvblk_height=8;						//每次输出8行
			break;
		case JPEG_444_SUBSAMPLING:
			mjpeg.yuvblk_size=24*mjpeg.Conf.ImageWidth;	//YUV420,每个YUV像素占3个字节.每次输出8行.8*3=24
			mjpeg.yuvblk_height=8;						//每次输出8行	
			break;
	}  
	mjpeg.yuvblk_curheight=0;	//当前行计数器清零 	
	if(mjpeg.outbuf[1].buf!=NULL)		//两个buf都申请OK
	{
 		JPEG_OUT_DMA_Init((u32)mjpeg.outbuf[0].buf,mjpeg.yuvblk_size);	//配置输出DMA
		JPEG_OUT_DMA_Start();			//启动DMA OUT传输,开始接收JPEG解码数据流 
	} 
} 
//初始化MJPEG
//offx,offy:视频在LCD上面,X,Y方向的偏移量
//width,height:图像的宽度和高度
//返回值:0,成功;其他,失败.
u8 mjpeg_init(u16 offx,u16 offy,u32 width,u32 height)
{
	u8 i;
	u8 res;
	res=mjpeg_jpeg_core_init(&mjpeg);		//初始化JPEG内核,不申请IN BUF
	if(res)return 1;
	for(i=0;i<JPEG_DMA_OUTBUF_NB;i++)
	{
		mjpeg.outbuf[i].buf=mymalloc(SRAMIN,width*24+32);	//最大是图片宽度的24倍,另外还可能会多需要32字节内存  
		if(mjpeg.outbuf[i].buf==NULL)return 2; 
	}
	rgb565buf=0;							//RGB565 BUF指针清零
	if(lcdltdc.pwidth==0||lcddev.dir==0)	//如果不是RGB横屏,则需要rgb565buf数组
	{
		rgb565buf=mymalloc(SRAMEX,width*height*2);	//申请RGB缓存
		if(rgb565buf==NULL)return  3;
	}
	imgoffx=offx;
	imgoffy=offy;
	mjpeg_rgb_framebuf=(u16*)ltdc_framebuf[lcdltdc.activelayer];//指向RGBLCD当前显存 
	return 0; 
}
//释放内存
void mjpeg_free(void)
{ 
	mjpeg_jpeg_core_destroy(&mjpeg); 
	myfree(SRAMEX,rgb565buf);	
} 

//对于RGB屏,且是横屏模式,则可以利用DMA2D,直接将JPEG解码的YUV数据转换成RGB数据,
//并填充到对应的GRAM里面,所有操作全硬件完成,速度非常快.
//sx,sy:起始坐标
//tjpeg:jpeg解码结构体 
void mjpeg_ltdc_dma2d_yuv2rgb_fill(u16 sx,u16 sy,jpeg_codec_typedef *tjpeg)
{
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	u32 regval=0;
	u32 cm=0;						//采样方式 
	if(tjpeg->Conf.ChromaSubsampling==JPEG_420_SUBSAMPLING)cm=DMA2D_CSS_420;	//YUV420转RGB
	if(tjpeg->Conf.ChromaSubsampling==JPEG_422_SUBSAMPLING)cm=DMA2D_CSS_422;	//YUV422转RGB
	else if(tjpeg->Conf.ChromaSubsampling==JPEG_444_SUBSAMPLING)cm=DMA2D_NO_CSS;//YUV444转RGB	 
	offline=lcdltdc.pwidth-tjpeg->Conf.ImageWidth;
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*sy+sx)); 
	RCC->AHB3ENR|=1<<4;				//使能DMA2D时钟	
	RCC->AHB3RSTR|=1<<4;			//复位DMA2D
	RCC->AHB3RSTR&=~(1<<4);			//结束复位
	DMA2D->CR&=~(1<<0);				//先停止DMA2D
	DMA2D->CR=1<<16;				//MODE[1:0]=01,存储器到存储器,带PFC模式
	DMA2D->OPFCCR=2<<0;				//CM[2:0]=010,输出为RGB565格式
	DMA2D->OOR=offline;				//设置行偏移 
	DMA2D->IFCR|=1<<1;				//清除传输完成标志  	
	regval=11<<0;					//CM[3:0]=1011,输入数据为YCbCr格式
	regval|=cm<<18;					//CSS[1:0]=cm,Chroma Sub-Sampling:0,4:4:4;1,4:2:2;2,4:2:0 
	DMA2D->FGPFCCR=regval;			//设置FGPCCR寄存器
	DMA2D->FGOR=0;					//前景层行偏移为0
	DMA2D->NLR=tjpeg->yuvblk_height|(tjpeg->Conf.ImageWidth<<16);	//设定行数寄存器 
	DMA2D->OMAR=addr;				//输出存储器地址 
	DMA2D->FGMAR=(u32)tjpeg->outbuf[tjpeg->outbuf_read_ptr].buf;	//源地址
	DMA2D->CR|=1<<0;				//启动DMA2D
	while((DMA2D->ISR&(1<<1))==0)	//等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFFF)break;	//超时退出
	} 
	tjpeg->yuvblk_curheight+=tjpeg->yuvblk_height;	//偏移到下一个内存地址 
	//YUV2RGB转码结束后,再复位一次DMA2D
	RCC->AHB3RSTR|=1<<4;			//复位DMA2D
	RCC->AHB3RSTR&=~(1<<4);			//结束复位 
} 
 
//填充颜色
//x,y:起始坐标
//width，height：宽度和高度。
//*color：颜色数组
void mjpeg_fill_color(u16 x,u16 y,u16 width,u16 height,u16 *color)
{  
	u16 i,j;
	u32 param1;	
	u32 param2;
	u32 param3;
	u16* pdata; 
	if(lcdltdc.pwidth!=0&&lcddev.dir==0)//如果是RGB屏,且竖屏,则填充函数不可直接用
	{ 
		param1=lcdltdc.pixsize*lcdltdc.pwidth*(lcdltdc.pheight-x-1)+lcdltdc.pixsize*y;	//将运算先做完,提高速度
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
	}else if(lcdltdc.pwidth==0)LCD_Color_Fill(x,y,x+width-1,y+height-1,color);			//是MCU屏(RGB横屏无需填充!!,在YUV转换的时候,直接就填充了)
}

//解码一副JPEG图片
//注意:
//1,待解吗图片的分辨率,必须小于等于屏幕的分辨率!
//2,请保证图片的宽度是16的倍数,以免左侧出现花纹.
//buf:jpeg数据流数组
//bsize:数组大小
//返回值:0,成功
//    其他,错误
u8 mjpeg_decode(u8* buf,u32 bsize) 
{ 
	vu32 timecnt=0;
	if(bsize==0)return 0;  
	JPEG_Decode_Init(&mjpeg);						//初始化硬件JPEG解码器
	mjpeg_remain_size=bsize;						//记录当前图片的大小(字节数)
	mjpeg.inbuf[0].buf=buf;							//指向jpeg数据流的首地址  
	mjpeg_fileover=0;								//标记未读完
	if(mjpeg_remain_size<JPEG_DMA_INBUF_LEN)		//图片比较小,一次就可以传输完成
	{
		//mjpeg.inbuf[0].size=mjpeg_remain_size;	//传输大小等于总大小
		//mjpeg_remain_size=0;						//一次传输就可以搞完
		return 0;									//图片尺寸比较小，直接不解码
	}else											//图片比较大,需要分多次传输
	{
		mjpeg.inbuf[0].size=JPEG_DMA_INBUF_LEN;		//按最大传输长度,分批次传输
		mjpeg_remain_size-=JPEG_DMA_INBUF_LEN;		//剩余长度
	} 
	JPEG_IN_DMA_Init((u32)mjpeg.inbuf[0].buf,mjpeg.inbuf[0].size);	//配置输入DMA 
	jpeg_in_callback=mjpeg_dma_in_callback;			//JPEG DMA读取数据回调函数
	jpeg_out_callback=mjpeg_dma_out_callback; 		//JPEG DMA输出数据回调函数
	jpeg_eoc_callback=mjpeg_endofcovert_callback;	//JPEG 解码结束回调函数
	jpeg_hdp_callback=mjpeg_hdrover_callback; 		//JPEG Header解码完成回调函数
 	JPEG_IN_DMA_Start();							//启动DMA IN传输,开始解码JPEG图片 
	while(1)
	{ 
		if(mjpeg.outbuf[mjpeg.outbuf_read_ptr].sta==1)	//buf里面有数据要处理
		{
			timecnt=0;									//计时器清零
			//SCB_CleanInvalidateDCache();				//清空D catch
			if(lcdltdc.pwidth==0||lcddev.dir==0)		//非RGB横屏,需要先将YUV数据解码到rgb565buf,然后再从rgb565buf拷贝到LCD的GRAM
			{
				JPEG_DMA2D_YUV2RGB_Conversion(&mjpeg,(u32*)rgb565buf);	//利用DMA2D,将YUV图像转成RGB565图像 
			}else										//RGB横屏,直接将YUV数据解码到LCD的GRAM,省去了拷贝操作,可以达到最佳性能
			{
				mjpeg_ltdc_dma2d_yuv2rgb_fill(imgoffx,imgoffy+mjpeg.yuvblk_curheight,&mjpeg);	//DMA2D将YUV解码到LCD GRAM,速度最快
			}
			//SCB_CleanInvalidateDCache();				//清空D catch
			mjpeg.outbuf[mjpeg.outbuf_read_ptr].sta=0;	//标记buf为空
			mjpeg.outbuf[mjpeg.outbuf_read_ptr].size=0;	//数据量清空
			mjpeg.outbuf_read_ptr++;
			if(mjpeg.outbuf_read_ptr>=JPEG_DMA_OUTBUF_NB)mjpeg.outbuf_read_ptr=0;	//限制范围
			if(mjpeg.yuvblk_curheight>=mjpeg.Conf.ImageHeight)break;				//当前高度等于或者超过图片分辨率的高度,则说明解码完成了,直接退出.
		}else if(mjpeg.outdma_pause==1&&mjpeg.outbuf[mjpeg.outbuf_write_ptr].sta==0)//out暂停,且当前writebuf已经为空了,则恢复out输出
		{
 			JPEG_OUT_DMA_Resume((u32)mjpeg.outbuf[mjpeg.outbuf_write_ptr].buf,mjpeg.yuvblk_size);//继续下一次DMA传输
 			mjpeg.outdma_pause=0;
		}
		if(mjpeg.state==JPEG_STATE_ERROR)		//解码出错,直接退出
		{ 
			break;
		} 
		if(mjpeg.state==JPEG_STATE_FINISHED)	//解码结束了,检查是否异常结束
		{ 
			if(mjpeg.yuvblk_curheight<mjpeg.Conf.ImageHeight)
			{
				if(mjpeg.Conf.ImageHeight>(mjpeg.yuvblk_curheight+16))	//数据异常,直接退出
				{
					mjpeg.state=JPEG_STATE_ERROR;	//标记错误
//					printf("early finished!\r\n");
					break;
				} 
			} 
		}
		if(mjpeg_fileover)	//文件读完了,及时退出,防止死循环
		{  
			if(mjpeg.state==JPEG_STATE_NOHEADER)break;	//解码JPEG头失败了
		}
		timecnt++; 
		if(timecnt>0X3FFFF)	//超时退出
		{
			printf("failed\r\n");
			break;						
		}   
	} 
	if(mjpeg.state==JPEG_STATE_FINISHED)	//解码完成了
	{
		mjpeg_fill_color(imgoffx,imgoffy,mjpeg.Conf.ImageWidth,mjpeg.Conf.ImageHeight,rgb565buf);  
	} 
	return 0;
}

























