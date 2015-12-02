/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.23
 * @filename timer.c
 * @description ��ʱ���������
 * @FunctionList
 *		1.void TIM3_Int_Init(u16 arr,u16 psc); //Init
 *    2.void TIM3_IRQHandler(void)           //�жϷ�����
 *    3.void TIM7_Int_Init(u16 arr,u16 psc); 
 *    4.void TIM7_IRQHandler(void);
 */ 

#include "timer.h"
#include "led.h"

//=======================================================
//                     TIM3
//=======================================================
/**
 * @Function void TIM3_Int_Init(u16 arr,u16 psc);
 * @description ��ʼ����ʱ��3
 *							��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
 *							Ft=��ʱ������Ƶ��,��λ:Mhz
 * @Input: u16 arr  �Զ���װֵ
 *         u16 psc  ʱ��Ԥ��Ƶ��
 * @Return void
 *
 */
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  					//ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 									//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  								//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);						//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 										  //������ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); 																			  //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 							//��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 	//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 				//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/**
 * @Function    void TIM3_IRQHandler(void);
 * @description TIM3�жϷ������ 
 */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		LED1=!LED1;//DS1��ת
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}

//=======================================================
//                     TIM5
//=======================================================
TIM_ICInitTypeDef  TIM5_ICInitStructure;

/**
 * @Function void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
 * @description ��ʱ��5ͨ��1���벶������
 *                PA0-WKUP-TIM5_CH1 ���벶��
 * @Input: u32 arr  arr���Զ���װֵ(TIM2,TIM5��32λ��!!)
 *         u16 psc  ʱ��Ԥ��Ƶ��
 * @Return void
 *
 */
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIM5ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTAʱ��	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 					//GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 			//����
	GPIO_Init(GPIOA,&GPIO_InitStructure); 							//��ʼ��PA0

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); //PA0����λ��ʱ��5
  
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  								//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   									//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	

	//��ʼ��TIM5���벶�����
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; 							//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;		//�����ز���
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//ӳ�䵽TI1��
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 				//���������Ƶ,����Ƶ 
  TIM5_ICInitStructure.TIM_ICFilter = 0x00;												//IC1F=0000 ���������˲��� ���˲�
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);
		
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//���������ж� ,����CC1IE�����ж�	
	
  TIM_Cmd(TIM5,ENABLE ); 	//ʹ�ܶ�ʱ��5

 
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;			 //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);												 //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	
}
//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
u8  TIM5CH1_CAPTURE_STA=0;	//���벶��״̬		    				
u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)
//��ʱ��5�жϷ������	 
void TIM5_IRQHandler(void)
{ 		    

 	if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{
		if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)//���
		{	     
			if(TIM5CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�������һ��
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM5CH1_CAPTURE_STA++;
			}	 
		}
		if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)//����1���������¼�
		{	
			if(TIM5CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
			  TIM5CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);//��ȡ��ǰ�Ĳ���ֵ.
	 			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIM5CH1_CAPTURE_STA=0;			//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
				TIM_Cmd(TIM5,DISABLE ); 	//�رն�ʱ��5
	 			TIM_SetCounter(TIM5,0);
	 			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
				TIM_Cmd(TIM5,ENABLE ); 	//ʹ�ܶ�ʱ��5
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
}


//=======================================================
//                     TIM7
//=======================================================


extern vu16 USART3_RX_STA;

/**
 * @Function void TIM7_Int_Init(u16 arr,u16 psc)
 * @description ͨ�ö�ʱ���жϳ�ʼ��
 *							��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
 *							Ft=��ʱ������Ƶ��,��λ:Mhz
 *							����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
 * @Input: u16 arr  �Զ���װֵ
 *         u16 psc  ʱ��Ԥ��Ƶ��
 * @Return void
 *
 */
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7ʱ��ʹ��    
	
	//��ʱ��TIM7��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; 										//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 									//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); 						//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,���������ж�
   
	TIM_Cmd(TIM7,ENABLE);//ʹ�ܶ�ʱ��7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);													//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}
	
/**
 * @Function    void TIM3_IRQHandler(void);
 * @description ��ʱ��7�жϷ������	 
 */
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART3_RX_STA|=1<<15;														//��ǽ������
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );   //���TIM7�����жϱ�־    
		TIM_Cmd(TIM7, DISABLE);  												//�ر�TIM7 
	}	    
}