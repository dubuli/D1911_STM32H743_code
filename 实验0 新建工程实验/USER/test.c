#include "sys.h" 
#include "usart.h" 
#include "delay.h" 
//ALIENTEK 阿波罗STM32H7开发板 实验0
//新建工程实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司

int main(void)
{ 
	u8 t=0;
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化
	uart_init(100,115200);			//串口初始化
	while(1)
	{
		printf("t:%d\r\n",t);
		delay_ms(500);
		t++;
	}
}
