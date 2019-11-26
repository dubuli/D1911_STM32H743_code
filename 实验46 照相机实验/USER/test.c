#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h" 
#include "malloc.h"  
#include "nand.h"    
#include "ftl.h"  
#include "w25qxx.h"    
#include "sdmmc_sdcard.h" 
#include "ff.h"  
#include "exfuns.h" 
#include "text.h"	
#include "piclib.h"	
#include "string.h"		
#include "ltdc.h" 
#include "ov5640.h" 
#include "dcmi.h"  
#include "pcf8574.h"  	 
//ALIENTEK 阿波罗STM32H7开发板 实验46
//照相机 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  



vu8 bmp_request=0;						//bmp拍照请求:0,无bmp拍照请求;1,有bmp拍照请求,需要在帧中断里面,关闭DCMI接口.
u8 ovx_mode=0;							//bit0:0,RGB565模式;1,JPEG模式 
u16 curline=0;							//摄像头输出数据,当前行编号
u16 yoffset=0;							//y方向的偏移量

#define jpeg_buf_size   4*1024*1024		//定义JPEG数据缓存jpeg_buf的大小(4M字节)
#define jpeg_line_size	2*1024			//定义DMA接收数据时,一行数据的最大值

u32 *dcmi_line_buf[2];					//RGB屏时,摄像头采用一行一行读取,定义行缓存  
u32 *jpeg_data_buf;						//JPEG数据缓存buf 

volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
		 
//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
void jpeg_data_process(void)
{
	u16 i;
	u16 rlen;			//剩余数据长度
	u32 *pbuf;
	curline=yoffset;	//行数复位
	if(ovx_mode&0X01)	//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{
			DMA1_Stream1->CR&=~(1<<0);		//停止当前传输
			while(DMA1_Stream1->CR&0X01);	//等待DMA1_Stream1可配置 
			rlen=jpeg_line_size-DMA1_Stream1->NDTR;//得到剩余数据长度	
			pbuf=jpeg_data_buf+jpeg_data_len;//偏移到有效数据末尾,继续添加
			if(DMA1_Stream1->CR&(1<<19))for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[1][i];//读取buf1里面的剩余数据
			else for(i=0;i<rlen;i++)pbuf[i]=dcmi_line_buf[0][i];//读取buf0里面的剩余数据 
			jpeg_data_len+=rlen;			//加上剩余长度
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{
			DMA1_Stream1->NDTR=jpeg_line_size;//传输长度为jpeg_buf_size*4字节
			DMA1_Stream1->CR|=1<<0;			//重新传输
			jpeg_data_ok=0;					//标记数据未采集
			jpeg_data_len=0;				//数据重新开始
		}
	}else
	{  
		if(bmp_request==1)	//有bmp拍照请求,关闭DCMI
		{
			DCMI_Stop();	//停止DCMI
			bmp_request=0;	//标记请求处理完成.
		}
		LCD_SetCursor(0,0);  
		LCD_WriteRAM_Prepare();				//开始写入GRAM  
	}  
} 
//jpeg数据接收回调函数
void jpeg_dcmi_rx_callback(void)
{  
	u16 i;
	vu32 *pbuf;
	pbuf=jpeg_data_buf+jpeg_data_len;		//偏移到有效数据末尾
	if(DMA1_Stream1->CR&(1<<19))			//buf0已满,正常处理buf1
	{ 
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[0][i];//读取buf0里面的数据
		jpeg_data_len+=jpeg_line_size;		//偏移
	}else 									//buf1已满,正常处理buf0
	{
		for(i=0;i<jpeg_line_size;i++)pbuf[i]=dcmi_line_buf[1][i];//读取buf1里面的数据
		jpeg_data_len+=jpeg_line_size;		//偏移 
	} 
} 

//RGB屏数据接收回调函数
void rgblcd_dcmi_rx_callback(void)
{  
	u16 *pbuf;
	if(DMA1_Stream1->CR&(1<<19))//DMA使用buf1,读取buf0
	{ 
		pbuf=(u16*)dcmi_line_buf[0]; 
	}else 						//DMA使用buf0,读取buf1
	{
		pbuf=(u16*)dcmi_line_buf[1]; 
	} 	
	LTDC_Color_Fill(0,curline,lcddev.width-1,curline,pbuf);//DM2D填充 
	if(curline<lcddev.height)curline++;
	if(bmp_request==1&&curline==(lcddev.height-1))//有bmp拍照请求,关闭DCMI
	{
		DCMI_Stop();	//停止DCMI
		bmp_request=0;	//标记请求处理完成.
	}
}

