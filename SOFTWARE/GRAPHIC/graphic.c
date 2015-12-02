/**
 * @copyright 王辰浩(2014~2024) QQ：975559549
 * @Author Feather @version 1.0 @date 2015.11.27
 * @filename graphic.c
 * @description LCD相关图像
 * @FunctionList
 *		1.u8 GT9147_Send_Cfg(u8 mode);                  //发送GT9147配置参数
 *		2.u8 GT9147_WR_Reg(u16 reg,u8 *buf,u8 len);     //向GT9147写入一次数据
 *		3.void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len);   //从GT9147读出一次数据
 *		4.u8 GT9147_Init(void);                         //初始化GT9147触摸屏
 *		5.u8 GT9147_Scan(u8 mode);                      //扫描触摸屏(采用查询方式)
 */ 

#include "graphic.h"
#include "lcd.h"

/**
 * @Function void gui_clear(u32 color);
 * @description 用color清屏
 * @Input  void
 * @Return void
 */	
void gui_clear(u32 color)
{
	LCD_Clear(color);
}
/**
 * @Function void gui_draw_dialog(u16 x, u16 y, u16 width, u16 height, u8 size, u8 * str, u32 charColor, u32 dialogColor);
 * @description 绘制对话框，设置大小,内容
 * @Input u16 x, u16 y: 起始位置(x,y)
 *				u16 width, u16 height: 对话框长宽
 *        u8 size: 字大小
 *				u8 * str: 对话框显示内容 //height >= size + 2
 *        u32 charColor, u32 dialogColor: 字和对话框边框颜色
 * @Return void
 */	
void gui_draw_dialog(u16 x, u16 y, u16 width, u16 height, u8 size, u8 * str, u32 charColor, u32 dialogColor)
{
	//LCD_Clear(WHITE);//清屏   
	POINT_COLOR=dialogColor;
	LCD_DrawLine(x, y, x + width-1, y);
	LCD_DrawLine(x, y+1, x + width-1, y+1);
	LCD_DrawLine(x, y+height-2, x + width-1, y+height-2);
	LCD_DrawLine(x, y+height-1, x + width-1, y+height-1);
	
	LCD_DrawLine(x, y+2, x, y + height-3);
	LCD_DrawLine(x+1, y+2, x+1, y + height-3);
	LCD_DrawLine(x+width-2, y+2, x+width-2, y + height-3);
	LCD_DrawLine(x+width-1, y+2, x+width-1, y + height-3);
 	POINT_COLOR=charColor;//设置字体为蓝色 
	LCD_ShowString(x+2,y+2,width,height,size,str);//显示清屏区域
  //POINT_COLOR=RED;//设置画笔蓝色 
}
////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
/**
 * @Function void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
 * @description 绘制水平线
 * @Input u16 x, u16 y: 起始位置(x,y)
 *				u16 len:线长度
 *        u32 color：颜色
 * @Return void
 */	
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//两个数之差的绝对值 
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
