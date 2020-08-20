#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "sys.h"

#define UART_TX_BUF_MAX_LEN 64
#define UART_RX_BUF_MAX_LEN 64
#define MODBUS_READ_REG_LEN 14  //寄存器个数

void Modbus_UART_Init(void);
void Modbus_Process_Task(void);

#endif
