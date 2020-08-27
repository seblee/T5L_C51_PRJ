/******************************************************************************
 * 文 件 名   : main.c
 * 版 本 号   : V1.0
 * 作    者   : pinot
 * 生成日期   : 2019年11月01日
 * 功能描述   : 主函数，外设和参数初始化，主循环中主要功能函数入口。
 * 修改历史   :
 * 日    期   :
 * 作    者   :
 * 修改内容   :
 ******************************************************************************/

/*****************************************************************************
系统库*/

/*****************************************************************************
自定义头文件*/
#include "T5LOS8051.H"
#include "sys.h"
// #include "handle.h"
//#include "password.h"
#include "timer.h"
#include "rtc.h"
#include "modbus.h"
#include "ui.h"
#include "curve.h"
/*****************************************************************************
主函数*/
u8 test[] = "uart4 send...";
void main(void)
{
    InitSys();
    init_rtc();  //硬件RTC初始化
    Modbus_UART_Init();
    curveInit();
    while (1)
    {
        WDT_RST();               //喂狗
        if (SysTick_RTC >= 500)  //原来是500，调试改为50
        {
            rdtime();  //更新硬件RTC时间
            // Uart4SendStr(test, sizeof(test));
            SysTick_RTC = 0;
        }
        // HandleProc();
        Modbus_Process_Task();  // Modbus串口处理流程
        RTC_Set_CMD();
        ui();
        curveProcess();
        if (timer1msFlag)
        {
            timer1msFlag   = 0;
            timer100msFlag = 0;
        }
    }
}
