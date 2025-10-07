#include <REGX52.H>
#include <string.h>
#include "Delay.h"
#include "MatrixKey.h"
#include "Nixie.h"
#include "UART.h"
#include "Buzzer.h"
#include "DS18B20.h"

//向串口发送四位数字以设置密码，每发送一次密码会更新
//温度高于一定值则密码警报不会响，但会有屏幕显示
unsigned char str[10]={'\0'},str1[10]={'\0'},flag1=0,flag2=0,flag_wrong=0,cnt=0,cnt1=0,keynum=20,flagt=0,ch='0'-0,flagn=0,flag=0;
float T;
void main()
{
	unsigned char i;
	UART_Init();	//串口初始化
	DS18B20_ConvertT();		//上电先转换一次温度，防止第一次读数据错误
	Delay(1000);			//等待转换完成
	while(1)
	{
		keynum = MatrixKey();
		DS18B20_ConvertT();	//转换温度
		T = DS18B20_ReadT();	//读取温度
		
		if(T<20)				//温度判断，加上flag形式使得下列操作可以屏蔽部分功能，手捏温度传感器和不捏温度传感器大概就是这个温度上下（确信
			flagt = 1;
		else flagt = 0;
		
		if(flag2)
		{
			flag2 = 0;	//用完就置0，保证每次只执行一次
			UART_Sendstring("Setok!");
		}
		if(keynum>='0' && keynum<='9')	//发送数据的时候是以文本形式发送，即全都是字符形式，所以矩阵键盘里面的参数要做出适当调整，否则下面strcmp函数永远是不等于0的
		{
			str1[cnt++] = keynum;	//把接收到的数据以字符的方式一个个以str1数组记录
			flag1 = 1;
		}
		
		if(keynum==11)	//不存在'10','11'...
			flagn = 1;	//确认键
		
		if(keynum==12)	//重置键
		{
			flag1 = 0;		//把用于数码管常亮的flag置0，相当于熄灭所有数码管
			flag_wrong = 0;
			cnt = 0;
			memset(str1,'\0',sizeof(str1));		//把str1数组清0，确保下次重新输入数据时无误
		}
		
		if(flag1)	//为了保持数码管常亮，需要格外拉出来用flag形式判断，让每次循环都能读到，而非调用一次Nixie函数就结束（否则就只会亮一次
		{
			if(str1[0]!='\0') Nixie(1,str1[0]-ch);	//这里str1数组里的元素已经转化成了ASCII码，需要减去ch变回数字形式
			if(str1[1]!='\0') Nixie(2,str1[1]-ch);	//加if判断，保证元素存在再显示
			if(str1[2]!='\0') Nixie(3,str1[2]-ch);
			if(str1[3]!='\0') Nixie(4,str1[3]-ch);
		}
		
	
		
		if(cnt==4 && flagn)	//用&&实现确认键功能
		{
			flagn = 0;
			if(strcmp(str1,str)==0)
			{
				UART_Sendstring("pwdOK!");
				if(flagt)				//温度高于一定值就屏蔽if里面的内容
				{
					P2 = 0x00;
					Delay(1000);
					P2 = 0xFF;
					Buzzer_Time(2000);
				}
			}
			
			else if(strcmp(str1,str)!=0)
			{
				flag_wrong = 1;
				UART_Sendstring("Wrong!");
				if(flagt)
					for(i=0;i<4;i++)	//用for循环实现蜂鸣器连续叫四下功能
					{
						Buzzer_Time(300);
						Delay(300);
					}
			}
			cnt = 0;			//注意cnt要重置
		}
		
		if(flag_wrong)	//输入错误显示E0
		{
			Nixie(7,10);
			Nixie(8,0);
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
			if(cnt1<4)
				str[cnt1++] = bytedata;
			if(cnt1==4)		//不能用else if，否则flag2永远别想等于1了（笑死了，还在这里犯过错
				flag2 = 1;	//淦，这里不能用UART_Sendstring，因为SBUF的读和写不能同时进行，读的时候不能写，否则就算发送也一直都是第一次发送的数据，具体看其流程图！
		}
		else	
		{
			cnt1 = 0;
		}
		RI=0;					//接收标志位清0
	}
}
