/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-08-27 17:35:07
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-08-29 18:40:49
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        :
 * @Description  :
 * @FilePath     : \T5L_C51_PRJ\GUI_APP\alarm.c
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/

#include "sys.h"
#include "alarm.h"
#include "T5L_lib.h"
#include "dgus.h"
#include "ui.h"
#include "timer.h"
#include "ChineseCharacter.h"
u16 alarmStart = 0;
u16 alarmPage  = 0;

alarmInfoStrc_t alarmInfomation = {0, 0, 0, 0};
static u16 alarmState[2];
#define START_Xs 105
#define START_Ys 100
#define START_Xe 285
#define START_Ye 125

#define END_Xs 295
#define END_Ys 100
#define END_Xe 475
#define END_Ye 125

#define STRING_Xs 485
#define STRING_Ys 100
#define STRING_Xe 685
#define STRING_Ye 125

#define END_X 295
#define STRING_X 485
const u16 showStartDes[16] = {
    //  VP               X  Y
    alarmVPStart + 1, START_Xs, START_Ys, 0, 0x8600, 0x0a2d, 0x2d2f, 0x3a3a, 0, 0, 0, 0, 0, 0, 0, 0,
};
const u16 showEndDes[16] = {
    //  VP               X  Y
    alarmVPStart + 4, END_Xs, END_Ys, 0, 0x8600, 0x0a2d, 0x2d2f, 0x3a3a, 0, 0, 0, 0, 0, 0, 0, 0,
};
const u16 showStringDes[16] = {
    //  VP               X  Y
    alarmVPStart + 8, STRING_Xs, STRING_Ys, 0,      STRING_Xs, STRING_Ys, STRING_Xe, STRING_Ye,
    0X0010,           0x0017,    0x1818,    0x0200, 0x0000,    0,         0,         0,
};

const u16 alarmShowDescriptionVP[10][3] = {
    {SHOWTIMESTART0, SHOWTIMEEND0, SHOWSTRING0},  //
    {SHOWTIMESTART1, SHOWTIMEEND1, SHOWSTRING1},  //
    {SHOWTIMESTART2, SHOWTIMEEND2, SHOWSTRING2},  //
    {SHOWTIMESTART3, SHOWTIMEEND3, SHOWSTRING3},  //
    {SHOWTIMESTART4, SHOWTIMEEND4, SHOWSTRING4},  //
    {SHOWTIMESTART5, SHOWTIMEEND5, SHOWSTRING5},  //
    {SHOWTIMESTART6, SHOWTIMEEND6, SHOWSTRING6},  //
    {SHOWTIMESTART7, SHOWTIMEEND7, SHOWSTRING7},  //
    {SHOWTIMESTART8, SHOWTIMEEND8, SHOWSTRING8},  //
    {SHOWTIMESTART9, SHOWTIMEEND9, SHOWSTRING9},  //
};
u8 showPage  = 0;
u8 showIndex = 0;
/*******************************************************************
 * @description:
 * @param {type}
 * @return {type}
 ******************************************************************/
void alarmInit(void)
{
    u8 cache[32] = {0};
    u16 i;
    alarmDataStrc_t *alarmData = (alarmDataStrc_t *)cache;
    alarmInfoStrc_t *alarmInfo = (alarmInfoStrc_t *)cache;

    for (i = 0; i < ALARM_LEN; i++)
    {
        T5L_Flash(READRFLASH, alarmTemp, ALARM_FLASH_START + i * 8, 8);

        ReadDGUS(alarmTemp, (u8 *)&cache, sizeof(alarmDataStrc_t));
        if (alarmData->flag == ALARMDATALAG)
        {
            continue;
        }
        if (alarmInfo->flag == ALARMINFOFLAG)
        {
            memcpy(&alarmInfomation, alarmInfo, sizeof(alarmInfoStrc_t));
            break;
        }
        break;
    }
    for (i = 0; i < 10; i++)
    {
    }
}
void alarmTask(void)
{
    u8 cache[50];
    if ((picNow != CURRENTALARMPAGE) || (timer100msFlag != 1))
    {
        return;
    }
    ReadDGUS(alarmTemp, (u8 *)cache, 2);
    if (memcmp(cache, alarmState, 2))
    {
        alarmDataStrc_t *alarmData;
        u8 i;
        memcpy(alarmState, cache, 2);
        for (i = 0; i < 16; i++)
        {
            ReadDGUS(alarmVPStart + i * 24, (u8 *)&cache, 48);  // get memory
            alarmData = (alarmDataStrc_t *)cache;
            if (alarmState[0] & (1 << i))
            {
                if (alarmData->flag != ALARMDATALAG)
                {
                    alarmData->flag = ALARMDATALAG;
                    ReadDGUS(0x0010, (u8 *)&cache[20], 8);  // read time
                    memcpy(&cache[2], &cache[20], 3);
                    memcpy(&cache[5], &cache[24], 3);
                    memset(&cache[8], 0, 8);
                    *((u16 *)&cache[14]) = i;
                    strncpy(&cache[16], &alarmMessage[i][0], 32);
                    WriteDGUS(alarmVPStart + i * 24, (u8 *)&cache, 48);  // write memory
                }
                if ((i > (showPage * 10)) && (showIndex < 10))
                {
                    *((u16 *)&cache[0]) = alarmVPStart + i * 24 + 1;
                    WriteDGUS(alarmShowDescriptionVP[showIndex][0], (u8 *)&cache, 2);
                    *((u16 *)&cache[0]) = alarmVPStart + i * 24 + 4;
                    WriteDGUS(alarmShowDescriptionVP[showIndex][1], (u8 *)&cache, 2);
                    *((u16 *)&cache[0]) = alarmVPStart + i * 24 + 8;
                    WriteDGUS(alarmShowDescriptionVP[showIndex][2], (u8 *)&cache, 2);
                }
            }
            else
            {
                if (alarmData->flag == ALARMDATALAG)
                {
                    ReadDGUS(0x0010, (u8 *)&cache[20], 8);  // read time
                    memcpy(&cache[8], &cache[20], 3);
                    memcpy(&cache[11], &cache[24], 3);
                    *((u16 *)&cache[14]) = i;
                    WriteDGUS(alarmTemp, (u8 *)&cache, 16);  // write memory
                    saveAlarmHistory();
                    *((u16 *)&cache[0]) = 0;
                    WriteDGUS(alarmVPStart + i * 24, (u8 *)&cache, 2);  // write memory
                }
            }
        }
    }
}

void alarmHandler(void)
{
}
void saveAlarmHistory(void)
{
    T5L_Flash(WRITERFLASH, alarmTemp, ALARM_FLASH_START + alarmInfomation.head_ptr * 8, 8);
    if (alarmInfomation.head_ptr > ALARM_LEN)
        alarmInfomation.head_ptr = 0;
    else
    {
        alarmInfomation.head_ptr++;
    }
    if (alarmInfomation.length >= ALARM_LEN)
    {
        if (alarmInfomation.tail_ptr > ALARM_LEN)
            alarmInfomation.tail_ptr = 0;
        else
        {
            alarmInfomation.tail_ptr++;
        }
    }
    else
    {
        alarmInfomation.length++;
    }
    WriteDGUS(alarmTemp, (u8 *)&alarmInfomation, sizeof(alarmInfoStrc_t));  // write memory
    T5L_Flash(WRITERFLASH, alarmTemp, ALARM_FLASH_START + alarmInfomation.head_ptr * 8, 4);
}