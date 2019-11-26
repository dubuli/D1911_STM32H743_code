#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//ADC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/10
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//20180611 	V1.1
//1,修改Adc3_Init/Get_Adc3/Get_Adc3_Average函数,支持内部温度传感器数据采集.
//2,新增Get_Temprate函数,用于获取内部温度传感器采集到的温度值
////////////////////////////////////////////////////////////////////////////////// 


#define ADC1_CH19	  		19			//通道19  	      	    
#define ADC3_TEMPSENSOR	  	18			//通道18,温度传感器通道  	      	    
	   									   
void Adc_Init(void); 					//ADC初始化
u16  Get_Adc(u8 ch); 					//获得某个通道值 
u16 Get_Adc_Average(u8 ch,u8 times);	//得到某个通道给定次数采样的平均值  
void Adc3_Init(void); 					//ADC3初始化
u16  Get_Adc3(u8 ch); 					//获得ADC3某个通道值 
u16 Get_Adc3_Average(u8 ch,u8 times);	//得到ADC3某个通道给定次数采样的平均值  
short Get_Temprate(void);				//获取当前温度
#endif 















