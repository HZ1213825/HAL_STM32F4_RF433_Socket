#ifndef _RF_WEITE_H_
#define _RF_WEITE_H_
#include <REG52.H>
#include<INTRINS.H>
//11.0592Mhz晶振 提前定义输出端口RF_WRITE_out
//3个uchar数据
sbit RF_WRITE_output=RF_WRITE_out;



void Delay5000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	i = 36;
	j = 5;
	k = 211;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
void Delay5ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 9;
	j = 244;
	do
	{
		while (--j);
	} while (--i);
}

//延迟 数据发送前
void Delay8300us()		//@11.0592MHz 8.3ms
{
	unsigned char i, j;

	_nop_();
	i = 15;
	j = 222;
	do
	{
		while (--j);
	} while (--i);
}

void Delay755us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 2;
	j = 86;
	do
	{
		while (--j);
	} while (--i);
}

void Delay305us()		//@11.0592MHz
{
	unsigned char i;

	i = 138;
	while (--i);
}
void RF_WRITE_send_1()//发1
{
    RF_WRITE_output=1;
    Delay755us();
    RF_WRITE_output=0;
    Delay305us();
}
void RF_WRITE_send_0()//发0
{
    RF_WRITE_output=1;
    Delay305us();
    RF_WRITE_output=0;
    Delay755us();
}
void RF_WRITE_send_before()//发送诱导波后延迟
{
    RF_WRITE_send_1();
	RF_WRITE_send_0();
	RF_WRITE_send_1();
    RF_WRITE_output=0;
    Delay8300us();
}
void RF_WRITE_send_after()//发送结束波
{
    RF_WRITE_output=1;
    Delay305us();
    RF_WRITE_output=0;
	Delay8300us();
}

void RF_WRITE_send1(unsigned char dat[3])//发送1次
{
	int i=0,j=0;
	
	//RF_WRITE_send_before();
	for(i=0;i<3;i++)
	{
		for(j=0;j<8;j++)
		{
			if(dat[i]&0x80)
			{
				RF_WRITE_send_1();
			}
			else
			{
				RF_WRITE_send_0();
			}
			dat[i]<<=1;
		}
	}
	RF_WRITE_send_after();

}
void RF_WRITE_not(unsigned char *dat)//反相
{
	int i=0;
	for(i=0;i<3;i++)
	{
		dat[i]=~dat[i];
	}
}

void RF_WRITE_send(unsigned char dat[3])//发送10次
{
    int i=10;
    unsigned char a[3];
    for (i = 0; i < 3; i++) a[i]=dat[i];
    i=10;
    RF_WRITE_not(a);
	while (i--)
	{
		RF_WRITE_send1(a);
	}
}

#endif