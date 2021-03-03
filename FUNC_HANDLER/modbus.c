/******************************************************************************
          版权所有 (C), 2020，DFX，Write by Food(181 1266 9427)
    本程序是基于DWin的4寸480*480温控器C语言例子程序，去除了不需要的按键检测、RTC等
不需要的命令、函数。
   配置如下：
     1. UART2 作为log的输出，用于监控程序的状态
     2. UART4 作为ModBus的通讯串口，处理发送和接收命令，
     3. 其他为迪文的DGUS基本配置，
     功能如下：
     1. 实现Modbus RTU主站命令，03读取命令，06写寄存器命令，控制、显示modbus从站状态
     2. 实现UI的显示，控制，状态的更新，

     说明：程序可自由传播，但请仔细阅读后再应用，对引起的任何问题无任何的责任。使用
过程中，如果有疑问，可以加V(181 1266 9427)共同探讨。
******************************************************************************/
#include "modbus.h"
#include "sys.h"
#include "crc16.h"
#include "uart.h"
#include "ui.h"

void Modbus_RX_Reset(void);
// void Modbus_TX_Reset(void);
void Modbus_Write_Register06H(modbosCmd_t *CmdNow, u16 value);
void Modbus_Write_Register10H(modbosCmd_t *CmdNow);
void Modbus_Read_Register03H(modbosCmd_t *CmdNow);
void modbus_process_command(u8 *pstr, u16 strlen);

u8 modbus_rx_count = 0;                 //接收到的字符串的长度
u8 modbus_rx_flag  = 0;                 //接收到的字符串的标志，为1表示有收到数据
u8 modbus_rx_buf[UART_RX_BUF_MAX_LEN];  //接收到的字符串的内容

// extern process_struct process_flag;  //命令状态标志
extern u32 data SysTick;        //每隔1ms+1
u32 uart_rx_check_tick    = 0;  //检查串口是否接收结束
u8 modbus_rx_count_before = 0;  //接收串口的数据

u32 modbus_tx_process_tick = 0;  // modbus发送命令的时间间隔

