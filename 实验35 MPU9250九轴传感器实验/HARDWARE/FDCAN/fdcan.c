#include "fdcan.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//FDCAN驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/7/15
//版本：V1.0 
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
  
  
FDCAN_HandleTypeDef FDCAN1_Handler;
FDCAN_RxHeaderTypeDef FDCAN1_RxHeader;
FDCAN_TxHeaderTypeDef FDCAN1_TxHeader;


//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:2~128;
//tseg1:时间段1的时间单元.范围:2~256;
//tseg2:时间段2的时间单元.范围:2~128;
//tnbrp :波特率分频器.范围:1~512;
//mode:0,普通模式;1,受限模式;2,总线监控模式;3,内部环回模式;4,外部回环模式.
//mode的几个定义在stm32h7xx_hal_fdcan.h里面有对应的宏定义

//设置CAN时钟源为:pll1_q_ck,一般为200Mhz
//设置CAN分频为:FDCAN_CLOCK_DIV4
//得到:fdcan_tq_ck=50Mhz(须≤APB1频率)
//波特率=fdcan_tq_ck/((tseg1+tseg2+1)*tnbrp);
//如设置:FDCAN1_Mode_Init(8,8,11,5,3)
//即可得到500Kbps的FDCAN波特率

//返回值:0,初始化OK;
//    其他,初始化失败; 
u8 FDCAN1_Mode_Init(u8 tnsjw,u16 tseg1,u8 tseg2,u16 tnbrp,u8 mode)
{
  	FDCAN_ClkCalUnitTypeDef FDCAN1_ClkCalUnit;
	FDCAN_FilterTypeDef FDCAN1_RXFilter;
        
    //初始化FDCAN1
    HAL_FDCAN_DeInit(&FDCAN1_Handler);                              //先清除以前的设置
    FDCAN1_Handler.Instance=FDCAN1;
    FDCAN1_Handler.Init.FrameFormat=FDCAN_FRAME_CLASSIC;            //传统模式
    FDCAN1_Handler.Init.Mode=mode;                                  //模式设置
    FDCAN1_Handler.Init.AutoRetransmission=DISABLE;                 //关闭自动重传！传统模式下一定要关闭！！！
    FDCAN1_Handler.Init.TransmitPause=DISABLE;                      //关闭传输暂停
    FDCAN1_Handler.Init.ProtocolException=DISABLE;                  //关闭协议异常处理
    FDCAN1_Handler.Init.NominalPrescaler=tnbrp;                     //分频系数
    FDCAN1_Handler.Init.NominalSyncJumpWidth=tnsjw;                 //重新同步跳跃宽度范围:2~128
    FDCAN1_Handler.Init.NominalTimeSeg1=tseg1;                      //tseg1范围:2~256
    FDCAN1_Handler.Init.NominalTimeSeg2=tseg2;                      //tseg2范围:2~128
    FDCAN1_Handler.Init.MessageRAMOffset=0;                         //信息RAM偏移
    FDCAN1_Handler.Init.StdFiltersNbr=0;                            //标准信息ID滤波器编号
    FDCAN1_Handler.Init.ExtFiltersNbr=0;                            //扩展信息ID滤波器编号
    FDCAN1_Handler.Init.RxFifo0ElmtsNbr=1;                          //接收FIFO0元素编号
    FDCAN1_Handler.Init.RxFifo0ElmtSize=FDCAN_DATA_BYTES_8;         //接收FIFO0元素大小：8字节
    FDCAN1_Handler.Init.RxBuffersNbr=0;                             //接收缓冲编号
    FDCAN1_Handler.Init.TxEventsNbr=0;                              //发送事件编号
    FDCAN1_Handler.Init.TxBuffersNbr=0;                             //发送缓冲编号
    FDCAN1_Handler.Init.TxFifoQueueElmtsNbr=1;                      //发送FIFO序列元素编号
    FDCAN1_Handler.Init.TxFifoQueueMode=FDCAN_TX_FIFO_OPERATION;    //发送FIFO序列模式
    FDCAN1_Handler.Init.TxElmtSize=FDCAN_DATA_BYTES_8;              //发送大小:8字节
    if(HAL_FDCAN_Init(&FDCAN1_Handler)!=HAL_OK) return 1;           //初始化FDCAN
	
  	FDCAN1_ClkCalUnit.ClockCalibration=DISABLE;						//不使用时钟校准功能
	FDCAN1_ClkCalUnit.ClockDivider=FDCAN_CLOCK_DIV4;				//4分频,得到50Mhz的fdca_tq_ck
	HAL_FDCAN_ConfigClockCalibration(&FDCAN1_Handler,&FDCAN1_ClkCalUnit);	//设置分频

    //配置RX滤波器   
    FDCAN1_RXFilter.IdType=FDCAN_STANDARD_ID;                       //标准ID
    FDCAN1_RXFilter.FilterIndex=0;                                  //滤波器索引                   
    FDCAN1_RXFilter.FilterType=FDCAN_FILTER_MASK;                   //滤波器类型
    FDCAN1_RXFilter.FilterConfig=FDCAN_FILTER_TO_RXFIFO0;           //过滤器0关联到FIFO0  
    FDCAN1_RXFilter.FilterID1=0x0000;                               //32位ID
    FDCAN1_RXFilter.FilterID2=0x0000;                               //如果FDCAN配置为传统模式的话，这里是32位掩码
    if(HAL_FDCAN_ConfigFilter(&FDCAN1_Handler,&FDCAN1_RXFilter)!=HAL_OK) return 2;//滤波器初始化
    HAL_FDCAN_Start(&FDCAN1_Handler);                               //开启FDCAN
    HAL_FDCAN_ActivateNotification(&FDCAN1_Handler,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0);
    return 0;
}

