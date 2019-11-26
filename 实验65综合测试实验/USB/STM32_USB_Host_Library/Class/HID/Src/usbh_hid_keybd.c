#include "usbh_hid_keybd.h"
#include "usbh_hid_parser.h"
  

HID_KEYBD_Info_TypeDef     keybd_info;
uint8_t	keybd_report_data[HID_QUEUE_SIZE];
 
 
//USBH���̳�ʼ��
//phost:USBH���ָ��
//����ֵ:USBH״̬ 
USBH_StatusTypeDef USBH_HID_KeybdInit(USBH_HandleTypeDef *phost)
{
	uint32_t x;
	HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData;   
	for(x=0; x< (sizeof(keybd_report_data)/sizeof(uint8_t)); x++)
	{
		keybd_report_data[x]=0;
	}

	if(HID_Handle->length > (sizeof(keybd_report_data)/sizeof(uint8_t)))
	{
		HID_Handle->length = (sizeof(keybd_report_data)/sizeof(uint8_t));
	}
	HID_Handle->pData = (uint8_t*)keybd_report_data;
	fifo_init(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE);
	return USBH_OK;    
} 
 
//USBH��ȡ������Ϣ
//phost:USBH���ָ��
//����ֵ:������Ϣ(HID_KEYBD_Info_TypeDef),�㶨����NULL,������δ�õ�
HID_KEYBD_Info_TypeDef *USBH_HID_GetKeybdInfo(USBH_HandleTypeDef *phost)
{
	u8 len=0;
	HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;
	if(HID_Handle->length==0)return NULL;
	len=fifo_read(&HID_Handle->fifo, &keybd_report_data, HID_Handle->length);
	if(len==HID_Handle->length)		//��ȡFIFO
	{
		KEYBRD_Decode(keybd_report_data); 
	} 
	return NULL;  
} 

//����Ԫ�ض�Ӧ��ϵ:A  A����  B  B���� Select Start  ��  ��   ��  ��
//                |   |     |   |     |       |    |   |   |   |
//          �ֱ�1:K   I     J   U    �ո�    �س�   W   S   A   D
//                |   |     |   |     |       |    |   |   |   |
//          �ֱ�2:3   6     2   5    �ո�    �س�  ��   ��  ��  ��

const u8 KEYBD_FCPAD1_TABLE[10]={0X0E,0X0C,0X0D,0X18,0X2C,0X28,0X1A,0X16,0X04,0X07};//ģ���ֱ�1
const u8 KEYBD_FCPAD2_TABLE[10]={0X5B,0X5E,0X5A,0X5D,0X2C,0X28,0X52,0X51,0X50,0X4F};//ģ���ֱ�2
//�������ݽ����FC�ֱ�����.
//fcbuf:������������
//mode:0,�����¼�����������;
//     1,���¼�����������
void KEYBRD_FCPAD_Decode(uint8_t *fcbuf,uint8_t mode)
{
	static u8 pbuf[6];		//�������һ��fc�ֱ���ֵ
	u8 j;
	u8 da1=0,db1=0;
	u8 da2=0,db2=0; 
	if(mode)
	{
		for(j=0;j<6;j++)pbuf[j]=fcbuf[j];//��������
	}
	for(j=0;j<6;j++)//���ж�A������?
	{ 
		if(KEYBD_FCPAD1_TABLE[1]==pbuf[j])
		{
			fcpad.b.a=!fcpad.b.a;	//�ֱ�1 A��ȡ��
			da1=1; 					//����ֱ�1 A������Ч
		} 
		if(KEYBD_FCPAD2_TABLE[1]==pbuf[j])
		{
			fcpad1.b.a=!fcpad1.b.a;	//�ֱ�2 A��ȡ��
			da2=1; 					//����ֱ�2 A������Ч
		}
		if(KEYBD_FCPAD1_TABLE[3]==pbuf[j])
		{
			fcpad.b.b=!fcpad.b.b;	//�ֱ�1 B��ȡ��
			db1=1; 					//����ֱ�1 B������Ч
		} 
		if(KEYBD_FCPAD2_TABLE[3]==pbuf[j])
		{
			fcpad1.b.b=!fcpad1.b.b;	//�ֱ�2 B��ȡ��
			db2=1; 					//����ֱ�2 B������Ч
		} 		
	}	
	if(da1==0)fcpad.b.a=0;
	if(da2==0)fcpad1.b.a=0;
	if(db1==0)fcpad.b.b=0;
	if(db2==0)fcpad1.b.b=0;
	fcpad.b.select=0;
	fcpad1.b.select=0;
 	fcpad.b.start=0;
	fcpad1.b.start=0;
 	fcpad.b.up=0;
	fcpad1.b.up=0;
 	fcpad.b.down=0;
	fcpad1.b.down=0;
 	fcpad.b.left=0;
	fcpad1.b.left=0;
 	fcpad.b.right=0;
	fcpad1.b.right=0; 
	for(j=0;j<6;j++)//���ж�A������?
	{ 
		if(da1==0)//�ֱ�1,A����û�а���
		{
			if(KEYBD_FCPAD1_TABLE[0]==pbuf[j])fcpad.b.a=1;	//�ֱ�1 A������ 
		}
		if(da2==0)//�ֱ�2,A����û�а���
		{
			if(KEYBD_FCPAD2_TABLE[0]==pbuf[j])fcpad1.b.a=1;	//�ֱ�2 A������ 
		}
		if(db1==0)//�ֱ�1,B����û�а���
		{
			if(KEYBD_FCPAD1_TABLE[2]==pbuf[j])fcpad.b.b=1;	//�ֱ�1 B������ 
		}
		if(db2==0)//�ֱ�2,B����û�а���
		{
			if(KEYBD_FCPAD2_TABLE[2]==pbuf[j])fcpad1.b.b=1;	//�ֱ�2 B������ 
		} 
		if(KEYBD_FCPAD1_TABLE[4]==pbuf[j])fcpad.b.select=1;	//�ֱ�1 select���� 
		if(KEYBD_FCPAD2_TABLE[4]==pbuf[j])fcpad1.b.select=1;//�ֱ�2 select���� 
		if(KEYBD_FCPAD1_TABLE[5]==pbuf[j])fcpad.b.start=1;	//�ֱ�1 start���� 
		if(KEYBD_FCPAD2_TABLE[5]==pbuf[j])fcpad1.b.start=1;	//�ֱ�2 start���� 		
		if(KEYBD_FCPAD1_TABLE[6]==pbuf[j])fcpad.b.up=1;		//�ֱ�1 up���� 
		if(KEYBD_FCPAD2_TABLE[6]==pbuf[j])fcpad1.b.up=1;	//�ֱ�2 up���� 
		if(KEYBD_FCPAD1_TABLE[7]==pbuf[j])fcpad.b.down=1;	//�ֱ�1 down���� 
		if(KEYBD_FCPAD2_TABLE[7]==pbuf[j])fcpad1.b.down=1;	//�ֱ�2 down���� 
		if(KEYBD_FCPAD1_TABLE[8]==pbuf[j])fcpad.b.left=1;	//�ֱ�1 left���� 
		if(KEYBD_FCPAD2_TABLE[8]==pbuf[j])fcpad1.b.left=1;	//�ֱ�2 left���� 
		if(KEYBD_FCPAD1_TABLE[9]==pbuf[j])fcpad.b.right=1;	//�ֱ�1 right���� 
		if(KEYBD_FCPAD2_TABLE[9]==pbuf[j])fcpad1.b.right=1;	//�ֱ�2 right���� 
	}	
//	printf("pad1:%x\r\n",fcpad);
//	printf("pad2:%x\r\n",fcpad1);
//	for(j=0;j<8;j++)
//	{
//		printf("%02x ",pbuf[j]);
//	}
//	printf("\r\n"); 
}

//�������ݽ���
void KEYBRD_Decode(uint8_t *pbuf)
{
	static u8 fcbuf[6];//�������һ��fc�ֱ���ֵ
	u8 i;
	pbuf+=2;
	for(i=0;i<6;i++)if(pbuf[i]!=fcbuf[i])break;//�����,˵�����µİ�������仯 
 	if(i==6)return;						//���ΰ���û���κα仯,ֱ�ӷ���
	for(i=0;i<6;i++)fcbuf[i]=pbuf[i];	//�����µİ���ֵ 
	KEYBRD_FCPAD_Decode(fcbuf,1);		//����FC�ֱ�����
}























