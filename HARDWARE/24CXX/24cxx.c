/**
 * @copyright ������(2014~2024) QQ��975559549
 * @Author Feather @version 1.0 @date 2015.11.28
 * @filename 24cxx.c
 * @description 24cxx EEPROM
 * @FunctionList
 *		1.u8 AT24CXX_ReadOneByte(u16 ReadAddr);														//ָ����ַ��ȡһ���ֽ�
 *		2.void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);				//ָ����ַд��һ���ֽ�
 *		3.void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
 *		4.u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);										//ָ����ַ��ʼ��ȡָ����������
 *		5.void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
 *		6.void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

 *		7.u8 AT24CXX_Check(void);  																				//�������
 *		8.void AT24CXX_Init(void); 																				//��ʼ��IIC
 */ 
#include "24cxx.h" 
#include "delay.h" 				 

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();//IIC��ʼ��
}

/**
 * @Function u8 AT24CXX_ReadOneByte(u16 ReadAddr);
 * @description ��AT24CXXָ����ַ����һ������
 * @Input 	u16 ReadAddr:��ʼ�����ĵ�ַ 
 * @Return  u8  ����������
 */
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	  		  //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);		//���͸ߵ�ַ	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr%256);    //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);            //�������ģʽ			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();											//����һ��ֹͣ����	    
	return temp;
}

/**
 * @Function void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);
 * @description ��AT24CXXָ����ַд��һ������
 * @Input 	u16 WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ  
 *          u8  DataToWrite:Ҫд�������
 * @Return  void
 */
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    		//����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);	//���͸ߵ�ַ	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();											//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/**
 * @Function void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
 * @description ��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
 *              �ú�������д��16bit����32bit������.
 * @Input 	u16 WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ  
 *          u8  DataToWrite:Ҫд�������
 *					u8  Len:Ҫд�����ݵĳ���2,4
 * @Return  void
 */
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/**
 * @Function u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
 * @description ��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
 *							�ú������ڶ���16bit����32bit������.
 * @Input 	u16 ReadAddr   :��ʼ�����ĵ�ַ 
 *					u8  Len        :Ҫ�������ݵĳ���2,4
 * @Return  u8 :����
 */
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/**
 * @Function u8 AT24CXX_Check(void)
 * @description ���AT24CXX�Ƿ�����
 *					��������24XX�����һ����ַ(255)���洢��־��.
 *					���������24Cϵ��,�����ַҪ�޸�
 * @Input 	void
 * @Return  1:���ʧ�� 
 *          0:���ɹ�
 */
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}


/**
 * @Function void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
 * @description ��AT24CXX�����ָ����ַ��ʼ����ָ������������
 * @Input 	u16 ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
 *					u8 *pBuffer  :���������׵�ַ
 *					u16 NumToRead:Ҫ�������ݵĸ���
 * @Return  void
 */
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
 
/**
 * @Function void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
 * @description ��AT24CXX�����ָ����ַ��ʼд��ָ������������
 * @Input 	u16 WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
 *					u8 *pBuffer  :���������׵�ַ
 *					u16 NumToWrite:Ҫд�����ݵĸ���
 * @Return  void
 */
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








