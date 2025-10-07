#include <REGX52.H>
#include <string.h>
#include "Nixie.h"
#include "Delay.h"
#include "Key.h"
#include "UART.h"
#include "Buzzer.h"
sbit Buzzer = P2^5;
unsigned char mode=20,flag1=0,flag2=0,flag3=0,flag4=0,flag5=0,Time=0;
void main()
{
	UART_Init();		//串口初始化
	while(1)
	{
		if(mode==0)
		{
				Nixie(1,10);
				Nixie(2,11);
				Delay(20);
		}
		
				if(mode==1)
		{
				Nixie(1,10);
				Nixie(2,11);
				Delay(10);
			if(flag1)
			{
				UART_Sendstring("L1");
				flag1 = 0;
			}
		}

				if(mode==2)
		{
				Nixie(1,10);
				Nixie(2,11);
				Delay(5);
			if(flag2)
			{
				UART_Sendstring("L2");
				flag2 = 0;
			}
		}

				if(mode==3)
		{
				Nixie(1,10);
				Nixie(2,11);
			if(flag3)
			{
				UART_Sendstring("L3");
				flag3 = 0;
			}
		}

			if(mode==4)
			{
				Nixie(1,16);
				Nixie(2,16);
				if(flag4)
				{
					UART_Sendstring("BEEPON!");
					for(Time=0;Time<100;Time++)
					{
							Buzzer_Time(Time);
							Delay(10);
							Buzzer = 0;
							Delay(100-Time);
					}
				}
				Buzzer=!Buzzer;
				Buzzer_Delay500us();
				flag4 = 0;
			}
			
			if(mode==5)
			{
				Nixie(1,16);
				Nixie(2,16);
				if(flag5)
				{
					UART_Sendstring("BEEPOFF!");
					for(Time=100;Time>0;Time--)
					{
							Buzzer_Time(Time);
							Delay(10);
							Buzzer = 0;
							Delay(100-Time);
					}
				}
				Buzzer = 0;
				flag5 = 0;
			}

	}
}

void UART_Routine() interrupt 4
{
	unsigned char bytedata = '\0';
	static unsigned char str[32] = {'\0'},str1[5] = "dim",str2[10] = "BEEPON",str3[10] = "BEEPOFF",cnt=0;
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
			
		if(cnt==3)
		{
			if(strcmp(str,str1)==0)
			{
				mode = 0;
				cnt = 0;
//				memset(str,'\0',sizeof(str));
			}
		}
		
		if(cnt==2)
		{
			if(str[0]=='L')
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
		
		if(cnt==6)
		{
			if(strcmp(str,str2)==0)
			{
				mode = 4;
				cnt = 0;
				flag4 = 1;
//				memset(str,'\0',sizeof(str));
			}
		}
		
		if(cnt==7)
		{
			if(strcmp(str,str3)==0)
			{
				mode = 5;
				cnt = 0;
				flag5 = 1;
//				memset(str,'\0',sizeof(str));
			}
		}
		
		RI=0;
	}
}
