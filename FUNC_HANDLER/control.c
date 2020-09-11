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
            default:
                break;
        }
        touchEventFlag = 0;
        WriteDGUS(TOUCH_EVENT_FLAG, (u8*)&touchEventFlag, 2);
    }
}
