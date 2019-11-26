实验器材:
	阿波罗STM32H7开发板
	
实验目的:
	学习STM32自带RTC的使用
	
硬件资源:
	1,DS0(连接在PB1),DS1(连接在PB0)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面) 
	3,ALIENTEK 2.8/3.5/4.3/7寸LCD模块(包括MCU屏和RGB屏,都支持) 
	4,RTC(实时时钟)
	 
实验现象:
	本实验通过LCD显示RTC时间，并可以通过usmart设置RTC时间/闹钟,从而调节时间,或设置RTC闹钟,
	以便大家验证RTC功能。 
	
注意事项:
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在lcd.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!
	3,串口波特率为115200
	4,请用USB线连接在USB_232,找到USB转串口后测试本例程.
	5,P4的PA9/PA10必须通过跳线帽连接在RXD/TXD上. 
	6,本实验为FLASH APP代码，起始地址为0x8020000，不能直接下载运行,必须通过IAP Bootloader V1.0程序进行IAP更新.
	 


					正点原子@ALIENTEK
					2018-6-10
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com





