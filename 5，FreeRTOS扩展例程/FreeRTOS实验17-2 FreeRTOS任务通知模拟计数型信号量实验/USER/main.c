#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"
#include "lcd.h"
#include "sdram.h"
#include "key.h"
#include "timer.h"
#include "pcf8574.h"
#include "malloc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
/************************************************
 ALIENTEK 阿波罗STM32H7开发板 FreeRTOS实验17-2
 FreeRTOS任务通知模拟计数型信号量实验-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//任务优先级
#define START_TASK_PRIO			1
//任务堆栈大小	
#define START_STK_SIZE 			256  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define SEMAPGIVE_TASK_PRIO		2
//任务堆栈大小	
#define SEMAPGIVE_STK_SIZE 		256  
//任务句柄
TaskHandle_t SemapGiveTask_Handler;
//任务函数
void SemapGive_task(void *pvParameters);

//任务优先级
#define SEMAPTAKE_TASK_PRIO 	3
//任务堆栈大小	
#define SEMAPTAKE_STK_SIZE  	256 
//任务句柄
TaskHandle_t SemapTakeTask_Handler;
//任务函数
void SemapTake_task(void *pvParameters);

//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

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

    POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"Apollo STM32H7");	
	LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 17-2");
	LCD_ShowString(30,50,200,16,16,"Task Notify Count Sem");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2018/6/19");
	
	POINT_COLOR = BLACK;
	LCD_DrawRectangle(5,110,234,314);	
	LCD_DrawLine(5,130,234,130);
	POINT_COLOR = RED;
	LCD_ShowString(54,111,200,16,16,"Notify Value:   0");
	POINT_COLOR = BLUE;
	
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
	
    //创建释放信号量任务
    xTaskCreate((TaskFunction_t )SemapGive_task,             
                (const char*    )"semapgive_task",           
                (uint16_t       )SEMAPGIVE_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )SEMAPGIVE_TASK_PRIO,        
                (TaskHandle_t*  )&SemapGiveTask_Handler);   
    //创建获取信号量任务
    xTaskCreate((TaskFunction_t )SemapTake_task,     
                (const char*    )"semaptake_task",   
                (uint16_t       )SEMAPTAKE_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )SEMAPTAKE_TASK_PRIO,
                (TaskHandle_t*  )&SemapTakeTask_Handler); 
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//释放计数型信号量任务函数
void SemapGive_task(void *pvParameters)
{
	u8 key,i=0;
	while(1)
	{
		key=KEY_Scan(0);           	//扫描按键
        if(SemapTakeTask_Handler!=NULL)  	
        {
            switch(key)
            {
                case WKUP_PRES:
					xTaskNotifyGive(SemapTakeTask_Handler);//发送任务通知
                    break;
            }
        }
        i++;
        if(i==50)
        {
            i=0;
            LED0_Toggle;
        }
        vTaskDelay(10);     //延时10ms，也就是10个时钟节拍	
	}
}

//获取计数型信号量任务函数
void SemapTake_task(void *pvParameters)
{
    u8 num;
    uint32_t NotifyValue;
	while(1)
	{
		NotifyValue=ulTaskNotifyTake(pdFALSE,portMAX_DELAY);//获取任务通知
        num++;
        LCD_ShowxNum(166,111,NotifyValue-1,3,16,0);         //显示当前任务通知值
		LCD_Fill(6,131,233,313,lcd_discolor[num%14]);   	//刷屏
		LED1_Toggle;
        vTaskDelay(1000);                               	//延时1s，也就是1000个时钟节拍	
	}
}