const modbosCmd_t modbusCmdlib[] = {
    // en         id         fun    len  timeout      mod    modP     VP  slaveAddr feedback
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x03, 0xc8, MODE_ALWA, 0x0000, 0xa020, 0x031c, 0x00ff},  //告警数
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE00, 0xa023, 0x0319, 0x00ff},  //位状态
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xa084, 0xa024, 0x0100, PAGE00},  // power switch
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE00, 0xa025, 0x0104, 0x00ff},  //诊断模式
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PARA, 0xa085, 0xa025, 0x0104, 0x00ff},  //诊断模式
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x03, 0xc8, MODE_PAGE, PAGE00, 0xa0a0, 0x011a, 0x00ff},  //温湿度
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x04, 0xc8, MODE_ALWA, 0x0000, 0xa0a3, 0x033b, 0x00ff},  //回风温湿度
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PAGE, PAGE10, 0xaa00, 0x0326, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x08, 0xc8, MODE_PAGE, PAGE10, 0xaa02, 0x0341, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE10, 0xaa0a, 0x036f, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE10, 0xaa0b, 0x0377, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x1f, 0xc8, MODE_PAGE, PAGE11, 0xab20, 0x038c, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xac80, 0xac20, 0x0000, PAGE12},  //清楚当前告警
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x06, 0xc8, MODE_ALWA, 0x000d, 0xaea0, 0x0320, 0x00ff},  //告警
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x04, 0xc8, MODE_PANP, 0xb300, 0xb320, 0x0117, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x05, 0xc8, MODE_PANP, 0xb300, 0xb324, 0x011c, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb380, 0xb320, 0x0117, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb381, 0xb321, 0x0118, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb382, 0xb322, 0x0119, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb383, 0xb323, 0x011a, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb384, 0xb324, 0x011c, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb385, 0xb325, 0x011d, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb386, 0xb326, 0x011e, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb387, 0xb327, 0x011f, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb388, 0xb328, 0x0120, PAGE19},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb400, 0xb420, 0x0138, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb400, 0xb421, 0x0158, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x06, 0xc8, MODE_PANP, 0xb400, 0xb422, 0x015b, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb400, 0xb428, 0x0162, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb400, 0xb429, 0x019e, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb400, 0xb42a, 0x0164, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb400, 0xb42b, 0x01d3, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb400, 0xb42c, 0x01d6, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb480, 0xb420, 0x0138, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb481, 0xb421, 0x0158, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb482, 0xb422, 0x015b, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb483, 0xb423, 0x015c, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb484, 0xb424, 0x015d, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb485, 0xb425, 0x015e, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb486, 0xb426, 0x015f, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb487, 0xb427, 0x0160, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb488, 0xb428, 0x0162, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb489, 0xb429, 0x019e, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb48a, 0xb42a, 0x0164, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb48b, 0xb42b, 0x01d3, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb48c, 0xb42c, 0x01d6, PAGE20},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb500, 0xb420, 0x0159, PAGE21},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb580, 0xb420, 0x0159, PAGE21},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x03, 0xc8, MODE_PANP, 0xb700, 0xb720, 0x013e, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x05, 0xc8, MODE_PANP, 0xb700, 0xb723, 0x0143, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb780, 0xb720, 0x013e, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb781, 0xb721, 0x013f, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb782, 0xb722, 0x0140, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb783, 0xb723, 0x0143, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb784, 0xb724, 0x0144, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb785, 0xb725, 0x0145, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb786, 0xb726, 0x0146, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb787, 0xb727, 0x0147, PAGE23},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x07, 0xc8, MODE_PANP, 0xb800, 0xb820, 0x0169, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PANP, 0xb800, 0xb827, 0x0171, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb800, 0xb829, 0x0174, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb800, 0xb82a, 0x017b, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xb800, 0xb82b, 0x022a, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb880, 0xb820, 0x0169, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb881, 0xb821, 0x016a, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb882, 0xb822, 0x016b, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb883, 0xb823, 0x016c, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb884, 0xb824, 0x016d, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb885, 0xb825, 0x016e, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb886, 0xb826, 0x016f, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb887, 0xb827, 0x0171, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb888, 0xb828, 0x0172, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb889, 0xb829, 0x0174, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb88a, 0xb82a, 0x017b, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xb88b, 0xb82b, 0x022a, PAGE24},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x15, 0xc8, MODE_PANP, 0xbc00, 0xbc20, 0x01b0, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc80, 0xbc20, 0x01b0, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc81, 0xbc21, 0x01b1, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc82, 0xbc22, 0x01b2, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc83, 0xbc23, 0x01b3, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc84, 0xbc24, 0x01b4, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc85, 0xbc25, 0x01b5, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc86, 0xbc26, 0x01b6, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc87, 0xbc27, 0x01b7, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc88, 0xbc28, 0x01b8, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc89, 0xbc29, 0x01b9, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc8a, 0xbc2a, 0x01ba, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc8b, 0xbc2b, 0x01bb, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc8c, 0xbc2c, 0x01bc, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc8d, 0xbc2d, 0x01bd, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc8e, 0xbc2e, 0x01be, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc8f, 0xbc2f, 0x01bf, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc90, 0xbc30, 0x01c0, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc91, 0xbc31, 0x01c1, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc92, 0xbc32, 0x01c2, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc93, 0xbc33, 0x01c3, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbc94, 0xbc34, 0x01c4, PAGE28},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xbd00, 0xbd20, 0x01c5, PAGE29},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbd80, 0xbd20, 0x01c5, PAGE29},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbd81, 0xbd21, 0x01c6, PAGE29},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbd82, 0xbd22, 0x01c7, PAGE29},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x03, 0xc8, MODE_PANP, 0xbe00, 0xbe20, 0x0232, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xbe00, 0xbe23, 0x0236, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xbe00, 0xbe24, 0x0274, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x03, 0xc8, MODE_PANP, 0xbe00, 0xbe25, 0x0276, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xbe00, 0xbe28, 0x0280, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xbe00, 0xbe29, 0x0286, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PANP, 0xbe00, 0xbe2a, 0x0289, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe80, 0xbe20, 0x0232, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe81, 0xbe21, 0x0233, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe82, 0xbe22, 0x0234, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe83, 0xbe23, 0x0236, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe84, 0xbe24, 0x0274, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe85, 0xbe25, 0x0276, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe86, 0xbe26, 0x0277, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe87, 0xbe27, 0x0278, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe88, 0xbe28, 0x0280, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe89, 0xbe29, 0x0286, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe8a, 0xbe2a, 0x0289, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbe8b, 0xbe2b, 0x028a, PAGE30},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PANP, 0xbf00, 0xbf20, 0x0102, PAGE31},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xbf00, 0xbf22, 0x0107, PAGE31},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xbf00, 0xbf23, 0x01a5, PAGE31},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbf80, 0xbf20, 0x0102, PAGE31},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbf81, 0xbf21, 0x0103, PAGE31},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbf82, 0xbf22, 0x0107, PAGE31},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xbf83, 0xbf23, 0x01a5, PAGE31},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x08, 0xc8, MODE_PANP, 0xc200, 0xc220, 0x0182, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xc200, 0xc228, 0x018c, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc280, 0xc220, 0x0182, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc281, 0xc221, 0x0183, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc282, 0xc222, 0x0184, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc284, 0xc224, 0x0186, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc285, 0xc225, 0x0187, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc286, 0xc226, 0x0188, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc287, 0xc227, 0x0189, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc288, 0xc228, 0x018c, PAGE34},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc580, 0xc520, 0x023b, PAGE15},  // reset factory
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xc700, 0xc720, 0x01aa, PAGE39},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xc700, 0xc721, 0x01ac, PAGE39},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc780, 0xc720, 0x01aa, PAGE39},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc781, 0xc721, 0x01ac, PAGE39},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc782, 0xc722, 0x020d, PAGE39},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE39, 0xc723, 0x0375, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE39, 0xc724, 0x0377, 0x00ff},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xc800, 0xc820, 0x0127, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xc800, 0xc821, 0x0129, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x04, 0xc8, MODE_PANP, 0xc800, 0xc822, 0x012d, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc880, 0xc820, 0x0127, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc881, 0xc821, 0x0129, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc882, 0xc822, 0x012d, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc883, 0xc823, 0x012e, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc884, 0xc824, 0x012f, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc885, 0xc825, 0x0130, PAGE40},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x0e, 0xc8, MODE_PAGE, PAGE41, 0xc9b0, 0x0378, 0x00ff},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc990, 0xc930, 0x023c, PAGE41},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xcc00, 0xcca0, 0x0303, PAGE44},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xcf00, 0xcf20, 0x0108, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PANP, 0xcf00, 0xcf21, 0x010a, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x04, 0xc8, MODE_PANP, 0xcf00, 0xcf23, 0x010d, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xcf00, 0xcf27, 0x0141, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xcf00, 0xcf28, 0x015a, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xcf00, 0xcf29, 0x01a7, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf80, 0xcf20, 0x0108, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf81, 0xcf21, 0x010a, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf82, 0xcf22, 0x010b, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf83, 0xcf23, 0x010d, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf84, 0xcf24, 0x010e, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf85, 0xcf25, 0x010f, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf86, 0xcf26, 0x0110, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf87, 0xcf27, 0x0141, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf88, 0xcf28, 0x015a, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf89, 0xcf29, 0x01a7, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf8a, 0xcf2a, 0x023b, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xcf8b, 0xcf2b, 0x023b, PAGE47},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x0A, 0xc8, MODE_PAGE, PAGE52, 0xd420, 0x039c, 0x00ff},
    // {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PAGE, PAGE52, 0xd42a, 0x03a8, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd500, 0xd520, 0x01d2, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd500, 0xd521, 0x01dd, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd500, 0xd522, 0x01df, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PANP, 0xd500, 0xd523, 0x0271, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xd580, 0xd520, 0x01d2, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xd581, 0xd521, 0x01dd, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xd582, 0xd522, 0x01df, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xd583, 0xd523, 0x0271, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xd584, 0xd524, 0x0272, PAGE53},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PAGE, PAGE54, 0xd520, 0x0341, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE54, 0xd522, 0x036f, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE54, 0xd523, 0x0377, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd900, 0xd920, 0x0113, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd901, 0xd921, 0x0119, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x05, 0xc8, MODE_PANP, 0xd902, 0xd922, 0x0121, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xd980, 0xd920, 0x0113, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd981, 0xd921, 0x0119, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd982, 0xd922, 0x0121, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd983, 0xd923, 0x0122, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd984, 0xd924, 0x0123, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd985, 0xd925, 0x0124, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xd986, 0xd926, 0x0125, PAGE57},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PANP, 0xda00, 0xda20, 0x01df, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xda00, 0xda22, 0x0207, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x02, 0xc8, MODE_PANP, 0xda00, 0xda23, 0x0216, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xda00, 0xda25, 0x0222, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xda80, 0xda20, 0x01df, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xda81, 0xda21, 0x01e0, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xda82, 0xda22, 0x0207, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xda83, 0xda23, 0x0216, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xda84, 0xda24, 0x0217, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xda85, 0xda25, 0x0222, PAGE58},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xc700, 0xc720, 0x01aa, PAGE62},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PANP, 0xc700, 0xc721, 0x01ac, PAGE62},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc780, 0xc720, 0x01aa, PAGE62},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc781, 0xc721, 0x01ac, PAGE62},
    {BUS_EN, SLAVE_ID, BUS_FUN_06H, 0x01, 0xc8, MODE_PANP, 0xc782, 0xc722, 0x020d, PAGE62},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE62, 0xc723, 0x0375, 0x00ff},
    {BUS_EN, SLAVE_ID, BUS_FUN_03H, 0x01, 0xc8, MODE_PAGE, PAGE62, 0xc724, 0x0377, 0x00ff},
};
modbosCmd_t modbusCmdNow = {0};
u8 CmdIndex              = 0;

