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
    if (!MS1msFlag)
        return;
    ReadDGUS(TOUCH_EVENT_FLAG, (u8*)&touchEventFlag, 2);
    if (touchEventFlag)
    {
        switch (touchEventFlag)
        {
            case ALARM_CLEAR_EVENT:
                alarmClearHandle();
                break;
            case CUR_ALARM_CLEAR_EVENT:
                curAlarmClearHandle();
                break;
            case OUTPUT_EVENT:
                forcedOutputHnadle();
                break;
            case RESET_EVENT:
                resetEventHandle();
                break;
            case CLEAR_RUNTIME_EVENT_1:
            case CLEAR_RUNTIME_EVENT_6:
            case CLEAR_RUNTIME_EVENT_c:
            case CLEAR_RUNTIME_EVENT_d:
            case CLEAR_RUNTIME_EVENT_10:
                clearRunTimeHandle(touchEventFlag);
                break;
            case POWER_SWITCH_EVENT:
                powerSwitchEventHandle();
                break;
            case ALARM_CONFIRM_EVENT:
                alarmConfirmEventHandle();
                break;
            case IN_MAINTAIN_MOD_EVENT:
                inMaintainModEventHandle();
                break;
            case OUT_MAINTAIN_MOD_EVENT:
                outMaintainModEventHandle();
                break;
            default:
                break;
        }
        touchEventFlag = 0;
        WriteDGUS(TOUCH_EVENT_FLAG, (u8*)&touchEventFlag, 2);
    }
}

void resetEventHandle(void)
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
void powerSwitchEventHandle(void)
{
    u16 cache;
    ReadDGUS(0xa023, (u8*)&cache, 2);
    if (cache & 1)
    {
        cache = 0;
    }
    else
    {
        cache = 1;
    }
    WriteDGUS(0xa024, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xa084, (u8*)&cache, 2);
}

void inMaintainModEventHandle(void)
{
    u16 cache = 0x005a;
    WriteDGUS(0xc700, (u8*)&cache, 2);
    cache = 1;
    WriteDGUS(0xa025, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xa085, (u8*)&cache, 2);
}
void outMaintainModEventHandle(void)
{
    u16 cache = 0;
    WriteDGUS(0xa025, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xa085, (u8*)&cache, 2);
}
