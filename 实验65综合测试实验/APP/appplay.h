#ifndef __APPPLAY_H
#define __APPPLAY_H 	
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-应用中心 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//20160904  V1.1
//新增光纤音频接收(SPDIF RX)功能
////////////////////////////////////////////////////////////////////////////////// 	   

#define APPPLAY_EX_BACKCOLOR	0X0000			//窗体外部背景色
#define APPPLAY_IN_BACKCOLOR	0X8C51			//窗体内部背景色
#define APPPLAY_NAME_COLOR		0X001F			//程序名颜色

#define APPPLAY_ALPHA_VAL 		18 				//APP选中透明度设置
#define APPPLAY_ALPHA_COLOR		WHITE			//APP透明色
	
	
////////////////////////////////////////////////////////////////////////////////////////////
//各图标/图片路径
extern u8 * const appplay_icospath_tbl[3][16];//icos的路径表
////////////////////////////////////////////////////////////////////////////////////////////


//APP图标参数管理
__packed typedef struct _m_app_icos
{										    
	u16 x;			//图标坐标及尺寸
	u16 y;
	u8 width;
	u8 height; 
	u8 * path;		//图标路径
	u8 * name;		//图标名字
}m_app_icos;

//APP控制器
typedef struct _m_app_dev
{										    
	u8 selico;					//当前选中的图标.
								//0~15,被选中的图标编号	 
								//其他,没有任何图标被选中	  
	m_app_icos icos[14];		//总共16个图标
}m_app_dev;
		 

u8 app_play(void); 
#endif























