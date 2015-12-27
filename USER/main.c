#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "beep.h"
#include "key.h"
#include "dht11.h"
#include "includes.h"

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

/*get temperature and huminity*/
#define TASK_DHT11_TASK_PRIO		4
#define TASK_DHT11_STK_SIZE 		128
OS_TCB Task_DHT11_TaskTCB;
CPU_STK TASK_DHT11_TASK_STK[TASK_DHT11_STK_SIZE];
void task_dht11_task(void *p_arg);

//任务优先级
#define TASK2_TASK_PRIO		4
//任务堆栈大小	
#define TASK2_STK_SIZE 		128
//任务控制块
OS_TCB Task2_TaskTCB;
//任务堆栈	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//任务函数
void task2_task(void *p_arg);

u8 lcd_id[12];

void functions_init();

//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);  	//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	uart_init(115200); 	//串口初始化
	
	functions_init();
	
	LCD_Clear(WHITE);
	POINT_COLOR = RED;
	LCD_ShowString(30,10,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,30,200,16,16,"UCOSIII Examp 6-3");
	LCD_ShowString(30,50,200,16,16,"Task Round-robin");
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,90,200,16,16,"2014/11/28");
	
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区			 
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}


//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
	//创建温度、湿度测量任务
	OSTaskCreate((OS_TCB 	* )&Task_DHT11_TaskTCB,		
				 (CPU_CHAR	* )"Task1 task", 		
                 (OS_TASK_PTR )task_dht11_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK_DHT11_TASK_PRIO,     
                 (CPU_STK   * )&TASK_DHT11_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK_DHT11_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK_DHT11_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )3,  //3个时间片，既3*5=15ms					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//创建TASK2任务
	OSTaskCreate((OS_TCB 	* )&Task2_TaskTCB,		
				 (CPU_CHAR	* )"task2 task", 		
                 (OS_TASK_PTR )task2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK2_TASK_PRIO,     	
                 (CPU_STK   * )&TASK2_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )3,	//3个时间片，既3*5=15ms					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);			 
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}


//task1任务函数
void task_dht11_task(void *p_arg)
{
  u8 temp, humi;
	OS_ERR err;
	p_arg = p_arg;
	 
	LCD_ShowString(30,140,90,16,16,"DHT11");
	while(1)
	{
		if(DHT11_Read_Data(&temp, &humi))
		{
			LCD_ShowString(30, 156, 110, 16, 16, "FAILED");
		}
		else 
		{
			LCD_ShowString(30, 156, 110, 16, 16, "      ");
			LCD_ShowNum(30, 156, temp, 16, 16);
			LCD_ShowNum(30, 172, humi, 16, 16);
		}
		OSTimeDlyHMSM(0,0,10,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时10s,测量一次温度
	}
}

//task2任务函数
void task2_task(void *p_arg)
{
	u8 i,task2_num=0;
	OS_ERR err;
	p_arg = p_arg;
	
	POINT_COLOR = RED;
	//LCD_ShowString(30,150,110,16,16,"Task2 Run:000");
	POINT_COLOR = BLUE;
	while(1)
	{
		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
		//LCD_ShowxNum(110,150,task2_num,3,16,0x80);  //显示任务执行次数
		for(i=0;i<5;i++) printf("Task2:56789\r\n");
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
	}
}

void functions_init()
{
	  u32 total, free;
	  u8 hc05_role;
	  u8 res;
	
	  u8 eth_speed;
	  u8 eth_buf[50];

	  LCD_Clear(BLACK);
	
	  LED_Init();            //LED
	  LCD_Init();            //LCD
	  KEY_Init();            //KEY
	
	  POINT_COLOR = BRRED;
	  LCD_ShowString(10+100,0, 24*20, 24, 24, "WELCOME to My Life");
	  POINT_COLOR = BLUE;
	  LCD_ShowString(10,16*1, 200, 16, 16, "LCD");     //LCD
	  sprintf((char *)lcd_id, "ID: %x", lcddev.id);
	  LCD_ShowString(330,16*1, 200, 16, 16, lcd_id);
	  POINT_COLOR = GREEN;
	  LCD_ShowString(400,16*1, 200, 16, 16, "OK");
	 
	  POINT_COLOR = BLUE;
	  LCD_ShowString(10,16*2, 200, 16, 16, "LED");
	  LCD_ShowString(10,16*3, 200, 16, 16, "KEY");
	  POINT_COLOR = GREEN;
	  LCD_ShowString(400,16*2, 200, 16, 16, "OK");
	  LCD_ShowString(400,16*3, 200, 16, 16, "OK");
	
    /*beep init*/
 	  BEEP_Init();
 	  POINT_COLOR = BLUE;
 	  LCD_ShowString(10,16*4, 200, 16, 16, "BEEP");
 	  POINT_COLOR = GREEN;
 	  LCD_ShowString(400,16*4, 200, 16, 16, "OK");	
	
	  /*DHT11*/
	  POINT_COLOR = BLUE;
 	  LCD_ShowString(10,16*5, 200, 16, 16, "DHT11");
	  while(DHT11_Init())
	  {
		  POINT_COLOR = RED;
 		  LCD_ShowString(170,16*5, 240, 16, 16,  " DHT11 Failed");   
		  delay_ms(200);
	  }
	  POINT_COLOR = GREEN;
 	  LCD_ShowString(400,16*8, 200, 16, 16, "OK");
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*2, 200, 16, 16, "RNG");
// 	POINT_COLOR = RED;
// 	while(RNG_Init())      //RNG
// 	{
// 		LCD_ShowString(400,16*2, 200, 16, 16, "NO");
// 	}
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*2, 200, 16, 16, "OK");

// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*5, 200, 16, 16, "TOUCH");
// 	POINT_COLOR = RED;
// 	while(tp_dev.init()) //触摸屏初始化
// 	{
// 		LCD_ShowString(400,16*5, 200, 16, 16, "NO");
// 	}
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*5, 200, 16, 16, "OK");
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*6, 200, 16, 16, "RTC");
// 	My_RTC_Init(); //RTC Init
// 	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits, 0); //配置WAKE UP中断,1秒钟中断一次
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*6, 200, 16, 16, "OK"); 

// 	Adc_Init();	  //ADC 初始化
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*7, 200, 16, 16, "ADC");
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*7, 200, 16, 16, "OK");	
// 	
// 	Remote_Init();                                 //Infrared Remote Control
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*8, 200, 16, 16, "Infrared");
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*8, 200, 16, 16, "OK");
// 	
// 	CanMode = CAN_Mode_LoopBack;
// 	CAN1_Mode_Init(CAN_SJW_1tq, CAN_BS2_6tq, CAN_BS1_7tq, 6, CanMode);//回环模式
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*9, 200, 16, 16, "Can");
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*9, 200, 16, 16, "OK");
// 	POINT_COLOR = MAGENTA;
// 	if(CanMode == CAN_Mode_LoopBack)
// 	{
// 		LCD_ShowString(240,16*5, 200, 16, 16, "LoopBack");
// 	}
// 	else
// 	{
// 		LCD_ShowString(240,16*5, 200, 16, 16, "Normal");
// 	}
// 	
// 	/*w25qxx init*/
// 	W25QXX_Init();
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*11, 200, 16, 16, "SPI FLASH");
// 	while(W25QXX_ReadID() != W25Q128)
// 	{
// 		POINT_COLOR = RED;
// 		LCD_ShowString(170,16*11, 240, 16, 16,  " W25Q128 Check Failed");
// 		delay_ms(250);
// 		LCD_ShowString(170,16*11, 240, 16, 16,  " Please Check .......");
// 		delay_ms(250);
// 	}
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(170,16*11, 240, 16, 16,   "                     ");	
// 	LCD_ShowString(400,16*11, 200, 16, 16, "OK");
// 	
// 	
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*12, 200, 16, 16, "HC05");
// 	while(HC05_Init())     //9600 baund HC05
// 	{
// 		POINT_COLOR = RED;
// 		LCD_ShowString(170,16*12, 240, 16, 16,  " HC05 Check Failed");
// 		delay_ms(250);
// 		LCD_ShowString(170,16*12, 240, 16, 16,  " Please Check ....");
// 		delay_ms(250);
// 	}
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(170,16*12, 240, 16, 16,   "                     ");	
// 	LCD_ShowString(400,16*12, 200, 16, 16, "OK");
// 	hc05_role = HC05_Get_Role();
// 	POINT_COLOR = BLUE;
// 	if(hc05_role == 1) 
// 	{
// 		LCD_ShowString(300,16*12, 200, 16, 16,  "MASTER");
// 	}
// 	else if(hc05_role == 0)
// 	{
// 		LCD_ShowString(300,16*12, 200, 16, 16,  "SLAVE");
// 	}
// 	else
// 	{
// 		POINT_COLOR = RED;
// 		LCD_ShowString(300,16*12, 200, 16, 16,  "0xFF");
// 	}
// 	
// 	/*RS485 init*/
// 	RS485_Init(9600);
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*13, 200, 16, 16, "RS485");
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*13, 200, 16, 16, "OK");	
// 	
// 	/*FSMC SRAM*/
// 	FSMC_SRAM_Init();
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*14, 200, 16, 16, "FSMC SRAM");
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*14, 200, 16, 16, "OK");
// 	
// 	/*MEM MANEGEMENT*/
// 	my_mem_init(SRAMIN);
// 	my_mem_init(SRAMEX);
// 	my_mem_init(SRAMCCM);
// 	
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*15, 200, 16, 16, "MEM MANEGEMENT");
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*15, 200, 16, 16, "OK");
// 	
// 	/*SD CARD*/
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*16, 200, 16, 16, "SD CARD");
// 	while(SD_Init())//检测不到SD卡
// 	{
// 		POINT_COLOR = RED;
// 		LCD_ShowString(170,16*16, 240, 16, 16,  " SD Card Error!   ");
// 		delay_ms(500);					
// 		LCD_ShowString(170,16*16, 240, 16, 16,  " Please Check ....");
// 		delay_ms(500);
// 	}
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(170,16*16, 240, 16, 16,   "                     ");	
// 	LCD_ShowString(400,16*16, 200, 16, 16, "OK");
// 	show_sdcard_info();	//打印SD卡相关信息
// 	
// 	/****FATFS***/
// 	exfuns_init(); //为fatfs相关变量申请内存
// 	f_mount(fs[0], "0:", 1);   //挂载SD卡
// 	res = f_mount(fs[1], "1:", 1);   //挂载flash
// 	if(res==0X0D)//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
// 	{
// 		LCD_ShowString(10,16*17, 200, 16, 16,"Flash Disk Formatting...");	//格式化FLASH
// 		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
// 		if(res==0)
// 		{
// 			f_setlabel((const TCHAR *)"1:ALIENTEK");	//设置Flash磁盘的名字为：ALIENTEK
// 			LCD_ShowString(10,16*17, 200, 16, 16,    "Flash Disk Format Finish");	//格式化完成
// 		}else LCD_ShowString(10,16*17, 200, 16, 16,"Flash Disk Format Error ");	//格式化失败
// 		delay_ms(1000);
// 	}													    
// 	LCD_ShowString(10,16*17, 200, 16, 16,        "                        ");	//清空			  
// 	while(exf_getfree("0",&total,&free))	//得到SD卡的总容量和剩余容量
// 	{
// 		LCD_ShowString(30,16*17,200,16,16,"SD Card Fatfs Error!");
// 		delay_ms(200);
// 		LCD_Fill(30,16*17,240,16*17+16,WHITE);	//清除显示			  
// 		delay_ms(200);
// 		LED0=!LED0;//DS0闪烁
// 	}													  			    
//  	POINT_COLOR=BLUE;//设置字体为蓝色	  
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*17, 200, 16, 16, "FATFS");
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,16*17, 200, 16, 16, "OK");	
// 	POINT_COLOR = BLACK;
// 	LCD_ShowString(10,16*18,200,16,16,"SD Total Size:     MB");	 
// 	LCD_ShowString(10,16*19,200,16,16,"SD  Free Size:     MB"); 	
// 	POINT_COLOR = MAGENTA;	
//  	LCD_ShowNum(10+8*14,16*18,total>>10,5,16);				//显示SD卡总容量 MB
//  	LCD_ShowNum(10+8*14,16*19,free>>10,5,16);					//显示SD卡剩余容量 MB		
// 	
// 	/*font prepare*/
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*20, 200, 16, 16, "FONT");
// 	while(font_init())//检测不到font
// 	{
// 		POINT_COLOR = RED;
// 		LCD_ShowString(170,16*20, 240, 16, 16,  "FONT     Error!   ");
// 		delay_ms(500);					
// 		LCD_ShowString(170,16*20, 240, 16, 16,  " Please Check ....");
// 		delay_ms(500);
// 	}
// 	POINT_COLOR = GREEN;
// 	Show_Str(300,16*20, 240, 16, "     字库初始化成功", 16, 0);
// 	
// 	/* ATK-RM04 */
// 	/*
// 	atk_rm04_init();
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,24*21, 200, 24, 24, "ATK-RM04");
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(400,24*21, 200, 24, 24, "OK");
// 	*/
// 	
// 	/*Init LWIP*/
// #if 0
// 	TIM3_Int_Init(999,839); //100khz的频率,计数1000为10ms
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*21, 200, 16, 16, "lwIP Init");
// 	while(lwip_comm_init()!=0) //先初始化lwIP(包括LAN8720初始化),此时必须插上网线,否则初始化会失败!! 
// 	{
// 		POINT_COLOR = RED;
// 		LCD_ShowString(200,16*21, 200, 16, 16, "lwIP Init failed!");
// 		delay_ms(1200);
// 		LCD_Fill(200,16*21, 400,16*21+16,WHITE);//清除显示
// 		LCD_ShowString(200,16*21, 200, 16, 16,"Retrying...");  
// 	}
// 	LCD_Fill(200,16*21, 400,16*21+16,WHITE);//清除显示
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(350,16*21, 200, 16, 16,"Successed");
// 	//等待DHCP获取 
// 	POINT_COLOR = BLUE;
// 	LCD_ShowString(10,16*22, 200, 16, 16, "DHCP IP config");
// 	while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
// 	{
// 		lwip_periodic_handle();
// 	}
// 	POINT_COLOR = GREEN;
// 	LCD_ShowString(350,16*22, 200, 16, 16,"Successed");
// 	if(lwipdev.dhcpstatus==2)sprintf((char*)eth_buf,"DHCP IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印动态IP地址
// 	else sprintf((char*)eth_buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//打印静态IP地址
// 	LCD_ShowString(10,16*23, 200, 16, 16,eth_buf); 
// 	eth_speed=LAN8720_Get_Speed();										//得到网速
// 	POINT_COLOR = BLACK;
// 	if(eth_speed&1<<1)LCD_ShowString(10,16*23, 200, 16, 16,"Ethernet Speed:100M   ");
// 	else LCD_ShowString(10,16*23, 200, 16, 16,"Ethernet Speed:10M   "); 
// 	//httpd_init();	//HTTP初始化(默认开启websever)

