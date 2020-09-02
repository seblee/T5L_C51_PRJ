/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         : main.c
 * @Author       : xiaowine
 * @Date         : 2020-08-14 18:29:09
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-09-02 17:28:55
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        : 主函数，外设和参数初始化，主循环中主要功能函数入口。
 * @Description  :
 * @FilePath     : \T5L_C51_PRJ\USER\main.c
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/

/*****************************************************************************
系统库*/

/*****************************************************************************
自定义头文件*/
#include "T5LOS8051.H"
#include "sys.h"
// #include "handle.h"
//#include "password.h"
#include "uart.h"
#include "timer.h"
#include "rtc.h"
#include "modbus.h"
#include "ui.h"
#include "curve.h"
#include "string.h"
#include "alarm.h"
#include "ChineseCharacter.h"
#include "control.h"
/*****************************************************************************
主函数*/
void main(void)
{
    InitSys();
    init_rtc();  //硬件RTC初始化
    Modbus_UART_Init();
    curveInit();
    alarmInit();
    while (1)
    {
        WDT_RST();               //喂狗
        if (SysTick_RTC >= 500)  //原来是500，调试改为50
        {
            rdtime();  //更新硬件RTC时间
            // Uart2SendStr(test, strlen(test));
            SysTick_RTC = 0;
            RTC_Set_CMD();
        }
        HandleProc();
        Modbus_Process_Task();  // Modbus串口处理流程
        ui();
        curveProcess();
        alarmTask();
        touchHandler();
        if (timer1msFlag)
        {
            timer1msFlag   = 0;
            timer100msFlag = 0;
        }
    }
}
