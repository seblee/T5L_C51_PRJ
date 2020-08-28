/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-08-27 17:35:14
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-08-28 14:42:39
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        :
 * @Description  :
 * @FilePath     : \T5L_C51_PRJ\GUI_APP\alarm.h
 * @ **************************************************************
 * @attention    : Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2015-2019</center></h2>
 * @All rights reserved
 ******************************************************************/
#ifndef __ALARM_H_
#define __ALARM_H_

/*****************************************************************************
系统库*/
#include "string.h"
#include "sys.h"

#define ALARM_FLASH_ADDR 0X2000

#define ALARM_LEN 512

#define alarmTemp 0x3000      //
#define ALARMDATALAG 0xa55a   //
#define ALARMINFOFLAG 0x5aa5  //

typedef struct alarmDataStruct
{
    u16 flag;
    u8 startY;
    u8 startM;
    u8 startD;
    u8 start_h;
    u8 start_m;
    u8 start_s;
    u8 endY;
    u8 endM;
    u8 endD;
    u8 end_h;
    u8 end_m;
    u8 end_s;
    u16 alarmCode;
} alarmDataStrc_t;
typedef struct alarmInfoStruct
{
    u16 flag;
    u16 head_ptr;
    u16 tail_ptr;
    u16 length;
} alarmInfoStrc_t;

#endif
