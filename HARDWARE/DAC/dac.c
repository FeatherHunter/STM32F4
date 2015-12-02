/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.26
 * @filename dac.c
 * @description DAC
 * @FunctionList
 *		1.void Dac1_Init(void);		    //DACͨ��1��ʼ��	 	 
 *		2.void Dac1_Set_Vol(u16 vol);	//����ͨ��1�����ѹ
 */ 
#include "dac.h"

/**
 * @Function void Dac1_Init(void);
 * @description DACͨ��1�����ʼ��
 *            PA4 DAC_CH1
 * @Input 	void
 * @Return  void
 */
void Dac1_Init(void)
{  
  GPIO_InitTypeDef  GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);	//ʹ��DACʱ��
	   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;	//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);					//��ʼ��

	DAC_InitType.DAC_Trigger=DAC_Trigger_None;												//��ʹ�ô������� TEN1=0
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;					//��ʹ�ò��η���
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//���Ρ���ֵ����
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;					//DAC1�������ر� BOFF1=1
  DAC_Init(DAC_Channel_1,&DAC_InitType);	 				//��ʼ��DACͨ��1

	DAC_Cmd(DAC_Channel_1, ENABLE);  								//ʹ��DACͨ��1
  
  DAC_SetChannel1Data(DAC_Align_12b_R, 0);  			//12λ�Ҷ������ݸ�ʽ����DACֵ
}

/**
 * @Function void Dac1_Set_Vol(u16 vol);
 * @description ����ͨ��1�����ѹ
 * @Input 	u16 vol:0~3300,����0~3.3V
 * @Return  void
 */
void Dac1_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}





















































