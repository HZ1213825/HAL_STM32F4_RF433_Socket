#include "RF.h"

/*
接收部分
*/

uint8_t RF_READ_OK = 0;        //解码成功标志
uint8_t RF_READ_ins = 0;       //状态指示
uint8_t RF_READ_i = 0;         //循环
uint8_t RF_READ_j = 0;         //循环
int RF_READ_time = 0;          //计算时间
uint8_t RF_READ_data[3] = {0}; //数据
/**
 * @brief 定时器归零
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-09-22 10:56:46
 */
void RF_Read_Clean(void)
{
    __HAL_TIM_SetCounter(&RF_Read_TIM_1, 0);
}
/**
 * @brief 提取定时器数
 * @param 无
 * @return 定时器数
 * @author HZ12138
 * @date 2022-09-22 10:58:09
 */
int RF_Read_GetTIMNum(void)
{
    int num = 0;
    num = __HAL_TIM_GetCounter(&RF_Read_TIM_1);
    return num;
}
/**
 * @brief 解码函数(外部中断调用)
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-09-22 10:58:28
 */
void RF_Read_Decode(void)
{
    if (RF_READ_OK == 0)
    {
        if (RF_READ_ins == 0) //初始化 检测到边沿
        {
            RF_Read_Clean();
            RF_READ_ins = 1;
        }
        else if (RF_READ_ins == 1)
        {

            RF_READ_time = RF_Read_GetTIMNum();                           //计算从上次边沿到此时间
            if (RF_READ_time > 8000 - 2000 && RF_READ_time < 8000 + 2000) //处理幻听
            {
                RF_Read_Clean();
                RF_READ_ins = 2; //开始解码
                RF_READ_i = 0;
            }
            else
            {
                RF_Read_Clean();
                RF_READ_ins = 0; //复位
            }
            for (int i = 0; i < RF_Data_Len; i++)
                RF_READ_data[i] = 0;
        }
        else if (RF_READ_ins == 2) //解码
        {

            RF_READ_time = RF_Read_GetTIMNum();                              //计算时间做验证
            if (HAL_GPIO_ReadPin(RF_Read_GPIO_Group, RF_Read_GPIO_Pin) == 0) //计算高电平时间
            {
                RF_READ_data[RF_READ_j] <<= 1;
                if (RF_READ_time > 400)
                    RF_READ_data[RF_READ_j] |= 0x01;
                else
                    RF_READ_data[RF_READ_j] &= 0xFE;

                RF_READ_i++;

                if (RF_READ_i >= 8) // 8位数据写完 换行
                {
                    RF_READ_i = 0;
                    RF_READ_j++;
                }
            }
            else if (HAL_GPIO_ReadPin(RF_Read_GPIO_Group, RF_Read_GPIO_Pin) == 1)
            {

                if (RF_READ_time > 1050 - 300 && RF_READ_time < 1050 + 300) //每位数据的时间长度验证
                {
                    RF_Read_Clean();
                }
                else
                {
                    RF_READ_ins = 0;
                }
            }

            if (RF_READ_j >= RF_Data_Len) // 数据写完 完成标志写1 复位
            {
                RF_READ_j = 0;
                RF_READ_i = 0;
                RF_READ_OK = 1;
                RF_READ_ins = 1;
                RF_Read_Clean();
            }
        }
    }
}

/*

示例调用

if (RF_READ_OK == 1)
    {
      for (int i = 0; i < 3; i++)
      {
        printf("%X", RF_READ_data[i]);
      }
      RF_READ_OK = 0;
      printf("\r\n");
    }
*/
/*
发送部分
 */
/**
 * @brief 发送1
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-09-22 17:31:32
 */
void RF_Write_Send1(void)
{
    HAL_GPIO_WritePin(RF_Write_GPIO_Group, RF_Write_GPIO_Pin, GPIO_PIN_SET);

    Delay_us(755);
    HAL_GPIO_WritePin(RF_Write_GPIO_Group, RF_Write_GPIO_Pin, GPIO_PIN_RESET);
    Delay_us(305);
}
/**
 * @brief 发送0
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-09-22 17:31:50
 */
void RF_Write_Send0(void)
{
    HAL_GPIO_WritePin(RF_Write_GPIO_Group, RF_Write_GPIO_Pin, GPIO_PIN_SET);
    Delay_us(305);
    HAL_GPIO_WritePin(RF_Write_GPIO_Group, RF_Write_GPIO_Pin, GPIO_PIN_RESET);
    Delay_us(755);
}
/**
 * @brief 发送诱导波后延迟
 * @param 无
 * @return 无
 * @author HZ12138
 * @date 2022-09-22 17:32:22
 */
void RF_Write_SendBefore(void)
{
    RF_Write_Send1();
    RF_Write_Send0();
    RF_Write_Send1();
    HAL_GPIO_WritePin(RF_Write_GPIO_Group, RF_Write_GPIO_Pin, GPIO_PIN_RESET);
    Delay_us(8300);
}
/**
 * @brief 发送
 * @param
 * @return
 * @author HZ12138
 * @date 2022-09-22 17:33:05
 */
void RF_Write_SendAfter(void)
{
    HAL_GPIO_WritePin(RF_Write_GPIO_Group, RF_Write_GPIO_Pin, GPIO_PIN_SET);
    Delay_us(305);
    HAL_GPIO_WritePin(RF_Write_GPIO_Group, RF_Write_GPIO_Pin, GPIO_PIN_RESET);
    Delay_us(8300);
}
/**
 * @brief 发送一次数据
 * @param dat:数据数组地址
 * @return 无
 * @author HZ12138
 * @date 2022-09-22 18:05:01
 */
void RF_Write_Send_1T(uint8_t dat[3])
{
    int i = 0, j = 0;

    // RF_Write_SendBefore();
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (dat[i] & 0x80)
            {
                RF_Write_Send1();
            }
            else
            {
                RF_Write_Send0();
            }
            dat[i] <<= 1;
        }
    }
    RF_Write_SendAfter();
}
/**
 * @brief 发送多次数据
 * @param dat:数据数组地址
 * @return 无
 * @author HZ12138
 * @date 2022-09-22 18:05:01
 */
void RF_Write_Send(uint8_t dat[RF_Data_Len]) //发送10次
{
    int i = 10;
    uint8_t a[RF_Data_Len];
    for (i = 0; i < RF_Data_Len; i++)
        a[i] = dat[i];
    i = 10;
    while (i--)
    {
        RF_Write_Send_1T(a);
    }
}