//切换为OV5640模式
void sw_ov5640_mode(void)
{ 
 	OV5640_WR_Reg(0X3017,0XFF);	//开启OV5650输出(可以正常显示)
	OV5640_WR_Reg(0X3018,0XFF); 
	//GPIOC8/9/11切换为 DCMI接口
 	GPIO_AF_Set(GPIOC,8,13);	//PC8,AF13  DCMI_D2
 	GPIO_AF_Set(GPIOC,9,13);	//PC9,AF13  DCMI_D3
 	GPIO_AF_Set(GPIOC,11,13);	//PC11,AF13 DCMI_D4  
} 
//切换为SD卡模式
void sw_sdcard_mode(void)
{
	OV5640_WR_Reg(0X3017,0X00);	//关闭OV5640全部输出(不影响SD卡通信)
	OV5640_WR_Reg(0X3018,0X00);  
 	//GPIOC8/9/11切换为 SDIO接口
  	GPIO_AF_Set(GPIOC,8,12);	//PC8,AF12
 	GPIO_AF_Set(GPIOC,9,12);	//PC9,AF12 
 	GPIO_AF_Set(GPIOC,11,12);	//PC11,AF12  
}
//文件名自增（避免覆盖）
//mode:0,创建.bmp文件;1,创建.jpg文件.
//bmp组合成:形如"0:PHOTO/PIC13141.bmp"的文件名
//jpg组合成:形如"0:PHOTO/PIC13141.jpg"的文件名
void camera_new_pathname(u8 *pname,u8 mode)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		if(mode==0)sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",index);
		else sprintf((char*)pname,"0:PHOTO/PIC%05d.jpg",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//尝试打开这个文件
		if(res==FR_NO_FILE)break;		//该文件名不存在=正是我们需要的.
		index++;
	}
}  
//OV5640拍照jpg图片
//返回值:0,成功
//    其他,错误代码
u8 ov5640_jpg_photo(u8 *pname)
{
	FIL* f_jpg; 
	u8 res=0,headok=0;
	u32 bwr;
	u32 i,jpgstart,jpglen;
	u8* pbuf;
	f_jpg=(FIL *)mymalloc(SRAMIN,sizeof(FIL));	//开辟FIL字节的内存区域 
	if(f_jpg==NULL)return 0XFF;				//内存申请失败.
	ovx_mode=1;
	jpeg_data_ok=0;
	sw_ov5640_mode();						//切换为OV5640模式 
	OV5640_JPEG_Mode();						//JPEG模式  
	OV5640_OutSize_Set(16,4,2592,1944);		//设置输出尺寸(500W)  
	dcmi_rx_callback=jpeg_dcmi_rx_callback;	//JPEG接收数据回调函数
	DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],jpeg_line_size,2,1);//DCMI DMA配置    
	DCMI_Start(); 			//启动传输 
	while(jpeg_data_ok!=1);	//等待第一帧图片采集完
	jpeg_data_ok=2;			//忽略本帧图片,启动下一帧采集 
	while(jpeg_data_ok!=1);	//等待第二帧图片采集完,第二帧,才保存到SD卡去. 
	DCMI_Stop(); 			//停止DMA搬运
	ovx_mode=0; 
	sw_sdcard_mode();		//切换为SD卡模式
	res=f_open(f_jpg,(const TCHAR*)pname,FA_WRITE|FA_CREATE_NEW);//模式0,或者尝试打开失败,则创建新文件	 
	if(res==0)
	{
		printf("jpeg data size:%d\r\n",jpeg_data_len*4);//串口打印JPEG文件大小
		pbuf=(u8*)jpeg_data_buf;
		jpglen=0;	//设置jpg文件大小为0
		headok=0;	//清除jpg头标记
		for(i=0;i<jpeg_data_len*4;i++)//查找0XFF,0XD8和0XFF,0XD9,获取jpg文件大小
		{
			if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD8))//找到FF D8
			{
				jpgstart=i;
				headok=1;	//标记找到jpg头(FF D8)
			}
			if((pbuf[i]==0XFF)&&(pbuf[i+1]==0XD9)&&headok)//找到头以后,再找FF D9
			{
				jpglen=i-jpgstart+2;
				break;
			}
		}
		if(jpglen)			//正常的jpeg数据 
		{
			pbuf+=jpgstart;	//偏移到0XFF,0XD8处
			res=f_write(f_jpg,pbuf,jpglen,&bwr);
			if(bwr!=jpglen)res=0XFE; 
			
		}else res=0XFD; 
	}
	jpeg_data_len=0;
	f_close(f_jpg); 
	sw_ov5640_mode();		//切换为OV5640模式
	OV5640_RGB565_Mode();	//RGB565模式  
	if(lcdltdc.pwidth!=0)	//RGB屏
	{
		dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB屏接收数据回调函数
		DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],lcddev.width/2,1,1);//DCMI DMA配置  
	}else					//MCU 屏
	{
		DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);			//DCMI DMA配置,MCU屏,竖屏
	}
	myfree(SRAMIN,f_jpg); 
	return res;
}  

