#include "lan8720.h"
#include "lwip_comm.h"
#include "pcf8574.h"
#include "delay.h"
#include "usart.h"
#include "mpu.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//LAN8720��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/10/30
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

ETH_HandleTypeDef LAN8720_ETHHandle;

//��̫���������ͻ�����
__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT];      //��̫��Rx DMA������,96�ֽ�,�������ڴ汣��,��ֹCACHE
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT];		//��̫��Tx DMA������,96�ֽ�,�������ڴ汣��,��ֹCACHE
__attribute__((at(0x30040200))) u8 ETH_Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE];	//��̫�����ջ�����

 
//��ʼ��LAN8720
int LAN8720_Init(void)
{         
    u8 macaddress[6];
    u32 timeout=0;
    u32 regval=0;
    u32 phylink=0;
    int status=LAN8720_STATUS_OK;
    //Ӳ����λ
    PCF8574_WriteBit(ETH_RESET_IO,1);       //Ӳ����λ
    delay_ms(100);
    PCF8574_WriteBit(ETH_RESET_IO,0);       //��λ����
    delay_ms(100);
	MPU_Set_Protection(0x30040000,256,8,MPU_REGION_FULL_ACCESS,0,0,1);//����SRAM3,��32K�ֽ�,��ֹ����,��ֹcache,������
  
	macaddress[0]=lwipdev.mac[0]; 
	macaddress[1]=lwipdev.mac[1]; 
	macaddress[2]=lwipdev.mac[2];
	macaddress[3]=lwipdev.mac[3];   
	macaddress[4]=lwipdev.mac[4];
	macaddress[5]=lwipdev.mac[5];	
    LAN8720_ETHHandle.Instance=ETH;                             //ETH
    LAN8720_ETHHandle.Init.MACAddr=macaddress;                  //mac��ַ
    LAN8720_ETHHandle.Init.MediaInterface=HAL_ETH_RMII_MODE;    //RMII�ӿ�
    LAN8720_ETHHandle.Init.RxDesc=DMARxDscrTab;                 //����������
    LAN8720_ETHHandle.Init.TxDesc=DMATxDscrTab;                 //����������
    LAN8720_ETHHandle.Init.RxBuffLen=ETH_MAX_PACKET_SIZE;       //���ճ���
    HAL_ETH_Init(&LAN8720_ETHHandle);                           //��ʼ��ETH
    HAL_ETH_SetMDIOClockRange(&LAN8720_ETHHandle);
    
    if(LAN8720_WritePHY(LAN8720_BCR,LAN8720_BCR_SOFT_RESET)>=0) //LAN8720�����λ
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
			if(timeout>=LAN8720_TIMEOUT) break; 	//��ʱ����,5S
		}while(regval&LAN8720_BCR_SOFT_RESET);		//�ȴ���λ����
    }else status=LAN8720_STATUS_WRITE_ERROR;		//д�����
    LAN8720_StartAutoNego();						//�����Զ�Э�̹��� 
    if(status==LAN8720_STATUS_OK)delay_ms(1000);	//���ǰ��������������ʱ1s        
    //�ȴ��������ӳɹ�
    timeout=0;
    while(LAN8720_GetLinkState()<=LAN8720_STATUS_LINK_DOWN)		//�ȴ�״̬���� 
    {
        delay_ms(10);
        timeout++;
        if(timeout>=LAN8720_TIMEOUT) 				//��ʱ����,5S
        {
            status=LAN8720_STATUS_LINK_DOWN;
            break; 				
        }
    }
    phylink=LAN8720_GetLinkState();					//��ȡLAN8720A����״̬
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
    
