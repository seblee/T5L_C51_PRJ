/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-09-02 10:33:07
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-09-02 11:14:22
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        :
 * @Description  :
 * @FilePath     : \T5L_C51_PRJ\FUNC_HANDLER\control.h
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/
#ifndef __CONTROL_H_
#define __CONTROL_H_
#include "sys.h"

#define TOUCH_EVENT_FLAG 0x5014

#define POWER_SWITCH_EVENT 0Xa000
#define ALARM_CLEAR_EVENT 0Xac00
#define CUR_ALARM_CLEAR_EVENT 0Xac01
#define ALARM_CONFIRM_EVENT 0Xa026
#define RESET_EVENT 0Xc500
#define IN_MAINTAIN_MOD_EVENT 0Xc600
#define OUTPUT_EVENT 0Xc700
#define OUT_MAINTAIN_MOD_EVENT 0Xc701
#define CLEAR_RUNTIME_EVENT_1 0Xc901
#define CLEAR_RUNTIME_EVENT_6 0Xc906
#define CLEAR_RUNTIME_EVENT_c 0Xc90c
#define CLEAR_RUNTIME_EVENT_d 0Xc90d
#define CLEAR_RUNTIME_EVENT_10 0Xc910

void touchHandler(void);

void resetEventHandle(void);
void clearRunTimeHandle(u16 eventId);
void powerSwitchEventHandle(void);
void inMaintainModEventHandle(void);
void outMaintainModEventHandle(void);

#endif
