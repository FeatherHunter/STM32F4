#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

extern u8  TIM5CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ 

void TIM3_Int_Init(u16 arr,u16 psc); //��ʱ��3��ʼ��
void TIM7_Int_Init(u16 arr,u16 psc); //��ʱ��7��ʼ��
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
void TIM14_PWM_Init(u32 arr,u32 psc);
#endif
