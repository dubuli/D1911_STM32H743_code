#include "lan8720.h"
#include "lwip_comm.h"
#include "pcf8574.h"
#include "delay.h"
#include "usart.h"
#include "mpu.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//LAN8720驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/10/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

ETH_HandleTypeDef LAN8720_ETHHandle;

//以太网描述符和缓冲区
__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT];      //以太网Rx DMA描述符,96字节,必须做内存保护,禁止CACHE
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT];		//以太网Tx DMA描述符,96字节,必须做内存保护,禁止CACHE
__attribute__((at(0x30040200))) u8 ETH_Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE];	//以太网接收缓冲区

 
//初始化LAN8720
int LAN8720_Init(void)
{         
    u8 macaddress[6];
    u32 timeout=0;
    u32 regval=0;
    u32 phylink=0;
    int status=LAN8720_STATUS_OK;
    //硬件复位
    PCF8574_WriteBit(ETH_RESET_IO,1);       //硬件复位
    delay_ms(100);
    PCF8574_WriteBit(ETH_RESET_IO,0);       //复位结束
    delay_ms(100);
	MPU_Set_Protection(0x30040000,256,8,MPU_REGION_FULL_ACCESS,0,0,1);//保护SRAM3,共32K字节,禁止共用,禁止cache,允许缓冲
  
	macaddress[0]=lwipdev.mac[0]; 
	macaddress[1]=lwipdev.mac[1]; 
	macaddress[2]=lwipdev.mac[2];
	macaddress[3]=lwipdev.mac[3];   
	macaddress[4]=lwipdev.mac[4];
	macaddress[5]=lwipdev.mac[5];	
    LAN8720_ETHHandle.Instance=ETH;                             //ETH
    LAN8720_ETHHandle.Init.MACAddr=macaddress;                  //mac地址
    LAN8720_ETHHandle.Init.MediaInterface=HAL_ETH_RMII_MODE;    //RMII接口
    LAN8720_ETHHandle.Init.RxDesc=DMARxDscrTab;                 //发送描述符
    LAN8720_ETHHandle.Init.TxDesc=DMATxDscrTab;                 //接收描述如
    LAN8720_ETHHandle.Init.RxBuffLen=ETH_MAX_PACKET_SIZE;       //接收长度
    HAL_ETH_Init(&LAN8720_ETHHandle);                           //初始化ETH
    HAL_ETH_SetMDIOClockRange(&LAN8720_ETHHandle);
    
    if(LAN8720_WritePHY(LAN8720_BCR,LAN8720_BCR_SOFT_RESET)>=0) //LAN8720软件复位
    {
		timeout=0;
		do
		{
			if(LAN8720_ReadPHY(LAN8720_BCR,&regval)<0)
			{
				status=LAN8720_STATUS_READ_ERROR;
				break;
			}
			delay_ms(10);
			timeout++;
			if(timeout>=LAN8720_TIMEOUT) break; 	//超时跳出,5S
		}while(regval&LAN8720_BCR_SOFT_RESET);		//等待复位结束
    }else status=LAN8720_STATUS_WRITE_ERROR;		//写入错误
    LAN8720_StartAutoNego();						//开启自动协商功能 
    if(status==LAN8720_STATUS_OK)delay_ms(1000);	//如果前面运行正常就延时1s        
    //等待网络连接成功
    timeout=0;
    while(LAN8720_GetLinkState()<=LAN8720_STATUS_LINK_DOWN)		//等待状态正常 
    {
        delay_ms(10);
        timeout++;
        if(timeout>=LAN8720_TIMEOUT) 				//超时跳出,5S
        {
            status=LAN8720_STATUS_LINK_DOWN;
            break; 				
        }
    }
    phylink=LAN8720_GetLinkState();					//获取LAN8720A连接状态
	switch(phylink)
	{
		case LAN8720_STATUS_100MBITS_FULLDUPLEX:
			printf("LAN8720:100Mb/s FullDuplex\r\n");
			break;
		case LAN8720_STATUS_100MBITS_HALFDUPLEX:
			printf("LAN8720:100Mb/s HalfDuplex\r\n");
			break;
		case LAN8720_STATUS_10MBITS_FULLDUPLEX:
			printf("LAN8720:10Mb/s FullDuplex\r\n");
			break;
		case LAN8720_STATUS_10MBITS_HALFDUPLEX:
			printf("LAN8720:10Mb/s HalfDuplex\r\n");
			break;
			
	} 
    return status; 
}

extern void lwip_pkt_handle(void);
    
