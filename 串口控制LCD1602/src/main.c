#include <REGX52.H>
#include <string.h>
#include "Delay.h"
#include "UART.h"
#include "LCD1602.h"
//#include "Key.h"
#include "I2C.h"
#include "AT24C02.h"
unsigned char KeyNumber0=0,str[32]={'\0'},str1[32]={'\0'},flag=0,cnt=0,keynum=10,flag1=0,cnt_dis=0,flag3=0,flag4=0,cnt_mem=0,flagt=1;	//flag用于判断，避免某些程序反复执行
unsigned int timer0=0;	//笑死了，最开始把这个定义成unsigned char，结果一直没发现过了255ms会自动变成0的，纠结了贼久
int cnt_Byte=0;
//好像已经不能再定义数据了，一定义就会说地址溢出...?
/*
*str 用于记录电脑向串口发送的字符串
*str1 用于备份str数组的数据
*flag 用于向串口发送数据时避免反复发送
*flag1 用于避免反复计数，到达3秒时停止
*flag3 用于避免反复写入数据
*flag4 用于避免反复读出数据
*cnt_Byte 用于记录每次发送的字符串长度
*cnt_dis 用于记录消失数据的个数
*cnt_mem 用于k3按下时记录数据长度，避免发送新的字符串时影响其长度
*cnt 用于记录发送的字符串长度（本来是想着判断是否超过16个字符来设定显示方式的，还没实现（懒）
*timer0 用于每隔1毫秒计数一次，满3000次即3秒，1000次即1秒
*/
unsigned char Key()
{
	
	if(P3_1==0){Delay(20);while(P3_1==0);Delay(20);KeyNumber0=1;}
	if(P3_0==0){Delay(20);while(P3_0==0);Delay(20);KeyNumber0=2;}
	if(P3_2==0){Delay(20);while(P3_2==0);Delay(20);KeyNumber0=3;flag3=0;}	//按下一次按键把flag3参数调成0
	if(P3_3==0){Delay(20);while(P3_3==0);Delay(20);KeyNumber0=4;flag4=0;}
	
	return KeyNumber0;
}

void main()
{
	unsigned char i,cnt_dis_mem=0;
	UART_Init();		//串口初始化
	LCD_Init();
	while(1)
	{
		keynum = Key();
		if(timer0>=2000)
		{
			flag1 = 0;	//停止timer0计数	
			if(cnt_Byte>=0 && (keynum!=3 && keynum!=4))
			{
				UART_SendByte(str1[cnt_Byte]);	//向电脑发送消失的数据***注意***这里第一次用于缓存，并没有消失，所以上面定2s比较合适，2+1=3
				str1[cnt_Byte--] = '\0';	//消失的数据在str1数组中抹除******注意******cnt_Byte在最后一轮循环中多减去了一次，所以cnt_mem应该要在此基础上加1
				if(cnt_Byte<0) cnt_Byte = 0;
				LCD_ShowString(1,1,"                ");	//清屏
				LCD_ShowString(1,1,str1);	//更新LCD显示数据
				cnt_dis++;	//记录消失的数据个数
				Delay(1000);	//每隔1秒消除一次数据
			}
		}
		
		
		
		if(flag)
		{
			LCD_ShowString(1,1,"                ");	//清屏
			flag = 0;																//防止多次执行
			UART_Sendstring(str);		//向电脑发送字符串
			if(cnt<=16)
			{
				LCD_ShowString(1,1,str);	//用LCD显示数据
			}
//			else//未完待续
//			{
//				
//			}
			strcpy(str1,str);	//备份数据
			cnt_Byte = strlen(str);
			memset(str,'\0',sizeof(str));	//把str数组清0，以便再次发送数据使用
			cnt = 0;		//cnt清0
		}
		else if(flag1)
		{
			Delay(1);
			timer0++;
		}
		
		
		if(keynum==3 && flag3==0)
		{
			flag3 = 1;	//避免重复写入数据
			
			if(flag1==0)
				cnt_Byte++;
			
			for(i=0;i<cnt_Byte;i++)
			{
				AT24C02_WriteByte(i,str1[i]);	//把数据写入存储芯片
				Delay(5);
			}
			cnt_mem = cnt_Byte;	//按下一次k3刷新一次存储的字符串
			cnt_dis_mem = cnt_dis;
		}
	
		if(keynum==4 && flag4==0)
		{
			memset(str1,'\0',sizeof(str1));	//把str1数组清0，便于操作
			for(i=0;i<cnt_mem;i++)
				str1[i] = AT24C02_ReadByte(i);	//把芯片中数据读出，用str1数组记录
			
			
			flag4 = cnt_mem;	//暂用flag4记录数据
			
				//保证每次都先按过k3，避免每次一发数据就可能更新
			
			for(i=0;i<cnt_dis_mem-1;i++)
				str1[cnt_mem++] = '*';	//把消失的数据用*代替
			
			cnt_mem = flag4;	//恢复cnt_mem
			flag4 = 1;	//避免重复读出数据
			
			cnt_dis = 0;
			
			LCD_ShowString(1,1,"                ");
			LCD_ShowString(1,1,str1);			//更新数据
		}
	}
}

void UART_Routine() interrupt 4
{
	unsigned char bytedata = '\0';
	if(RI==1)					//如果接收标志位为1，接收到了数据
	{
		bytedata = SBUF;
		if(bytedata != '\n')
		{
			strncat(str,&bytedata,1);	//把接收到的数据以字符的方式一个个以str数组记录
			cnt++;
		}
		else	//注意这里和串口通信那个程序的区别，那个程序是每次会判断cnt的个数，最后发现回车把数组清0，而这个程序不能在这里清0
		{
//			memset(str,'\0',sizeof(str));
//			cnt = 0;
			flag = 1;
			flag1 = 1;	//开始计时
			timer0 = 0;	//计时数据清0
			KeyNumber0 = 0;
		}
		RI=0;					//接收标志位清0
	}
}
