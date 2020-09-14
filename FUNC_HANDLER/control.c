/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-09-02 10:32:56
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-09-02 11:15:57
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        :
 * @Description  :
 * @FilePath     : \T5L_C51_PRJ\FUNC_HANDLER\control.c
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/

#include "control.h"
#include "dgus.h"
#include "alarm.h"
#include "timer.h"
#include "modbus.h"

void touchHandler(void)
{
    u16 touchEventFlag = 0;
    if (!timer1msFlag)
        return;
    ReadDGUS(TOUCH_EVENT_FLAG, (u8*)&touchEventFlag, 2);
    if (touchEventFlag)
    {
        switch (touchEventFlag)
        {
            case ALARM_CLEAR_EVENT:
                alarmClearHandler();
                break;
            case OUTPUT_EVENT:
                forcedOutput();
                break;
            case RESET_EVENT:
                resetEventHandl();
                break;
            case CLEAR_RUNTIME_EVENT_1:
            case CLEAR_RUNTIME_EVENT_6:
            case CLEAR_RUNTIME_EVENT_c:
            case CLEAR_RUNTIME_EVENT_d:
            case CLEAR_RUNTIME_EVENT_10:
                clearRunTimeHandle(touchEventFlag);
                break;
            default:
                break;
        }
        touchEventFlag = 0;
        WriteDGUS(TOUCH_EVENT_FLAG, (u8*)&touchEventFlag, 2);
    }
}

void resetEventHandl(void)
{
    u16 cache;
    cache = 0x005a;
    WriteDGUS(0xc520, (u8*)&cache, 2);
    WriteDGUS(0xc580, (u8*)&cache, 2);
}

void clearRunTimeHandle(u16 eventId)
{
    u16 cache = eventId & 0xff;
    WriteDGUS(0xc930, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xc990, (u8*)&cache, 2);
}
