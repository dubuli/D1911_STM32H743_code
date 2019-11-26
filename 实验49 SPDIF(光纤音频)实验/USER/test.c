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
#include "w25qxx.h"    
#include "sdmmc_sdcard.h" 
#include "ff.h"  
#include "exfuns.h" 
#include "text.h"
#include "sai.h"	 	
#include "wm8978.h"	 
#include "spdif.h"		
#include "string.h"	 
//ALIENTEK ������STM32H7������ ʵ��49
//SPDIF ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾    

#define	SPDIF_DBUF_SIZE		1024			//����SPDIF���ݽ��ջ������Ĵ�С,1K*4�ֽ�
	
u32 spdif_audiobuf[2][SPDIF_DBUF_SIZE]; 	//SPDIF��Ƶ���ݽ��ջ�����,��2��(˫����)

//��ʾ������
//samplerate:��Ƶ������(��λ:Hz)
void spdif_show_samplerate(u32 samplerate)
{  
	u8 *buf;
	buf=mymalloc(SRAMIN,100);	//�����ڴ�
	if(buf)						//����ɹ�
	{	
		if(samplerate)sprintf((char*)buf,"%dHz",samplerate);//��ӡ������ 
		else sprintf((char*)buf,"����ʶ��...");				//������Ϊ0,����ʾ����ʶ��
		LCD_Fill(30+56,170,230,170+16,WHITE);
		Show_Str(30+56,170,200,16,buf,16,0);
	}
	myfree(SRAMIN,buf);			//�ͷ��ڴ�
}

//SAI DMA��������жϻص�����
//����δ���κδ���.
void sai_dma_tx_callback(void)
{
}	

//SPDIF RX����ʱ�Ļص����� 
void spdif_rx_stopplay_callback(void)
{
	SAI_Play_Stop();			
	SPDIFRX->IFCR|=1<<5;//���ͬ����ɱ�־	
	spdif_show_samplerate(0);
    memset((u8*)&spdif_audiobuf[0],0,SPDIF_DBUF_SIZE*4);   
    memset((u8*)&spdif_audiobuf[1],0,SPDIF_DBUF_SIZE*4);
}	


int main(void)
{   
	u8 led0sta=1;
	u8 t; 
	u8 key;
	u8 vol=45;	//Ĭ������ 
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz 
	delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	usmart_dev.init(100);			//��ʼ��USMART
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
	KEY_Init();						//��ʼ������
	W25QXX_Init();					//��ʼ��W25Q256 
	WM8978_Init();					//��ʼ��WM8978
	WM8978_HPvol_Set(40,40);		//������������
	WM8978_SPKvol_Set(50);			//������������
	SPDIF_RX_Init();				//SPDIF��ʼ��
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
	exfuns_init();					//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 			//����SD��  
	POINT_COLOR=RED;      
	while(font_init()) 				//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 
	POINT_COLOR=RED;      
 	Show_Str(30,40,200,16,"������STM32F4/F7/H7������",16,0);				    	 
	Show_Str(30,60,200,16,"SPDIF(������Ƶ)ʵ��",16,0);				    	 
	Show_Str(30,80,200,16,"����ԭ��@ALIENTEK",16,0);				    	 
	Show_Str(30,100,200,16,"2018��8��17��",16,0); 
	Show_Str(30,120,200,16,"KEY_UP:VOL+  KEY1:VOL-",16,0); 	
	
	Show_Str(30,150,200,16,"����:",16,0);  
	Show_Str(30,170,200,16,"������:",16,0); 
	POINT_COLOR=BLUE;     
	LCD_ShowxNum(30+40,150,vol,2,16,0X80);	//��ʾ���� 
	spdif_show_samplerate(0);				//��ʾ������
	
	WM8978_ADDA_Cfg(1,0);		//����DAC
	WM8978_Input_Cfg(0,0,0);	//�ر�����ͨ��
	WM8978_Output_Cfg(1,0);		//����DAC��� 	
    SPDIF_RXDATA_DMA_Init((u32*)&spdif_audiobuf[0],(u32*)&spdif_audiobuf[1],SPDIF_DBUF_SIZE,2);  
	spdif_rx_stop_callback=spdif_rx_stopplay_callback;	//SPDIF ��������ʱ�Ļص����� 
	while(1)
	{ 
		key=KEY_Scan(1);
		if(key==WKUP_PRES||key==KEY1_PRES)	//��������
		{
			if(key==WKUP_PRES)
			{
				vol++;
				if(vol>63)vol=63; 
			}else
			{
				if(vol>0)vol--;
			}
			WM8978_HPvol_Set(vol,vol);		//���ö�����������
			WM8978_SPKvol_Set(vol);			//���������������� 
			LCD_ShowxNum(30+40,150,vol,2,16,0X80);//��ʾ���� 
		}
        if(spdif_dev.consta==0)				//δ����
        {
            if(SPDIF_RX_WaitSync())			//�ȴ�ͬ��
            {
                spdif_dev.samplerate=SPDIF_RX_GetSampleRate();	//��ò����� 
				if(spdif_dev.samplerate)	//��������Ч,����WM8978��SAI��
				{
					WM8978_I2S_Cfg(2,2);						//�����ֱ�׼,24λ���ݳ���
					SAIA_Init(0,1,6);							//����SAI,������,24λ����
					SAIA_SampleRate_Set(spdif_dev.samplerate);	//���ò�����   
					SPDIF_RX_Start();							//��SPDIF
					SAIA_TX_DMA_Init((u8*)&spdif_audiobuf[0],(u8*)&spdif_audiobuf[1],SPDIF_DBUF_SIZE,2); //����TX DMA,32λ
					sai_tx_callback=sai_dma_tx_callback;
					SAI_Play_Start();							//����DMA 
					spdif_show_samplerate(spdif_dev.samplerate);//��ʾ������.						
				}else SPDIF_RX_Stop();							//�����ʴ���,ֹͣSPDIF���� 
            }else LED0(led0sta^=1);								//LED0��˸
        }
		delay_ms(10);
		t++;
		if(t>=20)
		{
			t=0; 
			LED0(led0sta^=1);
		}
	} 
}






















