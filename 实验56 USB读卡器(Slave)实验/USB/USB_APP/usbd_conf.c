#include "usbd_conf.h" 
#include "usbd_core.h" 
#include "usbd_def.h" 
#include "stm32h7xx_hal_pcd.h"
#include "usart.h" 
#include "pcf8574.h"
#include "delay.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//usbd_conf 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/23
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
//PCD定义
PCD_HandleTypeDef hpcd;

//表示USB连接状态
//0,没有连接;
//1,已经连接;
vu8 bDeviceState=0;		//默认没有连接 


//初始化HSI48和CRS校准,选择HSI48作为USB时钟源(带CRS自动校准)
void USB_OTG_HSI48_CRS_Init(void)
{ 
	RCC->CR|=1<<12;				//HSI480N=1,使能HSI48
	while((RCC->CR&(1<<13))==0);//等待HSI48RDY=1,等待HSI48稳定
	RCC->APB1HENR|=1<<1;		//CRSEN=1,使能CRS
	RCC->APB1HRSTR|=1<<1;		//CRSRST=1,复位CRS
	RCC->APB1HRSTR&=~(1<<1);	//CRSRST=0,取消复位 
	CRS->CFGR&=~(3<<28);		//SYNCSRC[1:0]=0,选择USB2 SOF作为SYNC信号
	CRS->CR|=3<<5;				//CEN和AUTIOTRIMEN都为1,使能自动校准以及计数器 
	RCC->D2CCIP2R&=~(3<<20);	//USBSEL[1:0]=0,清零原来的设置.
	RCC->D2CCIP2R|=3<<20;		//USBSEL[1:0]=3,USB时钟源来自hsi48_ck.
}
   
//初始化PCD MSP
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_MspInit(PCD_HandleTypeDef * hpcd)
{ 
	if(hpcd->Instance==USB2_OTG_FS)
	{
		RCC->AHB4ENR|=1<<0;    				//使能PORTA时钟	     
		RCC->AHB1ENR|=1<<27;    			//使能USB2 OTG时钟	
		PWR->CR3|=1<<24;					//使能USB VDD3电压检测
		USB_OTG_HSI48_CRS_Init();			//设置USB时钟来自hsi48_ck,使能CRS	
		GPIO_Set(GPIOA,3<<11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_NONE);	//PA11/12复用功能输出	
 		PCF8574_WriteBit(USB_PWR_IO,1);		//开启USB HOST电源供电
		GPIO_AF_Set(GPIOA,11,10);			//PA11,AF10(USB)
		GPIO_AF_Set(GPIOA,12,10);			//PA12,AF10(USB)  
		MY_NVIC_Init(0,3,OTG_FS_IRQn,2);	//优先级设置为抢占0,子优先级3，组2	
	}else if (hpcd->Instance == USB1_OTG_HS)
	{
		//USB1 OTG本例程没用到,故不做处理
	}
} 

//USB OTG 中断服务函数
//处理所有USB中断
void OTG_FS_IRQHandler(void)
{
	HAL_PCD_IRQHandler(&hpcd);	
}

///////////////////////USBD LL PCD驱动回调函数(PCD->USB Device Library)///////////////////////
 
//USBD配置阶段回调函数
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef * hpcd)
{
	USBD_LL_SetupStage(hpcd->pData, (uint8_t *) hpcd->Setup);
}
 
//USBD OUT阶段回调函数
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
	USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

//USBD IN阶段回调函数
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
	USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

//USBD SOF回调函数
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_SOFCallback(PCD_HandleTypeDef * hpcd)
{
	USBD_LL_SOF(hpcd->pData);
}

//USBD 复位回调函数
//hpcd:PCD结构体指针
//返回值:无
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
	USBD_LL_Reset(hpcd->pData); 			//复位设备
	USBD_LL_SetSpeed(hpcd->pData, speed);
} 

//USBD 挂起回调函数
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef * hpcd)
{
	bDeviceState=0;
	printf("Device In suspend mode.\r\n");
	USBD_LL_Suspend(hpcd->pData);
}

//USBD 恢复回调函数
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef * hpcd)
{
	printf("Device Resumed\r\n");
	USBD_LL_Resume(hpcd->pData);
}
 
//USBD ISO OUT事务完成回调函数
//hpcd:PCD结构体指针
//epnum:端点号
//返回值:无
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
	USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

//USBD ISO IN事务完成回调函数
//hpcd:PCD结构体指针
//epnum:端点号
//返回值:无
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{ 
	USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

//USBD 连接成功回调函数
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef * hpcd)
{
	bDeviceState=1; 
	USBD_LL_DevConnected(hpcd->pData);
}

//USBD 断开连接回调函数
//hpcd:PCD结构体指针
//返回值:无
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef * hpcd)
{
	bDeviceState=0;
	printf("USB Device Disconnected.\r\n");
	USBD_LL_DevDisconnected(hpcd->pData);
}

/////////////////////USBD LL 驱动接口函数(USB Device Library --> PCD)/////////////////////
 
