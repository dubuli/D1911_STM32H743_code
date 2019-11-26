实验器材:
	阿波罗STM32H7开发板
	
实验目的:
	学习STM32H743 DSP功能(基础数学函数)的使用
	
硬件资源:
	1,DS0(连接在PF9) 
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,ALIENTEK 4.3/7寸/10.1寸RGBLCD模块 
	
	
实验现象:
	本实验测试STM32H743的DSP库基础数学函数：arm_cos_f32和arm_sin_f32和标准库基础数学函数
	：cosf和sinf的速度差别，并在LCD屏幕上面显示两者计算所用时间，DS0用于提示程序正在运行。
	
注意事项:
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程支持除ALIENTEK V1版本7寸屏以外的其他所有LCD屏模块（包括RGB屏和MCU屏）.
	3,RGB屏的ID，是我们人为设定的（通过R7/G7/B7确定），RGB屏的驱动IC并没有读ID的功能。
	5,本例程开启了硬件FPU！


					正点原子@ALIENTEK
					2018-8-21
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com