const dataCheckCmd_t dataCheckLib[] = {
    // en     page  data    back   flag
    {BUS_EN, PAGE19, 0xb320, 0xb350, 0xb380},  //
    {BUS_EN, PAGE19, 0xb321, 0xb351, 0xb381},  //
    {BUS_EN, PAGE19, 0xb322, 0xb352, 0xb382},  //
    {BUS_EN, PAGE19, 0xb323, 0xb353, 0xb383},  //
    {BUS_EN, PAGE19, 0xb324, 0xb354, 0xb384},  //
    {BUS_EN, PAGE19, 0xb325, 0xb355, 0xb385},  //
    {BUS_EN, PAGE19, 0xb326, 0xb356, 0xb386},  //
    {BUS_EN, PAGE19, 0xb327, 0xb357, 0xb387},  //
    {BUS_EN, PAGE19, 0xb328, 0xb358, 0xb388},  //
    {BUS_EN, PAGE20, 0xb420, 0xb450, 0xb480},  //
    {BUS_EN, PAGE20, 0xb421, 0xb451, 0xb481},  //
    {BUS_EN, PAGE20, 0xb422, 0xb452, 0xb482},  //
    {BUS_EN, PAGE20, 0xb423, 0xb453, 0xb483},  //
    {BUS_EN, PAGE20, 0xb424, 0xb454, 0xb484},  //
    {BUS_EN, PAGE20, 0xb425, 0xb455, 0xb485},  //
    {BUS_EN, PAGE20, 0xb426, 0xb456, 0xb486},  //
    {BUS_EN, PAGE20, 0xb427, 0xb457, 0xb487},  //
    {BUS_EN, PAGE20, 0xb428, 0xb458, 0xb488},  //
    {BUS_EN, PAGE20, 0xb429, 0xb459, 0xb489},  //
    {BUS_EN, PAGE20, 0xb42a, 0xb45a, 0xb48a},  //
    {BUS_EN, PAGE20, 0xb42b, 0xb45b, 0xb48b},  //
    {BUS_EN, PAGE20, 0xb42c, 0xb45c, 0xb48c},  //
    {BUS_EN, PAGE21, 0xb520, 0xb550, 0xb580},  //
    {BUS_EN, PAGE23, 0xb720, 0xb750, 0xb780},  //
    {BUS_EN, PAGE23, 0xb721, 0xb751, 0xb781},  //
    {BUS_EN, PAGE23, 0xb722, 0xb752, 0xb782},  //
    {BUS_EN, PAGE23, 0xb723, 0xb753, 0xb783},  //
    {BUS_EN, PAGE23, 0xb724, 0xb754, 0xb784},  //
    {BUS_EN, PAGE23, 0xb725, 0xb755, 0xb785},  //
    {BUS_EN, PAGE23, 0xb726, 0xb756, 0xb786},  //
    {BUS_EN, PAGE23, 0xb727, 0xb757, 0xb787},  //
    {BUS_EN, PAGE24, 0xb820, 0xb850, 0xb880},  //
    {BUS_EN, PAGE24, 0xb821, 0xb851, 0xb881},  //
    {BUS_EN, PAGE24, 0xb822, 0xb852, 0xb882},  //
    {BUS_EN, PAGE24, 0xb823, 0xb853, 0xb883},  //
    {BUS_EN, PAGE24, 0xb824, 0xb854, 0xb884},  //
    {BUS_EN, PAGE24, 0xb825, 0xb855, 0xb885},  //
    {BUS_EN, PAGE24, 0xb826, 0xb856, 0xb886},  //
    {BUS_EN, PAGE24, 0xb827, 0xb857, 0xb887},  //
    {BUS_EN, PAGE24, 0xb828, 0xb858, 0xb888},  //
    {BUS_EN, PAGE24, 0xb829, 0xb859, 0xb889},  //
    {BUS_EN, PAGE24, 0xb82a, 0xb85a, 0xb88a},  //
    {BUS_EN, PAGE24, 0xb82b, 0xb85b, 0xb88b},  //
    {BUS_EN, PAGE28, 0xbc20, 0xbc50, 0xbc80},  //
    {BUS_EN, PAGE28, 0xbc21, 0xbc51, 0xbc81},  //
    {BUS_EN, PAGE28, 0xbc22, 0xbc52, 0xbc82},  //
    {BUS_EN, PAGE28, 0xbc23, 0xbc53, 0xbc83},  //
    {BUS_EN, PAGE28, 0xbc24, 0xbc54, 0xbc84},  //
    {BUS_EN, PAGE28, 0xbc25, 0xbc55, 0xbc85},  //
    {BUS_EN, PAGE28, 0xbc26, 0xbc56, 0xbc86},  //
    {BUS_EN, PAGE28, 0xbc27, 0xbc57, 0xbc87},  //
    {BUS_EN, PAGE28, 0xbc28, 0xbc58, 0xbc88},  //
    {BUS_EN, PAGE28, 0xbc29, 0xbc59, 0xbc89},  //
    {BUS_EN, PAGE28, 0xbc2a, 0xbc5a, 0xbc8a},  //
    {BUS_EN, PAGE28, 0xbc2b, 0xbc5b, 0xbc8b},  //
    {BUS_EN, PAGE28, 0xbc2c, 0xbc5c, 0xbc8c},  //
    {BUS_EN, PAGE28, 0xbc2d, 0xbc5d, 0xbc8d},  //
    {BUS_EN, PAGE28, 0xbc2e, 0xbc5e, 0xbc8e},  //
    {BUS_EN, PAGE28, 0xbc2f, 0xbc5f, 0xbc8f},  //
    {BUS_EN, PAGE28, 0xbc30, 0xbc60, 0xbc90},  //
    {BUS_EN, PAGE28, 0xbc31, 0xbc61, 0xbc91},  //
    {BUS_EN, PAGE28, 0xbc32, 0xbc62, 0xbc92},  //
    {BUS_EN, PAGE28, 0xbc33, 0xbc63, 0xbc93},  //
    {BUS_EN, PAGE28, 0xbc34, 0xbc64, 0xbc94},  //
    {BUS_EN, PAGE29, 0xbd20, 0xbd50, 0xbd80},  //
    {BUS_EN, PAGE29, 0xbd21, 0xbd51, 0xbd81},  //
    {BUS_EN, PAGE29, 0xbd22, 0xbd52, 0xbd82},  //
    {BUS_EN, PAGE30, 0xbe20, 0xbe50, 0xbe80},  //
    {BUS_EN, PAGE30, 0xbe21, 0xbe51, 0xbe81},  //
    {BUS_EN, PAGE30, 0xbe22, 0xbe52, 0xbe82},  //
    {BUS_EN, PAGE30, 0xbe23, 0xbe53, 0xbe83},  //
    {BUS_EN, PAGE30, 0xbe24, 0xbe54, 0xbe84},  //
    {BUS_EN, PAGE30, 0xbe25, 0xbe55, 0xbe85},  //
    {BUS_EN, PAGE30, 0xbe26, 0xbe56, 0xbe86},  //
    {BUS_EN, PAGE30, 0xbe27, 0xbe57, 0xbe87},  //
    {BUS_EN, PAGE30, 0xbe28, 0xbe58, 0xbe88},  //
    {BUS_EN, PAGE30, 0xbe29, 0xbe59, 0xbe89},  //
    {BUS_EN, PAGE30, 0xbe2a, 0xbe5a, 0xbe8a},  //
    {BUS_EN, PAGE30, 0xbe2b, 0xbe5b, 0xbe8b},  //
    // {BUS_EN, PAGE31, 0xbf20, 0xbf50, 0xbf80},  //
    // {BUS_EN, PAGE31, 0xbf21, 0xbf51, 0xbf81},  //
    // {BUS_EN, PAGE31, 0xbf22, 0xbf52, 0xbf82},  //
    // {BUS_EN, PAGE31, 0xbf23, 0xbf53, 0xbf83},  //
    // {BUS_EN, PAGE34, 0xc220, 0xc250, 0xc280},  //
    // {BUS_EN, PAGE34, 0xc221, 0xc251, 0xc281},  //
    // {BUS_EN, PAGE34, 0xc222, 0xc252, 0xc282},  //
    // {BUS_EN, PAGE34, 0xc224, 0xc254, 0xc284},  //
    // {BUS_EN, PAGE34, 0xc225, 0xc255, 0xc285},  //
    // {BUS_EN, PAGE34, 0xc226, 0xc256, 0xc286},  //
    // {BUS_EN, PAGE34, 0xc227, 0xc257, 0xc287},  //
    // {BUS_EN, PAGE34, 0xc228, 0xc258, 0xc288},  //
    {BUS_EN, PAGE39, 0xc720, 0xc750, 0xc780},  //
    {BUS_EN, PAGE39, 0xc721, 0xc751, 0xc781},  //
    // {BUS_EN, PAGE40, 0xc820, 0xc850, 0xc880},  //
    // {BUS_EN, PAGE40, 0xc821, 0xc851, 0xc881},  //
    // {BUS_EN, PAGE40, 0xc822, 0xc852, 0xc882},  //
    // {BUS_EN, PAGE40, 0xc823, 0xc853, 0xc883},  //
    // {BUS_EN, PAGE40, 0xc824, 0xc854, 0xc884},  //
    // {BUS_EN, PAGE40, 0xc825, 0xc855, 0xc885},  //
    // {BUS_EN, PAGE47, 0xcf20, 0xcf50, 0xcf80},  //
    // {BUS_EN, PAGE47, 0xcf21, 0xcf51, 0xcf81},  //
    // {BUS_EN, PAGE47, 0xcf22, 0xcf52, 0xcf82},  //
    // {BUS_EN, PAGE47, 0xcf23, 0xcf53, 0xcf83},  //
    // {BUS_EN, PAGE47, 0xcf24, 0xcf54, 0xcf84},  //
    // {BUS_EN, PAGE47, 0xcf25, 0xcf55, 0xcf85},  //
    // {BUS_EN, PAGE47, 0xcf26, 0xcf56, 0xcf86},  //
    // {BUS_EN, PAGE47, 0xcf27, 0xcf57, 0xcf87},  //
    // {BUS_EN, PAGE47, 0xcf28, 0xcf58, 0xcf88},  //
    // {BUS_EN, PAGE47, 0xcf29, 0xcf59, 0xcf89},  //
    {BUS_EN, PAGE53, 0xd520, 0xd550, 0xd580},  //
    {BUS_EN, PAGE53, 0xd521, 0xd551, 0xd581},  //
    {BUS_EN, PAGE53, 0xd522, 0xd552, 0xd582},  //
    {BUS_EN, PAGE53, 0xd523, 0xd553, 0xd583},  //
    {BUS_EN, PAGE53, 0xd524, 0xd554, 0xd584},  //
    {BUS_EN, PAGE57, 0xd920, 0xd950, 0xd980},  //
    {BUS_EN, PAGE57, 0xd921, 0xd951, 0xd981},  //
    {BUS_EN, PAGE57, 0xd922, 0xd952, 0xd982},  //
    {BUS_EN, PAGE57, 0xd923, 0xd953, 0xd983},  //
    {BUS_EN, PAGE57, 0xd924, 0xd954, 0xd984},  //
    {BUS_EN, PAGE57, 0xd925, 0xd955, 0xd985},  //
    {BUS_EN, PAGE57, 0xd926, 0xd956, 0xd986},  //
    {BUS_EN, PAGE58, 0xda20, 0xda50, 0xda80},  //
    {BUS_EN, PAGE58, 0xda21, 0xda51, 0xda81},  //
    {BUS_EN, PAGE58, 0xda22, 0xda52, 0xda82},  //
    {BUS_EN, PAGE58, 0xda23, 0xda53, 0xda83},  //
    {BUS_EN, PAGE58, 0xda24, 0xda54, 0xda84},  //
    {BUS_EN, PAGE58, 0xda25, 0xda55, 0xda85},  //
};