// 	
// 	/*set up tcp/ip*/
// 	lwipdev.remoteip[0] = 192;
// 	lwipdev.remoteip[1] = 168;
// 	lwipdev.remoteip[2] = 16;
// 	lwipdev.remoteip[3] = 107;
// 	sprintf((char*)eth_buf,"Local IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//本地IP
// 	LCD_ShowString(10,16*24, 200, 16, 16,eth_buf);  
// 	sprintf((char*)eth_buf,"Remote IP:%d.%d.%d.%d",lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);//远端IP
// 	LCD_ShowString(10,16*25, 200, 16, 16,eth_buf);  
// 	sprintf((char*)eth_buf,"Remote Port:%d",TCP_CLIENT_PORT);//客户端端口号
// 	LCD_ShowString(10,16*26, 200, 16, 16,eth_buf);
// #endif
}

#if 0


//FWIP
/*
#include "netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_client_demo.h"
#include "tcp_client_demo.h"
#include "tcp_server_demo.h"
#include "udp_demo.h"
#include "httpd.h"
*/

/********定义变量 FATFS*********/
FIL fil;
FRESULT res;
UINT bww;
char buf[100];

u8 flush_key = 1;
u8 CanMode;
u16 w25q_id;

#if 1

void functions_init(); //初始化所有功能

