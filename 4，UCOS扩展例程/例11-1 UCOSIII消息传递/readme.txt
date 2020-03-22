实验器材:
	阿波罗STM32H7开发板
	
实验目的:
	学习使用UCOSIII的消息队列实现消息传递
	
硬件资源:
	1,DS0(连接在PB1)，DS1(连接在PB0上)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面) 
	3,ALIENTEK 2.8/3.5/4.3/7寸LCD模块(包括MCU屏和RGB屏,都支持) 
	4,按键KEY0(PH3)/KEY1(PH2)/KEY_UP(PA0,也称之为WK_UP)	
	
实验现象:
	下载程序到开发板中，默认会创建两个消息队列：KEY_Msg和DATA_Msg，其中KEY_Msg用来模拟UCOSII中
	的消息邮箱(UCOSIII没有消息邮箱的概念)。任务main_task检测是否按键按下，当检测到有按键按下的
	时候就会将按键值保存到KEY_Msg中并发送出去，任务Keyprocess_task不断的请求消息KEY_Msg，当
	请求到以后就会根据消息指来做出不同的处理。定时器回调函数tmr1_callback会在定时器开启以后向
	消息队列DATA_Msg中发送消息，直到消息队列满了。任务msgdis_task请求消息队列DATA_Msg中的内容，
	并将其显示出来。
	
注意事项:
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在lcd.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!  

	 

					正点原子@ALIENTEK
					2016-6-22
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