//中断服务函数
void ETH_IRQHandler(void)
{
    lwip_pkt_handle();
    //清除中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&LAN8720_ETHHandle,ETH_DMA_NORMAL_IT);	//清除DMA中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&LAN8720_ETHHandle,ETH_DMA_RX_IT);		//清除DMA接收中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&LAN8720_ETHHandle,ETH_DMA_TX_IT);		//清除DMA接收中断标志位
} 

//ETH底层驱动，引脚配置，时钟使能
//此函数会被HAL_ETH_Init()调用
//heth:ETH句柄
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
	//使能以太网时钟
	RCC->AHB1ENR|=7<<15;		//使能ETH MAC/MAC_Tx/MAC_Rx时钟 
	//ETH IO接口初始化
 	RCC->AHB4ENR|=1<<0;     	//使能PORTA时钟 
 	RCC->AHB4ENR|=1<<1;     	//使能PORTB时钟 
 	RCC->AHB4ENR|=1<<2;     	//使能PORTC时钟  
	RCC->AHB4ENR|=1<<6;     	//使能PORTG时钟 
 	RCC->APB4ENR|=1<<1;   		//使能SYSCFG时钟
	SYSCFG->PMCR|=4<<21;		//使用RMII PHY接口. 
	GPIO_Set(GPIOA,PIN1|PIN2|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);		//PA1,2,7复用输出
	GPIO_Set(GPIOC,PIN1|PIN4|PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);		//PC1,4,5复用输出
	GPIO_Set(GPIOG,PIN13|PIN14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);		//PG13,14复用输出
	GPIO_Set(GPIOB,PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);				//PB11复用输出
 
	GPIO_AF_Set(GPIOA,1,11);	//PA1,AF11
 	GPIO_AF_Set(GPIOA,2,11);	//PA2,AF11
 	GPIO_AF_Set(GPIOA,7,11);	//PA7,AF11 
  	GPIO_AF_Set(GPIOB,11,11);	//PB11,AF11 
	GPIO_AF_Set(GPIOC,1,11);	//PC1,AF11
 	GPIO_AF_Set(GPIOC,4,11);	//PC4,AF11
 	GPIO_AF_Set(GPIOC,5,11);	//PC5,AF11 
  	GPIO_AF_Set(GPIOG,13,11);	//PG13,AF11
 	GPIO_AF_Set(GPIOG,14,11);	//PG14,AF11 
 	MY_NVIC_Init(0,0,ETH_IRQn,2);	//配置ETH中的分组   	
}

//读取PHY寄存器值
//reg:要读取的寄存器地址
//regval:读取到寄存器值(指针)
//返回值:0 读取成功;
//      -1 读取失败;
int LAN8720_ReadPHY(u16 reg,u32 *regval)
{
    if(HAL_ETH_ReadPHYRegister(&LAN8720_ETHHandle,LAN8720_ADDR,reg,regval))return -1;	//非0,读取失败,返回-1
    return 0;	//读取成功,返回0
}

//向LAN8720指定寄存器写入值
//reg:要写入的寄存器
//value:要写入的值
//返回值:0 读取成功;
//      -1 读取失败;
int LAN8720_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    if(HAL_ETH_WritePHYRegister(&LAN8720_ETHHandle,LAN8720_ADDR,reg,temp))return -1;	//非0,写入失败,返回-1
    return 0;	//写入成功,返回0
}

//打开LAN8720 Power Down模式 
void LAN8720_EnablePowerDownMode(void)
{
    u32 readval=0;
    LAN8720_ReadPHY(LAN8720_BCR,&readval);
    readval|=LAN8720_BCR_POWER_DOWN;
    LAN8720_WritePHY(LAN8720_BCR,readval);
}

//关闭LAN8720 Power Down模式
void LAN8720_DisablePowerDownMode(void)
{
    u32 readval=0;
    LAN8720_ReadPHY(LAN8720_BCR,&readval);
    readval&=~LAN8720_BCR_POWER_DOWN;
    LAN8720_WritePHY(LAN8720_BCR,readval);
}

//开启LAN8720的自协商功能
void LAN8720_StartAutoNego(void)
{
    u32 readval=0;
    LAN8720_ReadPHY(LAN8720_BCR,&readval);
    readval|=LAN8720_BCR_AUTONEGO_EN;
    LAN8720_WritePHY(LAN8720_BCR,readval);
}

