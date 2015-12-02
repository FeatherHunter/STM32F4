/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.26
 * @filename adc.c
 * @description ADC
 * @FunctionList
 *		1.void Adc_Init(void); 								//ADCͨ����ʼ��
 *		2.u16  Get_Adc(u8 ch); 								//���ĳ��ͨ��ֵ 
 *		3.u16 Get_Adc_Average(u8 ch,u8 times);//�õ�ĳ��ͨ����������������ƽ��ֵ  
 *		4.short Get_Temprate(void);           //�õ��ڲ��¶�
 */ 
#include "adc.h"
#include "delay.h"		 

/**
 * @Function void  Adc_Init(void)
 * @description ��ʼ��ADC	
 * @Input 	void
 * @Return  void
 */
void  Adc_Init(void)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

  //�ȳ�ʼ��ADC1ͨ��5 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;							//PA5 ͨ��5
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;					//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;			//����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);								//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
	ADC_TempSensorVrefintCmd(ENABLE);                     //�����ڲ��¶ȴ���������λ
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;								//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;							//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);																//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;					//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;						//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);										//ADC��ʼ��
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); //ADC16,ADCͨ��,480������,��߲���ʱ�������߾���
 
	ADC_Cmd(ADC1, ENABLE);//����ADת����	

}				  

/**
 * @Function u16 Get_Adc(u8 ch) 
 * @description ���ADCֵ
 * @Input 	u8: ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
 * @Return  u16:ת�����
 */
u16 Get_Adc(u8 ch)   
{
	u16 wait = 1000;
  //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC1);										//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )&& wait) wait--;	//�ȴ�ת������

	if(wait > 0)return ADC_GetConversionValue(ADC1);						//�������һ��ADC1�������ת�����
	return 0;
}

/**
 * @Function u16 Get_Adc_Average(u8 ch,u8 times)
 * @description ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
 * @Input 	u8: ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
 *					U8: times:��ȡ����
 * @Return  u16:ͨ��ch��times��ת�����ƽ��ֵ
 */
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 

/**
 * @Function short Get_Temprate(void)
 * @description �õ��ڲ��¶�
 * @Input 	void
 * @Return  short:�¶�ֵ(����100��,��λ����C)
 */
short Get_Temprate(void)
{
	u32 adcx;
	short result;
 	double temperate;
	adcx=Get_Adc_Average(ADC_Channel_16,10);	//��ȡͨ��16��10��
	temperate=(float)adcx*(3.3/4096);		//��ѹֵ
	temperate=(temperate-0.76)/0.0025 + 25; //ת��Ϊ�¶� 
	result=temperate*=100;					//����100��
	return result;
}
	 









