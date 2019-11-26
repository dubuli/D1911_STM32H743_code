#include "adc.h"
#include "delay.h"		 
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


//初始化ADC
//这里我们仅以规则通道为例
//我们默认仅开启ADC1_CH19P
//16位转换时间位:采样周期+8.5个ADC周期
void  Adc_Init(void)
{    
	//先初始化IO口
 	RCC->AHB1ENR|=1<<5;    		//使能ADC1/2时钟 
	RCC->AHB4ENR|=1<<0;    		//使能PORTA时钟	  
	GPIO_Set(GPIOA,PIN5,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,模拟输入,不带上下拉   

	RCC->AHB1RSTR|=1<<5;   		//ADC1/2复位
	RCC->AHB1RSTR&=~(1<<5);		//复位结束	
	RCC->D3CCIPR&=~(3<<16);		//ADCSEL[1:0]清零
	RCC->D3CCIPR|=2<<16;		//ADCSEL[1:0]=2,per_ck作为ADC时钟源,默认选择hsi_ker_ck作为per_ck,频率:64Mhz
	ADC12_COMMON->CCR|=1<<18;	//PRESC[3:0]=1,输入时钟2分频,即adc_ker_ck=per_ck/2=64/2=32Mhz(不能超过36Mhz)
	
	ADC1->CR=0;   				//CR寄存器清零,DEEPPWD清零,从深度睡眠唤醒.
	ADC1->CR|=1<<28;  			//ADVREGEN=1,使能ADC稳压器
	delay_ms(10);				//等待稳压器启动完成,约10us,这里延时大一点,没关系.
	ADC1->CR|=1<<8;  			//BOOST=1,ADC工作在boost模式(ADC频率大于20M的时候,必须设置boost位)	
	ADC1->CFGR&=~(1<<13);		//CONT=0,单次转换模式
	ADC1->CFGR|=1<<12;			//OVRMOD=1,复写模式(DR寄存器可被复写)	
	ADC1->CFGR&=~(3<<10);		//EXTEN[1:0]=0,软件触发
	ADC1->CFGR&=~(7<<2);		//RES[2:0]位清零
	ADC1->CFGR|=0<<2;			//RES[2:0]=0,16位分辨率(0,16位;1,14位;2,12位;3,10位;4,8位.)

	ADC1->CFGR2&=~((u32)15<<28);//LSHIFT[3:0]=0,不左移,数据右对齐.
	ADC1->CFGR2&=~((u32)0X3FF<<16);//OSR[9:0]=0,不使用过采样
	
	
	ADC1->CR&=~((u32)1<<30);	//ADCALDIF=0,校准单端转换通道
	ADC1->CR|=1<<16;			//ADCALLIN=1,线性校准
	ADC1->CR|=(u32)1<<31;		//开启校准
	while(ADC1->CR&((u32)1<<31));//等待校准完成
	 
	ADC1->SQR1&=~(0XF<<0);		//L[3:0]清零
	ADC1->SQR1|=0<<0;     		//L[3:0]=0,1个转换在规则序列中 也就是只转换规则序列1 
	
	//设置通道19的采样时间
	ADC1->SMPR2&=~(7<<(3*9));	//通道19采样时间清空	  
 	ADC1->SMPR2|=7<<(3*9); 		//通道19 810.5个周期,提高采样时间可以提高精确度		
 	ADC1->CR|=1<<0;	   			//开启AD转换器	  
}				  
//获得ADC值
//ch:通道值 0~19
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
	ADC1->PCSEL|=1<<ch;			//ADC转换通道预选择 
	//设置转换序列	  		 
	ADC1->SQR1&=~(0X1F<<6*1);	//规则序列1通道清零
	ADC1->SQR1|=ch<<6*1;		//设置规则序列1的转换通道为ch 
	ADC1->CR|=1<<2;       		//启动规则转换通道 
	while(!(ADC1->ISR&1<<2));	//等待转换结束	 	   
	return ADC1->DR;			//返回adc值	
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}  