//�жϷ�����
void ETH_IRQHandler(void)
{
    lwip_pkt_handle();
    //����жϱ�־λ
    __HAL_ETH_DMA_CLEAR_IT(&LAN8720_ETHHandle,ETH_DMA_NORMAL_IT);	//���DMA�жϱ�־λ
    __HAL_ETH_DMA_CLEAR_IT(&LAN8720_ETHHandle,ETH_DMA_RX_IT);		//���DMA�����жϱ�־λ
    __HAL_ETH_DMA_CLEAR_IT(&LAN8720_ETHHandle,ETH_DMA_TX_IT);		//���DMA�����жϱ�־λ
} 

//ETH�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_ETH_Init()����
//heth:ETH���
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
	//ʹ����̫��ʱ��
	RCC->AHB1ENR|=7<<15;		//ʹ��ETH MAC/MAC_Tx/MAC_Rxʱ�� 
	//ETH IO�ӿڳ�ʼ��
 	RCC->AHB4ENR|=1<<0;     	//ʹ��PORTAʱ�� 
 	RCC->AHB4ENR|=1<<1;     	//ʹ��PORTBʱ�� 
 	RCC->AHB4ENR|=1<<2;     	//ʹ��PORTCʱ��  
	RCC->AHB4ENR|=1<<6;     	//ʹ��PORTGʱ�� 
 	RCC->APB4ENR|=1<<1;   		//ʹ��SYSCFGʱ��
	SYSCFG->PMCR|=4<<21;		//ʹ��RMII PHY�ӿ�. 
	GPIO_Set(GPIOA,PIN1|PIN2|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);		//PA1,2,7�������
	GPIO_Set(GPIOC,PIN1|PIN4|PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);		//PC1,4,5�������
	GPIO_Set(GPIOG,PIN13|PIN14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);		//PG13,14�������
	GPIO_Set(GPIOB,PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);				//PB11�������
 
	GPIO_AF_Set(GPIOA,1,11);	//PA1,AF11
 	GPIO_AF_Set(GPIOA,2,11);	//PA2,AF11
 	GPIO_AF_Set(GPIOA,7,11);	//PA7,AF11 
  	GPIO_AF_Set(GPIOB,11,11);	//PB11,AF11 
	GPIO_AF_Set(GPIOC,1,11);	//PC1,AF11
 	GPIO_AF_Set(GPIOC,4,11);	//PC4,AF11
 	GPIO_AF_Set(GPIOC,5,11);	//PC5,AF11 
  	GPIO_AF_Set(GPIOG,13,11);	//PG13,AF11
 	GPIO_AF_Set(GPIOG,14,11);	//PG14,AF11 
 	MY_NVIC_Init(0,0,ETH_IRQn,2);	//����ETH�еķ���   	
}

//��ȡPHY�Ĵ���ֵ
//reg:Ҫ��ȡ�ļĴ�����ַ
//regval:��ȡ���Ĵ���ֵ(ָ��)
//����ֵ:0 ��ȡ�ɹ�;
//      -1 ��ȡʧ��;
int LAN8720_ReadPHY(u16 reg,u32 *regval)
{
    if(HAL_ETH_ReadPHYRegister(&LAN8720_ETHHandle,LAN8720_ADDR,reg,regval))return -1;	//��0,��ȡʧ��,����-1
    return 0;	//��ȡ�ɹ�,����0
}

//��LAN8720ָ���Ĵ���д��ֵ
//reg:Ҫд��ļĴ���
//value:Ҫд���ֵ
//����ֵ:0 ��ȡ�ɹ�;
//      -1 ��ȡʧ��;
int LAN8720_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    if(HAL_ETH_WritePHYRegister(&LAN8720_ETHHandle,LAN8720_ADDR,reg,temp))return -1;	//��0,д��ʧ��,����-1
    return 0;	//д��ɹ�,����0
}

//��LAN8720 Power Downģʽ 
void LAN8720_EnablePowerDownMode(void)
{
    u32 readval=0;
    LAN8720_ReadPHY(LAN8720_BCR,&readval);
    readval|=LAN8720_BCR_POWER_DOWN;
    LAN8720_WritePHY(LAN8720_BCR,readval);
}

