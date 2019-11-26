#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "ltdc.h"  
#include "timer.h" 
#include "arm_math.h" 
#include "key.h" 
//ALIENTEK ������STM32H7������ ʵ��52_2
// DSP FFT���� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 


#define FFT_LENGTH		1024 		//FFT���ȣ�Ĭ����1024��FFT
float fft_inputbuf[FFT_LENGTH*2];	//FFT��������
float fft_outputbuf[FFT_LENGTH];	//FFT�������

u8 timeout;
int main(void)
{  
	u8 led0sta=1;
	arm_cfft_radix4_instance_f32 scfft;
 	u8 key,t=0;
	float time; 
	u8 buf[50]; 
	u16 i; 
	Stm32_Clock_Init(160,5,2,4);	//����ʱ��,400Mhz
    delay_init(400);				//��ʱ��ʼ��  
	uart_init(100,115200);			//���ڳ�ʼ��Ϊ115200 
	LED_Init();						//��ʼ����LED���ӵ�Ӳ���ӿ�  
	MPU_Memory_Protection();		//������ش洢����
	SDRAM_Init();					//��ʼ��SDRAM 
	KEY_Init();						//��ʼ������ 
	LCD_Init();						//��ʼ��LCD
 	TIM3_Int_Init(65535,200-1);		//1Mhz����Ƶ��
	POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"Apollo STM32F4/F7/H7");	
	LCD_ShowString(30,70,200,16,16,"DSP FFT TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2018/8/21");	
	LCD_ShowString(30,130,200,16,16,"KEY0:Run FFT");//��ʾ��ʾ��Ϣ 
	LCD_ShowString(30,160,200,16,16,"FFT runtime:");//��ʾFFTִ��ʱ��
 	POINT_COLOR=BLUE;		//��������Ϊ��ɫ   
	arm_cfft_radix4_init_f32(&scfft,FFT_LENGTH,0,1);//��ʼ��scfft�ṹ�壬�趨FFT��ز���
    while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			for(i=0;i<FFT_LENGTH;i++)//�����ź�����
			{
				 fft_inputbuf[2*i]=100+
				                   10*arm_sin_f32(2*PI*i/FFT_LENGTH)+
								   30*arm_sin_f32(2*PI*i*4/FFT_LENGTH)+
				                   50*arm_cos_f32(2*PI*i*8/FFT_LENGTH);	//���������ź�ʵ��
				 fft_inputbuf[2*i+1]=0;//�鲿ȫ��Ϊ0
			}
			TIM3->CNT=0;;//����TIM3��ʱ���ļ�����ֵ
			timeout=0;
			arm_cfft_radix4_f32(&scfft,fft_inputbuf);	//FFT���㣨��4��
            time=TIM3->CNT+(u32)timeout*65536;//��������ʱ��
			sprintf((char*)buf,"%0.3fms\r\n",time/1000);	
			LCD_ShowString(30+12*8,160,100,16,16,buf);	//��ʾ����ʱ��		
			arm_cmplx_mag_f32(fft_inputbuf,fft_outputbuf,FFT_LENGTH);	//��������������ģ�÷�ֵ 
			printf("\r\n%d point FFT runtime:%0.3fms\r\n",FFT_LENGTH,time/1000);
			printf("FFT Result:\r\n");
			for(i=0;i<FFT_LENGTH;i++)
			{
				printf("fft_outputbuf[%d]:%f\r\n",i,fft_outputbuf[i]);
			}
		}else delay_ms(10);
		t++;
		if((t%20)==0)LED0(led0sta^=1); 		  
	}
}

		; 
 



