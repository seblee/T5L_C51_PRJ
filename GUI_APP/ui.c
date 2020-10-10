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
        if (picNow == 0x000b)
        {
            caculateGroupCtrlPic();
        }
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

        {
            static u16 timerCounter = 0;
            u16 cache;
            ReadDGUS(0x0016, (u8*)&cache, 2);
            if (cache != 0)
            {
                cache = 0;
                WriteDGUS(0x0016, (u8*)&cache, 2);
                timerCounter = 0;
            }
            else
            {
                if (timerCounter < STANGBYTIME)
                    timerCounter++;
                else if (timerCounter == STANGBYTIME)
                {
                    timerCounter++;
                    JumpPage(0);
                    //用户等级
                }
            }
        }
    }
}
/*****************************************************************************
跳转指定页面*/
void JumpPage(uint16_t pageId)
{
    uint8_t temp[4] = {0x5A, 0x01, 0, 0};
    temp[2]         = (uint8_t)(pageId >> 8);
    temp[3]         = pageId;
    WriteDGUS(0x0084, temp, sizeof(temp));
    // do
    // {
    //     DelayMs(5);
    //     ReadDGUS(DHW_SPAGE, temp, 1);
    // } while (temp[0] != 0);
}

enum
{
    SYS_STS_FSM = 0,     // 140
    SYS_STS_MALFUN_0,    // 141
    SYS_STS_MALFUN_1,    // 142
    SYS_STS_ONLINE_0,    // 143
    SYS_STS_ONLINE_1,    // 144
    SYS_STS_FINALOUT_0,  // 145
    SYS_STS_FINALOUT_1,  // 146
    SYS_STS_STA_0_3,     // 147
    SYS_STS_STA_4_7,     // 148
    SYS_STS_STA_8_11,    // 149
    SYS_STS_STA_12_15,   // 150
    SYS_STS_STA_16_19,   // 151
    SYS_STS_STA_20_23,   // 152
    SYS_STS_STA_24_27,   // 153
    SYS_STS_STA_28_31,   // 154
    SYS_CONF_CNT,        // 155
    SYS_CONF_MODE = 26,  // 166
    MAX_SYS_STS

};
void caculateGroupCtrlPic(void)  //计算图标状态
{
#define GROUP_DEVICE_COUNT 18
    u16 mb_teamwork_sts_regs[31] = {0};
    u32 malfun_dev_bitmap, online_bitmap, final_out_bitmap;
    u8 i;
    u16 lw_teamwork_icon_sta[18] = {0};
    short status, temp;

    ReadDGUS(0xab20, (u8*)mb_teamwork_sts_regs, sizeof(mb_teamwork_sts_regs));
    malfun_dev_bitmap = mb_teamwork_sts_regs[SYS_STS_MALFUN_0] | (mb_teamwork_sts_regs[SYS_STS_MALFUN_1] << 16);
    online_bitmap     = mb_teamwork_sts_regs[SYS_STS_ONLINE_0] | (mb_teamwork_sts_regs[SYS_STS_ONLINE_1] << 16);
    final_out_bitmap  = mb_teamwork_sts_regs[SYS_STS_FINALOUT_0] | (mb_teamwork_sts_regs[SYS_STS_FINALOUT_1] << 16);
    for (i = 0; i < GROUP_DEVICE_COUNT; i++)
    {
        //在线状态
        if ((online_bitmap & (0x00000001 << i)) != 0)
        {
            //运行
            if ((final_out_bitmap & (0x00000001 << i)) != 0)
            {
                // get equipment status
                temp   = SYS_STS_STA_0_3 + (i / 4);
                status = (mb_teamwork_sts_regs[temp] >> (4 * (i % 4))) & 0x000f;
                switch (status)
                {
                    case 0x00:
                        lw_teamwork_icon_sta[i] = 5;
                        break;
                    case 0x01:  //制热
                        lw_teamwork_icon_sta[i] = 6;
                        break;
                    case 0x02:  //制冷
                        lw_teamwork_icon_sta[i] = 7;
                        break;
                    case 0x04:  //加湿
                        lw_teamwork_icon_sta[i] = 8;
                        break;
                    case 0x05:  //加湿+制热
                        lw_teamwork_icon_sta[i] = 9;
                        break;
                    case 0x06:  //加湿+制冷
                        lw_teamwork_icon_sta[i] = 10;
                        break;
                    case 0x08:  //除湿
                        lw_teamwork_icon_sta[i] = 3;
                        break;
                    case 0x09:  //除湿+制热
                        lw_teamwork_icon_sta[i] = 4;
                        break;
                    default:
                        lw_teamwork_icon_sta[i] = 5;
                        break;
                }
                //故障
                if ((malfun_dev_bitmap & (0x00000001 << i)) != 0)
                {
                    lw_teamwork_icon_sta[i] += 8;
                }
                else  //正常
                {
                    //保持原值
                }
            }
            else  //备用
            {     //故障
                if ((malfun_dev_bitmap & (0x00000001 << i)) != 0)
                {
                    lw_teamwork_icon_sta[i] = 2;
                }
                else  //正常
                {
                    lw_teamwork_icon_sta[i] = 1;
                }
            }
        }
        else
        {
            lw_teamwork_icon_sta[i] = 0;
        }
    }
    WriteDGUS(0xaba0, (u8*)lw_teamwork_icon_sta, sizeof(lw_teamwork_icon_sta));
}
