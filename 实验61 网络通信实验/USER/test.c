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
#include "pcf8574.h"  
#include "adc.h"  
#include "rtc.h" 
#include "timer.h"  
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_client_demo.h"
#include "tcp_server_demo.h"
#include "udp_demo.h"
#include "httpd.h"
//ALIENTEK ������STM32H7������ ʵ��61
//����ͨ�� ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  


//����UI
//mode:
//bit0:0,������;1,����ǰ�벿��UI
//bit1:0,������;1,���غ�벿��UI
void lwip_test_ui(u8 mode)
{
	u32 linkstate;
	u8 buf[30]; 
	POINT_COLOR=RED;
	if(mode&1<<0)
	{
		LCD_Fill(30,30,lcddev.width,110,WHITE);	//�����ʾ
		LCD_ShowString(30,30,200,16,16,"Apollo STM32F4/F7/H7");
		LCD_ShowString(30,50,200,16,16,"Ethernet lwIP Test");
		LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
		LCD_ShowString(30,90,200,16,16,"2018/10/30"); 	
	}
	if(mode&1<<1)
	{
		LCD_Fill(30,110,lcddev.width,lcddev.height,WHITE);	//�����ʾ
		LCD_ShowString(30,110,200,16,16,"lwIP Init Successed");
		if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		else sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
        linkstate=LAN8720_GetLinkState();       //��ȡ����״̬
        if(linkstate==LAN8720_STATUS_100MBITS_FULLDUPLEX)
            LCD_ShowString(30,150,200,16,16,"eth state:100M,FullDuplex");
        else if(linkstate==LAN8720_STATUS_100MBITS_HALFDUPLEX)
            LCD_ShowString(30,150,200,16,16,"eth state:100M,HalfDuplex");
        else if(linkstate==LAN8720_STATUS_10MBITS_FULLDUPLEX)
            LCD_ShowString(30,150,200,16,16,"eth state:10M,FullDuplex");
        else if(linkstate==LAN8720_STATUS_10MBITS_HALFDUPLEX)
            LCD_ShowString(30,150,200,16,16,"eth state:10M,HalfDuplex");
		LCD_ShowString(30,170,200,16,16,"KEY0:TCP Server Test");
		LCD_ShowString(30,190,200,16,16,"KEY1:TCP Client Test");
		LCD_ShowString(30,210,200,16,16,"KEY2:UDP Test");
	}
}

int main(void)
{
 	u8 led0sta=1;
	u8 t;
	u8 key;		
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	usmart_dev.init(100);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
	KEY_Init();						//��ʼ������ 
	Adc_Init();						//��ʼ��ADC1
	Adc3_Init();					//��ʼ��ADC3	
	PCF8574_Init();					//��ʼ��PCF8574
    RTC_Init();                     //��ʼ��RTC 
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
    TIM3_Int_Init(1000-1,2000-1);	//��ʱ��3��ʼ������ʱ��ʱ��Ϊ200M����Ƶϵ��Ϊ2000-1��
									//���Զ�ʱ��3��Ƶ��Ϊ200M/2000=100K���Զ���װ��Ϊ1000-1����ô��ʱ�����ھ���10ms
	lwip_test_ui(1);			    //����ǰ�벿��UI
    LCD_ShowString(30,110,200,16,16,"lwIP Initing...");
	while(lwip_comm_init())         //lwip��ʼ��
	{
		LCD_ShowString(30,110,200,20,16,"LWIP Init Falied! ");
		delay_ms(500);
		LCD_ShowString(30,110,200,16,16,"Retrying...       ");
        delay_ms(500);
	}
	LCD_ShowString(30,110,200,20,16,"LWIP Init Success!");
 	LCD_ShowString(30,130,200,16,16,"DHCP IP configing...");  //�ȴ�DHCP��ȡ 
#if LWIP_DHCP   //ʹ��DHCP
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{  
		lwip_periodic_handle();	//LWIP�ں���Ҫ��ʱ����ĺ���
	}
#endif
    lwip_test_ui(2);		//���غ�벿��UI 
	httpd_init();	        //HTTP��ʼ��(Ĭ�Ͽ���websever)
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES://TCP Serverģʽ
				tcp_server_test();
 				lwip_test_ui(3);//���¼���UI  
				break;
			case KEY1_PRES://TCP Clientģʽ
				tcp_client_test();
				lwip_test_ui(3);//���¼���UI
				break; 
			case KEY2_PRES://UDPģʽ
				udp_demo_test();
				lwip_test_ui(3);//���¼���UI
				break; 
		} 
		lwip_periodic_handle();
		delay_ms(2);
		t++;
		if(t==100)LCD_ShowString(30,230,200,16,16,"Please choose a mode!");
		if(t==200)
		{ 
			t=0;
			LCD_Fill(30,230,230,230+16,WHITE);//�����ʾ
			LED0(led0sta^=1);
		} 
	} 	
}