int main(void)
{  
	u8 led0sta=1;
	u8 res;
	float fac;							 
	u8 *pname;						//带路径的文件名 
	u8 key;							//键值		   
	u8 i;						 
	u8 sd_ok=1;						//0,sd卡不正常;1,SD卡正常. 
 	u8 scale=1;						//默认是全尺寸缩放
	u8 msgbuf[15];					//消息缓存区 
	u16 outputheight=0;
	 
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz 
	delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	KEY_Init();						//初始化按键
	PCF8574_Init();					//初始化PCF8574
	OV5640_Init();					//初始化OV5640
	sw_sdcard_mode();				//首先切换为OV5640模式
	W25QXX_Init();					//初始化W25Q256 
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
	exfuns_init();					//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 			//挂载SD卡  
	POINT_COLOR=RED;      
	while(font_init()) 			//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"阿波罗STM32F4/F7/H7开发板",16,0);	 			    	 
	Show_Str(30,70,200,16,"照相机实验",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:拍照(bmp格式)",16,0);			    	 
	Show_Str(30,110,200,16,"KEY1:拍照(jpg格式)",16,0);			    	 
	Show_Str(30,130,200,16,"KEY2:自动对焦",16,0);					    	 
	Show_Str(30,150,200,16,"WK_UP:FullSize/Scale",16,0);				    	 
	Show_Str(30,170,200,16,"2018年8月15日",16,0);
	res=f_mkdir("0:/PHOTO");		//创建PHOTO文件夹
	if(res!=FR_EXIST&&res!=FR_OK) 	//发生了错误
	{		
		res=f_mkdir("0:/PHOTO");		//创建PHOTO文件夹		
		Show_Str(30,190,240,16,"SD卡错误!",16,0);
		delay_ms(200);				  
		Show_Str(30,190,240,16,"拍照功能将不可用!",16,0);
		delay_ms(200);				  
		sd_ok=0;  	
	} 	
	dcmi_line_buf[0]=mymalloc(SRAMIN,jpeg_line_size*4);	//为jpeg dma接收申请内存	
	dcmi_line_buf[1]=mymalloc(SRAMIN,jpeg_line_size*4);	//为jpeg dma接收申请内存	
	jpeg_data_buf=mymalloc(SRAMEX,jpeg_buf_size);		//为jpeg文件申请内存(最大4MB)
 	pname=mymalloc(SRAMIN,30);//为带路径的文件名分配30个字节的内存	 
 	while(pname==NULL||!dcmi_line_buf[0]||!dcmi_line_buf[1]||!jpeg_data_buf)	//内存分配出错
 	{	    
		Show_Str(30,190,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,190,240,146,WHITE);//清除显示	     
		delay_ms(200);				  
	}   
	while(OV5640_Init())//初始化OV5640s
	{
		Show_Str(30,190,240,16,"OV5640 错误!",16,0);
		delay_ms(200);
	    LCD_Fill(30,190,239,206,WHITE);
		delay_ms(200);
	}	
 	Show_Str(30,210,230,16,"OV5640 正常",16,0); 
	//自动对焦初始化
	OV5640_RGB565_Mode();	//RGB565模式 
	OV5640_Focus_Init(); 
	OV5640_Light_Mode(0);	//自动模式
	OV5640_Color_Saturation(3);//色彩饱和度0
	OV5640_Brightness(4);	//亮度0
	OV5640_Contrast(3);		//对比度0
	OV5640_Sharpness(33);	//自动锐度
	OV5640_Focus_Constant();//启动持续对焦
	DCMI_Init();			//DCMI配置
	if(lcdltdc.pwidth!=0)	//RGB屏
	{
		dcmi_rx_callback=rgblcd_dcmi_rx_callback;//RGB屏接收数据回调函数
		DCMI_DMA_Init((u32)dcmi_line_buf[0],(u32)dcmi_line_buf[1],lcddev.width/2,1,1);//DCMI DMA配置  
	}else					//MCU 屏
	{
		DCMI_DMA_Init((u32)&LCD->LCD_RAM,0,1,1,0);			//DCMI DMA配置,MCU屏,竖屏
	}
	
	if(lcddev.height==1024)
	{
		yoffset=(lcddev.height-800)/2;
		outputheight=800;
		OV5640_WR_Reg(0x3035,0X51);//降低输出帧率，否则可能抖动
	}else if(lcddev.height==1280)
	{
		yoffset=(lcddev.height-600)/2;
		outputheight=600;
		OV5640_WR_Reg(0x3035,0X51);//降低输出帧率，否则可能抖动 
	}else 
	{
		yoffset=0;
		outputheight=lcddev.height;
	}
	curline=yoffset;		//行数复位
	OV5640_OutSize_Set(16,4,lcddev.width,outputheight);	//满屏缩放显示
	DCMI_Start(); 			//启动传输
	LCD_Clear(BLACK);
 	while(1)
	{	
		key=KEY_Scan(0);//不支持连按
		if(key)
		{ 
			if(key!=KEY2_PRES)
			{
				if(key==KEY0_PRES)//如果是BMP拍照,则等待1秒钟,去抖动,以获得稳定的bmp照片	
				{
					delay_ms(300);
					bmp_request=1;		//请求关闭DCMI
					while(bmp_request);	//等带请求处理完成
 				}else DCMI_Stop();
			}
			if(key==WKUP_PRES)		//缩放处理
			{
				scale=!scale;  
				if(scale==0)
				{
					fac=(float)800/outputheight;//得到比例因子
					OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
					sprintf((char*)msgbuf,"Full Size 1:1");
				}else 
				{
					OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
					sprintf((char*)msgbuf,"Scale");
				}
				delay_ms(800); 	
			}else if(key==KEY2_PRES)OV5640_Focus_Single(); //手动单次自动对焦
			else if(sd_ok)//SD卡正常才可以拍照
			{    
				sw_sdcard_mode();	//切换为SD卡模式
				if(key==KEY0_PRES)	//BMP拍照
				{
					camera_new_pathname(pname,0);	//得到文件名	
					res=bmp_encode(pname,0,yoffset,lcddev.width,outputheight,0);
					sw_ov5640_mode();				//切换为OV5640模式
				}else if(key==KEY1_PRES)//JPG拍照
				{
					camera_new_pathname(pname,1);//得到文件名	
					res=ov5640_jpg_photo(pname);
					if(scale==0)
					{
						fac=(float)800/outputheight;//得到比例因子
						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
 					}else 
					{
						OV5640_OutSize_Set(16,4,lcddev.width,outputheight);
 					}	
					if(lcddev.height>800)OV5640_WR_Reg(0x3035,0X51);//降低输出帧率，否则可能抖动
				}
				if(res)//拍照有误
				{
					Show_Str(30,130,240,16,"写入文件错误!",16,0);		 
				}else 
				{
					Show_Str(30,130,240,16,"拍照成功!",16,0);
					Show_Str(30,150,240,16,"保存为:",16,0);
					Show_Str(30+56,150,240,16,pname,16,0);		    
					PCF8574_WriteBit(BEEP_IO,0);	//蜂鸣器短叫，提示拍照完成
					delay_ms(100);
					PCF8574_WriteBit(BEEP_IO,1);	//关闭蜂鸣器
				}  
				delay_ms(1000);		//等待1秒钟	
				DCMI_Start();		//这里先使能dcmi,然后立即关闭DCMI,后面再开启DCMI,可以防止RGB屏的侧移问题.
				DCMI_Stop();			
			}else //提示SD卡错误
			{					    
				Show_Str(30,130,240,16,"SD卡错误!",16,0);
				Show_Str(30,150,240,16,"拍照功能不可用!",16,0);			    
			}   		
			if(key!=KEY2_PRES)DCMI_Start();//开始显示  
		} 
		delay_ms(10);
		i++;
		if(i==20)//DS0闪烁.
		{
			i=0;
			LED0(led0sta^=1);
 		}
	}
}










