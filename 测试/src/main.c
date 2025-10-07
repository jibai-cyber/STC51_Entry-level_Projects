#include <REGX52.H>
#include <string.h>
#include "Delay.h"
#include "UART.h"
unsigned char str[10] = {'\0'},KeyNum,MODE,TimeSetSelect,TimeSetFlashFlag,FreqSelect,MusicSelect,song,clock_h=0,clock_m=0,clock_s=0,flags=0;

void main()
{
	UART_Init();
	while(1)
	{
		if(flags)
		{
			flags = 0;
			song = 1;
			clock_h = str[0];
			UART_SendByte(clock_h);
		}
	}
}



void UART_Routine() interrupt 4
{
	unsigned char bytedata = '\0';
	if(RI==1)
	{
		bytedata = SBUF;
		if(bytedata != '\n')
		{
			strncat(str,&bytedata,1);	//把这个数写进str数组
				flags = 1;
		}
		RI=0;
	}
}