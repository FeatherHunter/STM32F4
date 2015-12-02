/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.27
 * @filename ctiic.c
 * @description ctiic
 * @FunctionList
 *		1.void CT_IIC_Init(void);                	//��ʼ��IIC��IO��				 
 *		2.void CT_IIC_Start(void);								//����IIC��ʼ�ź�
 *		3.void CT_IIC_Stop(void);	  							//����IICֹͣ�ź�
 *		4.void CT_IIC_Send_Byte(u8 txd);					//IIC����һ���ֽ�
 *		5.u8 CT_IIC_Read_Byte(unsigned char ack);	//IIC��ȡһ���ֽ�
 *		6.u8 CT_IIC_Wait_Ack(void); 							//IIC�ȴ�ACK�ź�
 *		7.void CT_IIC_Ack(void);									//IIC����ACK�ź�
 *		8.void CT_IIC_NAck(void);									//IIC������ACK�ź� 
 */ 
#include "ctiic.h"
#include "delay.h"	 


//����I2C�ٶȵ���ʱ
void CT_Delay(void)
{
	delay_us(2);
} 

/**
 * @Function void CT_IIC_Init(void);
 * @description ���ݴ���оƬIIC�ӿڳ�ʼ��
 *							PB0,PF11
 * @Input 	void
 * @Return  void
 */	
void CT_IIC_Init(void)
{			
  GPIO_InitTypeDef  GPIO_InitStructure;	
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOB,Fʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;			//PB0����Ϊ�������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);				//��ʼ��
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//PF11�����������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���ģʽ
	GPIO_Init(GPIOF, &GPIO_InitStructure);				//��ʼ��	

}


/**
 * @Function void CT_IIC_Start(void)
 * @description ����IIC��ʼ�ź�
 * @Input 	void
 * @Return  void
 */	
void CT_IIC_Start(void)
{
	CT_SDA_OUT();    //sda�����
	CT_IIC_SDA=1;	  	  
	CT_IIC_SCL=1;
	delay_us(30);
 	CT_IIC_SDA=0;		//START:when CLK is high,DATA change form high to low 
	CT_Delay();
	CT_IIC_SCL=0;		//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  

/**
 * @Function void CT_IIC_Stop(void)
 * @description ����IICֹͣ�ź�
 * @Input 	void
 * @Return  void
 */	
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda�����
	CT_IIC_SCL=1;
	delay_us(30);
	CT_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	CT_Delay();
	CT_IIC_SDA=1;//����I2C���߽����ź�  
}

/**
 * @Function u8 CT_IIC_Wait_Ack(void)
 * @description �ȴ�Ӧ���źŵ���
 * @Input  void
 * @Return 	1������Ӧ��ʧ��
 *          0������Ӧ��ɹ�
 */	
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	CT_SDA_IN();      //SDA����Ϊ����  
	CT_IIC_SDA=1;	   
	CT_IIC_SCL=1;
	CT_Delay();
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_IIC_Stop();
			return 1;
		} 
		CT_Delay();
	}
	CT_IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 

/**
 * @Function void CT_IIC_Ack(void)
 * @description ����ACKӦ��
 * @Input 	void
 * @Return  void
 */	
void CT_IIC_Ack(void)
{
	CT_IIC_SCL=0;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA=0;
	CT_Delay();
	CT_IIC_SCL=1;
	CT_Delay();
	CT_IIC_SCL=0;
}

/**
 * @Function void CT_IIC_NAck(void)
 * @description ������ACKӦ��		
 * @Input 	void
 * @Return  void
 */	
void CT_IIC_NAck(void)
{
	CT_IIC_SCL=0;
	CT_SDA_OUT();
	CT_Delay();
	CT_IIC_SDA=1;
	CT_Delay();
	CT_IIC_SCL=1;
	CT_Delay();
	CT_IIC_SCL=0;
}					 				     
	
/**
 * @Function void CT_IIC_Send_Byte(u8 txd)
 * @description IIC����һ���ֽ�
 * @Input 	void
 * @Return  1����Ӧ��
 *					0����Ӧ��	
 */	
void CT_IIC_Send_Byte(u8 txd)
{                        
  u8 t;   
	CT_SDA_OUT(); 	    
  CT_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	CT_Delay();
	for(t=0;t<8;t++)
  {              
    CT_IIC_SDA=(txd&0x80)>>7;
    txd<<=1; 	      
		CT_IIC_SCL=1; 
		CT_Delay();
		CT_IIC_SCL=0;	
		CT_Delay();
  }	 
} 	    
/**
 * @Function u8 CT_IIC_Read_Byte(unsigned char ack)
 * @description IIC��1���ֽ�
 * @Input 	ack=1ʱ������ACK��ack=0������nACK
 * @Return  u8 ���յ�����
 */	
u8 CT_IIC_Read_Byte(unsigned char ack)
{
	u8 i,receive=0;
 	CT_SDA_IN();//SDA����Ϊ����
	delay_us(30);
	for(i=0;i<8;i++ )
	{ 
		CT_IIC_SCL=0; 	    	   
		CT_Delay();
		CT_IIC_SCL=1;	 
		receive<<=1;
		if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_IIC_NAck();//����nACK
	else CT_IIC_Ack(); //����ACK   
 	return receive;
}





























