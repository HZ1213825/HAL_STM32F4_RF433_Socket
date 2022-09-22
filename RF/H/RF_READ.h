#ifndef _RF_READ_H_
#define _RF_READ_H_
#include <intrins.h>
#include <REG52.H>
//11.0592MHz晶振，外部中断调用
char RF_READ_OK=0;//解码成功标志
char RF_READ_ins=0;//状态指示
char RF_READ_i=0;//循环
char RF_READ_j=0;//循环
int RF_READ_time=0;//计算时间
unsigned char RF_READ_data[3]={0};//数据

//延迟500us
void Delay600us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 2;
	j = 15;
	do
	{
		while (--j);
	} while (--i);
}

//初始化 使用定时器0 外部中断0
void RF_READ_init()
{
    EA = 1;
    EX0 = 1;
    IT0 = 1;

    TMOD&=0xf0;
    TMOD |= 0x01;
    //设置为xxxx 0001
    TH0 = 0;
    TL0 = 0;
	TR0=1;
}
//将计时器0归零
void RF_READ_clean()
{
    TH0 = 0;
    TL0 = 0;
}
//提取计时器数
unsigned int RF_READ_GetT0num()
{
    unsigned long int num = 0;
    num = (TH0 << 8) | TL0;
    RF_READ_clean();
    return num;
}
//解码函数，建议外部中断调用 需其他调用请修改标识出的修改处
void RF_READ_decode()
{
    if(RF_READ_ins==0)//初始化 检测到下降沿
    {
        RF_READ_clean();
        RF_READ_ins=1;
    }
    else if(RF_READ_ins==1)
    {
			
        RF_READ_time=RF_READ_GetT0num();//计算从上次电平到此时间
		if(RF_READ_time>7900-500&&RF_READ_time<7900+2000)//数据码送入前的延迟
        {
            RF_READ_clean();
            RF_READ_ins=2;//开始解码
        }
        else
        {
            RF_READ_ins=0;//复位
        }
        RF_READ_data[0]=0;
        RF_READ_data[1]=0;
        RF_READ_data[2]=0;

        if(RF_READ_ins==2)//解码第一位码
        {
            Delay600us();
            if(!INT0)
            {
                RF_READ_data[0]=0;
            }
            else if (INT0)
            {
                RF_READ_data[0]=1;
            }
            RF_READ_j=0;
            RF_READ_i=1;
        }
    }
    else if(RF_READ_ins==2)//解码后面的
    {
        RF_READ_time=RF_READ_GetT0num();//计算时间做验证
        if(RF_READ_time>800&&RF_READ_time<1400)//1.05ms左右
        {
            Delay600us();//延迟
            if(!INT0/*修改*/)//判断这个时刻电平高低
            {//低
                RF_READ_data[RF_READ_j]<<=1;
                RF_READ_data[RF_READ_j]&= 0xFE;
            }
            else if (INT0/*修改*/)
            {//高
                RF_READ_data[RF_READ_j]<<=1;
                RF_READ_data[RF_READ_j] |= 0x01;
            }
            RF_READ_i++;
            if(RF_READ_i>=8)//8位数据写完 换行
            {
                RF_READ_i=0;
                RF_READ_j++;
            }
        }
        else
        {
            RF_READ_ins=0;
        }
        if(RF_READ_j>=3)//24位数据写完 完成标志写1 复位
        {
            RF_READ_j=0;
            RF_READ_i=0;
            RF_READ_OK=1;
            RF_READ_ins=0;
        }
    }
}

unsigned int RF_READ_3uchar_to_3uint(unsigned char *dat)
{
    unsigned int zj=0;
    int i=0;
    for(i=0;i<3;i++)
    {
        zj<<=8;
        zj+=dat[i];
    }
    return zj;
}



/* 示例调用
void RF_READ_action(unsigned char *dat,void *a())
{
    int i=0;
    if(RF_READ_OK==1)
    {
        EX0 = 0;
        a();
        send_24(date);
        RF_READ_OK=0;
        for(i=0;i<3;i++)
        {
            dat[i]=0;
        }
        EX0 = 1;
    }
}
*/
#endif