//初始化ADC3 
//我们默认仅开启ADC3_CH18P(内部温度传感器所在通道)
//16位转换时间位:采样周期+8.5个ADC周期
void  Adc3_Init(void)
{     
 	RCC->AHB4ENR|=1<<24;    	//使能ADC3时钟   

	RCC->AHB4RSTR|=1<<24;   	//ADC3复位
	RCC->AHB4RSTR&=~(1<<24);	//复位结束	
	RCC->D3CCIPR&=~(3<<16);		//ADCSEL[1:0]清零
	RCC->D3CCIPR|=2<<16;		//ADCSEL[1:0]=2,per_ck作为ADC时钟源,默认选择hsi_ker_ck作为per_ck,频率:64Mhz
	ADC3_COMMON->CCR|=1<<18;	//PRESC[3:0]=1,输入时钟2分频,即adc_ker_ck=per_ck/2=64/2=32Mhz(不能超过36Mhz)
	ADC3_COMMON->CCR|=1<<23;	//VSENSEEN=1,使能内部温度传感器通道
	
	ADC3->CR=0;   				//CR寄存器清零,DEEPPWD清零,从深度睡眠唤醒.
	ADC3->CR|=1<<28;  			//ADVREGEN=1,使能ADC稳压器
	delay_ms(10);				//等待稳压器启动完成,约10us,这里延时大一点,没关系.
	ADC3->CR|=1<<8;  			//BOOST=1,ADC工作在boost模式(ADC频率大于20M的时候,必须设置boost位)	
	ADC3->CFGR&=~(1<<13);		//CONT=0,单次转换模式
	ADC3->CFGR|=1<<12;			//OVRMOD=1,复写模式(DR寄存器可被复写)	
	ADC3->CFGR&=~(3<<10);		//EXTEN[1:0]=0,软件触发
	ADC3->CFGR&=~(7<<2);		//RES[2:0]位清零
	ADC3->CFGR|=0<<2;			//RES[2:0]=0,16位分辨率(0,16位;1,14位;2,12位;3,10位;4,8位.)

	ADC3->CFGR2&=~((u32)15<<28);//LSHIFT[3:0]=0,不左移,数据右对齐.
	ADC3->CFGR2&=~((u32)0X3FF<<16);//OSR[9:0]=0,不使用过采样
	
	
	ADC3->CR&=~((u32)1<<30);	//ADCALDIF=0,校准单端转换通道
	ADC3->CR|=1<<16;			//ADCALLIN=1,线性校准
	ADC3->CR|=(u32)1<<31;		//开启校准
	while(ADC3->CR&((u32)1<<31));//等待校准完成
	 
	ADC3->SQR1&=~(0XF<<0);		//L[3:0]清零
	ADC3->SQR1|=0<<0;     		//L[3:0]=0,1个转换在规则序列中 也就是只转换规则序列1 
	
	//设置通道18的采样时间
	ADC3->SMPR2&=~(7<<(3*8));	//通道18采样时间清空	  
 	ADC3->SMPR2|=7<<(3*8); 		//通道18 810.5个周期,提高采样时间可以提高精确度		
 	ADC3->CR|=1<<0;	   			//开启AD转换器	  
}	
				  
//获得ADC3某个通道的值
//ch:通道值 0~19
//返回值:转换结果
u16 Get_Adc3(u8 ch)   
{
	ADC3->PCSEL|=1<<ch;			//ADC转换通道预选择 
	//设置转换序列	  		 
	ADC3->SQR1&=~(0X1F<<6*1);	//规则序列1通道清零
	ADC3->SQR1|=ch<<6*1;		//设置规则序列1的转换通道为ch 
	ADC3->CR|=1<<2;       		//启动规则转换通道 
	while(!(ADC3->ISR&1<<2));	//等待转换结束	 	   
	return ADC3->DR;			//返回adc值	
}

//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc3_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc3(ch);
		delay_ms(5);
	}
	return temp_val/times;
}  

//得到温度值
//返回值:温度值(扩大了100倍,单位:℃.)
short Get_Temprate(void)
{
	u32 adcx;
	short result;
 	double temperate;
    float temp=0;
    u16 ts_cal1,ts_cal2;
    ts_cal1=*(vu16*)(0X1FF1E820);					//获取TS_CAL1
    ts_cal2=*(vu16*)(0X1FF1E840);					//获取TS_CAL2
    temp=(float)((110.0-30.0)/(ts_cal2-ts_cal1));	//获取比例因子
	adcx=Get_Adc3_Average(ADC3_TEMPSENSOR,10);		//读取内部温度传感器通道,10次取平均
    temperate=(float)(temp*(adcx-ts_cal1)+30);		//计算温度
	result=temperate*=100;							//扩大100倍.
	return result;

}