_TKS_FLAGA_type modbusFlag = {0};
/******************************************************************************
          版权所有 (C), 2020，DFX，Write by Food(181 1266 9427)
 ******************************************************************************
modbus 命令解析处理程序，实现：
1. 03H的回送命令解析
2. 06H的回送命令解析，如果回送命令正确，则停止UI的触发发送命令
******************************************************************************/

void modbus_process_command(u8 *pstr, u16 strlen)
{
    u16 num;
    u16 crc_data;
    u16 len;

    // printf("Modbus string:");
    for (num = 0; num < strlen; num++)
    {
        // printf("%02X ", (u16)(*(pstr + num)));
    }
    // printf(",length:%d\r\n", strlen);

    if (strlen < 5)
    {
        return;
    }
    num = 0;
    do
    {
        if ((*(pstr + num)) == SLAVE_ID)
        {
            switch (*(pstr + num + 1))  //判读下一个字节是modbus的哪个命令
            {
                case BUS_FUN_03H:
                    len = *(pstr + num + 2);
                    if ((len + num + 5) > strlen)  //长度超过最大长度
                    {
                        num = strlen;  //非modbus命令
                        break;
                    }
                    crc_data = crc16table(pstr + num, 3 + len);
                    // printf("num:%d,len:%d,crc data:%02X,%02X,", num, len, (u16)((crc_data >> 8) &
                    // 0xFF),(u16)(crc_data & 0xFF));
                    if ((*(pstr + num + len + 3) != ((crc_data >> 8) & 0xFF)) ||
                        (*(pstr + num + len + 4) != (crc_data & 0xFF)))  // CRC
                    {
                        break;
                    }
                    WriteDGUS(modbusCmdNow.VPAddr, (pstr + 3), *(pstr + 2));
                    memset(&modbusCmdNow, 0, sizeof(modbosCmd_t));
                    num       = len + 5;
                    cmdRxFlag = 1;
                    break;
                case BUS_FUN_06H:
                    if ((num + 8) > strlen)
                    {
                        num = strlen;  //非modbus命令
                        break;
                    }
                    crc_data = crc16table(pstr + num, 6);
                    if ((*(pstr + num + 6) != ((crc_data >> 8) & 0xFF)) ||
                        (*(pstr + num + 7) != (crc_data & 0xFF)))  // CRC
                    {
                        break;
                    }
                    num += 8;
                    memset(&modbusCmdNow, 0, sizeof(modbosCmd_t));
                    cmdRxFlag = 1;
                    break;
                case BUS_FUN_10H:
                    if ((num + 8) > strlen)
                    {
                        num = strlen;  //非modbus命令
                        break;
                    }
                    crc_data = crc16table(pstr + num, 6);
                    if ((*(pstr + num + 6) != ((crc_data >> 8) & 0xFF)) ||
                        (*(pstr + num + 7) != (crc_data & 0xFF)))  // CRC
                    {
                        break;
                    }
                    num += 8;
                    memset(&modbusCmdNow, 0, sizeof(modbosCmd_t));
                    cmdRxFlag = 1;
                    break;
                default:
                    break;
            }
        }
        num++;
    } while (num < (strlen - 5));  // addre,command,data,crch,crcl,至少需要有5个字节
}
/******************************************************************************
          版权所有 (C), 2020，DFX，Write by Food(181 1266 9427)
 ******************************************************************************
modbus 发送和接收任务处理程序，实现：
1. 监控串口接收，当判断接收结束后，调用处理函数，
2. 监控UI的触发命令，当有检测到发送命令时，发送modbus写命令
3. 每隔1秒钟触发一次查询modbus寄存器状态的命令
******************************************************************************/
void Modbus_Process_Task(void)
{
    modbosCmd_t *cmdTemp_t = NULL;
    if (modbus_rx_flag == 1)  //接收数据
    {
        if (modbus_rx_count > modbus_rx_count_before)
        {
            modbus_rx_count_before = modbus_rx_count;
            uart_rx_check_tick     = 0;
        }
        else if (modbus_rx_count == modbus_rx_count_before)
        {
            if (uart_rx_check_tick > 0)
            {
                if ((SysTick - uart_rx_check_tick) > RX_CHECK_TICK_TIME)
                {
                    modbus_process_command(modbus_rx_buf, modbus_rx_count);
                    Modbus_RX_Reset();
                }
            }
            else
            {
                uart_rx_check_tick = SysTick;
            }
        }
    }

    if (cmdTxFlag)
    {
        if ((cmdRxFlag) || ((SysTick - modbus_tx_process_tick) >= modbusCmdNow.timeout))
        {
            if (cmdRxFlag)
                CmdIndex++;
            goto processCMDLib;
        }
        return;
    }

    if ((SysTick - modbus_tx_process_tick) < MODBUS_SEND_TIME_PERIOD)  //间隔固定时间后再处理UI的设置命令，
    {
        return;
    }
processCMDLib:
    if (CmdIndex == 0)
        checkChange();
    modbus_tx_process_tick = SysTick;
    cmdRxFlag              = 0;
    cmdTxFlag              = 0;
    getCmd(&CmdIndex);
    if (CmdIndex < CMD_NUMBER)
    {
        memcpy(&modbusCmdNow, &modbusCmdlib[CmdIndex], sizeof(modbosCmd_t));
        if (modbusCmdNow.funCode == BUS_FUN_03H)
        {
            Modbus_Read_Register03H(&modbusCmdNow);
            cmdTxFlag = 1;
        }
        else if (modbusCmdNow.funCode == BUS_FUN_06H)
        {
            u16 value;
            ReadDGUS(modbusCmdNow.VPAddr, (u8 *)(&value), 2);
            Modbus_Write_Register06H(&modbusCmdNow, value);
            cmdTxFlag = 1;
        }
        else if (modbusCmdNow.funCode == BUS_FUN_10H)
        {
            Modbus_Write_Register10H(&modbusCmdNow);
            cmdTxFlag = 1;
        }
    }
    else
    {
        CmdIndex = 0;
    }
}
// modbus 03H 读取寄存器
void Modbus_Read_Register03H(modbosCmd_t *CmdNow)
{
    u16 crc_data;
    u8 len;
    u8 modbus_tx_buf[20];

    len                  = 0;
    modbus_tx_buf[len++] = CmdNow->slaveID;
    modbus_tx_buf[len++] = BUS_FUN_03H;                      // command
    modbus_tx_buf[len++] = (CmdNow->slaveAddr >> 8) & 0xFF;  // register
    modbus_tx_buf[len++] = CmdNow->slaveAddr & 0xFF;
    modbus_tx_buf[len++] = (CmdNow->length >> 8) & 0xFF;  // register number
    modbus_tx_buf[len++] = CmdNow->length & 0xFF;
    crc_data             = crc16table(modbus_tx_buf, len);
    modbus_tx_buf[len++] = (crc_data >> 8) & 0xFF;
    modbus_tx_buf[len++] = crc_data & 0xFF;
#ifdef MDO_UART2
    Uart2SendStr(modbus_tx_buf, len);
#endif
#ifdef MDO_UART5
    Uart5SendStr(modbus_tx_buf, len);
#endif
}

