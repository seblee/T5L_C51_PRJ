/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         :
 * @Author       : xiaowine
 * @Date         : 2020-09-02 10:32:56
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-09-02 11:15:57
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        :
 * @Description  :
 * @FilePath     : \T5L_C51_PRJ\FUNC_HANDLER\control.c
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/

#include "control.h"
#include "dgus.h"
#include "alarm.h"
#include "timer.h"
#include "modbus.h"
#include "ui.h"

const u8 password[][4] = {
    {0x00, 0x0a, 0x2d, 0x08},  //设置 666888
    {0x00, 0x02, 0x73, 0x60},  //维护 160608
};

const u8 pageLevel[][2] = {
    {PAGE00, 0},  //  PASSWORD_PAGEJUMP_00_EVENT
    {PAGE01, 0},  //  PASSWORD_PAGEJUMP_01_EVENT
    {PAGE02, 0},  //  PASSWORD_PAGEJUMP_02_EVENT
    {PAGE03, 0},  //  PASSWORD_PAGEJUMP_03_EVENT
    {PAGE04, 0},  //  PASSWORD_PAGEJUMP_04_EVENT
    {PAGE05, 0},  //  PASSWORD_PAGEJUMP_05_EVENT
    {PAGE06, 0},  //  PASSWORD_PAGEJUMP_06_EVENT
    {PAGE07, 0},  //  PASSWORD_PAGEJUMP_07_EVENT
    {PAGE08, 0},  //  PASSWORD_PAGEJUMP_08_EVENT
    {PAGE09, 0},  //  PASSWORD_PAGEJUMP_09_EVENT
    {PAGE10, 0},  //  PASSWORD_PAGEJUMP_0A_EVENT
    {PAGE11, 0},  //  PASSWORD_PAGEJUMP_0B_EVENT
    {PAGE12, 0},  //  PASSWORD_PAGEJUMP_0C_EVENT
    {PAGE13, 0},  //  PASSWORD_PAGEJUMP_0D_EVENT
    {PAGE14, 0},  //  PASSWORD_PAGEJUMP_0E_EVENT
    {PAGE15, 0},  //  set page
    {PAGE16, 0},  //  PASSWORD_PAGEJUMP_10_EVENT
    {PAGE17, 0},  //  PASSWORD_PAGEJUMP_11_EVENT
    {PAGE18, 0},  //  PASSWORD_PAGEJUMP_12_EVENT
    {PAGE19, 0},  //  PASSWORD_PAGEJUMP_13_EVENT
    {PAGE20, 0},  //  PASSWORD_PAGEJUMP_14_EVENT
    {PAGE21, 0},  //  PASSWORD_PAGEJUMP_15_EVENT
    {PAGE22, 0},  //  PASSWORD_PAGEJUMP_16_EVENT
    {PAGE23, 0},  //  PASSWORD_PAGEJUMP_17_EVENT
    {PAGE24, 0},  //  PASSWORD_PAGEJUMP_18_EVENT
    {PAGE25, 0},  //  PASSWORD_PAGEJUMP_19_EVENT
    {PAGE26, 0},  //  PASSWORD_PAGEJUMP_1A_EVENT
    {PAGE27, 0},  //  PASSWORD_PAGEJUMP_1B_EVENT
    {PAGE28, 0},  //  PASSWORD_PAGEJUMP_1C_EVENT
    {PAGE29, 0},  //  PASSWORD_PAGEJUMP_1D_EVENT
    {PAGE30, 0},  //  PASSWORD_PAGEJUMP_1E_EVENT
    {PAGE31, 0},  //  PASSWORD_PAGEJUMP_1F_EVENT
    {PAGE32, 0},  //  PASSWORD_PAGEJUMP_20_EVENT
    {PAGE33, 0},  //  PASSWORD_PAGEJUMP_21_EVENT
    {PAGE34, 0},  //  PASSWORD_PAGEJUMP_22_EVENT
    {PAGE35, 0},  //  PASSWORD_PAGEJUMP_23_EVENT
    {PAGE36, 0},  //  PASSWORD_PAGEJUMP_24_EVENT
    {PAGE37, 0},  //  PASSWORD_PAGEJUMP_25_EVENT
    {PAGE38, 1},  //  MAINTAIM PAGE
    {PAGE39, 0},  //  PASSWORD_PAGEJUMP_27_EVENT
    {PAGE40, 0},  //  PASSWORD_PAGEJUMP_28_EVENT
    {PAGE41, 0},  //  PASSWORD_PAGEJUMP_29_EVENT
    {PAGE42, 0},  //  PASSWORD_PAGEJUMP_2A_EVENT
    {PAGE43, 0},  //  PASSWORD_PAGEJUMP_2B_EVENT
    {PAGE44, 0},  //  PASSWORD_PAGEJUMP_2C_EVENT
    {PAGE45, 0},  //  PASSWORD_PAGEJUMP_2D_EVENT
    {PAGE46, 0},  //  PASSWORD_PAGEJUMP_2E_EVENT
    {PAGE47, 0},  //  PASSWORD_PAGEJUMP_2F_EVENT
    {PAGE48, 0},  //  PASSWORD_PAGEJUMP_30_EVENT
    {PAGE49, 0},  //  PASSWORD_PAGEJUMP_31_EVENT
};
const u8 funLevel[][2] = {
    {FUN00, 1},
    {FUN01, 1},
};

