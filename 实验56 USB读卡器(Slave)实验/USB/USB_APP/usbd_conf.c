#include "usbd_conf.h" 
#include "usbd_core.h" 
#include "usbd_def.h" 
#include "stm32h7xx_hal_pcd.h"
#include "usart.h" 
#include "pcf8574.h"
#include "delay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//usbd_conf ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//PCD����
PCD_HandleTypeDef hpcd;

//��ʾUSB����״̬
//0,û������;
//1,�Ѿ�����;
vu8 bDeviceState=0;		//Ĭ��û������ 


//��ʼ��HSI48��CRSУ׼,ѡ��HSI48��ΪUSBʱ��Դ(��CRS�Զ�У׼)
void USB_OTG_HSI48_CRS_Init(void)
{ 
	RCC->CR|=1<<12;				//HSI480N=1,ʹ��HSI48
	while((RCC->CR&(1<<13))==0);//�ȴ�HSI48RDY=1,�ȴ�HSI48�ȶ�
	RCC->APB1HENR|=1<<1;		//CRSEN=1,ʹ��CRS
	RCC->APB1HRSTR|=1<<1;		//CRSRST=1,��λCRS
	RCC->APB1HRSTR&=~(1<<1);	//CRSRST=0,ȡ����λ 
	CRS->CFGR&=~(3<<28);		//SYNCSRC[1:0]=0,ѡ��USB2 SOF��ΪSYNC�ź�
	CRS->CR|=3<<5;				//CEN��AUTIOTRIMEN��Ϊ1,ʹ���Զ�У׼�Լ������� 
	RCC->D2CCIP2R&=~(3<<20);	//USBSEL[1:0]=0,����ԭ��������.
	RCC->D2CCIP2R|=3<<20;		//USBSEL[1:0]=3,USBʱ��Դ����hsi48_ck.
}
   
//��ʼ��PCD MSP
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_MspInit(PCD_HandleTypeDef * hpcd)
{ 
	if(hpcd->Instance==USB2_OTG_FS)
	{
		RCC->AHB4ENR|=1<<0;    				//ʹ��PORTAʱ��	     
		RCC->AHB1ENR|=1<<27;    			//ʹ��USB2 OTGʱ��	
		PWR->CR3|=1<<24;					//ʹ��USB VDD3��ѹ���
		USB_OTG_HSI48_CRS_Init();			//����USBʱ������hsi48_ck,ʹ��CRS	
		GPIO_Set(GPIOA,3<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_NONE);	//PA11/12���ù������	
 		PCF8574_WriteBit(USB_PWR_IO,1);		//����USB HOST��Դ����
		GPIO_AF_Set(GPIOA,11,10);			//PA11,AF10(USB)
		GPIO_AF_Set(GPIOA,12,10);			//PA12,AF10(USB)  
		MY_NVIC_Init(0,3,OTG_FS_IRQn,2);	//���ȼ�����Ϊ��ռ0,�����ȼ�3����2	
	}else if (hpcd->Instance == USB1_OTG_HS)
	{
		//USB1 OTG������û�õ�,�ʲ�������
	}
} 

//USB OTG �жϷ�����
//��������USB�ж�
void OTG_FS_IRQHandler(void)
{
	HAL_PCD_IRQHandler(&hpcd);	
}

///////////////////////USBD LL PCD�����ص�����(PCD->USB Device Library)///////////////////////
 
//USBD���ý׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef * hpcd)
{
	USBD_LL_SetupStage(hpcd->pData, (uint8_t *) hpcd->Setup);
}
 
//USBD OUT�׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
	USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

//USBD IN�׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
	USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

//USBD SOF�ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SOFCallback(PCD_HandleTypeDef * hpcd)
{
	USBD_LL_SOF(hpcd->pData);
}

//USBD ��λ�ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ResetCallback(PCD_HandleTypeDef * hpcd)
{
	USBD_SpeedTypeDef speed = USBD_SPEED_FULL;
	/* Set USB Current Speed */
	switch (hpcd->Init.speed)
	{
		case PCD_SPEED_HIGH:
			printf("USB Device Library  [HS]\r\n");
			speed = USBD_SPEED_HIGH;
			break;

		case PCD_SPEED_FULL:
			printf("USB Device Library  [FS]\r\n");
			speed = USBD_SPEED_FULL;
			break; 
		default:
			printf("USB Device Library  [FS?]\r\n");
			speed = USBD_SPEED_FULL;
			break;
	}  
	USBD_LL_Reset(hpcd->pData); 			//��λ�豸
	USBD_LL_SetSpeed(hpcd->pData, speed);
} 

//USBD ����ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef * hpcd)
{
	bDeviceState=0;
	printf("Device In suspend mode.\r\n");
	USBD_LL_Suspend(hpcd->pData);
}

//USBD �ָ��ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef * hpcd)
{
	printf("Device Resumed\r\n");
	USBD_LL_Resume(hpcd->pData);
}
 
//USBD ISO OUT������ɻص�����
//hpcd:PCD�ṹ��ָ��
//epnum:�˵��
//����ֵ:��
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
	USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