// modbus 06H 发送
void Modbus_Write_Register06H(modbosCmd_t *CmdNow, u16 value)
{
    u16 crc_data;
    u8 len;
    u8 modbus_tx_buf[20];

    len                  = 0;
    modbus_tx_buf[len++] = CmdNow->slaveID;
    modbus_tx_buf[len++] = BUS_FUN_06H;                      // command
    modbus_tx_buf[len++] = (CmdNow->slaveAddr >> 8) & 0xFF;  // register
    modbus_tx_buf[len++] = CmdNow->slaveAddr & 0xFF;
    modbus_tx_buf[len++] = (value >> 8) & 0xFF;  // register value
    modbus_tx_buf[len++] = value & 0xFF;
    crc_data             = crc16table(modbus_tx_buf, len);
    modbus_tx_buf[len++] = (crc_data >> 8) & 0xFF;
    modbus_tx_buf[len++] = crc_data & 0xFF;
#ifdef MDO_UART2
    Uart2SendStr(modbus_tx_buf, len);
#endif
#ifdef MDO_UART5
    Uart5SendStr(modbus_tx_buf, len);
#endif
}  // modbus 06H 发送
void Modbus_Write_Register10H(modbosCmd_t *CmdNow)
{
    u16 crc_data;
    u8 len = 0;
    u8 modbus_tx_buf[64];

    modbus_tx_buf[len++] = CmdNow->slaveID;
    modbus_tx_buf[len++] = BUS_FUN_10H;                      // command
    modbus_tx_buf[len++] = (CmdNow->slaveAddr >> 8) & 0xFF;  // register
    modbus_tx_buf[len++] = CmdNow->slaveAddr & 0xFF;
    modbus_tx_buf[len++] = (CmdNow->length >> 8) & 0xFF;  // register number
    modbus_tx_buf[len++] = CmdNow->length & 0xFF;
    modbus_tx_buf[len++] = CmdNow->length * 2;
    ReadDGUS(modbusCmdNow.VPAddr, (u8 *)(&modbus_tx_buf[len]), CmdNow->length * 2);
    len += CmdNow->length * 2;
    crc_data             = crc16table(modbus_tx_buf, len);
    modbus_tx_buf[len++] = (crc_data >> 8) & 0xFF;
    modbus_tx_buf[len++] = crc_data & 0xFF;
#ifdef MDO_UART2
    Uart2SendStr(modbus_tx_buf, len);
#endif
#ifdef MDO_UART5
    Uart5SendStr(modbus_tx_buf, len);
#endif
}
//清除modbus RX的相关参数
void Modbus_RX_Reset(void)
{
    modbus_rx_count = 0;
    modbus_rx_flag  = 0;
    memset(modbus_rx_buf, '\0', UART_RX_BUF_MAX_LEN);
    modbus_rx_count_before = 0;
    uart_rx_check_tick     = 0;
}
//初始化modbus 相关参数
void Modbus_UART_Init(void)
{
    //	Modbus_TX_Reset();
    Modbus_RX_Reset();
    modbus_tx_process_tick = 0;  //初始化 0
}

