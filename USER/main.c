#include "main.h"
#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>

#include "led.h"
#include "beep.h"
#include "key.h"
//#include "exti.h"
//#include "iwdg.h"
//#include "wwdg.h"
#include "timer.h"
#include "lcd.h"
//#include "rng.h"
//#include "pwm.h"
//#include "dht11.h"
#include "hc05.h"
#include "usart3.h"
#include "rtc.h"
#include "adc.h"
#include "dac.h"
#include "pwmdac.h"
#include "adc3.h"
#include "lsens.h"
#include "tpad.h"
#include "rng.h" 

#include "graphic.h"
#include "touch.h"
#include "remote.h"
#include "can.h"
#include "infrared_remote_ctrl.h"
#include "stmflash.h"

#include "w25qxx.h"
#include "bluetooth.h"
#include "rs485.h"
#include "sram.h"
#include "usmart.h"
#include "malloc.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "exfuns.h"
//#include "font.h"
#include "text.h"
#include "piclib.h"
#include "common.h"

//FWIP
#include "netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_client_demo.h"
#include "tcp_client_demo.h"
#include "tcp_server_demo.h"
#include "udp_demo.h"
#include "httpd.h"

/********������� FATFS*********/
FIL fil;
FRESULT res;
UINT bww;
char buf[100];

u8 flush_key = 1;
u8 lcd_id[12];
u8 CanMode;
u16 w25q_id;

#if 1

void functions_init(); //��ʼ�����й���

void ctp_test(void)
{
	
}
//ͨ�����ڴ�ӡSD�������Ϣ
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//������ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//����Ե�ַ
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//��ʾ����
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//��ʾ���С
}

