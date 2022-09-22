#ifndef _RF_
#define _RF_
#include "main.h"
#include "Delay.h"
#include "Print.h"
/*
接收
需要:
1.外部中断
    上升沿触发
2.定时器1:
    分频后1us计数
    无需中断
 */
extern TIM_HandleTypeDef htim3;
#define RF_Read_TIM_1 htim3 //用于计时

#define RF_Read_GPIO_Group GPIOA    //外部中断的GPIO组
#define RF_Read_GPIO_Pin GPIO_PIN_2 //外部中断的管脚号
/*
发送
需要:
1.GPIO
    推挽/开漏上拉 输出
 */
#define RF_Write_GPIO_Group GPIOA    //输出的GPIO组
#define RF_Write_GPIO_Pin GPIO_PIN_1 //输出的端口号

#define RF_Data_Len 3

extern uint8_t RF_READ_OK;
extern uint8_t RF_READ_data[3];
void RF_Read_Decode(void);
void RF_Write_Send(uint8_t dat[3]);
#endif
