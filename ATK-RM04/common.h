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
#define AT_MODE_CONFIG		0				//0,����ES/RSTӲ�����ƽ���ATģʽ;
											//1,���ô�����������(+++1B1B1B)����ATģʽ
//ES/RST�źſ���IO��.
#define ES_CTRL				PFout(6)		//ES/RST��������
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void atk_rm04_init(void);                  //atk_rm04 ��ʼ��

void atk_rm04_at_response(u8 mode);      							//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
u8* atk_rm04_check_cmd(u8 *str);											//ATK-RM04���������,�����յ���Ӧ��
u8 atk_rm04_send_cmd(u8 *cmd,u8 *ack,u16 waittime); 	//��ATK-RM04��������
u8 atk_rm04_quit_trans(void);													//ATK-RM04�˳�͸��ģʽ
u8 atk_rm04_consta_check(void);												//��ȡATK-RM04ģ�������״̬
void atk_rm04_load_keyboard(u16 x,u16 y);							//���ؼ��̽��棨�ߴ�Ϊ240*140��
void atk_rm04_key_staset(u16 x,u16 y,u8 keyx,u8 sta); //����״̬����
u8 atk_rm04_get_keynum(u16 x,u16 y);									//�õ�������������
void atk_rm04_get_wanip(u8* ipbuf);										//��ȡwan ip��ַ
u8 atk_rm04_get_wifista_state(void);									//��ȡWIFI STA����״̬(���WIFI STA�Ƿ�����·����)
void atk_rm04_msg_show(u16 x,u16 y,u8 wanip);					//ATK-RM04ģ����Ϣ��ʾ
void atk_rm04_wificonf_show(u16 x,u16 y,u8* rmd,u8* ssid,u8* encryption,u8* password);//ATK-RM04ģ��WIFI���ò�����ʾ(��WIFI STA/WIFI APģʽ����ʱʹ��)
u8 atk_rm04_netpro_sel(u16 x,u16 y,u8* name);					//����ģʽѡ��
void atk_rm04_mtest_ui(u16 x,u16 y);									//���Խ�����UI

u8 atk_rm04_ip_set(u8* title,u8* mode,u8* port,u8* ip);//IP����
void atk_rm04_test(void);															 //ATK-RM04ģ�����������



u8 atk_rm04_cometh_test(void);	//������̫������
u8 atk_rm04_wifista_test(void);	//WIFI STA����
u8 atk_rm04_wifiap_test(void);	//WIFI AP����

//�û����ò���
extern const u8* portnum;			//���Ӷ˿�
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA ���ܷ�ʽ
extern const u8* wifista_password; 	//WIFI STA ����

extern const u8* wifiap_ssid;		//WIFI AP SSID
extern const u8* wifiap_encryption;	//WIFI AP ���ܷ�ʽ
extern const u8* wifiap_password; 	//WIFI AP ����

extern u8* kbd_fn_tbl[2];
extern const u8* ATK_RM04_NETMODE_TBL[4];
extern const u8* ATK_RM04_WORKMODE_TBL[4];
#endif