int main(void)
{
	u8 itouch;
	u8 connect_flag = 0;
	u8 login_flag = 0;
	u8 res;
	u8 * fn;
	u8 * pname;
	DIR picdir;
	FILINFO picfileinfo;
	//u8 temp = 0, humi = 0;
	u32 data_rng;
	//u8 role = 0xFF;
	
	struct tcp_pcb *tcppcb;  	//����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ
	
	u8 RTC_DBUF[20];
	u8 RTC_TBUF[20];
	
	short ADC_temperature;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //NVIC
	uart_init(115200);
	delay_init(168); //systick = sysCLK/8,50us = 50 * 21 systick.if use 84,1usʵ����Ϊ1/2us
	usmart_dev.init(84);  //84?why?	
	functions_init();
	
	//Dac1_Init();
	//TIM9_CH2_PWM_Init(255,TIM_CKD_DIV1);

	///////////////////////////////////////////////////////
	
	//EXTIX_Init(); //�ⲿ�ж�
	//TIM3_Int_Init(9999, 8399); //��ʱ��ʱ��84MHz
	
	//IWDG_Init(5, 500); //�������Ź�LSI
	//WWDG_Init(0x70,0x50, WWDG_Prescaler_8);//��Ƶ8
	///////////////////////////////////////////////////////////////
	//	TIM14_PWM_Init(500-1,84-1);	//84M/84=1Mhz����Ƶ��,��װ��ֵ500,����PWMƵ��Ϊ 1M/500=2Khz. 
  ///////////////////////////////////////////////////////////////
	//while(DHT11_Init())
	//{
	//	printf("not found DHT11!\n");      ==DHT11==
	//	delay_ms(200);
	//}
	///////////////////////////////////////////////////////////////
	//printf("HC05: Slave!\n");
	//HC05_Set_Cmd("AT+ROLE=0");
	///////////////////////////////////////////////////////////////
	//TIM_SetCompare2(TIM9, 20);
	///////////////////////////////////////////////////////////////
	//Lsens_Init();                              ==LSENSOR,TPAD==
	//TPAD_Init(16);
  //////////////////////////////////////////////////////////////////////////////
	//TIM5_CH1_Cap_Init(0XFFFFFFFF, 84-1); //84MHz/84 = 1Mhz 1Mhz/500 = 2Khz  ==IC==
	///////////////////////////////////////////////////////////////////////////////
	piclib_init();
	tcp_client_sendbuf = mymalloc(SRAMIN, 100);
	//picfileinfo.lfsize = _MAX_LFN * 2 + 1;
	//picfileinfo.lfname = mymalloc(SRAMIN, picfileinfo.lfsize);
	//pname = mymalloc(SRAMIN, picfileinfo.lfsize);
	
	if(tp_dev.touchtype & 0x80)
	{
		while(1)
		{
			if(flush_key)
			{
				gui_clear(WHITE);
				gui_draw_dialog(10, 10, 200, 28, 24, (u8 *)"LED0", BLUE, RED);
				gui_draw_dialog(10+200, 10, 200, 28, 24, (u8 *)"RNG", BLUE, RED);
				gui_draw_dialog(10, 38, 200, 28, 24, (u8 *)"Infrared Contrl", BLUE, RED);
				
				/*
				res = f_opendir(&picdir, (const TCHAR*)"0:/PICTURE"); //��Ŀ¼
				if(res == FR_OK)
				{
						res = f_readdir(&picdir, &picfileinfo);
						fn = (u8 *)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);
					  res = f_typetell(fn);
						if((res&0xF0) == 0x50)//�ж��ǲ���ͼƬ
						{
							strcpy((char *)pname, "0:/PICTURE/");
							strcat((char *)pname, (const char *)fn);
							ai_load_picfile(pname, 50, 50, 100, 100, 1);
							Show_Str(2,2,240,16,pname,16,1); 				//��ʾͼƬ����
						}
				}
				myfree(SRAMIN, pname);
				*/
				
				flush_key = 0;
			}
			tp_dev.scan(0);
			for(itouch = 0; itouch < OTT_MAX_TOUCH; itouch++)
			{
				if((tp_dev.sta)&(1<<itouch))
				{
					if((tp_dev.x[itouch] >= 10)&&(tp_dev.x[itouch] <= 10+200) //LED0
						&&(tp_dev.y[itouch] >= 10)&&(tp_dev.y[itouch] <= 10+28))
					{
						LED1 =!LED1;
					}
					else if((tp_dev.x[itouch] >= 210)&&(tp_dev.x[itouch] <= 210+200) //RNG
						&&(tp_dev.y[itouch] >= 10)&&(tp_dev.y[itouch] <= 10+28))
					{
						data_rng = RNG_Get_RandomRange(1, 6);
						POINT_COLOR = RED;
						LCD_ShowNum( 260, 12, data_rng, 16, 16);
						//delay_ms(1000);
					}
					else if((tp_dev.x[itouch] >= 10)&&(tp_dev.x[itouch] <= 10+200) //Infrared Remote Contrl
						&&(tp_dev.y[itouch] >= 38)&&(tp_dev.y[itouch] <= 38+28))
					{
						;
					}
				}
			}//end of for
			
			remote_ctrl();    //�����߿���
			bluetooth_deal(); //��������
			
			tp_dev.x[itouch] = 0;
			tp_dev.y[itouch] = 0;
			/*�õ����ں���ʱ��*/
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			sprintf(RTC_DBUF, "%4d/%2d/%2d week:%d", RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date, RTC_DateStruct.RTC_WeekDay);
	    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
			sprintf(RTC_TBUF, "%2d:%2d:%2d", RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
			POINT_COLOR = BLACK;
			LCD_ShowString(10, lcddev.height - 48, 300, 24, 24, RTC_DBUF);
			LCD_ShowString(310, lcddev.height - 48, 300, 24, 24, RTC_TBUF);
			/*�õ�CPU�¶�*/
			POINT_COLOR = BLUE;
			LCD_ShowString(20, lcddev.height - 72, 5 * 24, 24, 24, "CPU temp:");
			ADC_temperature = Get_Temprate();
			POINT_COLOR = RED;
			LCD_ShowNum(5*24+12, lcddev.height - 72, ADC_temperature/100, 2, 24);
			
		//if(TPAD_Scan(0))
		//{
		//	LED1 = !LED1;
		//	data = Lsens_Get_Val();              ==LSENSOR,TPAD==
		//	printf("LSENSOR:%d\n", data);
		//}
		//printf("TPAD INIT wait!\n");
		///////////////////////////////////////////////////////////
		//	data = TIM_GetCapture2(TIM9);
		//	printf("PWM val:%d ", data);                ==PWMDAC==
		//	printf("V: %f\n", (float)data*(3.3/256));
		//	data = Get_Adc_Average(ADC_Channel_5, 20);
		//	printf("adc val:%d ", data);
		//	printf("V: %f\n", (float)data*(3.3/4096));
		////////////////////////////////////////////////////////
		//Dac1_Set_Vol(2000);
		//data = Get_Adc_Average(ADC_Channel_5, 20);      ==ADC,DAC==
		//printf("adc val:%d ", data);
		//printf("V: %f\n", (float)data*(3.3/4096));
    //////////////////////////////////////////////////////
		//  u16 ledpwm = 0;
		//	if(++ledpwm == 500)
		//	{
		//		ledpwm = 0;                     ==PWM OC==
		//	}
		//	delay_ms(20);
		//	TIM_SetCompare1(TIM14, ledpwm);
		//////////////////////////////////////////////////////
		//DHT11_Read_Data(&temp, &humi);      ==DHT11==
		/////////////////////////////////////////////////////
		//key = KEY_Scan(0);
		//if(key == WKUP_PRES)
		//{                                   ==HC05==
		//	role = HC05_Get_Role();
		//	if(role != 0xFF)
		//	{
		//		if(role == 1) HC05_Set_Cmd("AT+ROLE=0");
		//		else HC05_Set_Cmd("AT+ROLE=1");
		//		role = HC05_Get_Role();
		//		role =!role;
		//		if(role == 1) printf("HC05: Master!\n");
		//		else if(role == 0) printf("HC05: Slave!\n");
		//    else printf("HC05 ERROR!%d\n", role);
		//		//HC05_Set_Cmd("AT+RESET");
		//	}	
		//}
		//if(USART3_RX_STA&0X8000)			//���յ�һ��������
		//{
		//	u3_bufcount = (USART3_RX_STA & 0X7FFF);
		//	USART3_RX_BUF[u3_bufcount] = 0;
		//	printf("���յ���");
		//	for(i = 0; i < u3_bufcount; i++)
		//	{
		//		printf("%c", USART3_RX_BUF[i]);
		//	}
    //
 		//	USART3_RX_STA=0;	 
		//}
		////////////////////////////////////////////////////////
		//RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct); ==RTC==
		//printf("year:%d month:%d date%d week%d\n", RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date, RTC_DateStruct.RTC_WeekDay);
	  //RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
		////////////////////////////////////////////////////////
		tcp_client_test(&connect_flag, &login_flag, &tcppcb, &rmtipaddr);
			
		}//end of while
	}//end of else
}

void functions_init()
{
	u32 total, free;
	u8 hc05_role;
	u8 res;
	
	u8 eth_speed;
	u8 eth_buf[50];

	LCD_Clear(BLACK);
	
	LED_Init();            //LED
	LCD_Init();            //LCD
	KEY_Init();            //KEY
	
	POINT_COLOR = BRRED;
	LCD_ShowString(10+100,0, 24*20, 24, 24, "WELCOME to My Life");
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*1, 200, 16, 16, "LCD");
	sprintf((char *)lcd_id, "ID: %x", lcddev.id);
	LCD_ShowString(330,16*1, 200, 16, 16, lcd_id);
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*1, 200, 16, 16, "OK");
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*2, 200, 16, 16, "RNG");
	POINT_COLOR = RED;
	while(RNG_Init())      //RNG
	{
		LCD_ShowString(400,16*2, 200, 16, 16, "NO");
	}
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*2, 200, 16, 16, "OK");
  POINT_COLOR = BLUE;
	LCD_ShowString(10,16*3, 200, 16, 16, "LED");
	LCD_ShowString(10,16*4, 200, 16, 16, "KEY");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*3, 200, 16, 16, "OK");
	LCD_ShowString(400,16*4, 200, 16, 16, "OK");
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*5, 200, 16, 16, "TOUCH");
	POINT_COLOR = RED;
	while(tp_dev.init()) //��������ʼ��
	{
		LCD_ShowString(400,16*5, 200, 16, 16, "NO");
	}
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*5, 200, 16, 16, "OK");
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*6, 200, 16, 16, "RTC");
	My_RTC_Init(); //RTC Init
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits, 0); //����WAKE UP�ж�,1�����ж�һ��
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*6, 200, 16, 16, "OK"); 

	Adc_Init();	  //ADC ��ʼ��
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*7, 200, 16, 16, "ADC");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*7, 200, 16, 16, "OK");	
	
	Remote_Init();                                 //Infrared Remote Control
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*8, 200, 16, 16, "Infrared");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*8, 200, 16, 16, "OK");
	
	CanMode = CAN_Mode_LoopBack;
	CAN1_Mode_Init(CAN_SJW_1tq, CAN_BS2_6tq, CAN_BS1_7tq, 6, CanMode);//�ػ�ģʽ
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*9, 200, 16, 16, "Can");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*9, 200, 16, 16, "OK");
	POINT_COLOR = MAGENTA;
	if(CanMode == CAN_Mode_LoopBack)
	{
		LCD_ShowString(240,16*5, 200, 16, 16, "LoopBack");
	}
	else
	{
		LCD_ShowString(240,16*5, 200, 16, 16, "Normal");
	}
  /*beep init*/
	BEEP_Init();
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*10, 200, 16, 16, "BEEP");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*10, 200, 16, 16, "OK");	
	
	/*w25qxx init*/
	W25QXX_Init();
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*11, 200, 16, 16, "SPI FLASH");
	while(W25QXX_ReadID() != W25Q128)
	{
		POINT_COLOR = RED;
		LCD_ShowString(170,16*11, 240, 16, 16,  " W25Q128 Check Failed");
		delay_ms(250);
		LCD_ShowString(170,16*11, 240, 16, 16,  " Please Check .......");
		delay_ms(250);
	}
	POINT_COLOR = GREEN;
	LCD_ShowString(170,16*11, 240, 16, 16,   "                     ");	
	LCD_ShowString(400,16*11, 200, 16, 16, "OK");
	
	
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*12, 200, 16, 16, "HC05");
	while(HC05_Init())     //9600 baund HC05
	{
		POINT_COLOR = RED;
		LCD_ShowString(170,16*12, 240, 16, 16,  " HC05 Check Failed");
		delay_ms(250);
		LCD_ShowString(170,16*12, 240, 16, 16,  " Please Check ....");
		delay_ms(250);
	}
	POINT_COLOR = GREEN;
	LCD_ShowString(170,16*12, 240, 16, 16,   "                     ");	
	LCD_ShowString(400,16*12, 200, 16, 16, "OK");
	hc05_role = HC05_Get_Role();
	POINT_COLOR = BLUE;
	if(hc05_role == 1) 
	{
		LCD_ShowString(300,16*12, 200, 16, 16,  "MASTER");
	}
	else if(hc05_role == 0)
	{
		LCD_ShowString(300,16*12, 200, 16, 16,  "SLAVE");
	}
	else
	{
		POINT_COLOR = RED;
		LCD_ShowString(300,16*12, 200, 16, 16,  "0xFF");
	}
	
	/*RS485 init*/
	RS485_Init(9600);
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*13, 200, 16, 16, "RS485");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*13, 200, 16, 16, "OK");	
	
	/*FSMC SRAM*/
	FSMC_SRAM_Init();
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*14, 200, 16, 16, "FSMC SRAM");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*14, 200, 16, 16, "OK");
	
	/*MEM MANEGEMENT*/
	my_mem_init(SRAMIN);
	my_mem_init(SRAMEX);
	my_mem_init(SRAMCCM);
	
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*15, 200, 16, 16, "MEM MANEGEMENT");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*15, 200, 16, 16, "OK");
	
	/*SD CARD*/
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*16, 200, 16, 16, "SD CARD");
	while(SD_Init())//��ⲻ��SD��
	{
		POINT_COLOR = RED;
		LCD_ShowString(170,16*16, 240, 16, 16,  " SD Card Error!   ");
		delay_ms(500);					
		LCD_ShowString(170,16*16, 240, 16, 16,  " Please Check ....");
		delay_ms(500);
	}
	POINT_COLOR = GREEN;
	LCD_ShowString(170,16*16, 240, 16, 16,   "                     ");	
	LCD_ShowString(400,16*16, 200, 16, 16, "OK");
	show_sdcard_info();	//��ӡSD�������Ϣ
	
	/****FATFS***/
	exfuns_init(); //Ϊfatfs��ر��������ڴ�
	f_mount(fs[0], "0:", 1);   //����SD��
	res = f_mount(fs[1], "1:", 1);   //����flash
	if(res==0X0D)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		LCD_ShowString(10,16*17, 200, 16, 16,"Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//����Flash���̵�����Ϊ��ALIENTEK
			LCD_ShowString(10,16*17, 200, 16, 16,    "Flash Disk Format Finish");	//��ʽ�����
		}else LCD_ShowString(10,16*17, 200, 16, 16,"Flash Disk Format Error ");	//��ʽ��ʧ��
		delay_ms(1000);
	}													    
	LCD_ShowString(10,16*17, 200, 16, 16,        "                        ");	//���			  
	while(exf_getfree("0",&total,&free))	//�õ�SD������������ʣ������
	{
		LCD_ShowString(30,16*17,200,16,16,"SD Card Fatfs Error!");
		delay_ms(200);
		LCD_Fill(30,16*17,240,16*17+16,WHITE);	//�����ʾ			  
		delay_ms(200);
		LED0=!LED0;//DS0��˸
	}													  			    
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*17, 200, 16, 16, "FATFS");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,16*17, 200, 16, 16, "OK");	
	POINT_COLOR = BLACK;
	LCD_ShowString(10,16*18,200,16,16,"SD Total Size:     MB");	 
	LCD_ShowString(10,16*19,200,16,16,"SD  Free Size:     MB"); 	
	POINT_COLOR = MAGENTA;	
 	LCD_ShowNum(10+8*14,16*18,total>>10,5,16);				//��ʾSD�������� MB
 	LCD_ShowNum(10+8*14,16*19,free>>10,5,16);					//��ʾSD��ʣ������ MB		
	
	/*font prepare*/
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*20, 200, 16, 16, "FONT");
	while(font_init())//��ⲻ��font
	{
		POINT_COLOR = RED;
		LCD_ShowString(170,16*20, 240, 16, 16,  "FONT     Error!   ");
		delay_ms(500);					
		LCD_ShowString(170,16*20, 240, 16, 16,  " Please Check ....");
		delay_ms(500);
	}
	POINT_COLOR = GREEN;
	Show_Str(300,16*20, 240, 16, "     �ֿ��ʼ���ɹ�", 16, 0);
	
	/* ATK-RM04 */
	/*
	atk_rm04_init();
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*21, 200, 24, 24, "ATK-RM04");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*21, 200, 24, 24, "OK");
	*/
	
	/*Init LWIP*/
	TIM3_Int_Init(999,839); //100khz��Ƶ��,����1000Ϊ10ms
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*21, 200, 16, 16, "lwIP Init");
	while(lwip_comm_init()!=0) //�ȳ�ʼ��lwIP(����LAN8720��ʼ��),��ʱ�����������,�����ʼ����ʧ��!! 
	{
		POINT_COLOR = RED;
		LCD_ShowString(200,16*21, 200, 16, 16, "lwIP Init failed!");
		delay_ms(1200);
		LCD_Fill(200,16*21, 400,16*21+16,WHITE);//�����ʾ
		LCD_ShowString(200,16*21, 200, 16, 16,"Retrying...");  
	}
	LCD_Fill(200,16*21, 400,16*21+16,WHITE);//�����ʾ
	POINT_COLOR = GREEN;
	LCD_ShowString(350,16*21, 200, 16, 16,"Successed");
	//�ȴ�DHCP��ȡ 
	POINT_COLOR = BLUE;
	LCD_ShowString(10,16*22, 200, 16, 16, "DHCP IP config");
	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{
		lwip_periodic_handle();
	}
	POINT_COLOR = GREEN;
	LCD_ShowString(350,16*22, 200, 16, 16,"Successed");
	if(lwipdev.dhcpstatus==2)sprintf((char*)eth_buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
	else sprintf((char*)eth_buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
	LCD_ShowString(10,16*23, 200, 16, 16,eth_buf); 
	eth_speed=LAN8720_Get_Speed();										//�õ�����
	POINT_COLOR = BLACK;
	if(eth_speed&1<<1)LCD_ShowString(10,16*23, 200, 16, 16,"Ethernet Speed:100M   ");
	else LCD_ShowString(10,16*23, 200, 16, 16,"Ethernet Speed:10M   "); 
	//httpd_init();	//HTTP��ʼ��(Ĭ�Ͽ���websever)
	
	/*set up tcp/ip*/
	lwipdev.remoteip[0] = 192;
	lwipdev.remoteip[1] = 168;
	lwipdev.remoteip[2] = 16;
	lwipdev.remoteip[3] = 107;
	sprintf((char*)eth_buf,"Local IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//����IP
	LCD_ShowString(10,16*24, 200, 16, 16,eth_buf);  
	sprintf((char*)eth_buf,"Remote IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//Զ��IP
	LCD_ShowString(10,16*25, 200, 16, 16,eth_buf);  
	sprintf((char*)eth_buf,"Remote Port:%d",TCP_CLIENT_PORT);//�ͻ��˶˿ں�
	LCD_ShowString(10,16*26, 200, 16, 16,eth_buf);

}
#endif