//USBD底层初始化函数
//pdev:USBD句柄指针
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef * pdev)
{
#ifdef USE_USB_FS	//针对USB FS,执行FS的初始化
	//设置LL驱动相关参数
	hpcd.Instance = USB2_OTG_FS;			//使用USB2 OTG
	hpcd.Init.dev_endpoints = 8;			//端点数为8
	hpcd.Init.use_dedicated_ep1 = 0;		//禁止EP1 dedicated中断
	hpcd.Init.ep0_mps = 0x40;				//设置端点0的最大包大小为0X40(64字节)
	hpcd.Init.low_power_enable = 0;			//不使能低功耗模式
	hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;//使用内部PHY
	hpcd.Init.Sof_enable = 0;				//使能SOF中断
	hpcd.Init.speed = PCD_SPEED_FULL;		//USB全速(12Mbps)	
	hpcd.Init.vbus_sensing_enable = 0;		//不使能VBUS检测
	hpcd.Init.lpm_enable = 0;				//使能连接电源管理
 
	hpcd.pData = pdev;						//hpcd的pData指向pdev
	pdev->pData = &hpcd;					//pdev的pData指向hpcd
 
	HAL_PCD_Init(&hpcd);					//初始化LL驱动

	HAL_PCDEx_SetRxFiFo(&hpcd, 0x80);		//设置接收FIFO大小为0X80(128字节)
	HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x40);	//设置发送FIFO 0的大小为0X40(64字节)
	HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x80);	//设置发送FIFO 1的大小为0X80(128字节)

#endif

#ifdef USE_USB_HS	//针对USB HS,执行HS的初始化
	//未实现
#endif 
  return USBD_OK;
} 

//USBD底层取消初始化(回复默认复位状态)函数 
//pdev:USBD句柄指针
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef * pdev)
{
	HAL_PCD_DeInit(pdev->pData);
	return USBD_OK;
}
 
//USBD底层驱动开始工作
//pdev:USBD句柄指针
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef * pdev)
{
	HAL_PCD_Start(pdev->pData);
	return USBD_OK;
}

//USBD底层驱动停止工作
//pdev:USBD句柄指针
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef * pdev)
{
	HAL_PCD_Stop(pdev->pData);
	return USBD_OK;
}

//USBD初始化(打开)某个端点
//pdev:USBD句柄指针
//ep_addr:端点号
//ep_mps:端点最大包容量(字节)
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef * pdev,uint8_t ep_addr,uint8_t ep_type, uint16_t ep_mps)
{
	HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);
	return USBD_OK;
}

//USBD取消初始化(关闭)某个端点
//pdev:USBD句柄指针
//ep_addr:端点号 
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	HAL_PCD_EP_Close(pdev->pData, ep_addr);
	return USBD_OK;
} 

//USBD清空某个端点的数据
//pdev:USBD句柄指针
//ep_addr:端点号 
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	HAL_PCD_EP_Flush(pdev->pData, ep_addr);
	return USBD_OK;
}

//USBD给某个端点设置一个暂停状态
//pdev:USBD句柄指针
//ep_addr:端点号 
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
	return USBD_OK;
}

//USBD取消某个端点的暂停状态
//pdev:USBD句柄指针
//ep_addr:端点号 
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef * pdev,
                                        uint8_t ep_addr)
{
	HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
	return USBD_OK;
}

//USBD返回是否处于暂停状态
//pdev:USBD句柄指针
//ep_addr:端点号 
//返回值:0,非暂停; 1,暂停.
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

//USBD为设备指定新的USB地址
//pdev:USBD句柄指针
//dev_addr:新的设备地址,USB1_OTG_HS/USB2_OTG_HS
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef * pdev,uint8_t dev_addr)
{
	bDeviceState=1;	//能执行到该函数,说明USB连接成功了 
	HAL_PCD_SetAddress(pdev->pData, dev_addr);
	return USBD_OK;
}
 
//USBD通过端点发送数据
//pdev:USBD句柄指针
//ep_addr:端点号 
//pbuf:数据缓冲区首地址
//size:要发送的数据大小
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef * pdev,uint8_t ep_addr,uint8_t * pbuf, uint16_t size)
{
	  HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
	  return USBD_OK;
}
 
//USBD准备一个端点接收数据
//pdev:USBD句柄指针
//ep_addr:端点号 
//pbuf:数据缓冲区首地址
//size:要接收的数据大小
//返回值:0,正常
//      1,忙
//      2,失败
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef * pdev,uint8_t ep_addr,uint8_t * pbuf, uint16_t size)
{
	  HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
	  return USBD_OK;
} 

//USBD获取最后一个传输包的大小
//pdev:USBD句柄指针
//ep_addr:端点号  
//返回值:包大小
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
	return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

//USBD 延时函数(以ms为单位)
//Delay:延时的ms数.
void USBD_LL_Delay(uint32_t Delay)
{
	delay_ms(Delay);
}



























