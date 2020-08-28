/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-08-27 17:35:07
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-08-28 14:49:25
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

#include "alarm.h"
#include "T5L_lib.h"
#include "dgus.h"

u16 alarmStart = 0;
u16 alarmPage  = 0;

alarmInfoStrc_t alarmInfomation = {0, 0, 0, 0};
/*******************************************************************
 * @description:
 * @param {type}
 * @return {type}
 ******************************************************************/
void alarmInit(void)
{
    u8 cache[32]          = {0};
    u16 i;
    alarmDataStrc_t *alarmData =(alarmDataStrc_t *) cache;
    alarmInfoStrc_t *alarmInfo = (alarmInfoStrc_t *)cache;

    for (  i = 0; i < ALARM_LEN; i++)
    {
        T5L_Flash(READRFLASH, alarmTemp, ALARM_FLASH_ADDR + i * 8, 8);

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
}
