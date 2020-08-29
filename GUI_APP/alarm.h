/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-08-27 17:35:14
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-08-29 19:02:45
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

#define ALARM_FLASH_START 0X2000
#define ALARM_FLASH_END 0X2000

#define ALARM_LEN 512

#define alarmTemp 0x3000
#define ALARMDATALAG 0xa55a
#define ALARMINFOFLAG 0x5aa5

#define CURRENTALARMPAGE 14
#define alarmVPStart 0x3500
// show alarm list description
#define SHOWTIMESTART0 0x3600
#define SHOWTIMESTART1 0x3610
#define SHOWTIMESTART2 0x3620
#define SHOWTIMESTART3 0x3630
#define SHOWTIMESTART4 0x3640
#define SHOWTIMESTART5 0x3650
#define SHOWTIMESTART6 0x3660
#define SHOWTIMESTART7 0x3670
#define SHOWTIMESTART8 0x3680
#define SHOWTIMESTART9 0x3690

#define SHOWTIMEEND0 0x36a0
#define SHOWTIMEEND1 0x36b0
#define SHOWTIMEEND2 0x36c0
#define SHOWTIMEEND3 0x36d0
#define SHOWTIMEEND4 0x36e0
#define SHOWTIMEEND5 0x36f0
#define SHOWTIMEEND6 0x3700
#define SHOWTIMEEND7 0x3710
#define SHOWTIMEEND8 0x3720
#define SHOWTIMEEND9 0x3730

#define SHOWSTRING0 0x3740
#define SHOWSTRING1 0x3750
#define SHOWSTRING2 0x3760
#define SHOWSTRING3 0x3770
#define SHOWSTRING4 0x3780
#define SHOWSTRING5 0x3790
#define SHOWSTRING6 0x37a0
#define SHOWSTRING7 0x37b0
#define SHOWSTRING8 0x37c0
#define SHOWSTRING9 0x37d0

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

void alarmInit(void);
void saveAlarmHistory(void);

#endif
