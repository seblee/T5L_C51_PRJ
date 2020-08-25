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

void curveInit(void)
{
    u16 temp[256] = {0};
    u16 i, j, pointTemp;
    T5L_Flash(READRFLASH, CURVE_VP_ADDR, CURVE_FLASH_ADDR, CURVE_LEN);

    for (i = Curve0Start; i < Curve0End; i += 256)
    {
        ReadDGUS(i, temp, 512);
        for (j = 0; j < 256; j++)
        {
            if (temp[j] == 0xa55a)
            {
                break;
            }
        }
    }
    pointTemp = i + j;
    if (pointTemp >= Curve0End)
    {
        pointTemp = Curve0Start;
    }
    pointTemp -= Curve0Start;
    WriteDGUS(Curve0Addr, &pointTemp, 2);
    WriteDGUS(Curve0Point, &pointTemp, 2);
    WriteDGUS(Curve1Addr, &pointTemp, 2);
    WriteDGUS(Curve1Point, &pointTemp, 2);
    WriteDGUS(CurveManual, &pointTemp, 2);
    WriteDGUS(CurveManualBak, &pointTemp, 2);
    temp[0] = 0x07fe;
    WriteDGUS(Curve0Len, temp, 2);
    WriteDGUS(Curve1Len, temp, 2);

    temp[0] = 0x0006;
    temp[1] = 0x0001;
    temp[2] = 0x002E;
    temp[3] = 149;
    temp[4] = 209;
    temp[5] = 225;
    temp[6] = 223;
    temp[8] = DragerRight;
    temp[8] = 209;
    temp[9] = 0xFF00;
    WriteDGUS(CurveDrager, temp, 20);
}