u16 jumpPage    = 0;
u16 currentPage = 0;
static _password_mode_t mode;
static u16 funOpt      = 0;
static u8 currentLevel = 0;

void touchHandler(void)
{
    u16 touchEventFlag = 0;
    if (!MS1msFlag)
        return;
    ReadDGUS(TOUCH_EVENT_FLAG, (u8*)&touchEventFlag, 2);
    if (touchEventFlag)
    {
        switch (touchEventFlag)
        {
            case POWER_SWITCH_EVENT:
                powerSwitchEventHandle();
                break;
            case ALARM_CONFIRM_EVENT:
                alarmConfirmEventHandle();
                break;
            case PASSWORD_CONFIRM_EVENT:
                passwordConfirmEventHandle();
                break;
            case PASSWORD_CANCLE_EVENT:
                passwordCancleEventHandle();
                break;
            case PASSWORD_PAGEJUMP_00_EVENT:
            case PASSWORD_PAGEJUMP_01_EVENT:
            case PASSWORD_PAGEJUMP_02_EVENT:
            case PASSWORD_PAGEJUMP_03_EVENT:
            case PASSWORD_PAGEJUMP_04_EVENT:
            case PASSWORD_PAGEJUMP_05_EVENT:
            case PASSWORD_PAGEJUMP_06_EVENT:
            case PASSWORD_PAGEJUMP_07_EVENT:
            case PASSWORD_PAGEJUMP_08_EVENT:
            case PASSWORD_PAGEJUMP_09_EVENT:
            case PASSWORD_PAGEJUMP_0A_EVENT:
            case PASSWORD_PAGEJUMP_0B_EVENT:
            case PASSWORD_PAGEJUMP_0C_EVENT:
            case PASSWORD_PAGEJUMP_0D_EVENT:
            case PASSWORD_PAGEJUMP_0E_EVENT:
            case PASSWORD_PAGEJUMP_0F_EVENT:
            case PASSWORD_PAGEJUMP_10_EVENT:
            case PASSWORD_PAGEJUMP_11_EVENT:
            case PASSWORD_PAGEJUMP_12_EVENT:
            case PASSWORD_PAGEJUMP_13_EVENT:
            case PASSWORD_PAGEJUMP_14_EVENT:
            case PASSWORD_PAGEJUMP_15_EVENT:
            case PASSWORD_PAGEJUMP_16_EVENT:
            case PASSWORD_PAGEJUMP_17_EVENT:
            case PASSWORD_PAGEJUMP_18_EVENT:
            case PASSWORD_PAGEJUMP_19_EVENT:
            case PASSWORD_PAGEJUMP_1A_EVENT:
            case PASSWORD_PAGEJUMP_1B_EVENT:
            case PASSWORD_PAGEJUMP_1C_EVENT:
            case PASSWORD_PAGEJUMP_1D_EVENT:
            case PASSWORD_PAGEJUMP_1E_EVENT:
            case PASSWORD_PAGEJUMP_1F_EVENT:
            case PASSWORD_PAGEJUMP_20_EVENT:
            case PASSWORD_PAGEJUMP_21_EVENT:
            case PASSWORD_PAGEJUMP_22_EVENT:
            case PASSWORD_PAGEJUMP_23_EVENT:
            case PASSWORD_PAGEJUMP_24_EVENT:
            case PASSWORD_PAGEJUMP_25_EVENT:
            case PASSWORD_PAGEJUMP_26_EVENT:
            case PASSWORD_PAGEJUMP_27_EVENT:
            case PASSWORD_PAGEJUMP_28_EVENT:
            case PASSWORD_PAGEJUMP_29_EVENT:
            case PASSWORD_PAGEJUMP_2A_EVENT:
            case PASSWORD_PAGEJUMP_2B_EVENT:
            case PASSWORD_PAGEJUMP_2C_EVENT:
            case PASSWORD_PAGEJUMP_2D_EVENT:
            case PASSWORD_PAGEJUMP_2E_EVENT:
            case PASSWORD_PAGEJUMP_2F_EVENT:
            case PASSWORD_PAGEJUMP_30_EVENT:
            case PASSWORD_PAGEJUMP_31_EVENT:
                passwordPageJumpEventHandle(touchEventFlag);
                break;
            case PASSWORD_FUN_00_EVENT:
                passwordFunEventHandle(touchEventFlag);
                break;
            case PASSWORD_FUN_01_EVENT:
                passwordFunEventHandle(touchEventFlag);
                break;
            case ALARM_CLEAR_EVENT:
                alarmClearHandle();
                break;
            case CUR_ALARM_CLEAR_EVENT:
                curAlarmClearHandle();
                break;
            case OUTPUT_EVENT:
                forcedOutputHnadle();
                break;
            case RESET_EVENT:
                resetEventHandle();
                break;
            case IN_MAINTAIN_MOD_EVENT:
                inMaintainModEventHandle();
                break;
            case OUT_MAINTAIN_MOD_EVENT:
                outMaintainModEventHandle();
                break;

            case CLEAR_RUNTIME_EVENT_0D:
            case CLEAR_RUNTIME_EVENT_07:
            case CLEAR_RUNTIME_EVENT_01:
            case CLEAR_RUNTIME_EVENT_0E:
            case CLEAR_RUNTIME_EVENT_11:
                clearRunTimeHandle(touchEventFlag);
                break;

            case REST_ORIGINAL_PARA:
                resetOriginalPara();
                break;
            case SAVE_FACTORY_CFG:
                saveFactoryCFG();
                break;
            default:
                break;
        }
        touchEventFlag = 0;
        WriteDGUS(TOUCH_EVENT_FLAG, (u8*)&touchEventFlag, 2);
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

void resetEventHandle(void)
{
    u16 cache;
    cache = 0x005a;
    WriteDGUS(0xc520, (u8*)&cache, 2);
    WriteDGUS(0xc580, (u8*)&cache, 2);
}

void clearRunTimeHandle(u16 eventId)
{
    u16 cache = eventId & 0xff;
    WriteDGUS(0xc930, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xc990, (u8*)&cache, 2);
}
void powerSwitchEventHandle(void)
{
    u16 cache;
    ReadDGUS(0xa023, (u8*)&cache, 2);
    if (cache & 1)
    {
        cache = 0;
    }
    else
    {
        cache = 1;
    }
    WriteDGUS(0xa024, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xa084, (u8*)&cache, 2);
}

void inMaintainModEventHandle(void)
{
    u16 cache = 0x005a;
    WriteDGUS(0xc700, (u8*)&cache, 2);
    cache = 1;
    WriteDGUS(0xa025, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xa085, (u8*)&cache, 2);
}
void outMaintainModEventHandle(void)
{
    u16 cache = 0;
    WriteDGUS(0xa025, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xa085, (u8*)&cache, 2);
}

void resetOriginalPara(void)
{
    u16 cache = 0x3c;
    WriteDGUS(0xcf2a, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xcf8a, (u8*)&cache, 2);
}

void saveFactoryCFG(void)
{
    u16 cache = 0x69;
    WriteDGUS(0xcf2b, (u8*)&cache, 2);
    cache = 0x005a;
    WriteDGUS(0xcf8b, (u8*)&cache, 2);
}

void passwordConfirmEventHandle(void)
{
    u8 cache[4] = {0};
    ReadDGUS(0xa420, cache, 4);
    if (memcmp(cache, &password[currentLevel][0], sizeof(cache)))
    {
        JumpPage(5);
    }
    else
    {
        switch (mode)
        {
            case PWM_PAGEJUMP:
                JumpPage(jumpPage);
                break;
            case PWM_FUN:
                passwordFunOPThandle(funOpt);
                JumpPage(currentPage);
                break;
            default:
                break;
        }
    }
    memset(cache, 0, 4);
    WriteDGUS(0xa420, cache, 4);
}

void passwordCancleEventHandle(void)
{
    JumpPage(currentPage);
}

void passwordPageJumpEventHandle(u16 event)
{
    jumpPage     = event - PASSWORD_PAGEJUMP_START;
    currentPage  = picNow;
    currentLevel = getPasswordLevel(event);
    JumpPage(4);
    mode = PWM_PAGEJUMP;
}

void passwordFunEventHandle(u16 event)
{
    funOpt       = event - PASSWORD_FUN_00_EVENT;
    currentLevel = getPasswordLevel(event);
    currentPage  = picNow;
    JumpPage(4);
    mode = PWM_FUN;
}

void passwordFunOPThandle(u16 fun)
{
    if (fun == 0)
    {
        curAlarmClearHandle();
    }
    else if (fun == 1)
    {
        alarmClearHandle();
    }
}

u8 getPasswordLevel(u16 event)
{
    if (event <= PASSWORD_PAGEJUMP_31_EVENT)
    {
        return pageLevel[event - PASSWORD_PAGEJUMP_START][1];
    }
    if (event <= PASSWORD_FUN_01_EVENT)
    {
        return funLevel[event - PASSWORD_FUN_00_EVENT][1];
    }
    return 0;
}
