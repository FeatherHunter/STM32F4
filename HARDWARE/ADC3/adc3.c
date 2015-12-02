#include "adc3.h"
#include "delay.h"		 
/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.27
 * @filename adc3.c
 * @description ADC3
 * @FunctionList
 *		1.void  Adc3_Init(void); 								//ADC3ͨ����ʼ��
 *		2.u16  Get_Adc3(u8 ch); 								//���ĳ��ͨ��ֵ 
 *		3.u16 Get_Adc3_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
 */ 	


/**
 * @Function void  Adc3_Init(void)
 * @description ��ʼ��ADC	3
 * @Input 	void
 * @Return  void
 */														   
void  Adc3_Init(void)
{    
 
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE); //ʹ��ADC3ʱ��

  
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);	  //ADC3��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);	//��λ����	 
 
 
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;										//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; 		//DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;									//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;											//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;																//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;													//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;											//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;																	//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC3, &ADC_InitStructure);//ADC��ʼ��
	
 
	ADC_Cmd(ADC3, ENABLE);//����ADת����	 
}				  

/**
 * @Function u16 Get_Adc3(u8 ch)   
 * @description ���ADCֵ
 * @Input 	u8: ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
 * @Return  u16:ת�����
 */
u16 Get_Adc3(u8 ch)   
{
	 //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC3, ch, 1, ADC_SampleTime_480Cycles );	//ADC3,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC3);		//ʹ��ָ����ADC3�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC3);	//�������һ��ADC3�������ת�����
}
 







