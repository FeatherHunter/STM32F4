#ifndef _WWDG_H
#define _WWDG_H
#include "sys.h"

void WWDG_Init(u8 tr,u8 wr,u32 fprer); //���ڿ��Ź���ʼ��
void WWDG_IRQHandler(void);            //���ڿ��Ź��ж�
#endif