void ctp_test(void)
{
	
}
//通过串口打印SD卡相关信息
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}

int main(void)
{
	u8 itouch;
	u8 connect_flag = 0;
	u8 login_flag = 0;
	u8 res;
	u8 * fn;
	u8 * pname;
	DIR picdir;
	FILINFO picfileinfo;
	//u8 temp = 0, humi = 0;
	u32 data_rng;
	//u8 role = 0xFF;
	
	//struct tcp_pcb *tcppcb;  	//定义一个TCP服务器控制块
	//struct ip_addr rmtipaddr;  	//远端ip地址
	
	u8 RTC_DBUF[20];
	u8 RTC_TBUF[20];
	
	short ADC_temperature;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //NVIC
	uart_init(115200);
	delay_init(168); //systick = sysCLK/8,50us = 50 * 21 systick.if use 84,1us实际上为1/2us
	usmart_dev.init(84);  //84?why?	
	functions_init();
	
	//Dac1_Init();
	//TIM9_CH2_PWM_Init(255,TIM_CKD_DIV1);

	///////////////////////////////////////////////////////
	
	//EXTIX_Init(); //外部中断
	//TIM3_Int_Init(9999, 8399); //定时器时钟84MHz
	
	//IWDG_Init(5, 500); //独立看门狗LSI
	//WWDG_Init(0x70,0x50, WWDG_Prescaler_8);//分频8
	///////////////////////////////////////////////////////////////
	//	TIM14_PWM_Init(500-1,84-1);	//84M/84=1Mhz计数频率,重装载值500,所以PWM频率为 1M/500=2Khz. 
  ///////////////////////////////////////////////////////////////
	//while(DHT11_Init())
	//{
	//	printf("not found DHT11!\n");      ==DHT11==
	//	delay_ms(200);
	//}
	///////////////////////////////////////////////////////////////
	//printf("HC05: Slave!\n");
	//HC05_Set_Cmd("AT+ROLE=0");
	///////////////////////////////////////////////////////////////
	//TIM_SetCompare2(TIM9, 20);
	///////////////////////////////////////////////////////////////
	//Lsens_Init();                              ==LSENSOR,TPAD==
	//TPAD_Init(16);
  //////////////////////////////////////////////////////////////////////////////
	//TIM5_CH1_Cap_Init(0XFFFFFFFF, 84-1); //84MHz/84 = 1Mhz 1Mhz/500 = 2Khz  ==IC==
	///////////////////////////////////////////////////////////////////////////////
	piclib_init();
	//tcp_client_sendbuf = mymalloc(SRAMIN, 100);
	//picfileinfo.lfsize = _MAX_LFN * 2 + 1;
	//picfileinfo.lfname = mymalloc(SRAMIN, picfileinfo.lfsize);
	//pname = mymalloc(SRAMIN, picfileinfo.lfsize);
	
	if(tp_dev.touchtype & 0x80)
	{
		while(1)
		{
			if(flush_key)
			{
				gui_clear(WHITE);
				gui_draw_dialog(10, 10, 200, 28, 24, (u8 *)"LED0", BLUE, RED);
				gui_draw_dialog(10+200, 10, 200, 28, 24, (u8 *)"RNG", BLUE, RED);
				gui_draw_dialog(10, 38, 200, 28, 24, (u8 *)"Infrared Contrl", BLUE, RED);
				
				/*
				res = f_opendir(&picdir, (const TCHAR*)"0:/PICTURE"); //打开目录
				if(res == FR_OK)
				{
						res = f_readdir(&picdir, &picfileinfo);
						fn = (u8 *)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);
					  res = f_typetell(fn);
						if((res&0xF0) == 0x50)//判断是不是图片
						{
							strcpy((char *)pname, "0:/PICTURE/");
							strcat((char *)pname, (const char *)fn);
							ai_load_picfile(pname, 50, 50, 100, 100, 1);
							Show_Str(2,2,240,16,pname,16,1); 				//显示图片名字
						}
				}
				myfree(SRAMIN, pname);
				*/
				
				flush_key = 0;
			}
			tp_dev.scan(0);
			for(itouch = 0; itouch < OTT_MAX_TOUCH; itouch++)
			{
				if((tp_dev.sta)&(1<<itouch))
				{
					if((tp_dev.x[itouch] >= 10)&&(tp_dev.x[itouch] <= 10+200) //LED0
						&&(tp_dev.y[itouch] >= 10)&&(tp_dev.y[itouch] <= 10+28))
					{
						LED1 =!LED1;
					}
					else if((tp_dev.x[itouch] >= 210)&&(tp_dev.x[itouch] <= 210+200) //RNG
						&&(tp_dev.y[itouch] >= 10)&&(tp_dev.y[itouch] <= 10+28))
					{
						data_rng = RNG_Get_RandomRange(1, 6);
						POINT_COLOR = RED;
						LCD_ShowNum( 260, 12, data_rng, 16, 16);
						//delay_ms(1000);
					}
					else if((tp_dev.x[itouch] >= 10)&&(tp_dev.x[itouch] <= 10+200) //Infrared Remote Contrl
						&&(tp_dev.y[itouch] >= 38)&&(tp_dev.y[itouch] <= 38+28))
					{
						;
					}
				}
			}//end of for
			
			remote_ctrl();    //红外线控制
			bluetooth_deal(); //蓝牙控制
			
			tp_dev.x[itouch] = 0;
			tp_dev.y[itouch] = 0;
			/*得到日期和事时间*/
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			sprintf(RTC_DBUF, "%4d/%2d/%2d week:%d", RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date, RTC_DateStruct.RTC_WeekDay);
	    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
			sprintf(RTC_TBUF, "%2d:%2d:%2d", RTC_TimeStruct.RTC_Hours, RTC_TimeStruct.RTC_Minutes, RTC_TimeStruct.RTC_Seconds);
			POINT_COLOR = BLACK;
			LCD_ShowString(10, lcddev.height - 48, 300, 24, 24, RTC_DBUF);
			LCD_ShowString(310, lcddev.height - 48, 300, 24, 24, RTC_TBUF);
			/*得到CPU温度*/
			POINT_COLOR = BLUE;
			LCD_ShowString(20, lcddev.height - 72, 5 * 24, 24, 24, "CPU temp:");
			ADC_temperature = Get_Temprate();
			POINT_COLOR = RED;
			LCD_ShowNum(5*24+12, lcddev.height - 72, ADC_temperature/100, 2, 24);
			
		//if(TPAD_Scan(0))
		//{
		//	LED1 = !LED1;
		//	data = Lsens_Get_Val();              ==LSENSOR,TPAD==
		//	printf("LSENSOR:%d\n", data);
		//}
		//printf("TPAD INIT wait!\n");
		///////////////////////////////////////////////////////////
		//	data = TIM_GetCapture2(TIM9);
		//	printf("PWM val:%d ", data);                ==PWMDAC==
		//	printf("V: %f\n", (float)data*(3.3/256));
		//	data = Get_Adc_Average(ADC_Channel_5, 20);
		//	printf("adc val:%d ", data);
		//	printf("V: %f\n", (float)data*(3.3/4096));
		////////////////////////////////////////////////////////
		//Dac1_Set_Vol(2000);
		//data = Get_Adc_Average(ADC_Channel_5, 20);      ==ADC,DAC==
		//printf("adc val:%d ", data);
		//printf("V: %f\n", (float)data*(3.3/4096));
    //////////////////////////////////////////////////////
		//  u16 ledpwm = 0;
		//	if(++ledpwm == 500)
		//	{
		//		ledpwm = 0;                     ==PWM OC==
		//	}
		//	delay_ms(20);
		//	TIM_SetCompare1(TIM14, ledpwm);
		//////////////////////////////////////////////////////
		//DHT11_Read_Data(&temp, &humi);      ==DHT11==
		/////////////////////////////////////////////////////
		//key = KEY_Scan(0);
		//if(key == WKUP_PRES)
		//{                                   ==HC05==
		//	role = HC05_Get_Role();
		//	if(role != 0xFF)
		//	{
		//		if(role == 1) HC05_Set_Cmd("AT+ROLE=0");
		//		else HC05_Set_Cmd("AT+ROLE=1");
		//		role = HC05_Get_Role();
		//		role =!role;
		//		if(role == 1) printf("HC05: Master!\n");
		//		else if(role == 0) printf("HC05: Slave!\n");
		//    else printf("HC05 ERROR!%d\n", role);
		//		//HC05_Set_Cmd("AT+RESET");
		//	}	
		//}
		//if(USART3_RX_STA&0X8000)			//接收到一次数据了
		//{
		//	u3_bufcount = (USART3_RX_STA & 0X7FFF);
		//	USART3_RX_BUF[u3_bufcount] = 0;
		//	printf("接收到！");
		//	for(i = 0; i < u3_bufcount; i++)
		//	{
		//		printf("%c", USART3_RX_BUF[i]);
		//	}
    //
 		//	USART3_RX_STA=0;	 
		//}
		////////////////////////////////////////////////////////
		//RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct); ==RTC==
		//printf("year:%d month:%d date%d week%d\n", RTC_DateStruct.RTC_Year, RTC_DateStruct.RTC_Month, RTC_DateStruct.RTC_Date, RTC_DateStruct.RTC_WeekDay);
	  //RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
		////////////////////////////////////////////////////////
		//tcp_client_test(&connect_flag, &login_flag, &tcppcb, &rmtipaddr);
			
		}//end of while
	}//end of else
}
#endif

#endif