//FDCAN1底层驱动，引脚配置，时钟使能
//HAL_FDCAN_Init()调用
//hsdram:FDCAN1句柄
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* hfdcan)
{

	RCC->AHB4ENR|=1<<0;  		//使能PORTA口时钟 
	RCC->APB1HENR|=1<<8;		//使能FDCAN时钟
	GPIO_Set(GPIOA,PIN11|PIN12,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PA11,PA12,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,11,9);	//PA11,AF9
	GPIO_AF_Set(GPIOA,12,9);	//PA12,AF9 	   
 
	RCC->D2CCIP1R&=(u32)3<<28;	//FDCANSEL[1:0]清零
	RCC->D2CCIP1R|=(u32)1<<28;	//FDCANSEL[1:0]=1,选择pll1_q_ck作为FDCAN时钟源,一般为200Mhz
	
#if FDCAN1_RX0_INT_ENABLE     
	MY_NVIC_Init(1,0,FDCAN1_IT0_IRQn,2);//组2
#endif	
}

//此函数会被HAL_FDCAN_DeInit调用
//hfdcan:fdcan句柄
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* hfdcan)
{
	RCC->APB1HRSTR|=1<<8;		//FDCANRST=1,复位FDCAN
	delay_ms(10);				//延迟
	RCC->APB1HRSTR&=~(1<<8);	//FDCANRST=0,结束复位 
}

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8),可设置为FDCAN_DLC_BYTES_2~FDCAN_DLC_BYTES_8				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 FDCAN1_Send_Msg(u8* msg,u32 len)
{	
    FDCAN1_TxHeader.Identifier=0x12;                           //32位ID
    FDCAN1_TxHeader.IdType=FDCAN_STANDARD_ID;                  //标准ID
    FDCAN1_TxHeader.TxFrameType=FDCAN_DATA_FRAME;              //数据帧
    FDCAN1_TxHeader.DataLength=len;                            //数据长度
    FDCAN1_TxHeader.ErrorStateIndicator=FDCAN_ESI_ACTIVE;            
    FDCAN1_TxHeader.BitRateSwitch=FDCAN_BRS_OFF;               //关闭速率切换
    FDCAN1_TxHeader.FDFormat=FDCAN_CLASSIC_CAN;                //传统的CAN模式
    FDCAN1_TxHeader.TxEventFifoControl=FDCAN_NO_TX_EVENTS;     //无发送事件
    FDCAN1_TxHeader.MessageMarker=0;                           
    
    if(HAL_FDCAN_AddMessageToTxFifoQ(&FDCAN1_Handler,&FDCAN1_TxHeader,msg)!=HAL_OK) return 1;//发送
    return 0;	
}

//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 FDCAN1_Receive_Msg(u8 *buf)
{	
    if(HAL_FDCAN_GetRxMessage(&FDCAN1_Handler,FDCAN_RX_FIFO0,&FDCAN1_RxHeader,buf)!=HAL_OK)return 0;//接收数据
	return FDCAN1_RxHeader.DataLength>>16;	
}

#if FDCAN1_RX0_INT_ENABLE  
//FDCAN1中断服务函数
void FDCAN1_IT0_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&FDCAN1_Handler);
}

//FIFO0回调函数
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    u8 i=0;
    u8 rxdata[8];
    if((RxFifo0ITs&FDCAN_IT_RX_FIFO0_NEW_MESSAGE)!=RESET)   //FIFO1新数据中断
    {
        //提取FIFO0中接收到的数据
        HAL_FDCAN_GetRxMessage(hfdcan,FDCAN_RX_FIFO0,&FDCAN1_RxHeader,rxdata);
        printf("id:%#x\r\n",FDCAN1_RxHeader.Identifier);
        printf("len:%d\r\n",FDCAN1_RxHeader.DataLength>>16);
        for(i=0;i<8;i++)
        printf("rxdata[%d]:%d\r\n",i,rxdata[i]);
        HAL_FDCAN_ActivateNotification(hfdcan,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0);
    }
}
#endif	














