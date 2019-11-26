实验器材:
	阿波罗STM32H7开发板
	
实验目的:  
	学习STM32H7 SPDIF RX功能的使用，实现光纤接收SPDIF音频信号并通过开发板的DAC播放.
	
硬件资源:
	1,DS0(连接在PB1)  
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)  
	3,ALIENTEK 2.8/3.5/4.3/7寸/10.1寸LCD模块(包括MCU屏和RGB屏,都支持) 
	4,SD卡,通过SDMMC1(SDMMC_D0~D4(PC8~PC11),SDMMC_SCK(PC12),SDMMC_CMD(PD2))连接 
	5,按键KEY2(PH2)/KEY_UP(PA0,也称之为WK_UP) 
	6,WM8978音频CODEC芯片(连接关系见:STM32H7开发指南,52.2节)
	7,SAI音频接口(用于驱动WM8978)   
	8,光纤音频接口(OPTICAL)
	9,SPDIF RX音频接收器(用于接收光纤音频信号)   
	
	
实验现象:
	本实验开机后，先初始化各外设，然后检测字库是否存在，如果检测无问题，则初始化WM8978的DAC
	工作,并开启DAC输出,随后设置SPDIF的DMA和回调函数.然后进入死循环等待,不停的检测SPDIF的连接
	状态,当SPDIF RX同步完成,且与SAI时钟同步完成后,开启SPDIF RX和SAI的DMA数据传输,此时,在屏幕
	上会显示当前的音频信号采样率,同时在喇叭/耳机,可以听到音乐.另外,我们可以通过KEY_UP和KEY1来
	调节音量,KEY_UP用于增加音量,KEY1用于减少音量.  
	
	
注意事项:
	1,4.3/7/10.1寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在lcd.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!     
	3,本例程需要自备光纤音频输出设备一台(比如ALIENTEK的BASE蓝牙音频接收器),并通过光纤线连接
	  开发板的OPTICAL光纤音频接口。 
	 

					正点原子@ALIENTEK
					2018-8-17
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com





