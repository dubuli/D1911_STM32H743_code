实验器材:
	阿波罗STM32H7开发板
	
实验目的:  
	学习STM32的USB SLAVE功能使用,实现一个简单的USB读卡器
	
硬件资源:
	1,DS0(连接在PB1)  
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)  
	3,ALIENTEK 2.8/3.5/4.3/7寸/10.1寸LCD模块(包括MCU屏和RGB屏,都支持) 
	4,SD卡,通过SDMMC1(SDMMC_D0~D4(PC8~PC11),SDMMC_SCK(PC12),SDMMC_CMD(PD2))连接 
	5,W25Q256(SPI FLASH芯片,连接在QSPI上)
	6,NAND FLASH(512MB)
	7,USB_SLAVE接口(通过跳线帽将D-/D+连接在PA11/PA12上)
	
	
实验现象:
	本实验代码,开机的时候先检测SD卡、SPI FLASH和NAND FLASH是否存在，如果存在则获取其容量，并
	显示在LCD上面（如果不存在，则报错）。之后开始USB配置，在配置成功之后就可以在电脑上发现三个
	可移动磁盘。我们用DS1来指示USB正在读写，并在液晶上显示出来，同样，我们还是用DS0来指示程序
	正在运行。
	
	
注意事项:
	1,4.3/7/10.1寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在lcd.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!   
	3,NAND FLASH里面默认是存储了一些文件的,大家不要乱写数据，以防数据丢失.建议测试扇区2即可.
	4,本实验USB_HOST接口不要插任何设备。
	5,请自备SD卡一个（如果没有，则只能使用内部SPI FLASH和NAND FLASH盘）。
	6,本实验开发板配套的T口USB线应该插USB_SLAVE。
	 

					正点原子@ALIENTEK
					2018-8-23
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com





