#include "main.h"
#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "stdio.h"

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

u8 flush_key = 1;
u8 lcd_id[12];
u8 CanMode;

#if 1

void functions_init(); //��ʼ�����й���

void ctp_test(void)
{
	
}
int main(void)
{
	u8 key;
	u8 u3_bufcount;
	u8 i = 0;
	u8 ci;
	u8 itouch;
	u8 t = 0;
	//u8 temp = 0, humi = 0;
	u32 data;
	u32 data_rng;
	long long temp;
	u8 role = 0xFF;
	
	u8 RTC_DBUF[20];
	u8 RTC_TBUF[20];
	
	short ADC_temperature;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //NVIC
	uart_init(115200);
	delay_init(168); //systick = sysCLK/8,50us = 50 * 21 systick.if use 84,1usʵ����Ϊ1/2us
	
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
	//while(HC05_Init())//9600bound
	//{
	//	printf("HC05 ERROR!\n");      //==HC05==
	//	delay_ms(200);
	//}
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
			
			remote_ctrl(); //�����߿���
			
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
	  
		}//end of while
	}//end of else
}

void functions_init()
{
	LED_Init();            //LED
	LCD_Init();            //LCD
	KEY_Init();            //KEY
	
	POINT_COLOR = BRRED;
	LCD_ShowString(10+100,0, 24*20, 24, 24, "WELCOME to My Life");
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*1, 200, 24, 24, "LCD");
	sprintf((char *)lcd_id, "ID: %x", lcddev.id);
	LCD_ShowString(200,24*1, 200, 24, 24, lcd_id);
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*1, 200, 24, 24, "OK");
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*2, 200, 24, 24, "RNG");
	POINT_COLOR = RED;
	while(RNG_Init())      //RNG
	{
		LCD_ShowString(400,24*2, 200, 24, 24, "NO");
	}
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*2, 200, 24, 24, "OK");
  POINT_COLOR = BLUE;
	LCD_ShowString(10,24*3, 200, 24, 24, "LED");
	LCD_ShowString(10,24*4, 200, 24, 24, "KEY");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*3, 200, 24, 24, "OK");
	LCD_ShowString(400,24*4, 200, 24, 24, "OK");
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*5, 200, 24, 24, "TOUCH");
	POINT_COLOR = RED;
	while(tp_dev.init()) //��������ʼ��
	{
		LCD_ShowString(400,24*5, 200, 24, 24, "NO");
	}
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*5, 200, 24, 24, "OK");
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*6, 200, 24, 24, "RTC");
	My_RTC_Init(); //RTC Init
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits, 0); //����WAKE UP�ж�,1�����ж�һ��
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*6, 200, 24, 24, "OK"); 

	Adc_Init();	  //ADC ��ʼ��
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*7, 200, 24, 24, "ADC");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*7, 200, 24, 24, "OK");	
	
	Remote_Init();                                 //Infrared Remote Control
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*8, 200, 24, 24, "Infrared");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*8, 200, 24, 24, "OK");
	
	CanMode = CAN_Mode_LoopBack;
	CAN1_Mode_Init(CAN_SJW_1tq, CAN_BS2_6tq, CAN_BS1_7tq, 6, CanMode);//�ػ�ģʽ
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*9, 200, 24, 24, "Can");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*9, 200, 24, 24, "OK");
	POINT_COLOR = MAGENTA;
	if(CanMode == CAN_Mode_LoopBack)
	{
		LCD_ShowString(240,24*5, 200, 24, 24, "LoopBack");
	}
	else
	{
		LCD_ShowString(240,24*5, 200, 24, 24, "Normal");
	}
	
	BEEP_Init();
	POINT_COLOR = BLUE;
	LCD_ShowString(10,24*10, 200, 24, 24, "BEEP");
	POINT_COLOR = GREEN;
	LCD_ShowString(400,24*10, 200, 24, 24, "OK");	
	
	delay_ms(1500);

}
#endif