//获取LAN8720的连接状态
//返回值：
//		LAN8720_STATUS_LINK_DOWN				连接断开
//		LAN8720_STATUS_AUTONEGO_NOTDONE			自动协商未完成
//		LAN8720_STATUS_100MBITS_FULLDUPLEX		100M全双工
//		LAN8720_STATUS_100MBITS_HALFDUPLEX		100M半双工
//		LAN8720_STATUS_10MBITS_FULLDUPLEX		10M全双工
//		LAN8720_STATUS_10MBITS_HALFDUPLEX		10M半双工
u32 LAN8720_GetLinkState(void)
{
    u32 readval=0; 
    //读取两遍，确保读取正确！！！
    LAN8720_ReadPHY(LAN8720_BSR,&readval);
    LAN8720_ReadPHY(LAN8720_BSR,&readval); 
    //获取连接状态(硬件，网线的连接，不是TCP、UDP等软件连接！)
    if((readval&LAN8720_BSR_LINK_STATUS)==0)return LAN8720_STATUS_LINK_DOWN; 
    //获取自动协商状态
    LAN8720_ReadPHY(LAN8720_BCR,&readval);
    if((readval&LAN8720_BCR_AUTONEGO_EN)!=LAN8720_BCR_AUTONEGO_EN)  //未使能自动协商
    {
		
        if(((readval&LAN8720_BCR_SPEED_SELECT)==LAN8720_BCR_SPEED_SELECT)&&
                ((readval&LAN8720_BCR_DUPLEX_MODE)==LAN8720_BCR_DUPLEX_MODE)) 
            return LAN8720_STATUS_100MBITS_FULLDUPLEX;
        else if((readval&LAN8720_BCR_SPEED_SELECT)==LAN8720_BCR_SPEED_SELECT)
            return LAN8720_STATUS_100MBITS_HALFDUPLEX;
        else if((readval&LAN8720_BCR_DUPLEX_MODE)==LAN8720_BCR_DUPLEX_MODE)
            return LAN8720_STATUS_10MBITS_FULLDUPLEX;
        else
            return LAN8720_STATUS_10MBITS_HALFDUPLEX;
    }else                                                            //使能了自动协商    
    {
        LAN8720_ReadPHY(LAN8720_PHYSCSR,&readval);
        if((readval&LAN8720_PHYSCSR_AUTONEGO_DONE)==0)
            return LAN8720_STATUS_AUTONEGO_NOTDONE;
        if((readval&LAN8720_PHYSCSR_HCDSPEEDMASK)==LAN8720_PHYSCSR_100BTX_FD)
            return LAN8720_STATUS_100MBITS_FULLDUPLEX;
        else if ((readval&LAN8720_PHYSCSR_HCDSPEEDMASK)==LAN8720_PHYSCSR_100BTX_HD)
            return LAN8720_STATUS_100MBITS_HALFDUPLEX;
        else if ((readval&LAN8720_PHYSCSR_HCDSPEEDMASK)==LAN8720_PHYSCSR_10BT_FD)
            return LAN8720_STATUS_10MBITS_FULLDUPLEX;
        else
            return LAN8720_STATUS_10MBITS_HALFDUPLEX;         
    }
}

//设置LAN8720的连接状态
//linkstate取值范围：
//		LAN8720_STATUS_100MBITS_FULLDUPLEX		100M全双工
//		LAN8720_STATUS_100MBITS_HALFDUPLEX		100M半双工
//		LAN8720_STATUS_10MBITS_FULLDUPLEX		10M全双工
//		LAN8720_STATUS_10MBITS_HALFDUPLEX		10M半双工
void LAN8720_SetLinkState(u32 linkstate)
{ 
    u32 bcrvalue=0;
    LAN8720_ReadPHY(LAN8720_BCR,&bcrvalue);
    //关闭连接配置，比如自动协商，速度和双工
    bcrvalue&=~(LAN8720_BCR_AUTONEGO_EN|LAN8720_BCR_SPEED_SELECT|LAN8720_BCR_DUPLEX_MODE);
    if(linkstate==LAN8720_STATUS_100MBITS_FULLDUPLEX)       //100M全双工
        bcrvalue|=(LAN8720_BCR_SPEED_SELECT|LAN8720_BCR_DUPLEX_MODE);
    else if(linkstate==LAN8720_STATUS_100MBITS_HALFDUPLEX)  //100M半双工
        bcrvalue|=LAN8720_BCR_SPEED_SELECT; 
    else if(linkstate==LAN8720_STATUS_10MBITS_FULLDUPLEX)   //10M全双工
        bcrvalue|=LAN8720_BCR_DUPLEX_MODE; 
    LAN8720_WritePHY(LAN8720_BCR,bcrvalue);					//配置寄存器,设置生效
}






