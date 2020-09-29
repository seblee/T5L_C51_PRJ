/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-08-24 15:54:57
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-09-01 16:10:57
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        :
 * @Description  :
 * @FilePath     : \T5L_C51_PRJ\GUI_APP\ui.h
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/
/**
 *                   .::::.
 *                 .::::::::.
 *                :::::::::::
 *             ..:::::::::::'
 *          '::::::::::::'
 *            .::::::::::
 *       '::::::::::::::..        女神助攻,流量冲天
 *            ..::::::::::::.     永不宕机,代码无bug
 *          ``::::::::::::::::
 *           ::::``:::::::::'        .:::.
 *          ::::'   ':::::'       .::::::::.
 *        .::::'      ::::     .:::::::'::::.
 *       .:::'       :::::  .:::::::::' ':::::.
 *      .::'        :::::.:::::::::'      ':::::.
 *     .::'         ::::::::::::::'         ``::::.
 * ...:::           ::::::::::::'              ``::.
 *```` ':.          ':::::::::'                  ::::..
 *                   '.:::::'                    ':'````..
 *******************************************************************/
#ifndef __UI_H
#define __UI_H

#include "sys.h"
#include "dgus.h"

typedef struct dgus_hex
{
    u16 VP;
    u16 X;
    u16 Y;
    u16 Color;
    u8 Byte_Num;
    u8 Lib_ID;
    u8 Font_x;
    char String_Code[15];
} dgus_hex_t;
typedef struct dgus_string
{
    u16 VP;
    u16 X;
    u16 Y;
    u16 Color;
    u16 Xs;
    u16 Ys;
    u16 Xe;
    u16 Ye;
    u16 Text_length;
    u8 Font0_ID;
    u8 Font1_ID;
    u8 Font_X_Dots;
    u8 Font_Y_Dots;
    u8 Encode_Mode;
    u8 HOR_Dis;
    u8 VER_Dis;
    u8 NC;
} dgus_string_t;

#define STANGBYTIME 300 * 2

extern u16 picNow;

void ui(void);
void JumpPage(uint16_t pageId);

#endif
