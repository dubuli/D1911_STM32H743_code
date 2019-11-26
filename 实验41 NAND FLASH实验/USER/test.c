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
#include "nand.h"    
#include "ftl.h"  
//ALIENTEK ������STM32H7������ ʵ��41
//NAND FLASH ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  

 
int main(void)
{  
	u8 led0sta=1;
	u8 key,t=0;
	u16 i;
	u8 *buf;
	u8 *backbuf;  
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
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7"); 
	LCD_ShowString(30,70,200,16,16,"NAND TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/8/1");	 	 
	LCD_ShowString(30,130,200,16,16,"KEY0:Read Sector 2");
	LCD_ShowString(30,150,200,16,16,"KEY1:Write Sector 2");
	LCD_ShowString(30,170,200,16,16,"KEY2:Recover Sector 2");		   
 	while(FTL_Init())			//���NAND FLASH,����ʼ��FTL
	{
		LCD_ShowString(30,190,200,16,16,"NAND Error!");
		delay_ms(500);				 
		LCD_ShowString(30,190,200,16,16,"Please Check");
		delay_ms(500);	 
		LED0(led0sta^=1);//DS0��˸
	}
	backbuf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);	//����һ�������Ļ���
	buf=mymalloc(SRAMIN,NAND_ECC_SECTOR_SIZE);		//����һ�������Ļ���
 	POINT_COLOR=BLUE;								//��������Ϊ��ɫ 
	sprintf((char*)buf,"NAND Size:%dMB",(nand_dev.block_totalnum/1024)*(nand_dev.page_mainsize/1024)*nand_dev.block_pagenum);
	LCD_ShowString(30,190,200,16,16,buf);			//��ʾNAND����  
	FTL_ReadSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//Ԥ�ȶ�ȡ����2����������,��ֹ��д�����ļ�ϵͳ��.
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://KEY0����,��ȡsector
				key=FTL_ReadSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);//��ȡ����
				if(key==0)//��ȡ�ɹ�
				{
					LCD_ShowString(30,210,200,16,16,"USART1 Sending Data...  ");
					printf("Sector 2 data is:\r\n");
					for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)
					{
						printf("%x ",buf[i]);//�������
					}
					printf("\r\ndata end.\r\n");
					LCD_ShowString(30,210,200,16,16,"USART1 Send Data Over!  "); 
				}
				break;
			case KEY1_PRES://KEY1����,д��sector
				for(i=0;i<NAND_ECC_SECTOR_SIZE;i++)buf[i]=i+t;	//�������(�����,����t��ֵ��ȷ��) 
				LCD_ShowString(30,210,210,16,16,"Writing data to sector..");
				key=FTL_WriteSectors(buf,2,NAND_ECC_SECTOR_SIZE,1);//д������
				if(key==0)LCD_ShowString(30,210,200,16,16,"Write data successed    ");//д��ɹ�
				else LCD_ShowString(30,210,200,16,16,"Write data failed       ");//д��ʧ��
				break;
			case KEY2_PRES://KEY2����,�ָ�sector������
				LCD_ShowString(30,210,210,16,16,"Recovering data...      ");
				key=FTL_WriteSectors(backbuf,2,NAND_ECC_SECTOR_SIZE,1);//д������
				if(key==0)LCD_ShowString(30,210,200,16,16,"Recovering data OK      ");//�ָ��ɹ�
				else LCD_ShowString(30,210,200,16,16,"Recovering data failed  ");//�ָ�ʧ��
				break;
		}
		t++;
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);
			t=0;
		}
	} 
}











