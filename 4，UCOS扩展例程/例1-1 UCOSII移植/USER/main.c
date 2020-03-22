#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h"
#include "includes.h"
/************************************************
 ALIENTEK 阿波罗STM32H7开发板 UCOS实验
 例1-1 UCOSII移植实验-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//START 任务
//设置任务优先级
#define START_TASK_PRIO			10  ///开始任务的优先级为最低
//设置任务堆栈大小
#define START_STK_SIZE			128
//任务任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);

//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO			7
//设置任务堆栈大小
#define LED0_STK_SIZE			128
//任务堆栈
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);

//LED1任务
//设置任务优先级
#define LED1_TASK_PRIO			6
//设置任务堆栈大小
#define LED1_STK_SIZE			128
//任务堆栈
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *pdata);

//浮点测试任务
#define FLOAT_TASK_PRIO			5
//设置任务堆栈大小
#define FLOAT_STK_SIZE			128
//任务堆栈
//如果任务中使用printf来打印浮点数据的话一点要8字节对齐
__align(8) OS_STK FLOAT_TASK_STK[FLOAT_STK_SIZE]; 
//任务函数
void float_task(void *pdata);

int main(void)
{
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();				        //初始化HAL库
	Stm32_Clock_Init(160,5,2,4);    //设置时钟,400Mhz 
	delay_init(400);				//延时初始化
	uart_init(115200);				//串口初始化
	LED_Init();						//初始化LED
    
    OSInit();                       //UCOS初始化
    OSTaskCreateExt((void(*)(void*) )start_task,                //任务函数
                    (void*          )0,                         //传递给任务函数的参数
                    (OS_STK*        )&START_TASK_STK[START_STK_SIZE-1],//任务堆栈栈顶
                    (INT8U          )START_TASK_PRIO,           //任务优先级
                    (INT16U         )START_TASK_PRIO,           //任务ID，这里设置为和优先级一样
                    (OS_STK*        )&START_TASK_STK[0],        //任务堆栈栈底
                    (INT32U         )START_STK_SIZE,            //任务堆栈大小
                    (void*          )0,                         //用户补充的存储区
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);//任务选项,为了保险起见，所有任务都保存浮点寄存器的值
	OSStart(); //开始任务
}

//开始任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //开启统计任务
	
	OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
    //LED0任务
    OSTaskCreateExt((void(*)(void*) )led0_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED0_TASK_STK[LED0_STK_SIZE-1],
                    (INT8U          )LED0_TASK_PRIO,            
                    (INT16U         )LED0_TASK_PRIO,            
                    (OS_STK*        )&LED0_TASK_STK[0],         
                    (INT32U         )LED0_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//LED1任务
    OSTaskCreateExt((void(*)(void*) )led1_task,                 
                    (void*          )0,
                    (OS_STK*        )&LED1_TASK_STK[LED1_STK_SIZE-1],
                    (INT8U          )LED1_TASK_PRIO,            
                    (INT16U         )LED1_TASK_PRIO,            
                    (OS_STK*        )&LED1_TASK_STK[0],         
                    (INT32U         )LED1_STK_SIZE,             
                    (void*          )0,                         
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
	//浮点测试任务
    OSTaskCreateExt((void(*)(void*) )float_task,                 
                    (void*          )0,
                    (OS_STK*        )&FLOAT_TASK_STK[FLOAT_STK_SIZE-1],
                    (INT8U          )FLOAT_TASK_PRIO,          
                    (INT16U         )FLOAT_TASK_PRIO,            
                    (OS_STK*        )&FLOAT_TASK_STK[0],         
                    (INT32U         )FLOAT_STK_SIZE,            
                    (void*          )0,                           
                    (INT16U         )OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR|OS_TASK_OPT_SAVE_FP);
    OS_EXIT_CRITICAL();             //退出临界区(开中断)
	OSTaskSuspend(START_TASK_PRIO); //挂起开始任务
}
 

//LED0任务
void led0_task(void *pdata)
{
	while(1)
	{
		LED0(0);    //LED0打开
		OSTimeDlyHMSM(0,0,0,200); //延时200ms
		LED0(1);    //LED0关闭
		OSTimeDlyHMSM(0,0,0,500); //延时500ms
	}
}

//LED1任务
void led1_task(void *pdata)
{
	while(1)
	{
        LED1_Toggle;
        delay_ms(500);//延时500ms
	}
}

//浮点测试任务
void float_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	static double double_num=0.00;
	while(1)
	{
		double_num+=0.01f;
		OS_ENTER_CRITICAL();	//进入临界区(关闭中断)
        printf("double_num的值为: %.4f\r\n",double_num);
		OS_EXIT_CRITICAL();		//退出临界区(开中断)
        delay_ms(1000);			//延时1000ms
	}
}

