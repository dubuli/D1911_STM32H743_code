#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h"    
#include "malloc.h" 
#include "pcf8574.h" 
#include "usbh_core.h" 
#include "usbh_hid.h"
#include "usbh_hid_parser.h" 
//ALIENTEK ������STM32H7������ ʵ��60
//USB������(Host) ʵ�� 
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  


USBH_HandleTypeDef hUSBHost;	//USB Host����ṹ��

//Ӧ��״̬�ṹ�����Ͷ���
typedef enum {	
  APPLICATION_IDLE = 0,			//����״̬
  APPLICATION_DISCONNECT,		//�Ͽ�����
  APPLICATION_START,			//Ӧ�ÿ�ʼ���������ˣ�
  APPLICATION_READY,			//׼����ɣ�����ִ�����Ӧ�ô����ˣ�
  APPLICATION_RUNNING,			//������...
}HID_ApplicationTypeDef;


extern HID_MOUSE_Info_TypeDef    mouse_info;
HID_ApplicationTypeDef App_State = APPLICATION_IDLE;

u8 USB_FIRST_PLUGIN_FLAG=0;	//USB��һ�β����־,���Ϊ1,˵���ǵ�һ�β���




//USB��Ϣ��ʾ
//msgx:0,USB������
//     1,USB����
//     2,USB���
//     3,��֧�ֵ�USB�豸
void USBH_Msg_Show(u8 msgx)
{
	POINT_COLOR=RED;
	switch(msgx)
	{
		case 0:	//USB������
			LCD_ShowString(30,130,200,16,16,"USB Connecting...");	
			LCD_Fill(0,150,lcddev.width,lcddev.height,WHITE);
			break;
		case 1:	//USB����
			LCD_ShowString(30,130,200,16,16,"USB Connected    ");	
			LCD_ShowString(30,150,200,16,16,"USB KeyBoard");	 
			LCD_ShowString(30,180,210,16,16,"KEYVAL:");	
			LCD_ShowString(30,200,210,16,16,"INPUT STRING:");	
			break;
		case 2:	//USB���
			LCD_ShowString(30,130,200,16,16,"USB Connected    ");	
			LCD_ShowString(30,150,200,16,16,"USB Mouse");	 
			LCD_ShowString(30,180,210,16,16,"BUTTON:");	
			LCD_ShowString(30,200,210,16,16,"X POS:");	
			LCD_ShowString(30,220,210,16,16,"Y POS:");	
			LCD_ShowString(30,240,210,16,16,"Z POS:");	
			break; 		
		case 3:	//��֧�ֵ�USB�豸
			LCD_ShowString(30,130,200,16,16,"USB Connected    ");	
			LCD_ShowString(30,150,200,16,16,"Unknow Device");	 
			break; 	 
	} 
} 


//���USBH��ͬ�û�״̬�µ����ݴ���
//phost:USBH���ָ��
//id:USBH��ǰ���û�״̬
void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
{
	u8 dev_type=0XFF;	//�豸����,1,����;2,���;����,��֧�ֵ��豸.
    switch(id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_DISCONNECTION:
            USBH_Msg_Show(0);					//��ʾ�Ѿ��Ͽ�����,׼����������
            App_State=APPLICATION_DISCONNECT;
            break;
        case HOST_USER_CLASS_ACTIVE: 	
            App_State=APPLICATION_READY;
			dev_type=phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bInterfaceProtocol;
			if(dev_type==HID_KEYBRD_BOOT_CODE)	//�����豸
			{  
				USB_FIRST_PLUGIN_FLAG=1;//��ǵ�һ�β���
				USBH_Msg_Show(1);		//��ʾ���̽���
			}else if(dev_type==HID_MOUSE_BOOT_CODE)	//����豸
			{ 
				USB_FIRST_PLUGIN_FLAG=1;//��ǵ�һ�β���
				USBH_Msg_Show(2);		//��ʾ������
			}else USBH_Msg_Show(3);		//��ʾ��֧��
            break;
        case HOST_USER_CONNECTION: 		
            App_State=APPLICATION_START;
            break;
        default:
            break;
    }
}

//��ʱ����,���ڴ���������/������������(4.3��,����������2016�ֽ�)
__align(4) u8 tbuf[2017]; 

