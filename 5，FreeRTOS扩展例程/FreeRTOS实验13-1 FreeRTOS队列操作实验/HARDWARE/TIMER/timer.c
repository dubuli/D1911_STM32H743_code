#include "timer.h"
#include "led.h"
#include "usart.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//定时器中断驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/8/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

TIM_HandleTypeDef TIM3_Handler;     //定时器句柄 
TIM_HandleTypeDef TIM5_Handler;     //TIM5句柄

//FreeRTOS时间统计所用的节拍计数器
volatile unsigned long long FreeRTOSRunTimeTicks;

//初始化TIM3使其为FreeRTOS的时间统计提供时基
void ConfigureTimeForRunTimeStats(void)
{
	//定时器3初始化，定时器时钟为200M，分频系数为200-1，所以定时器3的频率
	//为200M/200=1M，自动重装载为50-1，那么定时器周期就是50us
	FreeRTOSRunTimeTicks=0;
	TIM3_Init(50-1,200-1);	//初始化TIM3
}

//通用定时器3中断初始化,定时器3在APB1上，APB1的定时器时钟为200MHz
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE    
}

//通用定时器5中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
void TIM5_Init(u32 arr,u16 psc)
{  
    TIM5_Handler.Instance=TIM5;                          //通用定时器3
    TIM5_Handler.Init.Prescaler=psc;                     //分频系数
    TIM5_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM5_Handler.Init.Period=arr;                        //自动装载值
    TIM5_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM5_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM5_Handler); //使能定时器9和定时器9更新中断：TIM_IT_UPDATE   
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,1,0);    //设置中断优先级，抢占优先级4，子优先级0
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	}else if(htim->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();            //使能TIM5时钟
		HAL_NVIC_SetPriority(TIM5_IRQn,8,0);  	//设置中断优先级，抢占优先级8
		HAL_NVIC_EnableIRQ(TIM5_IRQn);          //开启ITM5中断   
	} 
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

//定时器5中断服务函数
void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM5_Handler);
}

extern QueueHandle_t Message_Queue;	//信息队列句柄
extern void disp_str(u8* str);

//定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    u8 *buffer;
	BaseType_t xTaskWokenByReceive=pdFALSE;
	BaseType_t err;
    
    if(htim==(&TIM3_Handler))
    {
        FreeRTOSRunTimeTicks++;
    }
    else if(htim==(&TIM5_Handler))
	{
		buffer=mymalloc(SRAMIN,USART_REC_LEN);
        if(Message_Queue!=NULL)
        {
			memset(buffer,0,USART_REC_LEN);	//清除缓冲区
			err=xQueueReceiveFromISR(Message_Queue,buffer,&xTaskWokenByReceive);//请求消息Message_Queue
            if(err==pdTRUE)			//接收到消息
            {
				disp_str(buffer);	//在LCD上显示接收到的消息
            }
        }
		myfree(SRAMIN,buffer);		//释放内存
		
		portYIELD_FROM_ISR(xTaskWokenByReceive);//如果需要的话进行一次任务切换
	}
}
