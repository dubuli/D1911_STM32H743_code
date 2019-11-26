#include "sys.h"
#include "usart3.h"	  
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "timer.h"
#include "ucos_ii.h"
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口3初始化代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2016/9/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved  
////////////////////////////////////////////////////////////////////////////////// 	


//串口发送缓存区 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART3_RX_STA=0;   	 
void USART3_IRQHandler(void)
{
	u8 res;	    
	OSIntEnter();    
	if(USART3->ISR&(1<<5))//接收到数据
	{	 
		res=USART3->RDR; 			 
		if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//还可以接收数据
			{
				TIM7->CNT=0;         				//计数器清空
				if(USART3_RX_STA==0) 				//使能定时器7的中断 
				{
					TIM7->CR1|=1<<0;     			//使能定时器7
				}
				USART3_RX_BUF[USART3_RX_STA++]=res;	//记录接收到的值	 
			}else 
			{
				USART3_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	} 								 
	OSIntExit();  											 
}   
//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
void usart3_init(u32 pclk1,u32 bound)
{  	 
	u32 temp;   
	temp=(pclk1*1000000+bound/2)/bound;	//得到USARTDIV@OVER8=0,采用四舍五入计算 

	RCC->AHB4ENR|=1<<1;   			//使能PORTB口时钟  
	RCC->APB1LENR|=1<<18;  			//使能串口3时钟 
	GPIO_Set(GPIOB,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PB10,PB11,复用功能,上拉输出
 	GPIO_AF_Set(GPIOB,10,7);		//PB10,AF7
	GPIO_AF_Set(GPIOB,11,7);		//PB11,AF7   
 	USART3->BRR=temp; 				//波特率设置	
	USART3->CR3|=1<<12;	 			//禁止OVERRUN功能
	USART3->CR1=0;		 			//清零CR1寄存器
	USART3->CR1|=0<<28;	 			//设置M1=0
	USART3->CR1|=0<<12;	 			//设置M0=0&M1=0,选择8位字长 
	USART3->CR1|=0<<15; 			//设置OVER8=0,16倍过采样  
	USART3->CR1|=1<<3;  			//串口发送使能  
	USART3->CR1|=1<<2;  			//串口接收使能
	USART3->CR1|=1<<5;    			//接收缓冲区非空中断使能 
	MY_NVIC_Init(1,2,USART3_IRQn,2);//组2，优先级0,2,最高优先级 
	TIM7_Int_Init(100-1,20000-1);	//10ms中断一次
	TIM7->CR1&=~(1<<0);				//关闭定时器7
	USART3->CR1|=1<<0;  			//串口使能
	USART3_RX_STA=0;				//清零 
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
		while((USART3->ISR&0X40)==0);//循环发送,直到发送完毕   
		USART3->TDR=USART3_TX_BUF[j];  
	}
}



































