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
//ALIENTEK ������STM32H7������ ʵ��39
//�ڴ���� ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  

const u8* SRAM_NAME_BUF[SRAMBANK]={" SRAMIN "," SRAMEX "," SRAM12 "," SRAM4  ","SRAMDTCM","SRAMITCM"};
int main(void)
{  
	u8 led0sta=1;
	u8 paddr[20];					//���P Addr:+p��ַ��ASCIIֵ
	u16 memused=0;
	u8 key;		 
 	u8 i=0;	    
	u8 *p=0;
	u8 *tp=0;
	u8 sramx=0;						//Ĭ��Ϊ�ڲ�sram    
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	usmart_dev.init(100);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
	KEY_Init();						//��ʼ������ 
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
   	POINT_COLOR=RED;
	LCD_ShowString(30,20,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,40,200,16,16,"MALLOC TEST");	
	LCD_ShowString(30,60,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,80,200,16,16,"2018/7/30");	 		
	LCD_ShowString(30,100,200,16,16,"KEY0:Malloc  KEY2:Free");
	LCD_ShowString(30,120,200,16,16,"KEY_UP:SRAMx KEY1:Read"); 
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(30,140,200,16,16," SRAMIN ");
	LCD_ShowString(30,156,200,16,16,"SRAMIN   USED:");
	LCD_ShowString(30,172,200,16,16,"SRAMEX   USED:");
	LCD_ShowString(30,188,200,16,16,"SRAM12   USED:");
	LCD_ShowString(30,204,200,16,16,"SRAM4    USED:");
	LCD_ShowString(30,220,200,16,16,"SRAMDTCM USED:");
	LCD_ShowString(30,236,200,16,16,"SRAMITCM USED:");
 	while(1)
	{	
		key=KEY_Scan(0);//��֧������	
		switch(key)
		{
			case 0://û�а�������	
				break;
			case KEY0_PRES:	//KEY0����
				p=mymalloc(sramx,2048);//����2K�ֽ�
				if(p!=NULL)sprintf((char*)p,"Memory Malloc Test%03d",i);//��pд��һЩ����
				break;
			case KEY1_PRES:	//KEY1����	   
				if(p!=NULL)
				{
					sprintf((char*)p,"Memory Malloc Test%03d",i);//������ʾ���� 	 
					LCD_ShowString(30,280,209,16,16,p);			 //��ʾP������
				}
				break;
			case KEY2_PRES:	//KEY2����	  
				myfree(sramx,p);//�ͷ��ڴ�
				p=0;			//ָ��յ�ַ
				break;
			case WKUP_PRES:	//KEY UP���� 
				sramx++;  
				if(sramx>5)sramx=0;
				LCD_ShowString(30,140,200,16,16,(u8*)SRAM_NAME_BUF[sramx]); 
				break;
		}
		if(tp!=p&&p!=NULL)
		{
			tp=p;
			sprintf((char*)paddr,"P Addr:0X%08X",(u32)tp);
			LCD_ShowString(30,260,209,16,16,paddr);	//��ʾp�ĵ�ַ
			if(p)LCD_ShowString(30,280,280,16,16,p);//��ʾP������
		    else LCD_Fill(30,280,209,296,WHITE);	//p=0,�����ʾ
		}
		delay_ms(10);   
		i++;
		if((i%20)==0)//DS0��˸.
		{
			memused=my_mem_perused(SRAMIN);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,156,200,16,16,paddr);	//��ʾ�ڲ��ڴ�ʹ����
			memused=my_mem_perused(SRAMEX);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,172,200,16,16,paddr);	//��ʾ�ⲿ�ڴ�ʹ����
			memused=my_mem_perused(SRAM12);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,188,200,16,16,paddr);	//��ʾTCM�ڴ�ʹ���� 
			memused=my_mem_perused(SRAM4);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,204,200,16,16,paddr);	//��ʾ�ڲ��ڴ�ʹ����
			memused=my_mem_perused(SRAMDTCM);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,220,200,16,16,paddr);	//��ʾ�ⲿ�ڴ�ʹ����
			memused=my_mem_perused(SRAMITCM);
			sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
			LCD_ShowString(30+112,236,200,16,16,paddr);	//��ʾTCM�ڴ�ʹ���� 
 			LED0(led0sta^=1);
 		}
	}	 
}










