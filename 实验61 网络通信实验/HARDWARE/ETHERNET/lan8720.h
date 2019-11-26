#ifndef __LAN8720_H
#define __LAN8720_H
#include "sys.h"
#include "stm32h7xx_hal_eth.h"
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

extern ETH_HandleTypeDef LAN8720_ETHHandle;
//��̫���������ͻ�����
extern __attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT];	//��̫��Rx DMA������,96�ֽ�,�������ڴ汣��,��ֹCACHE
extern __attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT];	//��̫��Tx DMA������,96�ֽ�,�������ڴ汣��,��ֹCACHE
extern __attribute__((at(0x30040200))) u8 ETH_Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE];//��̫�����ջ�����
	
	
#define LAN8720_ADDR			0			//LAN8720��ַΪ0
#define LAN8720_TIMEOUT			500			//LAN8720��ʱʱ��

//LAN8720�Ĵ���
#define LAN8720_BCR      		((u16)0x0000U)
#define LAN8720_BSR      		((u16)0x0001U)
#define LAN8720_PHYI1R   		((u16)0x0002U)
#define LAN8720_PHYI2R   		((u16)0x0003U)
#define LAN8720_ANAR     		((u16)0x0004U)
#define LAN8720_ANLPAR   		((u16)0x0005U)
#define LAN8720_ANER     		((u16)0x0006U)
#define LAN8720_ANNPTR   		((u16)0x0007U)
#define LAN8720_ANNPRR   		((u16)0x0008U)
#define LAN8720_MMDACR  		((u16)0x000DU)
#define LAN8720_MMDAADR 		((u16)0x000EU)
#define LAN8720_ENCTR    		((u16)0x0010U)
#define LAN8720_MCSR     		((u16)0x0011U)
#define LAN8720_SMR      		((u16)0x0012U)
#define LAN8720_TPDCR    		((u16)0x0018U)
#define LAN8720_TCSR     		((u16)0x0019U)
#define LAN8720_SECR     		((u16)0x001AU)
#define LAN8720_SCSIR    		((u16)0x001BU)
#define LAN8720_CLR      		((u16)0x001CU)
#define LAN8720_ISFR    		((u16)0x001DU)
#define LAN8720_IMR      		((u16)0x001EU)
#define LAN8720_PHYSCSR  		((u16)0x001FU)

  
//LAN8720 BCR�Ĵ�����λ����  
#define LAN8720_BCR_SOFT_RESET        			((u16)0x8000U)
#define LAN8720_BCR_LOOPBACK          			((u16)0x4000U)
#define LAN8720_BCR_SPEED_SELECT       			((u16)0x2000U)
#define LAN8720_BCR_AUTONEGO_EN        			((u16)0x1000U)
#define LAN8720_BCR_POWER_DOWN         			((u16)0x0800U)
#define LAN8720_BCR_ISOLATE            			((u16)0x0400U)
#define LAN8720_BCR_RESTART_AUTONEGO  			((u16)0x0200U)
#define LAN8720_BCR_DUPLEX_MODE      			((u16)0x0100U) 

//LAN8720��BSR�Ĵ�����λ����
#define LAN8720_BSR_100BASE_T4      			((u16)0x8000U)
#define LAN8720_BSR_100BASE_TX_FD    			((u16)0x4000U)
#define LAN8720_BSR_100BASE_TX_HD   			((u16)0x2000U)
#define LAN8720_BSR_10BASE_T_FD     			((u16)0x1000U)
#define LAN8720_BSR_10BASE_T_HD     			((u16)0x0800U)
#define LAN8720_BSR_100BASE_T2_FD   			((u16)0x0400U)
#define LAN8720_BSR_100BASE_T2_HD   			((u16)0x0200U)
#define LAN8720_BSR_EXTENDED_STATUS  			((u16)0x0100U)
#define LAN8720_BSR_AUTONEGO_CPLT    			((u16)0x0020U)
#define LAN8720_BSR_REMOTE_FAULT     			((u16)0x0010U)
#define LAN8720_BSR_AUTONEGO_ABILITY 			((u16)0x0008U)
#define LAN8720_BSR_LINK_STATUS      			((u16)0x0004U)
#define LAN8720_BSR_JABBER_DETECT    			((u16)0x0002U)
#define LAN8720_BSR_EXTENDED_CAP    			((u16)0x0001U)

//LAN8720 PHYSCSR�Ĵ�����λ����
#define LAN8720_PHYSCSR_AUTONEGO_DONE  			((u16)0x1000U)
#define LAN8720_PHYSCSR_HCDSPEEDMASK   			((u16)0x001CU)
#define LAN8720_PHYSCSR_10BT_HD        			((u16)0x0004U)
#define LAN8720_PHYSCSR_10BT_FD        			((u16)0x0014U)
#define LAN8720_PHYSCSR_100BTX_HD      			((u16)0x0008U)
#define LAN8720_PHYSCSR_100BTX_FD      			((u16)0x0018U) 
  
//LAN8720״̬��ض���
#define  LAN8720_STATUS_READ_ERROR    			((int)-5)
#define  LAN8720_STATUS_WRITE_ERROR				((int)-4)
#define  LAN8720_STATUS_ADDRESS_ERROR			((int)-3)
#define  LAN8720_STATUS_RESET_TIMEOUT      	 	((int)-2)
#define  LAN8720_STATUS_ERROR              	 	((int)-1)
#define  LAN8720_STATUS_OK						((int) 0)
#define  LAN8720_STATUS_LINK_DOWN				((int) 1)
#define  LAN8720_STATUS_100MBITS_FULLDUPLEX		((int) 2)
#define  LAN8720_STATUS_100MBITS_HALFDUPLEX		((int) 3)
#define  LAN8720_STATUS_10MBITS_FULLDUPLEX		((int) 4)
#define  LAN8720_STATUS_10MBITS_HALFDUPLEX		((int) 5)
#define  LAN8720_STATUS_AUTONEGO_NOTDONE		((int) 6)




int LAN8720_Init(void);
int LAN8720_ReadPHY(u16 reg,u32 *regval);
int LAN8720_WritePHY(u16 reg,u16 value);
void LAN8720_EnablePowerDownMode(void);
void LAN8720_DisablePowerDownMode(void);
void LAN8720_StartAutoNego(void);  
u32 LAN8720_GetLinkState(void);
void LAN8720_SetLinkState(u32 linkstate);

#endif 

