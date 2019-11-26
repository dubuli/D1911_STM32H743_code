实验器材:
	阿波罗STM32H7开发板
	
实验目的:
	学习STM32通过LTDC接口驱动RGB接口的LCD屏
	
硬件资源:
	1,DS0(连接在PF9) 
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 4.3/7寸/10.1寸RGBLCD模块
	4,外部SDRAM(W9825G6KH) 
	
	
实验现象:
	本实验利用STM32H743的LTDC接口来驱动RGB屏，RGBLCD模块的接口在核心板上，通过40P的FPC排线
	连接RGBLCD模块，实现RGBLCD模块的驱动和显示，下载成功后，按下复位之后，就可以看到RGBLCD
	模块不停的显示一些信息并不断切换底色。同时，屏幕上会显示LCD的ID。
	
注意事项:
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程支持除ALIENTEK V1版本7寸屏以外的其他所有LCD屏模块（包括RGB屏和MCU屏）.
	3,RGB屏的ID，是我们人为设定的（通过R7/G7/B7确定），RGB屏的驱动IC并没有读ID的功能。
	 


					正点原子@ALIENTEK
					2018-6-8
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com





