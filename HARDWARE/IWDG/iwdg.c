/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.23
 * @filename iwdg.c
 * @description �������Ź��������
 * @FunctionList
 *		1.void IWDG_Init(u8 prer,u16 rlr);
 *    2.void IWDG_Feed(void)
 *    
 */ 
 
#include "iwdg.h"

/**
 * @Function void IWDG_Init(u8 prer,u16 rlr);
 * @description ��ʼ���������Ź�
 *							��Ƶ����=4*2^prer.�����ֵֻ����256!
 * 							ʱ�����(���):Tout=((4*2^prer)*rlr)/32 (ms).
 * @Input: u8 prer  ��Ƶ��:0~7(ֻ�е�3λ��Ч!)
 *         u16 rlr  �Զ���װ��ֵ,0~0XFFF,��11λ��Ч.
 * @Return void
 *
 */
void IWDG_Init(u8 prer,u16 rlr)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //ʹ�ܶ�IWDG->PR IWDG->RLR��д
	
	IWDG_SetPrescaler(prer); 										  //����IWDG��Ƶϵ��

	IWDG_SetReload(rlr);     											//����IWDGװ��ֵ

	IWDG_ReloadCounter();												  //reload
	
	IWDG_Enable();       													//ʹ�ܿ��Ź�
}

/**
 * @Function void IWDG_Feed(void);
 * @description ι�������Ź�
 */
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}
