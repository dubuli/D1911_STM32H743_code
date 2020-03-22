实验器材:
	阿波罗STM32H7发板
	
实验目的:
	学习使用UCOSIII同时等待多个内核对象
	
硬件资源:
	1,DS0(连接在PB1)，DS1(连接在PB0上)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面) 
	3,ALIENTEK 2.8/3.5/4.3/7寸LCD模块(包括MCU屏和RGB屏,都支持) 
	4,按键KEY0(PH3)/KEY1(PH2)/KEY_UP(PA0,也称之为WK_UP)
	
实验现象:
	下载程序到开发板中，任务multi_task会同时等待多个内核对象：Test_Sem1、Test_Sem2和Test_Q，
	即两个信号量和一个消息队列，只有这三个内核对象都同时有效，任务multi_task才会执行后面的程序。

	
注意事项:
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在lcd.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!  

	 

					正点原子@ALIENTEK
					2018-6-22
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
