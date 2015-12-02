/**
 * @copyright 王辰浩(2014~2024) QQ：975559549
 * @Author Feather @version 1.0 @date 2015.12.2
 * @filename infrared_remote_ctrl.c
 * @description Infrared Remote Control Menu
 * @FunctionList
 *		1.void remote_ctrl(void); //处理远程控制 
 */ 

#include "infrared_remote_ctrl.h"
#include "lcd.h"
#include "main.h"
#include "remote.h"
#include "can.h"
#include "stmflash.h"

//要写入到STM32 FLASH的字符串数组
const u8 TEXT_Buffer[]={"STM32 FLASH TEST"};
#define TEXT_LENTH sizeof(TEXT_Buffer)	 		  	//数组长度	
#define SIZE TEXT_LENTH/4+((TEXT_LENTH%4)?1:0)
#define FLASH_SAVE_ADDR  0X080e0000 	//设置FLASH 保存地址(必须为偶数，且所在扇区,要大于本代码所占用到的扇区.
										//否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机.
										
u8 REMOTE_POWER = 0;
/**
 * @Function void remote_ctrl(void)
 * @description 红外遥控菜单
 * @Input    void
 * @Return   void
 */
void remote_ctrl(void)
{
	u8 res ;
	u8 InfraredKey;
	u8 *REMOTE_IBUF;
	u8 Can_Buf[8];
	
	u8 flash_data[SIZE];
	
	if(InfraredKey = Remote_Scan())
	{
			LCD_ShowNum(20+6*24,  200,InfraredKey, 3, 24);
			LCD_ShowNum(20+6*24,  224,RmtCnt, 3, 24);
			switch(InfraredKey)
			{
				case 0:REMOTE_IBUF="ERROR";break;			   
				case 162:REMOTE_IBUF="POWER";REMOTE_POWER = !REMOTE_POWER ;break;	    
				case 98:REMOTE_IBUF="UP";break;	    
				case 2:REMOTE_IBUF="PLAY";break;		 
				case 226:REMOTE_IBUF="ALIENTEK";break;		  
				case 194:REMOTE_IBUF="RIGHT";break;	   
				case 34:REMOTE_IBUF="LEFT";break;		  
				case 224:REMOTE_IBUF="VOL-";break;		  
				case 168:REMOTE_IBUF="DOWN";break;		   
				case 144:REMOTE_IBUF="VOL+";break;		    
				case 104:REMOTE_IBUF="1";
					{
							res = CAN1_Send_Msg("CAN 1", 6);
							LCD_ShowString(20, 272, 10 * 24, 24, 24, "SEND : CAN 1");
							if(res) LCD_ShowString(200, 272, 6 * 24, 24, 24, "failed!");
							else LCD_ShowString(200, 272, 6 * 24, 24, 24, "    OK!");
					}break;		  
			  case 152:REMOTE_IBUF="2";
					{
							CanMode=!CanMode;
							CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CanMode);	//CAN普通模式初始化,普通模式,波特率500Kbps
							POINT_COLOR=RED;//设置字体为红色 
							if(CanMode==CAN_Mode_Normal)//普通模式，需要2个开发板
							{
									LCD_ShowString(20, 296, 6 * 24, 24, 24,"Nnormal Mode ");	    
							}else //回环模式,一个开发板就可以测试了.
							{
									LCD_ShowString(20, 296, 6 * 24, 24, 24,"LoopBack Mode");
							}
					}break;	   
				case 176:REMOTE_IBUF="3";
					{
							res = CAN1_Send_Msg("CAN 3", 6);
							LCD_ShowString(20, 272, 10 * 24, 24, 24, "SEND : CAN 3");
							if(res) LCD_ShowString(200, 272, 6 * 24, 24, 24, "failed!");
							else LCD_ShowString(200, 272, 6 * 24, 24, 24, "    OK!");
					}break;											
			  case 48:REMOTE_IBUF="4";
						{ 
							LCD_ShowString(20, 300, 15 * 24, 24, 24,"Start Write FLASH....");
							STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer,SIZE);
							LCD_ShowString(20, 300, 15 * 24, 24, 24, "FLASH Write Finished!");//提示传送完成
						}
					break;		    
				case 24:REMOTE_IBUF="5";
						{
							LCD_ShowString(20, 300, 15 * 24, 24, 24,"Start Read FLASH.... ");
							STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)flash_data,SIZE);
							LCD_ShowString(20, 300, 15 * 24, 24, 24,"The Data Readed Is:  ");//提示传送完成
							LCD_ShowString(20, 324, 15 * 24, 24, 24,flash_data);//显示读到的字符串
						}
						break;		    
				case 122:REMOTE_IBUF="6";break;		  
				case 16:REMOTE_IBUF="7";break;			   					
				case 56:REMOTE_IBUF="8";break;	 
				case 90:REMOTE_IBUF="9";break;
				case 66:REMOTE_IBUF="0";break;
				case 82:REMOTE_IBUF="DELETE";break;
			}//end of switch
			if(REMOTE_POWER)
			{
				POINT_COLOR = MAGENTA;
				LCD_ShowString(20, 200, 6 * 24, 24, 24, "InfraredKey:");
				LCD_ShowString(20, 224, 6 * 24, 24, 24, "   RmtCnt  :");
				LCD_ShowString(20, 248, 6 * 24, 24, 24, "Key Content:");
			}
			else
			{
				flush_key = 1;
			}
	}//end of if
	res = CAN1_Receive_Msg(Can_Buf);
	if(res)//接收到有数据
	{			
			LCD_ShowString(20, 320, 3 * 24, 24, 24, "Rev");
			LCD_ShowString(20+3*24, 320, 6 * 24, 24, 24, Can_Buf);
	}		
	LCD_ShowString(20+6*24, 248, 6 * 24, 24, 24, REMOTE_IBUF);

}