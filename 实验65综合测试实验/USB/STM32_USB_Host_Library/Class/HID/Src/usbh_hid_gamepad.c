#include "usbh_hid_gamepad.h"
#include "usbh_hid_parser.h"
#include "usart.h"


//FC��Ϸ�ֱ�ģ��
//��ȡfcpad.ctrlval,���ɵõ���ǰ�ֱ���ʽת����FC�ֱ���ʽ.
FC_GamePad_TypeDef fcpad;
FC_GamePad_TypeDef fcpad1;

u8	gamepad_report_data[HID_QUEUE_SIZE];	//����ϱ����ݳ���,���HID_QUEUE_SIZE���ֽ�

 
//USBH��Ϸ�ֱ���ʼ��
//phost:USBH���ָ��
//����ֵ:USBH״̬  
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

//USBH��ȡ�����Ϣ
//phost:USBH���ָ��
//����ֵ:�����Ϣ(HID_MOUSE_Info_TypeDef)
FC_GamePad_TypeDef *USBH_HID_GetGamePadInfo(USBH_HandleTypeDef *phost)
{
	u8 len=0;
	HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
	if(HID_Handle->length==0)return NULL;
	len=fifo_read(&HID_Handle->fifo, &gamepad_report_data, HID_Handle->length);
	if(len==HID_Handle->length)		//��ȡFIFO
	{
		USBH_HID_GamePad_Decode(gamepad_report_data); 
		return &fcpad;
	}else
	{
		return NULL; 
	}
}

//ͨ��USB���������,��������fcpad����
//data0,data1:USB�õ��ķ������� 
//���ݸ�ʽ����:
//data0:00,�������;ff,�Ҽ�����,7F,û�а�������
//data1:00,�ϼ�����;ff,�¼�����,7F,û�а�������
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
//ͨ��USB AB������,��������fcpad����
//data:USB�õ���1/2/3/4������ 
//���ݸ�ʽ����:
//data:���4λ��Ч  
//b4=1,1������(FC:B������)
//b5=1,2������(FC:A������)
//b6=1,3������(FC:A��)
//b7=1,4������(FC:B��)
void USBH_HID_GamePad_nAB_Decode(u8 data)
{
	if(data&0X10)fcpad.b.b=!fcpad.b.b;	//B��ȡ��
	else
	{
		if(data&0X80)fcpad.b.b=1;		//B������
		else fcpad.b.b=0;				//B���ɿ�  
	}
	if(data&0X20)fcpad.b.a=!fcpad.b.a;	//A��ȡ�� 
	else
	{
		if(data&0X40)fcpad.b.a=1;		//A������
		else fcpad.b.a=0;				//A���ɿ�
	}	
}
//ͨ��USB ���ܼ�����,��������fcpad����
//data:USB�õ���Select/Start������ 
//���ݸ�ʽ����:
//data:b4,b5��Ч.
//b4=1,Select������
//b5=1,Start������ 
void USBH_HID_GamePad_nFun_Decode(u8 data)
{
	if(data&0X10)fcpad.b.select=1;	//Select������
	else fcpad.b.select=0;			//Select���ɿ�
	if(data&0X20)fcpad.b.start=1;	//Start������
	else fcpad.b.start=0;			//Start���ɿ� 
} 

//game pad���ݽ���
void USBH_HID_GamePad_Decode(u8 *data)
{   
	//u8 i; 
	if(data[2]==0X7F)				//��ҡ���ֱ��ֱ�,�������ֽ�Ϊ0X7F
	{
		USBH_HID_GamePad_nDir_Decode(data[3],data[4]);	//���뷽���
		USBH_HID_GamePad_nAB_Decode(data[5]);			//����AB��
		USBH_HID_GamePad_nFun_Decode(data[6]);			//���빦�ܼ�
//		USR_GAMEPAD_ProcessData(fcpad.ctrlval);			//��ʾ���
	}else							//��ҡ�˵��ֱ�
	{
		USBH_HID_GamePad_nDir_Decode(data[0],data[1]);	//���뷽���
		USBH_HID_GamePad_nAB_Decode(data[5]);			//����AB��
		USBH_HID_GamePad_nFun_Decode(data[6]);			//���빦�ܼ�
//		USR_GAMEPAD_ProcessData(fcpad.ctrlval);			//��ʾ���		
	}
	//for(i=0;i<8;i++)printf("%02x ",data[i]);
	//printf("\r\n");
} 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u8* const JOYPAD_SYMBOL_TBL[8]=
{"Right","Left","Down","Up","Start","Select","A","B"};//�ֱ��������Ŷ���
//USB�������ݴ���
//data:USB������ݽṹ��ָ��
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



















