#include <INTRINS.h>
#include <REGX52.H>
#include "LCD1602.h"
#include "Delay.h"
#include "DHT11.h"
#define uchar unsigned char
#define uint unsigned int
uchar rec_dat[15],RH,TH;   //用于显示的接收数据数组

/*这里使用lcd1602来显示采集到的数据，lcd1602驱动程序在博主的51单片机系列博文中有详细讲解*/
void main()
{
   LCD_Init();   //lcd1602初始化
   while(1)
   {   
       Delay(1500);    //DHT11上电后要等待1S以越过不稳定状态在此期间不能发送任何指令
				DHT11_receive(&RH,&TH);
				rec_dat[0]='0'+(RH/10);
        rec_dat[1]='0'+(RH%10);
        rec_dat[2]='R';
        rec_dat[3]='H';
        rec_dat[4]=' ';
        rec_dat[5]=' ';
        rec_dat[6]='0'+(TH/10);
        rec_dat[7]='0'+(TH%10);
        rec_dat[8]='C';
       LCD_ShowString(1,1,rec_dat);                      
   }
}