//USBD ISO IN������ɻص�����
//hpcd:PCD�ṹ��ָ��
//epnum:�˵��
//����ֵ:��
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{ 
	USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

//USBD ���ӳɹ��ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef * hpcd)
{
	bDeviceState=1; 
	USBD_LL_DevConnected(hpcd->pData);
}

//USBD �Ͽ����ӻص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef * hpcd)
{
	bDeviceState=0;
	printf("USB Device Disconnected.\r\n");
	USBD_LL_DevDisconnected(hpcd->pData);
}

/////////////////////USBD LL �����ӿں���(USB Device Library --> PCD)/////////////////////
 
//USBD�ײ��ʼ������
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef * pdev)
{
#ifdef USE_USB_FS	//���USB FS,ִ��FS�ĳ�ʼ��
	//����LL������ز���
	hpcd.Instance = USB2_OTG_FS;			//ʹ��USB2 OTG
	hpcd.Init.dev_endpoints = 8;			//�˵���Ϊ8
	hpcd.Init.use_dedicated_ep1 = 0;		//��ֹEP1 dedicated�ж�
	hpcd.Init.ep0_mps = 0x40;				//���ö˵�0��������СΪ0X40(64�ֽ�)
	hpcd.Init.low_power_enable = 0;			//��ʹ�ܵ͹���ģʽ
	hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;//ʹ���ڲ�PHY
	hpcd.Init.Sof_enable = 0;				//ʹ��SOF�ж�
	hpcd.Init.speed = PCD_SPEED_FULL;		//USBȫ��(12Mbps)	
	hpcd.Init.vbus_sensing_enable = 0;		//��ʹ��VBUS���
	hpcd.Init.lpm_enable = 0;				//ʹ�����ӵ�Դ����
 
	hpcd.pData = pdev;						//hpcd��pDataָ��pdev
	pdev->pData = &hpcd;					//pdev��pDataָ��hpcd
 
	HAL_PCD_Init(&hpcd);					//��ʼ��LL����

	HAL_PCDEx_SetRxFiFo(&hpcd, 0x80);		//���ý���FIFO��СΪ0X80(128�ֽ�)
	HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x40);	//���÷���FIFO 0�Ĵ�СΪ0X40(64�ֽ�)
	HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x80);	//���÷���FIFO 1�Ĵ�СΪ0X80(128�ֽ�)

#endif

#ifdef USE_USB_HS	//���USB HS,ִ��HS�ĳ�ʼ��
	//δʵ��
#endif 
  return USBD_OK;
} 

//USBD�ײ�ȡ����ʼ��(�ظ�Ĭ�ϸ�λ״̬)���� 
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef * pdev)
{
	HAL_PCD_DeInit(pdev->pData);
	return USBD_OK;
}
 
//USBD�ײ�������ʼ����
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef * pdev)
{
	HAL_PCD_Start(pdev->pData);
	return USBD_OK;
}

//USBD�ײ�����ֹͣ����
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef * pdev)
{
	HAL_PCD_Stop(pdev->pData);
	return USBD_OK;
}

//USBD��ʼ��(��)ĳ���˵�
//pdev:USBD���ָ��
//ep_addr:�˵��
//ep_mps:�˵���������(�ֽ�)
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef * pdev,uint8_t ep_addr,uint8_t ep_type, uint16_t ep_mps)
{
	HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);
	return USBD_OK;
}

//USBDȡ����ʼ��(�ر�)ĳ���˵�
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	HAL_PCD_EP_Close(pdev->pData, ep_addr);
	return USBD_OK;
} 

//USBD���ĳ���˵������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	HAL_PCD_EP_Flush(pdev->pData, ep_addr);
	return USBD_OK;
}

//USBD��ĳ���˵�����һ����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
	return USBD_OK;
}

//USBDȡ��ĳ���˵����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef * pdev,
                                        uint8_t ep_addr)
{
	HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
	return USBD_OK;
}

//USBD�����Ƿ�����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����ͣ; 1,��ͣ.
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	PCD_HandleTypeDef *hpcd = pdev->pData;
	if ((ep_addr & 0x80) == 0x80)
	{
		return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
	}
	else
	{
		return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
	}
}

//USBDΪ�豸ָ���µ�USB��ַ
//pdev:USBD���ָ��
//dev_addr:�µ��豸��ַ,USB1_OTG_HS/USB2_OTG_HS
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef * pdev,uint8_t dev_addr)
{
	bDeviceState=1;	//��ִ�е��ú���,˵��USB���ӳɹ��� 
	HAL_PCD_SetAddress(pdev->pData, dev_addr);
	return USBD_OK;
}
 
//USBDͨ���˵㷢������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//pbuf:���ݻ������׵�ַ
//size:Ҫ���͵����ݴ�С
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef * pdev,uint8_t ep_addr,uint8_t * pbuf, uint16_t size)
{
	  HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
	  return USBD_OK;
}
 
//USBD׼��һ���˵��������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//pbuf:���ݻ������׵�ַ
//size:Ҫ���յ����ݴ�С
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef * pdev,uint8_t ep_addr,uint8_t * pbuf, uint16_t size)
{
	  HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
	  return USBD_OK;
} 

//USBD��ȡ���һ��������Ĵ�С
//pdev:USBD���ָ��
//ep_addr:�˵��  
//����ֵ:����С
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

//USBD ��ʱ����(��msΪ��λ)
//Delay:��ʱ��ms��.
void USBD_LL_Delay(uint32_t Delay)
{
	delay_ms(Delay);
}



























