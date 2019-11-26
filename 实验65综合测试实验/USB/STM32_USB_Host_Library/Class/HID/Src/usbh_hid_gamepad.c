#include "usbh_hid_gamepad.h"
#include "usbh_hid_parser.h"
#include "usart.h"


//FC游戏手柄模拟
//读取fcpad.ctrlval,即可得到当前手柄格式转换成FC手柄格式.
FC_GamePad_TypeDef fcpad;
FC_GamePad_TypeDef fcpad1;

u8	gamepad_report_data[HID_QUEUE_SIZE];	//鼠标上报数据长度,最多HID_QUEUE_SIZE个字节

 
//USBH游戏手柄初始化
//phost:USBH句柄指针
//返回值:USBH状态  
USBH_StatusTypeDef USBH_HID_GamePadInit(USBH_HandleTypeDef *phost) 
{ 
	HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData; 
	if(HID_Handle->length > sizeof(gamepad_report_data))
	{
		HID_Handle->length = sizeof(gamepad_report_data);
	}
	HID_Handle->pData = (uint8_t *)gamepad_report_data;
	fifo_init(&HID_Handle->fifo, phost->device.Data,HID_QUEUE_SIZE);
	return USBH_OK;
} 

//USBH获取鼠标信息
//phost:USBH句柄指针
//返回值:鼠标信息(HID_MOUSE_Info_TypeDef)
FC_GamePad_TypeDef *USBH_HID_GetGamePadInfo(USBH_HandleTypeDef *phost)
{
	u8 len=0;
	HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
	if(HID_Handle->length==0)return NULL;
	len=fifo_read(&HID_Handle->fifo, &gamepad_report_data, HID_Handle->length);
	if(len==HID_Handle->length)		//读取FIFO
	{
		USBH_HID_GamePad_Decode(gamepad_report_data); 
		return &fcpad;
	}else
	{
		return NULL; 
	}
}

//通用USB方向键解码,结果存放在fcpad里面
//data0,data1:USB得到的方向数据 
//数据格式如下:
//data0:00,左键按下;ff,右键按下,7F,没有按键按下
//data1:00,上键按下;ff,下键按下,7F,没有按键按下
void USBH_HID_GamePad_nDir_Decode(u8 data0,u8 data1)
{
	switch(data0)
	{
		case 0X7F:
			fcpad.b.left=0;
			fcpad.b.right=0;
			break;
		case 0X00:
			fcpad.b.left=1; 
			break;
		case 0XFF:
			fcpad.b.right=1; 
			break;
	}
	switch(data1)
	{
		case 0X7F:
			fcpad.b.up=0;
			fcpad.b.down=0;
			break;
		case 0X00:
			fcpad.b.up=1; 
			break;
		case 0XFF:
			fcpad.b.down=1; 
			break;
	}
}
//通用USB AB键解码,结果存放在fcpad里面
//data:USB得到的1/2/3/4键数据 
//数据格式如下:
//data:最高4位有效  
//b4=1,1键按下(FC:B键连按)
//b5=1,2键按下(FC:A键连按)
//b6=1,3键按下(FC:A键)
//b7=1,4键按下(FC:B键)
void USBH_HID_GamePad_nAB_Decode(u8 data)
{
	if(data&0X10)fcpad.b.b=!fcpad.b.b;	//B键取反
	else
	{
		if(data&0X80)fcpad.b.b=1;		//B键按下
		else fcpad.b.b=0;				//B键松开  
	}
	if(data&0X20)fcpad.b.a=!fcpad.b.a;	//A键取反 
	else
	{
		if(data&0X40)fcpad.b.a=1;		//A键按下
		else fcpad.b.a=0;				//A键松开
	}	
}
//通用USB 功能键解码,结果存放在fcpad里面
//data:USB得到的Select/Start键数据 
//数据格式如下:
//data:b4,b5有效.
//b4=1,Select键按下
//b5=1,Start键按下 
void USBH_HID_GamePad_nFun_Decode(u8 data)
{
	if(data&0X10)fcpad.b.select=1;	//Select键按下
	else fcpad.b.select=0;			//Select键松开
	if(data&0X20)fcpad.b.start=1;	//Start键按下
	else fcpad.b.start=0;			//Start键松开 
} 

//game pad数据解析
void USBH_HID_GamePad_Decode(u8 *data)
{   
	//u8 i; 
	if(data[2]==0X7F)				//无摇杆手柄手柄,第三个字节为0X7F
	{
		USBH_HID_GamePad_nDir_Decode(data[3],data[4]);	//解码方向键
		USBH_HID_GamePad_nAB_Decode(data[5]);			//解码AB键
		USBH_HID_GamePad_nFun_Decode(data[6]);			//解码功能键
//		USR_GAMEPAD_ProcessData(fcpad.ctrlval);			//显示结果
	}else							//有摇杆的手柄
	{
		USBH_HID_GamePad_nDir_Decode(data[0],data[1]);	//解码方向键
		USBH_HID_GamePad_nAB_Decode(data[5]);			//解码AB键
		USBH_HID_GamePad_nFun_Decode(data[6]);			//解码功能键
//		USR_GAMEPAD_ProcessData(fcpad.ctrlval);			//显示结果		
	}
	//for(i=0;i<8;i++)printf("%02x ",data[i]);
	//printf("\r\n");
} 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u8* const JOYPAD_SYMBOL_TBL[8]=
{"Right","Left","Down","Up","Start","Select","A","B"};//手柄按键符号定义
//USB键盘数据处理
//data:USB鼠标数据结构体指针
void  USR_GAMEPAD_ProcessData(uint8_t data)
{
	u8 i;
 	if(data)
	{ 
		for(i=0;i<8;i++)
		{
			if(data&(0X80>>i))
			{
				printf("key:%s\r\n",(u8*)JOYPAD_SYMBOL_TBL[i]);
 			}		
		}		    
	}
}



















