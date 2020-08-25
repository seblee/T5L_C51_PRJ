#ifndef __MODBUS_H__
#define __MODBUS_H__

#include "sys.h"

#define UART_TX_BUF_MAX_LEN 64
#define UART_RX_BUF_MAX_LEN 64

#define MODBUS_READ_REG_ADDRESS 0x4001  //寄存器首地址

#define MODBUS_WRITE_REG_ADDRESS 0x5001  //寄存器写地址

#define MODBUS_SEND_TIME_PERIOD 500  // 0.5秒钟
#define RX_CHECK_TICK_TIME 10  //超过10个字节的时间没有收到新的字符，认为接收结束，10*10*1000/9600,

#define SLAVE_ID 0x01  //设备端地址
#define MODBUS_EN 0X5A
#define MODBUS_DIS 0X00
#define MODBUS_READ_03H 0x03   //读寄存器地址命令
#define MODBUS_WRITE_06H 0x06  //写寄存器地址命令
#define CMD_NUMBER 11

typedef struct modbosCmd
{
    u8 modbusEn;
    u8 slaveID;
    u8 funCode;
    u8 length;
    u8 timeout;
    u8 mode;
    u16 moddPara;
    u16 VPAddr;
    u16 slaveAddr;
    u16 feedbadck;
} modbosCmd_t;

extern u8 modbus_rx_flag;                      //接收到的字符串的标志，为1表示有收到数据
extern u8 modbus_rx_buf[UART_RX_BUF_MAX_LEN];  //接收到的字符串的内容
extern u8 modbus_rx_count;                     //接收到的字符串的长度

extern _TKS_FLAGA_type modbusFlag;
#define cmdTxFlag modbusFlag.bits.b0
#define cmdRxFlag modbusFlag.bits.b1

void Modbus_UART_Init(void);
void Modbus_Process_Task(void);
void getCmd(u8 *index);
#endif
