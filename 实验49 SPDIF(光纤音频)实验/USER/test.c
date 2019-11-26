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
#include "sai.h"	 	
#include "wm8978.h"	 
#include "spdif.h"		
#include "string.h"	 
//ALIENTEK 阿波罗STM32H7开发板 实验49
//SPDIF 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司    

#define	SPDIF_DBUF_SIZE		1024			//定义SPDIF数据接收缓冲区的大小,1K*4字节
	
u32 spdif_audiobuf[2][SPDIF_DBUF_SIZE]; 	//SPDIF音频数据接收缓冲区,共2个(双缓冲)

//显示采样率
//samplerate:音频采样率(单位:Hz)
void spdif_show_samplerate(u32 samplerate)
{  
	u8 *buf;
	buf=mymalloc(SRAMIN,100);	//申请内存
	if(buf)						//申请成功
	{	
		if(samplerate)sprintf((char*)buf,"%dHz",samplerate);//打印采样率 
		else sprintf((char*)buf,"正在识别...");				//采样率为0,则提示正在识别
		LCD_Fill(30+56,170,230,170+16,WHITE);
		Show_Str(30+56,170,200,16,buf,16,0);
	}
	myfree(SRAMIN,buf);			//释放内存
}

//SAI DMA发送完成中断回调函数
//这里未做任何处理.
void sai_dma_tx_callback(void)
{
}	

//SPDIF RX结束时的回调函数 
void spdif_rx_stopplay_callback(void)
{
	SAI_Play_Stop();			
	SPDIFRX->IFCR|=1<<5;//清除同步完成标志	
	spdif_show_samplerate(0);
    memset((u8*)&spdif_audiobuf[0],0,SPDIF_DBUF_SIZE*4);   
    memset((u8*)&spdif_audiobuf[1],0,SPDIF_DBUF_SIZE*4);
}	


int main(void)
{   
	u8 led0sta=1;
	u8 t; 
	u8 key;
	u8 vol=45;	//默认音量 
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz 
	delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	KEY_Init();						//初始化按键
	W25QXX_Init();					//初始化W25Q256 
	WM8978_Init();					//初始化WM8978
	WM8978_HPvol_Set(40,40);		//耳机音量设置
	WM8978_SPKvol_Set(50);			//喇叭音量设置
	SPDIF_RX_Init();				//SPDIF初始化
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
	exfuns_init();					//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 			//挂载SD卡  
	POINT_COLOR=RED;      
	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
	POINT_COLOR=RED;      
 	Show_Str(30,40,200,16,"阿波罗STM32F4/F7/H7开发板",16,0);				    	 
	Show_Str(30,60,200,16,"SPDIF(光纤音频)实验",16,0);				    	 
	Show_Str(30,80,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(30,100,200,16,"2018年8月17日",16,0); 
	Show_Str(30,120,200,16,"KEY_UP:VOL+  KEY1:VOL-",16,0); 	
	
	Show_Str(30,150,200,16,"音量:",16,0);  
	Show_Str(30,170,200,16,"采样率:",16,0); 
	POINT_COLOR=BLUE;     
	LCD_ShowxNum(30+40,150,vol,2,16,0X80);	//显示音量 
	spdif_show_samplerate(0);				//显示采样率
	
	WM8978_ADDA_Cfg(1,0);		//开启DAC
	WM8978_Input_Cfg(0,0,0);	//关闭输入通道
	WM8978_Output_Cfg(1,0);		//开启DAC输出 	
    SPDIF_RXDATA_DMA_Init((u32*)&spdif_audiobuf[0],(u32*)&spdif_audiobuf[1],SPDIF_DBUF_SIZE,2);  
	spdif_rx_stop_callback=spdif_rx_stopplay_callback;	//SPDIF 结束播放时的回调函数 
	while(1)
	{ 
		key=KEY_Scan(1);
		if(key==WKUP_PRES||key==KEY1_PRES)	//音量控制
		{
			if(key==WKUP_PRES)
			{
				vol++;
				if(vol>63)vol=63; 
			}else
			{
				if(vol>0)vol--;
			}
			WM8978_HPvol_Set(vol,vol);		//设置耳机音量设置
			WM8978_SPKvol_Set(vol);			//设置喇叭音量设置 
			LCD_ShowxNum(30+40,150,vol,2,16,0X80);//显示音量 
		}
        if(spdif_dev.consta==0)				//未连接
        {
            if(SPDIF_RX_WaitSync())			//等待同步
            {
                spdif_dev.samplerate=SPDIF_RX_GetSampleRate();	//获得采样率 
				if(spdif_dev.samplerate)	//采样率有效,配置WM8978和SAI等
				{
					WM8978_I2S_Cfg(2,2);						//飞利浦标准,24位数据长度
					SAIA_Init(0,1,6);							//设置SAI,主发送,24位数据
					SAIA_SampleRate_Set(spdif_dev.samplerate);	//设置采样率   
					SPDIF_RX_Start();							//打开SPDIF
					SAIA_TX_DMA_Init((u8*)&spdif_audiobuf[0],(u8*)&spdif_audiobuf[1],SPDIF_DBUF_SIZE,2); //配置TX DMA,32位
					sai_tx_callback=sai_dma_tx_callback;
					SAI_Play_Start();							//开启DMA 
					spdif_show_samplerate(spdif_dev.samplerate);//显示采样率.						
				}else SPDIF_RX_Stop();							//采样率错误,停止SPDIF播放 
            }else LED0(led0sta^=1);								//LED0闪烁
        }
		delay_ms(10);
		t++;
		if(t>=20)
		{
			t=0; 
			LED0(led0sta^=1);
		}
	} 
}






















