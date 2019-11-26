实验器材:
	阿波罗STM32H7开发板
	
实验目的:  
	学习STM32的USB功能使用,实现一个USB虚拟串口功能.
	
硬件资源:
	1,DS0(连接在PB1)  
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)  
	3,ALIENTEK 2.8/3.5/4.3/7寸/10.1寸LCD模块(包括MCU屏和RGB屏,都支持)  
	4,USB_SLAVE接口(通过跳线帽将D-/D+连接在PA11/PA12上)
	
	
实验现象:
	本实验利用STM32自带的USB功能，连接电脑USB,虚拟出一个USB串口，实现电脑和开发板的数据通信。本
	例程功能完全同实验3（串口通信实验），只不过串口变成了STM32的USB虚拟串口。当USB连接电脑（USB
	线插入USB_SLAVE接口），开发板将通过USB和电脑建立连接，并虚拟出一个串口（注意：需要先安装:光盘
	\6，软件资料\1，软件\STM32 USB虚拟串口驱动\VCP_V1.4.0_Setup.exe这个驱动软件），USB和电脑连
	接成功后，DS1常亮。
	
	
注意事项:
	1,4.3/7/10.1寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在lcd.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!   
	3,本实验需要用跳线帽短接P10的D+和PA12，以及D-和PA11。
	4,本实验USB_HOST接口不要插任何设备。 
	5,本实验开发板配套的T口USB线应该插USB_SLAVE。 
	6,本实验需安装:光盘\6，软件资料\1，软件\STM32 USB虚拟串口驱动\VCP_V1.4.0_Setup.exe这个驱动
	7,如果电脑显示驱动安装不正常（有惊叹号），请手动选择驱动安装，驱动路径（以WIN7系统为例）：C:\
	  Program Files (x86)\STMicroelectronics\Software\Virtual comport driver\Win7
	  
	  
					正点原子@ALIENTEK
					2018-9-2
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com