//�ر�LAN8720 Power Downģʽ
void LAN8720_DisablePowerDownMode(void)
{
    u32 readval=0;
    LAN8720_ReadPHY(LAN8720_BCR,&readval);
    readval&=~LAN8720_BCR_POWER_DOWN;
    LAN8720_WritePHY(LAN8720_BCR,readval);
}

//����LAN8720����Э�̹���
void LAN8720_StartAutoNego(void)
{
    u32 readval=0;
    LAN8720_ReadPHY(LAN8720_BCR,&readval);
    readval|=LAN8720_BCR_AUTONEGO_EN;
    LAN8720_WritePHY(LAN8720_BCR,readval);
}

//��ȡLAN8720������״̬
//����ֵ��
//		LAN8720_STATUS_LINK_DOWN				���ӶϿ�
//		LAN8720_STATUS_AUTONEGO_NOTDONE			�Զ�Э��δ���
//		LAN8720_STATUS_100MBITS_FULLDUPLEX		100Mȫ˫��
//		LAN8720_STATUS_100MBITS_HALFDUPLEX		100M��˫��
//		LAN8720_STATUS_10MBITS_FULLDUPLEX		10Mȫ˫��
//		LAN8720_STATUS_10MBITS_HALFDUPLEX		10M��˫��
u32 LAN8720_GetLinkState(void)
{
    u32 readval=0; 
    //��ȡ���飬ȷ����ȡ��ȷ������
    LAN8720_ReadPHY(LAN8720_BSR,&readval);
    LAN8720_ReadPHY(LAN8720_BSR,&readval); 
    //��ȡ����״̬(Ӳ�������ߵ����ӣ�����TCP��UDP��������ӣ�)
    if((readval&LAN8720_BSR_LINK_STATUS)==0)return LAN8720_STATUS_LINK_DOWN; 
    //��ȡ�Զ�Э��״̬
    LAN8720_ReadPHY(LAN8720_BCR,&readval);
    if((readval&LAN8720_BCR_AUTONEGO_EN)!=LAN8720_BCR_AUTONEGO_EN)  //δʹ���Զ�Э��
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
    }else                                                            //ʹ�����Զ�Э��    
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

//����LAN8720������״̬
//linkstateȡֵ��Χ��
//		LAN8720_STATUS_100MBITS_FULLDUPLEX		100Mȫ˫��
//		LAN8720_STATUS_100MBITS_HALFDUPLEX		100M��˫��
//		LAN8720_STATUS_10MBITS_FULLDUPLEX		10Mȫ˫��
//		LAN8720_STATUS_10MBITS_HALFDUPLEX		10M��˫��
void LAN8720_SetLinkState(u32 linkstate)
{ 
    u32 bcrvalue=0;
    LAN8720_ReadPHY(LAN8720_BCR,&bcrvalue);
    //�ر��������ã������Զ�Э�̣��ٶȺ�˫��
    bcrvalue&=~(LAN8720_BCR_AUTONEGO_EN|LAN8720_BCR_SPEED_SELECT|LAN8720_BCR_DUPLEX_MODE);
    if(linkstate==LAN8720_STATUS_100MBITS_FULLDUPLEX)       //100Mȫ˫��
        bcrvalue|=(LAN8720_BCR_SPEED_SELECT|LAN8720_BCR_DUPLEX_MODE);
    else if(linkstate==LAN8720_STATUS_100MBITS_HALFDUPLEX)  //100M��˫��
        bcrvalue|=LAN8720_BCR_SPEED_SELECT; 
    else if(linkstate==LAN8720_STATUS_10MBITS_FULLDUPLEX)   //10Mȫ˫��
        bcrvalue|=LAN8720_BCR_DUPLEX_MODE; 
    LAN8720_WritePHY(LAN8720_BCR,bcrvalue);					//���üĴ���,������Ч
}