//USB������ݴ���
//data:USB������ݽṹ��ָ��
void MYUSR_MOUSE_ProcessData(HID_MOUSE_Info_TypeDef *data)
{  
	static signed short x,y,z; 
	if(USB_FIRST_PLUGIN_FLAG)//��һ�β���,����������
	{
		USB_FIRST_PLUGIN_FLAG=0;
		x=y=z=0;
	}
	x+=(signed char)data->x;
	if(x>9999)x=9999;
	if(x<-9999)x=-9999;
	y+=(signed char)data->y;
	if(y>9999)y=9999;
	if(y<-9999)y=-9999;
	z+=(signed char)data->z;
	if(z>9999)z=9999;
	if(z<-9999)z=-9999;
	POINT_COLOR=BLUE;
	sprintf((char*)tbuf,"BUTTON:");
	if(data->button&0X01)strcat((char*)tbuf,"LEFT ");
	if((data->button&0X03)==0X02)strcat((char*)tbuf,"RIGHT");
	else if((data->button&0X03)==0X03)strcat((char*)tbuf,"+RIGHT");
	if((data->button&0X07)==0X04)strcat((char*)tbuf,"MID  ");
	else if((data->button&0X07)>0X04)strcat((char*)tbuf,"+MID");  
 	LCD_Fill(30+56,180,lcddev.width-1,180+16,WHITE);	
	LCD_ShowString(30,180,210,16,16,tbuf);	
	sprintf((char*)tbuf,"X POS:%05d",x); 
	LCD_ShowString(30,200,200,16,16,tbuf);	
	sprintf((char*)tbuf,"Y POS:%05d",y);
	LCD_ShowString(30,220,200,16,16,tbuf);	
	sprintf((char*)tbuf,"Z POS:%05d",z);
	LCD_ShowString(30,240,200,16,16,tbuf);	 
	//printf("btn,X,Y,Z:0x%x,%d,%d,%d\r\n",data->button,(signed char)data->x,(signed char)data->y,(signed char)data->z);   
} 


//USB�������ݴ���
//data:USB������ݽṹ��ָ��
void  MYUSR_KEYBRD_ProcessData(uint8_t data)
{ 
	static u16 pos; 
	static u16 endx,endy;
	static u16 maxinputchar;
	
	u8 buf[4];
	if(USB_FIRST_PLUGIN_FLAG)//��һ�β���,����������
	{
		USB_FIRST_PLUGIN_FLAG=0;
		endx=((lcddev.width-30)/8)*8+30;		//�õ�endxֵ
		endy=((lcddev.height-220)/16)*16+220;	//�õ�endyֵ
		maxinputchar=((lcddev.width-30)/8);
		maxinputchar*=(lcddev.height-220)/16;	//��ǰLCD��������ʾ���ַ���.
 		pos=0; 
	}
	POINT_COLOR=BLUE;
	sprintf((char*)buf,"%02X",data);
	LCD_ShowString(30+56,180,200,16,16,buf);//��ʾ��ֵ	 
	if(data>=' '&&data<='~')
	{
		tbuf[pos++]=data;
		tbuf[pos]=0;		//��ӽ�����. 
		if(pos>maxinputchar)pos=maxinputchar;//���������ô��
	}else if(data==0X0D)	//�˸��
	{
		if(pos)pos--;
		tbuf[pos]=0;		//��ӽ�����.  
	}
	if(pos<=maxinputchar)	//û�г�����ʾ��
	{
		LCD_Fill(30,220,endx,endy,WHITE);
		LCD_ShowString(30,220,endx-30,endy-220,16,tbuf);
	}		
	//printf("KEY Board Value:%02X\r\n",data);
	//printf("KEY Board Char:%c\r\n",data); 
}


//USB����/�����ʾdemo���ݴ���
//phost:USBH���ָ��
void USB_Demo(USBH_HandleTypeDef * phost)
{
    char c;
    HID_KEYBD_Info_TypeDef *k_pinfo;
    HID_MOUSE_Info_TypeDef *m_pinfo; 
	if(App_State==APPLICATION_READY)
	{
		if(USBH_HID_GetDeviceType(phost)==HID_KEYBOARD)	//�����豸
		{ 
			k_pinfo=USBH_HID_GetKeybdInfo(phost);		//��ȡ������Ϣ
			if(k_pinfo!=NULL)
			{		
				c=USBH_HID_GetASCIICode(k_pinfo);		//ת����ASCII��
				MYUSR_KEYBRD_ProcessData(c);			//��LCD����ʾ�������ַ�
			} 
		}else if (USBH_HID_GetDeviceType(phost)==HID_MOUSE)	//����豸
		{ 
			m_pinfo=USBH_HID_GetMouseInfo(phost);		//��ȡ�����Ϣ
			if(m_pinfo!=NULL)
			{
				MYUSR_MOUSE_ProcessData(&mouse_info);	//LCD����ʾ�����Ϣ
			} 
		}
	}	 
}

int main(void)
{  
 	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200   
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	LCD_Init();						//��ʼ��LCD 
    PCF8574_Init();			        //��ʼ��PCF8574 
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��(AXI)
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��(SDRAM)
	my_mem_init(SRAM12);			//��ʼ��SRAM12�ڴ��(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//��ʼ��SRAM4�ڴ��(SRAM4)
	my_mem_init(SRAMDTCM);			//��ʼ��DTCM�ڴ��(DTCM)
	my_mem_init(SRAMITCM);			//��ʼ��ITCM�ڴ��(ITCM)
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");	
	LCD_ShowString(30,70,200,16,16,"USB MOUSE/KEYBOARD TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/9/6");	 
	LCD_ShowString(30,130,200,16,16,"USB Connecting...");	
    
    USBH_Init(&hUSBHost, USBH_UserProcess, 0);
    USBH_RegisterClass(&hUSBHost,USBH_HID_CLASS);
    USBH_Start(&hUSBHost);
    while(1)
    {
        USBH_Process(&hUSBHost);
        USB_Demo(&hUSBHost);
    }	 
}























