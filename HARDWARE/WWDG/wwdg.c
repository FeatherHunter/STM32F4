/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.23
 * @filename wwdg.c
 * @description ���ڿ��Ź��������
 * @FunctionList
 *		1.void WWDG_Init(u8 tr,u8 wr,u32 fprer); ���ڿ��Ź���ʼ��
 *    2.void WWDG_IRQHandler(void);            �жϷ�����
 *    
 */ 
#include "wwdg.h"
#include "led.h"


//����WWDG������������ֵ,Ĭ��Ϊ���. 
u8 WWDG_CNT=0X7F; 	

/**
 * @Function void WWDG_Init(u8 tr,u8 wr,u32 fprer);
 * @description ��ʼ�����ڿ��Ź�
 *							Fwwdg=PCLK1/(4096*2^fprer). һ��PCLK1=42Mhz
 * @Input:  u8  tr   :T[6:0],������ֵ 
 *					u8  wr   :W[6:0],����ֵ 
 *          u32 fprer:��Ƶϵ����WDGTB��,�����2λ��Ч 
 * @Return void
 *
 */
void WWDG_Init(u8 tr,u8 wr,u32 fprer)
{
 
	NVIC_InitTypeDef NVIC_InitStructure;
 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE); //ʹ�ܴ��ڿ��Ź�ʱ��
	
	WWDG_CNT=tr&WWDG_CNT;   														//��ʼ��WWDG_CNT. 
	WWDG_SetPrescaler(fprer); 													//���÷�Ƶֵ
	WWDG_SetWindowValue(wr); 														//���ô���ֵ
//	WWDG_SetCounter(WWDG_CNT);//���ü���ֵ
	WWDG_Enable(WWDG_CNT);  														//�������Ź�
	
	NVIC_InitStructure.NVIC_IRQChannel=WWDG_IRQn;  			//���ڿ��Ź��ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;  //��ռ���ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;					//�����ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  							//ʹ�ܴ��ڿ��Ź�
	NVIC_Init(&NVIC_InitStructure);
	
	WWDG_ClearFlag();																		//�����ǰ�����жϱ�־λ
  WWDG_EnableIT();																		//������ǰ�����ж�
}

/**
 * @Function void WWDG_IRQHandler(void);
 * @description ���ڿ��Ź��жϷ������ 
 */
void WWDG_IRQHandler(void)
{
	WWDG_SetCounter(WWDG_CNT); //���贰�ڿ��Ź�ֵ
	WWDG_ClearFlag();					//�����ǰ�����жϱ�־λ
	LED1=!LED1;
}
