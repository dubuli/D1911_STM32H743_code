#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h" 
#include "ltdc.h" 
#include "timer.h" 
#include "ov5640.h" 
#include "dcmi.h" 
#include "usart2.h"  
//ALIENTEK 阿波罗STM32H7开发板 实验38
//摄像头 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  


u8 ovx_mode=0;							//bit0:0,RGB565模式;1,JPEG模式 
u16 curline=0;							//摄像头输出数据,当前行编号
u16 yoffset=0;							//y方向的偏移量

#define jpeg_buf_size   1*1024*1024		//定义JPEG数据缓存jpeg_buf的大小(1*4M字节)
#define jpeg_line_size	2*1024			//定义DMA接收数据时,一行数据的最大值

u32 dcmi_line_buf[2][jpeg_line_size];	//RGB屏时,摄像头采用一行一行读取,定义行缓存  
u32 jpeg_data_buf[jpeg_buf_size] __attribute__((at(0XC0000000+1280*800*2)));//JPEG数据缓存buf,定义在LCD帧缓存之后

volatile u32 jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile u8 jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
		 
//JPEG尺寸支持列表
const u16 jpeg_img_size_tbl[][2]=
{
	160,120,	//QQVGA 
	320,240,	//QVGA  
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,800,	//WXGA 
	1440,900,	//WXGA+
	1280,1024,	//SXGA
	1600,1200,	//UXGA	
	1920,1080,	//1080P
	2048,1536,	//QXGA
	2592,1944,	//500W 
};

