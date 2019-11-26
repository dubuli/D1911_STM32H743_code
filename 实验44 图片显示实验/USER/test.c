#include "sys.h"
#include "delay.h" 
#include "led.h"  
#include "usart.h"
#include "mpu.h"   
#include "lcd.h" 
#include "sdram.h" 
#include "usmart.h"  
#include "key.h" 
#include "malloc.h"  
#include "nand.h"    
#include "ftl.h"  
#include "w25qxx.h"    
#include "sdmmc_sdcard.h" 
#include "ff.h"  
#include "exfuns.h" 
#include "text.h"	
#include "piclib.h"	
#include "string.h"		
#include "math.h"	 
//ALIENTEK 阿波罗STM32H7开发板 实验44
//图片显示 实验 
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  


//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO *tfileinfo;	//临时文件信息	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));//申请内存
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,tfileinfo);       		//读取目录下的一个文件  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//错误了/到末尾了,退出	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	}  
	myfree(SRAMIN,tfileinfo);//释放内存
	return rval;
} 
 
int main(void)
{  
	u8 led0sta=1;
	u8 res;
 	DIR picdir;	 			//图片目录
	FILINFO *picfileinfo;	//文件信息 
	u8 *pname;				//带路径的文件名
	u16 totpicnum; 			//图片文件总数
	u16 curindex;			//图片当前索引
	u8 key;					//键值
	u8 pause=0;				//暂停标记
	u8 t;
	u16 temp;
	u32 *picoffsettbl;		//图片文件offset索引表 
	Stm32_Clock_Init(160,5,2,4);//设置时钟,400Mhz
    delay_init(400);			//延时初始化  
	uart_init(100,115200);		//串口初始化为115200   
	usmart_dev.init(100);		//初始化USMART
	LED_Init();					//初始化与LED连接的硬件接口  
	MPU_Memory_Protection();	//保护相关存储区域
	SDRAM_Init();				//初始化SDRAM 
	LCD_Init();					//初始化LCD 
	KEY_Init();					//初始化按键
	W25QXX_Init();				//初始化W25Q256 
	my_mem_init(SRAMIN);		//初始化内部内存池(AXI)
	my_mem_init(SRAMEX);		//初始化外部内存池(SDRAM)
	my_mem_init(SRAM12);		//初始化SRAM12内存池(SRAM1+SRAM2)
	my_mem_init(SRAM4);			//初始化SRAM4内存池(SRAM4)
	my_mem_init(SRAMDTCM);		//初始化DTCM内存池(DTCM)
	my_mem_init(SRAMITCM);		//初始化ITCM内存池(ITCM)
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
 	f_mount(fs[2],"2:",1); 		//挂载NAND FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 		//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"阿波罗STM32F4/F7/H7开发板",16,0);				    	 
	Show_Str(30,70,200,16,"图片显示程序",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:NEXT KEY2:PREV",16,0);				    	 
	Show_Str(30,110,200,16,"KEY_UP:PAUSE",16,0);				    	 
	Show_Str(30,130,200,16,"正点原子@ALIENTEK",16,0);				    	 
	Show_Str(30,150,200,16,"2018年8月2日",16,0);
 	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(30,170,240,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(30,170,240,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//申请内存
 	pname=mymalloc(SRAMIN,FF_MAX_LFN*2+1);					//为带路径的文件名分配内存
 	picoffsettbl=mymalloc(SRAMIN,4*totpicnum);				//申请4*totpicnum个字节的内存,用于存放图片索引
 	while(!picfileinfo||!pname||!picoffsettbl)				//内存分配出错
 	{	    	
		Show_Str(30,170,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.dptr;								//记录当前dptr偏移
	        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picoffsettbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"开始显示...",16,0); 
	delay_ms(1500);
	piclib_init();										//初始化画图	   	   
	curindex=0;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出
		strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		strcat((char*)pname,(const char*)picfileinfo->fname);//将文件名接在后面
 		LCD_Clear(BLACK);
 		ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//显示图片    
		Show_Str(2,2,lcddev.width,16,pname,16,1); 				//显示图片名字
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//扫描按键
			if(t>250)key=1;			//模拟一次按下KEY0    
			if((t%20)==0)LED0(led0sta^=1);//LED0闪烁,提示程序正在运行.
			if(key==KEY2_PRES)		//上一张
			{
				if(curindex)curindex--;
				else curindex=totpicnum-1;
				break;
			}else if(key==KEY0_PRES)//下一张
			{
				curindex++;		   	
				if(curindex>=totpicnum)curindex=0;//到末尾的时候,自动从头开始
				break;
			}else if(key==WKUP_PRES)
			{
				pause=!pause;
				LED1(!pause); 	//暂停的时候LED1亮.  
			}
			if(pause==0)t++;
			delay_ms(10); 
		}					    
		res=0;  
	} 							    
	myfree(SRAMIN,picfileinfo);			//释放内存						   		    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picoffsettbl);			//释放内存
}









