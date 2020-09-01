/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-08-24 15:54:49
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-09-01 16:10:39
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        :
 * @Description  :
 * @FilePath     : \T5L_C51_PRJ\GUI_APP\ui.c
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/

#include "ui.h"
#include "timer.h"
u16 picNow = 0;

void ui(void)
{
    if (timer1msFlag)
    {
        ReadDGUS(0x0014, (u8*)(&picNow), 2);
    }
}