void getCmd(u8 *index)
{
    u8 i;
    for (i = *index; i < CMD_NUMBER; i++)
    {
        if ((modbusCmdlib[i].modbusEn != BUS_EN) || (modbusCmdlib[i].length == 0))
        {
            continue;
        }
        if (modbusCmdlib[i].mode == MODE_ALWA)
        {
            goto getCmdExit;
        }
        else if (modbusCmdlib[i].mode == MODE_PAGE)
        {
            if (picNow == modbusCmdlib[i].modePara)
            {
                goto getCmdExit;
            }
            continue;
        }
        else if (modbusCmdlib[i].mode == MODE_PARA)
        {
            u16 paraTemp;
            ReadDGUS(modbusCmdlib[i].modePara, (u8 *)(&paraTemp), 2);
            if ((paraTemp & 0xff) == 0x5a)
            {
                if (i < CMD_NUMBER - 1)
                {
                    if ((modbusCmdlib[i + 1].mode == MODE_PARA) &&
                        (modbusCmdlib[i].modePara == modbusCmdlib[i + 1].modePara))
                    {
                        goto getCmdExit;
                    }
                }
                paraTemp = 0;
                WriteDGUS(modbusCmdlib[i].modePara, (u8 *)(&paraTemp), 2);
                goto getCmdExit;
            }
            continue;
        }
        else if (modbusCmdlib[i].mode == MODE_PANP)
        {
            u16 paraTemp;
            if (modbusCmdlib[i].feedback != picNow)
            {
                continue;
            }
            ReadDGUS(modbusCmdlib[i].modePara, (u8 *)(&paraTemp), 2);
            if ((paraTemp & 0xff) == 0x5a)
            {
                if (i < CMD_NUMBER - 1)
                {
                    if ((modbusCmdlib[i + 1].mode == MODE_PANP) &&
                        (modbusCmdlib[i].modePara == modbusCmdlib[i + 1].modePara))
                    {
                        goto getCmdExit;
                    }
                }
                paraTemp = 0;
                WriteDGUS(modbusCmdlib[i].modePara, (u8 *)(&paraTemp), 2);
                goto getCmdExit;
            }
            continue;
        }
    }
getCmdExit:
    *index = i;
}

