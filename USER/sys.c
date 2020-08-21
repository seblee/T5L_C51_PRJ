/******************************************************************************
 * 文 件 名   : sys.c
 * 版 本 号   : V1.0
 * 作    者   : pinot
 * 生成日期   : 2019年11月01日
 * 功能描述   : 初始化T5L ASIC 的寄存器
 * 修改历史   :
 * 日    期   :
 * 作    者   :
 * 修改内容   :
 ******************************************************************************/

/*****************************************************************************
自定义头文件*/
#include "sys.h"
#include "uart.h"
#include "timer.h"
#include "T5L_lib.h"
/*****************************************************************************
全局变量*/

/*****************************************************************************
延时us*//*振荡周期T=1/206438400*/
// void DelayUs(uint16_t n)
//{
//  uint16_t i,j;
//  for(i=0;i<n;i++)
//    for(j=0;j<15;j++);
//}
/*****************************************************************************
延时ms*/
void DelayMs(uint16_t n)
{
    uint16_t i, j;
    for (i = 0; i < n; i++)
        for (j = 0; j < 7400; j++)
            ;
}

/*****************************************************************************
初始化T5L必须配置的寄存器*/
// void InitCFG(void)
//{
//  CKCON      = 0x00;
//  T2CON      = 0x70;
//  DPC        = 0x00;
//  PAGESEL    = 0x01;
//  D_PAGESEL  = 0x02;
//  MUX_SEL    = 0x60;
//  PORTDRV    = 0x01;
//  RAMMODE    = 0x00;
//}

/*****************************************************************************
中断配置*/
void InitInt(void)
{
    initcpu();
    // IEN0 = 0x00; /*关闭所有中断*/
    // IEN1 = 0x00;
    // IEN2 = 0x00;
    // IP0  = 0x00; /*中断优先级默认*/
    // IP1  = 0x00;
}
/*****************************************************************************
GPIO输出*/
void InitGPIO(void)
{
    PORTDRV = 0x01;
    P0MDOUT = 0x03; /*开启P0_0和P0_1的输出*/
    P1MDOUT = 0x03; /*开启P1_0和P1_1的输出*/
    P2MDOUT = 0x00;
    P3MDOUT = 0x0C;
    P0      = 0x00;
    P1      = 0x00;
    P2      = 0x00;
    P3      = 0x00;
}
void EX0_ISR_PC(void) interrupt 0
{
    EA = 0;

    EA = 1;
}
void EX1_ISR_PC(void) interrupt 2
{
    EA = 0;

    EA = 1;
}
/*****************************************************************************
初始化T5L ASIC*/
void InitSys(void)
{
    EA = 0;
    //  InitCFG();      /*改为在A51启动文件初始化*/
    InitInt();
    InitGPIO();
    InitUart();
    InitTimer();
    WDT_ON();
    EA = 1;
}
