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
//ALIENTEK 阿波罗STM32H7开发板 实验60
//USB鼠标键盘(Host) 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  


USBH_HandleTypeDef hUSBHost;	//USB Host处理结构体

//应用状态结构体类型定义
typedef enum {	
  APPLICATION_IDLE = 0,			//挂起状态
  APPLICATION_DISCONNECT,		//断开连接
  APPLICATION_START,			//应用开始（连接上了）
  APPLICATION_READY,			//准备完成（可以执行相关应用代码了）
  APPLICATION_RUNNING,			//运行中...
}HID_ApplicationTypeDef;


extern HID_MOUSE_Info_TypeDef    mouse_info;
HID_ApplicationTypeDef App_State = APPLICATION_IDLE;

u8 USB_FIRST_PLUGIN_FLAG=0;	//USB第一次插入标志,如果为1,说明是第一次插入




//USB信息显示
//msgx:0,USB无连接
//     1,USB键盘
//     2,USB鼠标
//     3,不支持的USB设备
void USBH_Msg_Show(u8 msgx)
{
	POINT_COLOR=RED;
	switch(msgx)
	{
		case 0:	//USB无连接
			LCD_ShowString(30,130,200,16,16,"USB Connecting...");	
			LCD_Fill(0,150,lcddev.width,lcddev.height,WHITE);
			break;
		case 1:	//USB键盘
			LCD_ShowString(30,130,200,16,16,"USB Connected    ");	
			LCD_ShowString(30,150,200,16,16,"USB KeyBoard");	 
			LCD_ShowString(30,180,210,16,16,"KEYVAL:");	
			LCD_ShowString(30,200,210,16,16,"INPUT STRING:");	
			break;
		case 2:	//USB鼠标
			LCD_ShowString(30,130,200,16,16,"USB Connected    ");	
			LCD_ShowString(30,150,200,16,16,"USB Mouse");	 
			LCD_ShowString(30,180,210,16,16,"BUTTON:");	
			LCD_ShowString(30,200,210,16,16,"X POS:");	
			LCD_ShowString(30,220,210,16,16,"Y POS:");	
			LCD_ShowString(30,240,210,16,16,"Z POS:");	
			break; 		
		case 3:	//不支持的USB设备
			LCD_ShowString(30,130,200,16,16,"USB Connected    ");	
			LCD_ShowString(30,150,200,16,16,"Unknow Device");	 
			break; 	 
	} 
} 


//完成USBH不同用户状态下的数据处理
//phost:USBH句柄指针
//id:USBH当前的用户状态
void USBH_UserProcess(USBH_HandleTypeDef * phost, uint8_t id)
{
	u8 dev_type=0XFF;	//设备类型,1,键盘;2,鼠标;其他,不支持的设备.
    switch(id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        case HOST_USER_DISCONNECTION:
            USBH_Msg_Show(0);					//显示已经断开连接,准备重新连接
            App_State=APPLICATION_DISCONNECT;
            break;
        case HOST_USER_CLASS_ACTIVE: 	
            App_State=APPLICATION_READY;
			dev_type=phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bInterfaceProtocol;
			if(dev_type==HID_KEYBRD_BOOT_CODE)	//键盘设备
			{  
				USB_FIRST_PLUGIN_FLAG=1;//标记第一次插入
				USBH_Msg_Show(1);		//显示键盘界面
			}else if(dev_type==HID_MOUSE_BOOT_CODE)	//鼠标设备
			{ 
				USB_FIRST_PLUGIN_FLAG=1;//标记第一次插入
				USBH_Msg_Show(2);		//显示鼠标界面
			}else USBH_Msg_Show(3);		//显示不支持
            break;
        case HOST_USER_CONNECTION: 		
            App_State=APPLICATION_START;
            break;
        default:
            break;
    }
}

//零时数组,用于存放鼠标坐标/键盘输入内容(4.3屏,最大可以输入2016字节)
__align(4) u8 tbuf[2017]; 

//USB鼠标数据处理
//data:USB鼠标数据结构体指针
void MYUSR_MOUSE_ProcessData(HID_MOUSE_Info_TypeDef *data)
{  
	static signed short x,y,z; 
	if(USB_FIRST_PLUGIN_FLAG)//第一次插入,将数据清零
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


//USB键盘数据处理
//data:USB鼠标数据结构体指针
void  MYUSR_KEYBRD_ProcessData(uint8_t data)
{ 
	static u16 pos; 
	static u16 endx,endy;
	static u16 maxinputchar;
	
	u8 buf[4];
	if(USB_FIRST_PLUGIN_FLAG)//第一次插入,将数据清零
	{
		USB_FIRST_PLUGIN_FLAG=0;
		endx=((lcddev.width-30)/8)*8+30;		//得到endx值
		endy=((lcddev.height-220)/16)*16+220;	//得到endy值
		maxinputchar=((lcddev.width-30)/8);
		maxinputchar*=(lcddev.height-220)/16;	//当前LCD最大可以显示的字符数.
 		pos=0; 
	}
	POINT_COLOR=BLUE;
	sprintf((char*)buf,"%02X",data);
	LCD_ShowString(30+56,180,200,16,16,buf);//显示键值	 
	if(data>=' '&&data<='~')
	{
		tbuf[pos++]=data;
		tbuf[pos]=0;		//添加结束符. 
		if(pos>maxinputchar)pos=maxinputchar;//最大输入这么多
	}else if(data==0X0D)	//退格键
	{
		if(pos)pos--;
		tbuf[pos]=0;		//添加结束符.  
	}
	if(pos<=maxinputchar)	//没有超过显示区
	{
		LCD_Fill(30,220,endx,endy,WHITE);
		LCD_ShowString(30,220,endx-30,endy-220,16,tbuf);
	}		
	//printf("KEY Board Value:%02X\r\n",data);
	//printf("KEY Board Char:%c\r\n",data); 
}


//USB键盘/鼠标演示demo数据处理
//phost:USBH句柄指针
void USB_Demo(USBH_HandleTypeDef * phost)
{
    char c;
    HID_KEYBD_Info_TypeDef *k_pinfo;
    HID_MOUSE_Info_TypeDef *m_pinfo; 
	if(App_State==APPLICATION_READY)
	{
		if(USBH_HID_GetDeviceType(phost)==HID_KEYBOARD)	//键盘设备
		{ 
			k_pinfo=USBH_HID_GetKeybdInfo(phost);		//获取键盘信息
			if(k_pinfo!=NULL)
			{		
				c=USBH_HID_GetASCIICode(k_pinfo);		//转换成ASCII码
				MYUSR_KEYBRD_ProcessData(c);			//在LCD上显示出键盘字符
			} 
		}else if (USBH_HID_GetDeviceType(phost)==HID_MOUSE)	//鼠标设备
		{ 
			m_pinfo=USBH_HID_GetMouseInfo(phost);		//获取鼠标信息
			if(m_pinfo!=NULL)
			{
				MYUSR_MOUSE_ProcessData(&mouse_info);	//LCD上显示鼠标信息
			} 
		}
	}	 
}

int main(void)
{  
 	Stm32_Clock_Init(160,5,2,4);	//设置时钟,400Mhz
    delay_init(400);				//延时初始化  
	uart_init(100,115200);			//串口初始化为115200   
	LED_Init();						//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();		//保护相关存储区域
	SDRAM_Init();					//初始化SDRAM 
	LCD_Init();						//初始化LCD 
    PCF8574_Init();			        //初始化PCF8574 
	my_mem_init(SRAMIN);			//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);			//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);			//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);				//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);			//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);			//初始化ITCM内存池(ITCM)
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























