#ifndef __COMMON_H__
#define __COMMON_H__	 
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "lcd.h"   
#include "w25qxx.h" 	 
#include "touch.h" 	 
#include "malloc.h"
#include "string.h"    
#include "text.h"		
#include "usart3.h" 
#include "ff.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
#define AT_MODE_CONFIG		0				//0,采用ES/RST硬件控制进入AT模式;
											//1,采用串口特殊序列(+++1B1B1B)进入AT模式
//ES/RST信号控制IO口.
#define ES_CTRL				PFout(6)		//ES/RST控制引脚
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void atk_rm04_init(void);                  //atk_rm04 初始化

void atk_rm04_at_response(u8 mode);      							//将收到的AT指令应答数据返回给电脑串口
u8* atk_rm04_check_cmd(u8 *str);											//ATK-RM04发送命令后,检测接收到的应答
u8 atk_rm04_send_cmd(u8 *cmd,u8 *ack,u16 waittime); 	//向ATK-RM04发送命令
u8 atk_rm04_quit_trans(void);													//ATK-RM04退出透传模式
u8 atk_rm04_consta_check(void);												//获取ATK-RM04模块的连接状态
void atk_rm04_load_keyboard(u16 x,u16 y);							//加载键盘界面（尺寸为240*140）
void atk_rm04_key_staset(u16 x,u16 y,u8 keyx,u8 sta); //按键状态设置
u8 atk_rm04_get_keynum(u16 x,u16 y);									//得到触摸屏的输入
void atk_rm04_get_wanip(u8* ipbuf);										//获取wan ip地址
u8 atk_rm04_get_wifista_state(void);									//获取WIFI STA连接状态(检查WIFI STA是否连上路由器)
void atk_rm04_msg_show(u16 x,u16 y,u8 wanip);					//ATK-RM04模块信息显示
void atk_rm04_wificonf_show(u16 x,u16 y,u8* rmd,u8* ssid,u8* encryption,u8* password);//ATK-RM04模块WIFI配置参数显示(仅WIFI STA/WIFI AP模式测试时使用)
u8 atk_rm04_netpro_sel(u16 x,u16 y,u8* name);					//工作模式选择
void atk_rm04_mtest_ui(u16 x,u16 y);									//测试界面主UI

u8 atk_rm04_ip_set(u8* title,u8* mode,u8* port,u8* ip);//IP设置
void atk_rm04_test(void);															 //ATK-RM04模块测试主函数



u8 atk_rm04_cometh_test(void);	//串口以太网测试
u8 atk_rm04_wifista_test(void);	//WIFI STA测试
u8 atk_rm04_wifiap_test(void);	//WIFI AP测试

//用户配置参数
extern const u8* portnum;			//连接端口
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA 加密方式
extern const u8* wifista_password; 	//WIFI STA 密码

extern const u8* wifiap_ssid;		//WIFI AP SSID
extern const u8* wifiap_encryption;	//WIFI AP 加密方式
extern const u8* wifiap_password; 	//WIFI AP 密码

extern u8* kbd_fn_tbl[2];
extern const u8* ATK_RM04_NETMODE_TBL[4];
extern const u8* ATK_RM04_WORKMODE_TBL[4];
#endif





