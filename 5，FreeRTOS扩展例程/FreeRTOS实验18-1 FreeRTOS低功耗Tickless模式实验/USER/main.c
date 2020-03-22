#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "timer.h"
#include "exti.h"
#include "pcf8574.h"
#include "malloc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "limits.h"
/************************************************
 ALIENTEK 阿波罗STM32H7开发板 FreeRTOS实验18-1
 FreeRTOS低功耗Tickless模式实验-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	`
#define START_STK_SIZE 		256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		2
//任务堆栈大小	
#define TASK1_STK_SIZE 		256  
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define DATAPROCESS_TASK_PRIO 3
//任务堆栈大小	
#define DATAPROCESS_STK_SIZE  256 
//任务句柄
TaskHandle_t DataProcess_Handler;
//任务函数
void DataProcess_task(void *pvParameters);

//二值信号量句柄
SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄

//用于命令解析用的命令值
#define LED1ON	1
#define LED1OFF	2
#define BEEPON	3
#define BEEPOFF	4
#define COMMANDERR	0XFF

//进入低功耗模式前需要处理的事情
//ulExpectedIdleTime：低功耗模式运行时间
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
	//关闭某些低功耗模式下不使用的外设时钟
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_GPIOF_CLK_DISABLE();
	__HAL_RCC_GPIOG_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();  
}

//退出低功耗模式以后需要处理的事情
//ulExpectedIdleTime：低功耗模式运行时间
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
	//退出低功耗模式以后打开那些被关闭的外设时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();                
}

//将字符串中的小写字母转换为大写
//str:要转换的字符串
//len：字符串长度
void LowerToCap(u8 *str,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		if((96<str[i])&&(str[i]<123))	//小写字母
		str[i]=str[i]-32;				//转换为大写
	}
}

//命令处理函数，将字符串命令转换成命令值
//str：命令
//返回值: 0XFF，命令错误；其他值，命令值
u8 CommandProcess(u8 *str)
{
	u8 CommandValue=COMMANDERR;
	if(strcmp((char*)str,"LED1ON")==0) CommandValue=LED1ON;
	else if(strcmp((char*)str,"LED1OFF")==0) CommandValue=LED1OFF;
	else if(strcmp((char*)str,"BEEPON")==0) CommandValue=BEEPON;
	else if(strcmp((char*)str,"BEEPOFF")==0) CommandValue=BEEPOFF;
	return CommandValue;
}

int main(void)
{
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();				        //初始化HAL库
	Stm32_Clock_Init(160,5,2,4);    //设置时钟,400Mhz 
	delay_init(400);				//延时初始化
	uart_init(115200);				//串口初始化
    LED_Init();                     //初始化LED 
	KEY_Init();						//初始化按键
	PCF8574_Init();                 //初始化PCF8574
	SDRAM_Init();					//初始化SDRAM
	LCD_Init();						//初始化LCD
    my_mem_init(SRAMIN);            //初始化内部内存池

    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}


//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
	//创建二值信号量
	BinarySemaphore=xSemaphoreCreateBinary();	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )DataProcess_task,     
                (const char*    )"keyprocess_task",   
                (uint16_t       )DATAPROCESS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DATAPROCESS_TASK_PRIO,
                (TaskHandle_t*  )&DataProcess_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	while(1)
	{
		LED0_Toggle;
        vTaskDelay(500);             	//延时500ms，也就是500个时钟节拍	
	}
}

//DataProcess_task函数
void DataProcess_task(void *pvParameters)
{
	u8 len=0;
	u8 CommandValue=COMMANDERR;
	BaseType_t err=pdFALSE;
	
	u8 *CommandStr;
	while(1)
	{
		err=xSemaphoreTake(BinarySemaphore,portMAX_DELAY);	//获取信号量
		if(err==pdTRUE)										//获取信号量成功
		{
			len=USART_RX_STA&0x3fff;						//得到此次接收到的数据长度
			CommandStr=mymalloc(SRAMIN,len+1);				//申请内存
			sprintf((char*)CommandStr,"%s",USART_RX_BUF);
			CommandStr[len]='\0';							//加上字符串结尾符号
			LowerToCap(CommandStr,len);						//将字符串转换为大写		
			CommandValue=CommandProcess(CommandStr);		//命令解析
			if(CommandValue!=COMMANDERR)
			{
				printf("命令为:%s\r\n",CommandStr);
				switch(CommandValue)						//处理命令
				{
					case LED1ON: 
						LED1(0);
						break;
					case LED1OFF:
						LED1(1);
						break;
					case BEEPON:
						PCF8574_WriteBit(BEEP_IO,0);
						break;
					case BEEPOFF:
						PCF8574_WriteBit(BEEP_IO,1);
						break;
				}
			}
			else
			{
				printf("无效的命令，请重新输入!!\r\n");
			}
			USART_RX_STA=0;
			memset(USART_RX_BUF,0,USART_REC_LEN);			//串口接收缓冲区清零
			myfree(SRAMIN,CommandStr);						//释放内存
		}
	}
}
