#include "dac.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//DAC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved 
////////////////////////////////////////////////////////////////////////////////// 	
 
//DAC通道1输出初始化
void Dac1_Init(void)
{   	
	RCC->APB1LENR|=1<<29;	//使能DAC时钟	   
	RCC->AHB4ENR|=1<<0;  	//使能PORTA时钟	  
	GPIO_Set(GPIOA,PIN4,GPIO_MODE_AIN,0,0,GPIO_PUPD_PU);//PA4,模拟输入,下拉   
 
	DAC1->CCR=0;
	DAC1->MCR&=~(7<<0);		//MODE1[2:0]清零
	DAC1->MCR|=0;			//MODE1[2:0]=0,DAC通道1连接到外部引脚,并使能BUFFER
	
	DAC1->CR=0;				//DR寄存器清零
	DAC1->CR|=0<<1;			//TEN1=0,不使用触发功能 
	DAC1->CR|=0<<2;			//TSEL[3:0]=0,软件触发
	DAC1->CR|=0<<6;			//WAVE1[1:0]=0,不使用波形发生
 	DAC1->CR|=0<<12;		//DMAEN1=0,DAC1 DMA不使能    
	DAC1->CR|=0<<14;		//CEN1=0,DAC1工作在普通模式 

	DAC1->CR|=1<<0;			//使能DAC1 
	DAC1->DHR12R1=0;
}
//设置通道1输出电压
//vol:0~3300,代表0~3.3V
void Dac1_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC1->DHR12R1=temp;
}





















































