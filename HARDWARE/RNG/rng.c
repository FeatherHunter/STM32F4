#include "rng.h"
#include "delay.h"
/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.23
 * @filename rng.c
 * @description Ӳ�������������
 * @FunctionList
 *		1.u8 RNG_Init(void);
 *		2.u32 RNG_Get_RandomNum(void)
 *		3.int RNG_Get_RandomRange(int min,int max)
 */ 

/**
 * @Function u8 RNG_Init(void);
 * @Description Init_RNG
 * @Return 0:SUCESS
 *				 1:Error 
 */
u8 RNG_Init(void)
{
	u16 retry=0; 
	
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);//����RNGʱ��,����PLL48CLK
	
	RNG_Cmd(ENABLE);	//ʹ��RNG
	
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET&&retry<10000)	//�ȴ����������
	{
		retry++;
		delay_us(100);
	}
	if(retry>=10000)return 1;//���������������������
	return 0;
}

/**
 * @Function u32 RNG_Get_RandomNum(void);
 * @Description Get_RandomNum
 * @Return u32 �õ������
 */
u32 RNG_Get_RandomNum(void)
{	 
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET);	//�ȴ����������  
	return RNG_GetRandomNumber();	
}

/**
 * @Function int RNG_Get_RandomRange(int min,int max);
 * @Description Get_RandomRange
 * @Input: int min: ����
 *         int max: ����
 * @Return u32 �õ������
 */
int RNG_Get_RandomRange(int min,int max)
{ 
   return RNG_Get_RandomNum()%(max-min+1) +min;
}



