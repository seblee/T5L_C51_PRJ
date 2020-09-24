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
    if (MS1msFlag)
    {
        ReadDGUS(0x0014, (u8*)(&picNow), 2);
    }
    if (MS500msFlag)
    {
        if (picNow == 0x0029)
        {
            u8 i;
            u16 hour_reg;
            u16 mb_comp_runtime[14], sys_comp_runtime[14];
            ReadDGUS(0xc9b0, (u8*)mb_comp_runtime, 28);

            for (i = 0; i < 7; i++)
            {
                hour_reg                = (mb_comp_runtime[2 * i] << 4) + (mb_comp_runtime[2 * i + 1] >> 12);
                sys_comp_runtime[i]     = hour_reg / 24;
                sys_comp_runtime[i + 7] = hour_reg % 24;
            }
            WriteDGUS(0xc920, (u8*)sys_comp_runtime, 28);
        }
        if (picNow == 0x002c)
        {
            u16 cache[4];
            ReadDGUS(0xcca0, (u8*)&cache[3], 2);
            cache[0] = ((cache[3] >> 4) & 0x0f00);
            cache[0] |= ((cache[3] >> 7) & 0x1f);
            cache[1] = ((cache[3] & 0x007f) << 8);
            cache[2] = SOFTWARE_VER;
            WriteDGUS(0xcc20, (u8*)cache, 6);
        }
        {
            static u8 diagnosisPageInCount  = 0;
            static u8 diagnosisPageOutCount = 0;
            if (picNow == 0x0027)
            {
                if (diagnosisPageInCount < 5)
                {
                    u16 cache = 1;
                    WriteDGUS(0xa025, (u8*)&cache, 2);
                    cache = 0x005a;
                    WriteDGUS(0xa085, (u8*)&cache, 2);
                    diagnosisPageInCount++;
                }
                diagnosisPageOutCount = 0;
            }
            else
            {
                if (diagnosisPageOutCount < 5)
                {
                    u16 cache = 0;
                    WriteDGUS(0xa025, (u8*)&cache, 2);
                    cache = 0x005a;
                    WriteDGUS(0xa085, (u8*)&cache, 2);
                    diagnosisPageOutCount++;
                }
                diagnosisPageInCount = 0;
            }
        }
    }
}
