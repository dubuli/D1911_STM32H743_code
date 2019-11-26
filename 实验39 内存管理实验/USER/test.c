#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h" 
#include "malloc.h"  
//ALIENTEK 阿波罗STM32H7开发板 实验39
//内存管理 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  

const u8* SRAM_NAME_BUF[SRAMBANK]={" SRAMIN "," SRAMEX "," SRAM12 "," SRAM4  ","SRAMDTCM","SRAMITCM"};
int main(void)
{  
	u8 led0sta=1;
	u8 paddr[20];					//存放P Addr:+p地址的ASCII值
	u16 memused=0;
	u8 key;		 
 	u8 i=0;	    
	u8 *p=0;
	u8 *tp=0;
	u8 sramx=0;						//默认为内部sram    
	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	usmart_dev.init(100);			//初始化USMART
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
	KEY_Init();						//初始化按键 
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
   	POINT_COLOR=RED;
	LCD_ShowString(30,20,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,40,200,16,16,"MALLOC TEST");	
	LCD_ShowString(30,60,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,80,200,16,16,"2018/7/30");	 		
	LCD_ShowString(30,100,200,16,16,"KEY0:Malloc  KEY2:Free");
	LCD_ShowString(30,120,200,16,16,"KEY_UP:SRAMx KEY1:Read"); 
 	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(30,140,200,16,16," SRAMIN ");
	LCD_ShowString(30,156,200,16,16,"SRAMIN   USED:");
	LCD_ShowString(30,172,200,16,16,"SRAMEX   USED:");
	LCD_ShowString(30,188,200,16,16,"SRAM12   USED:");
	LCD_ShowString(30,204,200,16,16,"SRAM4    USED:");
	LCD_ShowString(30,220,200,16,16,"SRAMDTCM USED:");
	LCD_ShowString(30,236,200,16,16,"SRAMITCM USED:");
 	while(1)
	{	
		key=KEY_Scan(0);//不支持连按	
		switch(key)
		{
			case 0://没有按键按下	
				break;
			case KEY0_PRES:	//KEY0按下
				p=mymalloc(sramx,2048);//申请2K字节
				if(p!=NULL)sprintf((char*)p,"Memory Malloc Test%03d",i);//向p写入一些内容
				break;
			case KEY1_PRES:	//KEY1按下	   
				if(p!=NULL)
				{
					sprintf((char*)p,"Memory Malloc Test%03d",i);//更新显示内容 	 
					LCD_ShowString(30,280,209,16,16,p);			 //显示P的内容
				}
				break;
			case KEY2_PRES:	//KEY2按下	  
				myfree(sramx,p);//释放内存
				p=0;			//指向空地址
				break;
			case WKUP_PRES:	//KEY UP按下 
				sramx++;  
				if(sramx>5)sramx=0;
				LCD_ShowString(30,140,200,16,16,(u8*)SRAM_NAME_BUF[sramx]); 
				break;
		}
		if(tp!=p&&p!=NULL)
		{
			tp=p;
			sprintf((char*)paddr,"P Addr:0X%08X",(u32)tp);
			LCD_ShowString(30,260,209,16,16,paddr);	//显示p的地址
			if(p)LCD_ShowString(30,280,280,16,16,p);//显示P的内容
		    else LCD_Fill(30,280,209,296,WHITE);	//p=0,清除显示
		}
		delay_ms(10);   
		i++;
		if((i%20)==0)//DS0闪烁.
		{
			memused=my_mem_perused(SRAMIN);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,156,200,16,16,paddr);	//显示内部内存使用率
			memused=my_mem_perused(SRAMEX);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,172,200,16,16,paddr);	//显示外部内存使用率
			memused=my_mem_perused(SRAM12);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,188,200,16,16,paddr);	//显示TCM内存使用率 
			memused=my_mem_perused(SRAM4);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,204,200,16,16,paddr);	//显示内部内存使用率
			memused=my_mem_perused(SRAMDTCM);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,220,200,16,16,paddr);	//显示外部内存使用率
			memused=my_mem_perused(SRAMITCM);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,236,200,16,16,paddr);	//显示TCM内存使用率 
 			LED0(led0sta^=1);
 		}
	}	 
}










