#include <REGX52.H>
#include <string.h>
#include "LCD1602.h"
#include "Delay.h"
#include "UART.h"
#include "Timer0.h"
sbit Motor=P1^0;
unsigned char mode=20,flag1=0,flag2=0,flag3=0,Counter,Compare;
void main()
{
	UART_Init();		//串口初始化
	LCD_Init();
	Timer0_Init();
	while(1)
	{
		
				if(mode==1)
		{
			LCD_ShowString(1,1,"Speed:S");
				LCD_ShowNum(1,8,1,1);
			if(flag1)
			{
				Compare = 50;
				UART_Sendstring("S1");
				flag1 = 0;
			}
		}

				if(mode==2)
		{
			LCD_ShowString(1,1,"Speed:S");
				LCD_ShowNum(1,8,2,1);
			if(flag2)
			{
				Compare = 75;
				UART_Sendstring("S2");
				flag2 = 0;
			}
		}

				if(mode==3)
		{
			LCD_ShowString(1,1,"Speed:S");
				LCD_ShowNum(1,8,3,1);
			if(flag3)
			{
				Compare = 100;
				UART_Sendstring("S3");
				flag3 = 0;
			}
		}

			

	}
}

void UART_Routine() interrupt 4
{
	unsigned char bytedata = '\0';
	static unsigned char str[32] = {'\0'},cnt=0;
	if(RI==1)
	{
		bytedata = SBUF;
		if(bytedata != '\n')
		{
			strncat(str,&bytedata,1);
			cnt++;
		}
		else
		{
			memset(str,'\0',sizeof(str));
			cnt = 0;
		}
			
	
		
		if(cnt==2)
		{
			if(str[0]=='S')
			{
				switch (str[1])
				{
					case '1':
						mode = 1;
						cnt = 0;
						flag1 = 1;
						break;
					case '2':
						mode = 2;
						cnt = 0;
						flag2 = 1;
						break;
					case '3':
						mode = 3;
						cnt = 0;
						flag3 = 1;
						break;
				}
//				memset(str,'\0',sizeof(str));
			}
		}
		
		
		RI=0;
	}
}

void Timer0_Routine() interrupt 1
{
	TL0 = 0x9C;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	Counter++;
	Counter%=100;	//计数值变化范围限制在0~99
	if(Counter<Compare)	//计数值小于比较值
	{
		Motor=1;		//输出1
	}
	else				//计数值大于比较值
	{
		Motor=0;		//输出0
	}
}