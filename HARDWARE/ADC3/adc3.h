#ifndef __ADC3_H
#define __ADC3_H	
#include "sys.h" 
 							   
void Adc3_Init(void); 				      //ADC3ͨ����ʼ��
u16  Get_Adc3(u8 ch); 				      //ADC3���ĳ��ͨ��ֵ 
u16 Get_Adc3_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
#endif 















