/**
 ****************************************************************************
 * @Warning Without permission from the author,Not for commercial use
 * @File    curve.c
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
#include "curve.h"
#include "T5L_lib.h"
#include "dgus.h"
#include "timer.h"
static u16 curvePoint = 0;
// static u16 curveManual    = 0;
// static u16 curveManualBak = 0;

void curveInit(void)
{
    u16 temp[256] = {0};
    u16 i, j, pointTemp;
    T5L_Flash(READRFLASH, CURVE_VP_ADDR, CURVE_FLASH_ADDR, CURVE_LEN);

    for (i = Curve0Start; i < Curve0End; i += 256)
    {
        ReadDGUS(i, (u8*)temp, 512);
        for (j = 0; j < 256; j++)
        {
            if (temp[j] == 0xa55a)
            {
                goto checkout;
            }
        }
    }
checkout:
    pointTemp = i + j;
    if (pointTemp >= Curve0End)
    {
        pointTemp = Curve0Start;
    }
    pointTemp -= Curve0Start;
    curvePoint = pointTemp;
    // WriteDGUS(Curve0Addr, &pointTemp, 2);
    WriteDGUS(Curve0Point, (u8*)&pointTemp, 2);
    // WriteDGUS(Curve1Addr, &pointTemp, 2);
    WriteDGUS(Curve1Point, (u8*)&pointTemp, 2);
    // curveManual    = pointTemp;
    // curveManualBak = pointTemp;
    WriteDGUS(CurveManual, (u8*)&pointTemp, 2);
    WriteDGUS(CurveManualBak, (u8*)&pointTemp, 2);
    temp[0] = 0x07fe;
    WriteDGUS(Curve0Len, (u8*)temp, 2);
    WriteDGUS(Curve1Len, (u8*)temp, 2);

    temp[0] = 0x0006;
    temp[1] = 0x0001;
    temp[2] = 0x002E;
    temp[3] = 332;
    temp[4] = 369;
    temp[5] = 375;
    temp[6] = 394;
    temp[7] = DragerRight;
    temp[8] = 369;
    temp[9] = 0xFF00;
    WriteDGUS(CurveDrager, (u8*)temp, 20);
}
void curveProcess(void)
{
    static u16 TimerCount = 0;
    if (MS100msFlag)
    {
        TimerCount++;
        dragCuave();
    }
    if (TimerCount >= TimerPeriod)
    {
        u16 temp[4];
        TimerCount = 0;
        temp[0]    = curvePoint + Curve0Start;
        ReadDGUS(Curve0Data, (u8*)&temp[1], 2);
        temp[1] += 200;  //负数显示
        WriteDGUS(temp[0], (u8*)&temp[1], 2);

        temp[0] = curvePoint + Curve1Start;
        ReadDGUS(Curve1Data, (u8*)&temp[1], 2);
        WriteDGUS(temp[0], (u8*)&temp[1], 2);

        temp[3] = curvePoint;
        curvePoint++;
        if (curvePoint >= CurveMAX)
        {
            curvePoint = 0;
        }
        WriteDGUS(Curve0Point, (u8*)&curvePoint, 2);
        WriteDGUS(Curve1Point, (u8*)&curvePoint, 2);
        temp[2] = CurveMAX;
        WriteDGUS(CurveManual, (u8*)&temp[2], 2);
        WriteDGUS(CurveManualBak, (u8*)&temp[2], 2);
        // curveManual    = CurveMAX;
        // curveManualBak = CurveMAX;
        temp[2] = DragerRight;
        WriteDGUS(DragerX, (u8*)&temp[2], 2);

        if (curvePoint & 0x01)
        {
            return;
        }
        temp[3]--;
        ReadDGUS(temp[3] + Curve0Start, (u8*)&temp[0], 4);
        temp[2] = 0xa55a;
        WriteDGUS(Curvetemp, (u8*)&temp[0], 6);
        T5L_Flash(WRITERFLASH, Curvetemp, CURVE_FLASH_ADDR + temp[3], 4);
        ReadDGUS(temp[3] + Curve1Start, (u8*)&temp[0], 4);
        temp[2] = 0xa55a;
        WriteDGUS(Curvetemp, (u8*)&temp[0], 6);
        T5L_Flash(WRITERFLASH, Curvetemp, CURVE_FLASH_ADDR + temp[3] + 0x0800, 4);
    }
}
void dragCuave(void)
{
    u16 temp[4];
    ReadDGUS(CurveManual, (u8*)&temp[0], 4);
    if (temp[0] == temp[1])
    {
        return;
    }
    WriteDGUS(CurveManualBak, (u8*)&temp[1], 2);
    temp[2] = curvePoint + temp[0];
    if (temp[2] >= CurveMAX)
    {
        temp[2] -= CurveMAX;
    }
    WriteDGUS(Curve0Point, (u8*)&temp[2], 2);
    WriteDGUS(Curve1Point, (u8*)&temp[2], 2);
    temp[0] >>= 2;
    temp[0] += DragerLeft;
    WriteDGUS(DragerX, (u8*)&temp[0], 2);
}

void curveClearHandle(void)
{
    u16 cache[16];
    u16 i;
    memset(cache, 0, sizeof(cache));
    for (i = 0; i < 256; i++)
    {
        WriteDGUS(CURVE_VP_ADDR + (i << 4), (u8*)&cache[0], sizeof(cache));
    }
    T5L_Flash(WRITERFLASH, CURVE_VP_ADDR, CURVE_FLASH_ADDR, CURVE_LEN);
    // clear buffer
    curvePoint = 0;
    WriteDGUS(Curve0Point, (u8*)&curvePoint, 2);
    WriteDGUS(Curve1Point, (u8*)&curvePoint, 2);

    cache[2] = CurveMAX;
    WriteDGUS(CurveManual, (u8*)&cache[2], 2);
    WriteDGUS(CurveManualBak, (u8*)&cache[2], 2);
    cache[2] = DragerRight;
    WriteDGUS(DragerX, (u8*)&cache[2], 2);
}
