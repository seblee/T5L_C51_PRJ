/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    ui.c
 * @Author  xiaowine@cee0.com
 * @date
 * @version V1.0
 *************************************************
 * @brief   ??????
 ****************************************************************************
 * @attention
 * Powered By Xiaowine
 * <h2><center>&copy;  Copyright(C) cee0.com 2015-2019</center></h2>
 * All rights reserved
 *
 **/

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
