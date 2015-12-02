/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.25
 * @filename usart3.c
 * @description ����3�������ں�HC05ͨ��
 * @FunctionList
 *		1.void usart3_init(u32 bound);							//����3��ʼ�� 
 *		2.void TIM7_Int_Init(u16 arr,u16 psc);      //��ʱ��7���ܴ�������
 *		3.void u3_printf(char* fmt, ...);           //��������
 */
#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "timer.h"	   


__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�

#ifdef USART3_RX_EN   								//���ʹ���˽���   	  

u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 							//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.


/**
 * @Function void USART3_IRQHandler(void)
 * @description USART3�жϴ�����
 *             һ������������:��������2���ַ�ʱ��<=100ms.
 *						 �������ݽ�����ϣ���������2���ַ�ʱ��>100ms.   
 * ����״̬USART3_RX_STA
 *         [15]:0,û�н��յ�����;
 *							1,���յ���һ������.
 *				 [14:0]:���յ������ݳ��� 
 * ����USART3_RX_BUF
 * @Return   void
 */
u16 USART3_RX_STA=0;   	 
void USART3_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
	{	 
 
	res =USART_ReceiveData(USART3);		
	if((USART3_RX_STA&(1<<15))==0)						//�������һ������,��û�б�����,���ٽ�����������
	{ 
		if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//�����Խ�������
		{
			TIM_SetCounter(TIM7,0);								//���������        				 
			if(USART3_RX_STA==0)		
				TIM_Cmd(TIM7, ENABLE);  						//ʹ�ܶ�ʱ��7 
			USART3_RX_BUF[USART3_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			USART3_RX_STA|=1<<15;									//ǿ�Ʊ�ǽ������
		} 
	}  	
 }										 
}  
#endif	

/**
 * @Function void usart3_init(u32 bound)
 * @description ��ʼ��IO ����3 
 *              PB10,PB11
 * @Calls
 *    1.TIM7_Int_Init(100-1,8400-1);		//10ms�ж�һ��
 *    2.TIM_Cmd(TIM7, DISABLE); 				//�رն�ʱ��7
 * @Input    u32 bound: ������
 * @Return   void
 */
void usart3_init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
 
	USART_DeInit(USART3);  															 //��λ����3
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11��GPIOB10��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 					//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 						//����
	GPIO_Init(GPIOB,&GPIO_InitStructure); 									//��ʼ��GPIOB11����GPIOB10
	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10����ΪUSART3	  
	
	USART_InitStructure.USART_BaudRate = bound;											//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 		//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;					//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;							//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART3, &USART_InitStructure); 											//��ʼ������3
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�  
		
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
	
 
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);													//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM7_Int_Init(100-1,8400-1);		//10ms�ж�һ��
	
  TIM_Cmd(TIM7, DISABLE); 				//�رն�ʱ��7
	
	USART3_RX_STA=0;								//���� 
}


/**
 * @Function void u3_printf(char* fmt,...) 
 * @description ����3,printf ����
 *			ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
 * @Calls
 * @Input    char* fmt,...
 * @Return   void
 */
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)//ѭ����������
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  //�ȴ��ϴδ������ 
		USART_SendData(USART3,(uint8_t)USART3_TX_BUF[j]); 	 //�������ݵ�����3 
	}
	
}
 
 
 











