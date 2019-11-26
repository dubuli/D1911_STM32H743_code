#include "usmart.h"
#include "usmart_str.h"
////////////////////////////用户配置区///////////////////////////////////////////////
//这下面要包含所用到的函数所申明的头文件(用户自己添加) 
#include "delay.h"	 
#include "piclib.h"  
#include "w25qxx.h" 
#include "ov5640.h" 
#include "ftl.h"   
#include "nandtester.h"
#include "fattester.h"  
#include "gui.h"
#include "nand.h"
#include "lcd.h"

//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		
	(void*)W25QXX_Erase_Chip,"void W25QXX_Erase_Chip(void)", 
	(void*)NAND_EraseChip,"void NAND_EraseChip(void)", 
 	(void*)gui_color_chg,"u16 gui_color_chg(u32 rgb)", 
 	(void*)bmp_encode,"u8 bmp_encode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 mode)", 
 
	(void*)LCD_Fast_DrawPoint,"void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)",
	(void*)LCD_ReadPoint,"u16 LCD_ReadPoint(u16 x,u16 y)",					
		
	(void*)mf_mount,"u8 mf_mount(u8* path,u8 mt)",

	(void*)mf_open,"u8 mf_open(u8*path,u8 mode)",
	(void*)mf_close,"u8 mf_close(void)",
	(void*)mf_read,"u8 mf_read(u16 len)",
	(void*)mf_write,"u8 mf_write(u8*dat,u16 len)",
 	(void*)mf_opendir,"u8 mf_opendir(u8* path)",
	(void*)mf_readdir,"u8 mf_readdir(void)",

 	(void*)mf_scan_files,"u32 mf_scan_files(u8 * path)",
 	(void*)mf_showfree,"u32 mf_showfree(u8 *drv)",
	(void*)mf_lseek,"u8 mf_lseek(u32 offset)",
	(void*)mf_tell,"u8 mf_tell(void)",
 	(void*)mf_size,"u32 mf_size(void)",


	(void*)mf_fmkfs,"u8 mf_fmkfs(u8 drv,u8 mode,u16 au)",
	(void*)mf_mkdir,"u8 mf_mkdir(u8*name)",
	(void*)mf_unlink,"u8 mf_unlink(u8 *name)",
 	(void*)mf_rename,"u8 mf_rename(u8 *oldname,u8* newname)", 
	(void*)mf_scan_files,"u8 mf_scan_files(u8 * path)", 	 
	(void*)ai_load_picfile,"u8 ai_load_picfile(const u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 fast)", 
//  	(void*)OV5640_WR_Reg,"u8 OV5640_WR_Reg(u16 reg,u8 data)",
//  	(void*)OV5640_RD_Reg,"u8 OV5640_RD_Reg(u16 reg)",  
		 
	(void*)FTL_Init,"u8 FTL_Init(void)",
	(void*)FTL_CheckBadBlock,"u8 FTL_CheckBadBlock(u32 blocknum)",
	(void*)FTL_UsedBlockMark,"u8 FTL_UsedBlockMark(u32 blocknum)",
 	(void*)FTL_FindUnusedBlock,"u32 FTL_FindUnusedBlock(u32 sblock,u8 flag)",
	(void*)FTL_FindSamePlaneUnusedBlock,"u32 FTL_FindSamePlaneUnusedBlock(u32 blocknum)",
	(void*)FTL_LBNToPBN,"u16 FTL_LBNToPBN(u32 blocknum)",
	(void*)FTL_CreateLUT,"u8 FTL_CreateLUT(void)",
	(void*)FTL_Format,"u8 FTL_Format(void)", 
};						  					  				  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab, 
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   



















