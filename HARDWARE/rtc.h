
#ifndef _RTC_H_
#define _RTC_H_

#include "sys.h"

typedef struct _dev_time
{
    u8 year;
    u8 month;
    u8 day;
    u8 week;
    u8 hour;
    u8 min;
    u8 sec;
    u8 res;
} rtc_time;

//#define NULL ((void*)0)
#define DHW_NPAGE 0x0014  //当前页面
#define RTC_Set 0x009C
#define RTC 0x0010

sbit RTC_SDA = P3 ^ 3;
sbit RTC_SCL = P3 ^ 2;

extern u16 data SysTick_RTC;
extern uint8_t page[2];
extern uint8_t page1[2];

void delayus(unsigned char t);
void SDA_IN(void);
void SDA_OUT(void);
void i2cstart(void);
void i2cstop(void);
void mack(void);
void mnak(void);
void cack(void);
void i2cbw(unsigned char dat);
unsigned char i2cbr(void);
void init_rtc(void);
void RTC_Set_Time(u8* prtc_set);
void rdtime(void);

  void RTC_Set_CMD();
  uint8_t RTC_Get_Week(uint8_t, uint8_t, uint8_t);

#endif