void checkChange(void)
{
    u16 cache[20] = {0};
    u16 i;
    for (i = 0; i < CHECK_NUMBER; i++)
    {
        if (dataCheckLib[i].page != picNow)
            continue;
        ReadDGUS(dataCheckLib[i].dataAddr, (u8 *)&cache[0], 2);
        ReadDGUS(dataCheckLib[i].backAddr, (u8 *)&cache[1], 2);
        if (cache[0] != cache[1])
        {
            WriteDGUS(dataCheckLib[i].backAddr, (u8 *)&cache[0], 2);
            cache[2] = 0x5a;
            WriteDGUS(dataCheckLib[i].flagAddr, (u8 *)&cache[2], 2);
        }
    }
}

void forcedOutputHnadle(void)
{
    u16 cache[7] = {0};
    ReadDGUS(0xc7a0, (u8 *)cache, 12);
    cache[7] = 0x00;
    cache[7] |= ((cache[0] & 1) << 0x00);
    cache[7] |= ((cache[1] & 1) << 0x01);
    cache[7] |= ((cache[2] & 1) << 0x02);
    cache[7] |= ((cache[3] & 1) << 0x06);
    cache[7] |= ((cache[4] & 1) << 0x0c);
    cache[7] |= ((cache[5] & 1) << 0x0d);
    WriteDGUS(0xc722, (u8 *)&cache[7], 2);
    cache[7] = 0x005a;
    WriteDGUS(0xc782, (u8 *)&cache[7], 2);
}