const u8*EFFECTS_TBL[7]={"Normal","Cool","Warm","B&W","Yellowish ","Inverse","Greenish"};	//7种特效 
const u8*JPEG_SIZE_TBL[12]={"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"};//JPEG图片 12种尺寸 

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
			jpeg_data_ok=1; 				//标记JPEG数据采集完成,等待其他函数处理
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
//JPEG测试
//JPEG数据,通过串口2发送给电脑.
void jpeg_test(void)
{
	u32 i,jpgstart,jpglen; 
	u8 *p;
	u8 key,headok=0;
	u8 effect=0,contrast=2;
	u8 size=2;			//默认是QVGA 320*240尺寸
	u8 msgbuf[15];		//消息缓存区 
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");
	LCD_ShowString(30,70,200,16,16,"OV5640 JPEG Mode");
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//对比度
	LCD_ShowString(30,120,200,16,16,"KEY1:Auto Focus"); 		//执行自动对焦
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//特效 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:Size");				//分辨率设置 
	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	LCD_ShowString(30,180,200,16,16,msgbuf);					//显示当前JPEG分辨率
	//自动对焦初始化
	OV5640_RGB565_Mode();	//RGB565模式 
	OV5640_Focus_Init(); 
	
 	OV5640_JPEG_Mode();		//JPEG模式
	
	OV5640_Light_Mode(0);	//自动模式
	OV5640_Color_Saturation(3);//色彩饱和度0
	OV5640_Brightness(4);	//亮度0
	OV5640_Contrast(3);		//对比度0
	OV5640_Sharpness(33);	//自动锐度
	OV5640_Focus_Constant();//启动持续对焦 
	DCMI_Init();			//DCMI配置 
	dcmi_rx_callback=jpeg_dcmi_rx_callback;//JPEG接收数据回调函数
	DCMI_DMA_Init((u32)&dcmi_line_buf[0],(u32)&dcmi_line_buf[1],jpeg_line_size,2,1);//DCMI DMA配置    
 	OV5640_OutSize_Set(4,0,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸 
	DCMI_Start(); 		//启动传输
	while(1)
	{
		if(jpeg_data_ok==1)	//已经采集完一帧图像了
		{  
			p=(u8*)jpeg_data_buf;
			printf("jpeg_data_len:%d\r\n",jpeg_data_len*4);//打印帧率
			LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //提示正在传输数据 
			jpglen=0;	//设置jpg文件大小为0
			headok=0;	//清除jpg头标记
			for(i=0;i<jpeg_data_len*4;i++)//查找0XFF,0XD8和0XFF,0XD9,获取jpg文件大小
			{
				if((p[i]==0XFF)&&(p[i+1]==0XD8))//找到FF D8
				{
					jpgstart=i;
					headok=1;	//标记找到jpg头(FF D8)
				}
				if((p[i]==0XFF)&&(p[i+1]==0XD9)&&headok)//找到头以后,再找FF D9
				{
					jpglen=i-jpgstart+2;
					break;
				}
			}
			if(jpglen)	//正常的jpeg数据 
			{
				p+=jpgstart;			//偏移到0XFF,0XD8处 
				for(i=0;i<jpglen;i++)	//发送整个jpg文件
				{
					USART2->TDR=p[i];  
					while((USART2->ISR&0X40)==0);	//循环发送,直到发送完毕   
					key=KEY_Scan(0); 
					if(key)break;
				}  
			}
			if(key)	//有按键按下,需要处理
			{  
				LCD_ShowString(30,210,210,16,16,"Quit Sending data   ");//提示退出数据传输
				switch(key)
				{				    
					case KEY0_PRES:	//对比度设置
						contrast++;
						if(contrast>6)contrast=0;
						OV5640_Contrast(contrast);
						sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
						break; 
					case KEY1_PRES:	//执行一次自动对焦
						OV5640_Focus_Single();
						break;
					case KEY2_PRES:	//特效设置				 
						effect++;
						if(effect>6)effect=0;
						OV5640_Special_Effects(effect);//设置特效
						sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
						break;
					case WKUP_PRES:	//JPEG输出尺寸设置   
						size++;  
						if(size>11)size=0;   
						OV5640_OutSize_Set(16,4,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸  
						sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
						break;
				}
				LCD_Fill(30,180,239,190+16,WHITE);
				LCD_ShowString(30,180,210,16,16,msgbuf);//显示提示内容
				delay_ms(800); 				  
			}else LCD_ShowString(30,210,210,16,16,"Send data complete!!");//提示传输结束设置 
			jpeg_data_ok=2;	//标记jpeg数据处理完了,可以让DMA去采集下一帧了.
		}		
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
}
//RGB565测试
//RGB数据直接显示在LCD上面
void rgb565_test(void)
{ 
	u8 key;
	float fac=0;
	u8 effect=0,contrast=2;
	u8 scale=1;		//默认是全尺寸缩放
	u8 msgbuf[15];	//消息缓存区 
	u16 outputheight=0;
	
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");
	LCD_ShowString(30,70,200,16,16,"OV5640 RGB565 Mode"); 
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//对比度
	LCD_ShowString(30,120,200,16,16,"KEY1:Auto Focus"); 		//执行自动对焦
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//特效 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:FullSize/Scale");	//1:1尺寸(显示真实尺寸)/全尺寸缩放
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
	TIM3->CR1&=~(0x01);		//关闭定时器3,关闭帧率统计，打开的话，RGB屏，在串口打印的时候，会抖
	
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
	OV5640_OutSize_Set(4,0,lcddev.width,outputheight);		//满屏缩放显示
	DCMI_Start(); 			//启动传输
	LCD_Clear(BLACK);
	while(1)
	{ 
		key=KEY_Scan(0); 
		if(key)
		{ 
			if(key!=KEY1_PRES)DCMI_Stop(); //非KEY1按下,停止显示
			switch(key)
			{				    
				case KEY0_PRES:	//对比度设置
					contrast++;
					if(contrast>6)contrast=0;
					OV5640_Contrast(contrast);
					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
					break;
				case KEY1_PRES:	//执行一次自动对焦
					OV5640_Focus_Single();
					break;
				case KEY2_PRES:	//特效设置				 
					effect++;
					if(effect>6)effect=0;
					OV5640_Special_Effects(effect);//设置特效
					sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
					break;
				case WKUP_PRES:	//1:1尺寸(显示真实尺寸)/缩放	    
					scale=!scale;  
					if(scale==0)
					{
						fac=(float)800/outputheight;	//得到比例因子
 						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*outputheight)/2,lcddev.width,outputheight); 	 
						sprintf((char*)msgbuf,"Full Size 1:1");
					}else 
					{
						OV5640_OutSize_Set(4,0,lcddev.width,outputheight);
 						sprintf((char*)msgbuf,"Scale");
					}
					break;
			}
			if(key!=KEY1_PRES)	//非KEY1按下
			{
				LCD_ShowString(30,50,210,16,16,msgbuf);//显示提示内容
				delay_ms(800); 
				DCMI_Start();	//重新开始传输
			}
		} 
		delay_ms(10);		
	}    
} 

int main(void)
{  
	u8 led0sta=1;
	u8 key; 
	u8 t;   
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usart2_init(100,921600);		//初始化串口2波特率为921600
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	KEY_Init();						//初始化按键 
	TIM3_Int_Init(10000-1,20000-1);	//10Khz计数,1秒钟中断一次 
	POINT_COLOR=RED;				//设置字体为红色 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");
	LCD_ShowString(30,70,200,16,16,"OV5640 TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/7/20");	
 
	while(OV5640_Init())//初始化OV5640
	{
		LCD_ShowString(30,130,240,16,16,"OV5640 ERROR");
		delay_ms(200);
	    LCD_Fill(30,130,239,170,WHITE);
		delay_ms(200);
		LED0(led0sta^=1);
	} 
	LCD_ShowString(30,130,200,16,16,"OV5640 OK");
  	while(1)
	{	
		key=KEY_Scan(0);
		if(key==KEY0_PRES){ovx_mode=0;break;}		//RGB565模式
	    else if(key==KEY1_PRES){ovx_mode=1;break;}	//JPEG模式 
		t++; 									  
		if(t==100)LCD_ShowString(30,150,230,16,16,"KEY0:RGB565  KEY1:JPEG"); //闪烁显示提示信息
 		if(t==200)
		{	
			LCD_Fill(30,150,210,150+16,WHITE);
			t=0; 
			LED0(led0sta^=1);
		}
		delay_ms(5);	  
	}
	if(ovx_mode==1)jpeg_test();
	else rgb565_test();  
}










