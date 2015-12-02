/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.12.2
 * @filename infrared_remote_ctrl.c
 * @description Infrared Remote Control Menu
 * @FunctionList
 *		1.void remote_ctrl(void); //����Զ�̿��� 
 */ 

#include "infrared_remote_ctrl.h"
#include "lcd.h"
#include "main.h"
#include "remote.h"
#include "can.h"
#include "stmflash.h"

//Ҫд�뵽STM32 FLASH���ַ�������
const u8 TEXT_Buffer[]={"STM32 FLASH TEST"};
#define TEXT_LENTH sizeof(TEXT_Buffer)	 		  	//���鳤��	
#define SIZE TEXT_LENTH/4+((TEXT_LENTH%4)?1:0)
#define FLASH_SAVE_ADDR  0X080e0000 	//����FLASH �����ַ(����Ϊż��������������,Ҫ���ڱ�������ռ�õ�������.
										//����,д������ʱ��,���ܻᵼ�²�����������,�Ӷ����𲿷ֳ���ʧ.��������.
										
u8 REMOTE_POWER = 0;
/**
 * @Function void remote_ctrl(void)
 * @description ����ң�ز˵�
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
							CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CanMode);	//CAN��ͨģʽ��ʼ��,��ͨģʽ,������500Kbps
							POINT_COLOR=RED;//��������Ϊ��ɫ 
							if(CanMode==CAN_Mode_Normal)//��ͨģʽ����Ҫ2��������
							{
									LCD_ShowString(20, 296, 6 * 24, 24, 24,"Nnormal Mode ");	    
							}else //�ػ�ģʽ,һ��������Ϳ��Բ�����.
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
							LCD_ShowString(20, 300, 15 * 24, 24, 24, "FLASH Write Finished!");//��ʾ�������
						}
					break;		    
				case 24:REMOTE_IBUF="5";
						{
							LCD_ShowString(20, 300, 15 * 24, 24, 24,"Start Read FLASH.... ");
							STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)flash_data,SIZE);
							LCD_ShowString(20, 300, 15 * 24, 24, 24,"The Data Readed Is:  ");//��ʾ�������
							LCD_ShowString(20, 324, 15 * 24, 24, 24,flash_data);//��ʾ�������ַ���
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
	if(res)//���յ�������
	{			
			LCD_ShowString(20, 320, 3 * 24, 24, 24, "Rev");
			LCD_ShowString(20+3*24, 320, 6 * 24, 24, 24, Can_Buf);
	}		
	LCD_ShowString(20+6*24, 248, 6 * 24, 24, 24, REMOTE_IBUF);

}