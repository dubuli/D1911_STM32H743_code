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
#include "piclib.h"	 
#include "usbh_core.h"
#include "usbh_msc.h"
//ALIENTEK ������STM32H7������ ʵ��59
//U��(Host) ʵ��
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  

USBH_HandleTypeDef hUSBHost;	//USB Host����ṹ�� 
 

static void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
{
    u32 total,free;
	u8 res=0;
    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_DISCONNECTION:
            f_mount(0,"3:",1); 	//ж��U��
            POINT_COLOR=RED;    //��������Ϊ��ɫ	   
            Show_Str(30,140,200,16,"�豸������...",16,0);
            LCD_Fill(30,160,239,220,WHITE); 
            break;
        case HOST_USER_CLASS_ACTIVE:
            Show_Str(30,140,200,16,"�豸���ӳɹ�!.",16,0);	
            f_mount(fs[3],"3:",1); 	//���¹���U��
            res=exf_getfree("3:",&total,&free);
        	if(res==0)
            {
                POINT_COLOR=BLUE;//��������Ϊ��ɫ	   
                LCD_ShowString(30,160,200,16,16,"FATFS OK!");	
                LCD_ShowString(30,180,200,16,16,"U Disk Total Size:     MB");	 
                LCD_ShowString(30,200,200,16,16,"U Disk  Free Size:     MB"); 	    
                LCD_ShowNum(174,180,total>>10,5,16);//��ʾU�������� MB
                LCD_ShowNum(174,200,free>>10,5,16);	
            }
            else
            {
                printf("U�̴洢�ռ��ȡʧ��\r\n");
            }
            break;
        case HOST_USER_CONNECTION:
            break;
        default:
            break;
    }
}


int main(void)
{  
	u8 led0sta=1;
	u8 t=0;	
	
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
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
	exfuns_init();					//Ϊfatfs��ر��������ڴ�  
	piclib_init();					//��ʼ����ͼ 
 	f_mount(fs[0],"0:",1); 			//����SD�� 
 	f_mount(fs[1],"1:",1); 			//����FLASH.
 	f_mount(fs[2],"2:",1); 			//����NAND FLASH.
	POINT_COLOR=RED;      
 	while(font_init())				//����ֿ�
	{	    
		LCD_ShowString(60,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(60,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
	Show_Str(30,50,200,16,"������STM32F4/F7/H7������",16,0);				    	 
	Show_Str(30,70,200,16,"USB U��ʵ��",16,0);					    	 
	Show_Str(30,90,200,16,"2018��9��3��",16,0);	    	 
	Show_Str(30,110,200,16,"����ԭ��@ALIENTEK",16,0); 
	Show_Str(30,140,200,16,"�豸������...",16,0);	

    USBH_Init(&hUSBHost, USBH_UserProcess, 0);
    USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
    USBH_Start(&hUSBHost);
	
    while (1)
    {
        USBH_Process(&hUSBHost);
        delay_ms(10);
		t++; 
		if(t==50)
        { 
            t=0;
            LED0(led0sta^=1);
        }
    }	 
}







