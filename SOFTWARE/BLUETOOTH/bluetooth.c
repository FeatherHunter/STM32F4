/**
 * @copyright 王辰浩(2014~2024) QQ：975559549
 * @Author Feather @version 1.0 @date 2015.12.4
 * @filename bluetooth.c
 * @description Deal with bluetooth
 * @FunctionList
 *		1.void bluetooth_deal(void); //处理蓝牙
 */ 
#include "bluetooth.h"
#include "usart3.h"
#include <string.h>
#include "lcd.h"
#include "w25qxx.h"
#include "rs485.h"

void bluetooth_deal(void)
{
	u8 i;
	u8 u3_hc05_count;
	u8 SPI_Buffer[]={"Explorer STM32F4 SPI TEST"};
  u8 datatemp[100];
  u8 spi_size = sizeof(SPI_Buffer);
  u32 spiflash_size = 16*1024*1024;
  char buf[100]; 
	if(USART3_RX_STA & 0x8000)
	{
		u3_hc05_count = USART3_RX_STA & 0x7FFF;
		for(i = 0; i < u3_hc05_count; i++)
		{
			buf[i] = USART3_RX_BUF[i];
		}
		buf[i] = '\0';
		if(strncmp(buf, "SPI+WRITE", u3_hc05_count) == 0)
		{
			LCD_ShowString(20, 300, 15 * 24, 24, 24,"Start Write SPI FLASH....");
			W25QXX_Write((u8 *)SPI_Buffer, spiflash_size - 100, spi_size); //写入到spi flash最大地址-100的地方
			LCD_ShowString(20, 300, 15 * 24, 24, 24, "SPI FLASH Write Finished!");//提示传送完成
			
		}
		else if(strncmp(buf, "SPI+READ", u3_hc05_count) == 0)
		{
			LCD_ShowString(20, 300, 15 * 24, 24, 24,"Start Read FLASH.... ");
			W25QXX_Read(datatemp, spiflash_size - 100, spi_size);  // 从spi flash最大地址-100 读取
			LCD_ShowString(20, 300, 15 * 24, 24, 24,"The Data Readed Is:  ");//提示传送完成
			LCD_ShowString(20, 324, 15 * 24, 24, 24,datatemp);//显示读到的字符串
		}
		else if(strncmp(buf, "485+SEND", 8) == 0)
		{
			LCD_ShowString(20, 324+24, 15 * 24, 24, 24,"Start Send RS485....");
			RS485_Send_Data((u8 *)buf+9, u3_hc05_count - 9);
			LCD_ShowString(20, 324 + 24, 15 * 24, 24, 24,   "RS485 Send Finished!");//提示传送完成
			LCD_ShowString(20, 348+24, 15 * 24, 24, 24,(u8 *)buf+8);//显示读到的字符串
		}
		else if(strncmp(buf, "SRAM+TEST", u3_hc05_count) == 0)
		{
			fsmc_sram_test(20, 396);
		}
		USART3_RX_STA = 0;
	}
	
}
