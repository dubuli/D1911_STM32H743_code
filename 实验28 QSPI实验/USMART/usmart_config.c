#include "usmart.h"
#include "usmart_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
#include "delay.h"	 	
#include "sys.h" 
#include "delay.h"	 
#include "w25qxx.h" 
 								  
extern void PWM_DAC_Set(u16 vol);					  									  
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
	(void*)delay_ms,"void delay_ms(u16 nms)",
 	(void*)delay_us,"void delay_us(u32 nus)",	
		
	(void*)W25QXX_ReadSR,"u8 W25QXX_ReadSR(u8 regno)",	 
	(void*)W25QXX_Write_SR,"void W25QXX_Write_SR(u8 regno,u8 sr)",	 
	(void*)W25QXX_ReadID,"u16 W25QXX_ReadID(void)",	 
	(void*)W25QXX_Write_Enable,"void W25QXX_Write_Enable(void)",
	(void*)W25QXX_Qspi_Enable,"void W25QXX_Qspi_Enable(void)",
	(void*)W25QXX_Qspi_Disable,"void W25QXX_Qspi_Disable(void)",

	(void*)W25QXX_Erase_Chip,"void W25QXX_Erase_Chip(void)",
	(void*)W25QXX_Erase_Sector,"void W25QXX_Erase_Sector(u32 Dst_Addr)",		
};						  				  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab, 
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   



















