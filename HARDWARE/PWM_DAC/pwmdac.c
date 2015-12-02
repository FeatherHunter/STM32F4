/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.26
 * @filename pwmdac.c
 * @description DAC
 * @FunctionList
 *		1.void TIM9_CH2_PWM_Init(u16 arr,u16 psc);	��ʼ��TIM9_CH2ΪPWM
 */ 
#include "pwmdac.h"
#include "led.h"
#include "usart.h" 
 
/**
 * @Function void TIM9_CH2_PWM_Init(u16 arr,u16 psc);
 * @description ��ʼ��TIM9 CH2 ΪPWM
 * @Input 	u16 arr:�Զ���װ��ֵ
 *          u16 psc:prescaler
 * @Return  void
 */
void TIM9_CH2_PWM_Init(u16 arr,u16 psc)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	//TIM9ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //ʹ��PORTAʱ��	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 						//GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 				//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//����
	GPIO_Init(GPIOA,&GPIO_InitStructure); 								//��ʼ��PA3
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM9); //GPIOA3����λ��ʱ��9 AF3

	TIM_TimeBaseStructure.TIM_Prescaler=psc;  								//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   									//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);						//��ʼ����ʱ��9

	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 						//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OC2Init(TIM9, &TIM_OCInitStructure);  										//����Tָ���Ĳ�����ʼ������TIM9 OC2

	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);  						//ʹ��TIM9��CCR2�ϵ�Ԥװ�ؼĴ���
 
  TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPEʹ�� 
	
	TIM_Cmd(TIM9, ENABLE);  //ʹ��TIM9
	
 
} 
