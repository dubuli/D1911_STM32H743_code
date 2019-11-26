#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "key.h"
#include "dma.h" 
//ALIENTEK 阿波罗STM32H7开发板 实验24
//DMA 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
  
#define SEND_BUF_SIZE 7800	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.

u8 SendBuff[SEND_BUF_SIZE];	//发送数据缓冲区
const u8 TEXT_TO_SEND[]={"ALIENTEK Apollo STM32H7 DMA 串口实验"};	

int main(void)
{  
 	u8 led0sta=1;
	u16 i;
	u8 t=0;
	u8 j,mask=0;
	float pro=0;					//进度
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD
	KEY_Init(); 					//按键初始化
 	MYDMA_Config(DMA2_Stream7,42,(u32)&USART1->TDR,(u32)SendBuff,SEND_BUF_SIZE);//DMA2,STEAM7,CH42,外设为串口1,存储器为SendBuff,长度为:SEND_BUF_SIZE.
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");  
	LCD_ShowString(30,70,200,16,16,"DMA TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/6/13");	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Start");	 
	POINT_COLOR=BLUE;//设置字体为蓝色      	 
	//显示提示信息	
	j=sizeof(TEXT_TO_SEND);	   
	for(i=0;i<SEND_BUF_SIZE;i++)//填充ASCII字符集数据
    {
		if(t>=j)//加入换行符
		{
			if(mask)
			{
				SendBuff[i]=0x0a;
				t=0;
			}else 
			{
				SendBuff[i]=0x0d;
				mask++;
			}	
		}else//复制TEXT_TO_SEND语句
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}   	   
    }		 
	POINT_COLOR=BLUE;//设置字体为蓝色	  
	i=0;
	while(1)
	{
		t=KEY_Scan(0);
		if(t==KEY0_PRES)//KEY0按下
		{
			printf("\r\nDMA DATA:\r\n"); 	    
			LCD_ShowString(30,150,200,16,16,"Start Transimit....");
			LCD_ShowString(30,170,200,16,16,"   %");//显示百分号
		    USART1->CR3=1<<7;           //使能串口1的DMA发送       
			MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE);//开始一次DMA传输！	  
		    //等待DMA传输完成，此时我们来做另外一些事，点灯
		    //实际应用中，传输数据期间，可以执行另外的任务
		    while(1)
		    {
				if(DMA2->HISR&(1<<27))	//等待DMA2_Steam7传输完成
				{
					DMA2->HIFCR|=1<<27;	//清除DMA2_Steam7传输完成标志
					break; 
		        }
				pro=DMA2_Stream7->NDTR;	//得到当前还剩余多少个数据
				pro=1-pro/SEND_BUF_SIZE;//得到百分比	  
				pro*=100;      			//扩大100倍
				LCD_ShowNum(30,170,pro,3,16);	  
		    }			    
			LCD_ShowNum(30,170,100,3,16);//显示100%	  
			LCD_ShowString(30,150,200,16,16,"Transimit Finished!");//提示传送完成
		}
		i++;
		delay_ms(10);
		if(i==20)
		{
			LED0(led0sta^=1);//提示系统正在运行	
			i=0;
		}		   
	}		    
}













