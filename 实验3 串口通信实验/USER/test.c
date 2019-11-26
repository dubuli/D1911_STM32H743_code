#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h" 
//ALIENTEK 阿波罗STM32H7开发板 实验3
//串口通信实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
  
int main(void)
{ 
	u8 t;
	u8 len;	
	u16 times=0;  
	u8 led0sta=1;				//LED0前状态
	Stm32_Clock_Init(160,5,2,4);//设置时钟,400Mhz
    delay_init(400);			//延时初始化  
	uart_init(100,115200);		//串口初始化为115200
	LED_Init();		  			//初始化与LED连接的硬件接口   
	while(1)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART1->TDR=USART_RX_BUF[t];
				while((USART1->ISR&0X40)==0);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0; 
		}else
		{
			times++;
			if(times%5000==0)
			{
				printf("\r\nALIENTEK 阿波罗STM32F4/F7/H7开发板 串口实验\r\n");
				printf("正点原子@ALIENTEK\r\n\r\n\r\n");
			}
			if(times%200==0)printf("请输入数据,以回车键结束\r\n");  
			if(times%30==0)LED0(led0sta^=1);//闪烁LED,提示系统正在运行.
			delay_ms(10);   
		}
	}
}
















