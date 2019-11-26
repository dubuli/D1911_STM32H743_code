#include "timer.h"
#include "led.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/7
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//修改说明
//V1.1 20180607
//新增TIM3_PWM_Init函数,用于PWM输出							  
////////////////////////////////////////////////////////////////////////////////// 	 

//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		  
	static u8 led1sta=1;
	if(TIM3->SR&0X0001)//溢出中断
	{
		LED1(led1sta^=1);	 
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器3中断初始化
//TIM3的时钟来自APB1,当D2PPRE1≥2分频的时候
//TIM3的时钟为APB1时钟的2倍，而APB1为100M
//所以,TIM3的时钟频率为200Mhz
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1LENR|=1<<1;				//TIM3时钟使能  
	//如果开启TIM3时钟后,直接开始配置TIM3,则会有问题
	while((RCC->APB1LENR&(1<<1))==0);	//等待时钟设置OK  
 	TIM3->ARR=arr;  				//设定计数器自动重装值 
	TIM3->PSC=psc;  				//预分频器
	TIM3->SMCR=0;					//SMS[3:0]=0000,CK_INT作为时钟源
	TIM3->DIER|=1<<0;  				//允许更新中断	  
	TIM3->CR1&=~(3<<5);				//CMS[1:0]=00,边沿对齐模式
	TIM3->CR1&=~(1<<4);				//DIR=0,向上计数模式
	TIM3->CR1|=0x01;				//使能定时器3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//抢占1，子优先级3，组2		 
}

//TIM3 PWM部分初始化 
//TIM3的时钟来自APB1,当D2PPRE1≥2分频的时候
//TIM3的时钟为APB1时钟的2倍，而APB1为100M
//所以,TIM3的时钟频率为200Mhz
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB1LENR|=1<<1;	//TIM3时钟使能    
	RCC->AHB4ENR|=1<<1;   	//使能PORTB时钟	
	GPIO_Set(GPIOB,PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOB,1,2);	//PB1,AF2 
	
	TIM3->ARR=arr;			//设定计数器自动重装值 
	TIM3->PSC=psc;			//预分频器不分频 
	TIM3->CCMR2|=6<<12;  	//CH4 PWM1模式		 
	TIM3->CCMR2|=1<<11; 	//CH4 预装载使能	   
	TIM3->CCER|=1<<12;   	//OC4 输出使能	
	TIM3->CCER|=1<<13;   	//OC4 低电平有效	   
	TIM3->CR1|=1<<7;   		//ARPE使能 
	TIM3->CR1|=1<<0;    	//使能定时器3											  
}  













