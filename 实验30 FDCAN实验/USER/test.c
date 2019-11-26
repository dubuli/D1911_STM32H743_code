#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "key.h"
#include "usmart.h"
#include "fdcan.h"
//ALIENTEK ������STM32H7������ ʵ��30
//FDCAN ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 


int main(void)
{   
 	u8 led0sta=1; 
   	u8 key;
	u8 i=0,t=0;
	u8 cnt=0;
	u8 canbuf[8];
	u8 res;
	u8 mode=1;
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200  
	usmart_dev.init(200);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD
	KEY_Init(); 					//������ʼ�� 
		
    FDCAN1_Mode_Init(8,8,11,5,3);  //�ڲ����ز���
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");	
	LCD_ShowString(30,70,200,16,16,"FDCAN TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/7/15");
	LCD_ShowString(30,130,200,16,16,"LoopBack Mode");	 
	LCD_ShowString(30,150,200,16,16,"KEY0:Send WK_UP:Mode");//��ʾ��ʾ��Ϣ		
    POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
	LCD_ShowString(30,170,200,16,16,"Count:");		  	//��ʾ��ǰ����ֵ	
	LCD_ShowString(30,190,200,16,16,"Send Data:");		//��ʾ���͵�����	
	LCD_ShowString(30,250,200,16,16,"Receive Data:");	//��ʾ���յ�������	    
    while(1)
    { 
		key=KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0����,����һ������
		{
			for(i=0;i<8;i++)
			{
				canbuf[i]=cnt+i;	//��䷢�ͻ�����
				if(i<4)LCD_ShowxNum(30+i*32,210,canbuf[i],3,16,0X80);	//��ʾ����
				else LCD_ShowxNum(30+(i-4)*32,230,canbuf[i],3,16,0X80);	//��ʾ����
 			}
			res=FDCAN1_Send_Msg(canbuf,FDCAN_DLC_BYTES_8);				//����8���ֽ� 
			if(res)LCD_ShowString(30+80,190,200,16,16,"Failed");		//��ʾ����ʧ��
			else LCD_ShowString(30+80,190,200,16,16,"OK    ");	 		//��ʾ���ͳɹ�								   
		}else if(key==WKUP_PRES)	//WK_UP���£��ı�CAN�Ĺ���ģʽ
		{	   
  			POINT_COLOR=RED;	//��������Ϊ��ɫ  
			mode=!mode;
            if(mode==0)
			{
				FDCAN1_Mode_Init(8,8,11,5,0);	//��ͨģʽ,������500Kbps
				LCD_ShowString(30,130,200,16,16,"Normal Mode  ");
			}else if(mode==1)
			{
				FDCAN1_Mode_Init(8,8,11,5,3);	//�ڲ�����ģʽ,������500Kbps
 				LCD_ShowString(30,130,200,16,16,"LoopBack Mode");
			}
 			POINT_COLOR=BLUE;	//��������Ϊ��ɫ 
		}		 	
		key=FDCAN1_Receive_Msg(canbuf);
		if(key)					//���յ�������
		{			
			LCD_Fill(30,270,160,310,WHITE);//���֮ǰ����ʾ
 			for(i=0;i<key;i++)
			{									    
				if(i<4)LCD_ShowxNum(30+i*32,270,canbuf[i],3,16,0X80);	//��ʾ����
				else LCD_ShowxNum(30+(i-4)*32,290,canbuf[i],3,16,0X80);	//��ʾ����
 			}
		}
		t++; 
		delay_ms(10);
		if(t==20)
		{
			LED0(led0sta^=1);//��ʾϵͳ��������	
			t=0;
			cnt++;
			LCD_ShowxNum(30+48,170,cnt,3,16,0X80);	//��ʾ����
		}		   
	}  	     
}
















