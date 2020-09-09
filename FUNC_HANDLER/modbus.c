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
void Modbus_Read_Register(modbosCmd_t *CmdNow);
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
    // en         id         fun           len  timeout mod    modP     VP  slaveAddr feedback
    {MODBUS_DIS, SLAVE_ID, MODBUS_READ_03H, 0x04, 0xc8, 0x01, 0x000e, 0x5000, 0x0300, 0x00ff},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x02, 0xc8, 0x01, 0x000a, 0xaa00, 0x0326, 0x00ff},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x08, 0xc8, 0x01, 0x000a, 0xaa02, 0x0341, 0x00ff},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x01, 0xc8, 0x01, 0x000a, 0xaa0a, 0x036f, 0x00ff},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x01, 0xc8, 0x01, 0x000a, 0xaa0b, 0x0377, 0x00ff},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x07, 0xc8, 0x03, 0xb300, 0xb320, 0x011a, 0x0013},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb380, 0xb320, 0x011a, 0x0013},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb382, 0xb322, 0x011c, 0x0013},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb383, 0xb323, 0x011d, 0x0013},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb384, 0xb324, 0x011e, 0x0013},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb385, 0xb325, 0x011f, 0x0013},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb386, 0xb326, 0x0120, 0x0013},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x0b, 0xc8, 0x03, 0xb400, 0xb420, 0x0158, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb480, 0xb420, 0x0158, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb481, 0xb421, 0x0159, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb483, 0xb423, 0x015b, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb484, 0xb424, 0x015c, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb485, 0xb425, 0x015d, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb486, 0xb426, 0x015e, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb487, 0xb427, 0x015f, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb488, 0xb428, 0x0160, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb48a, 0xb42a, 0x0162, 0x0014},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x06, 0xc8, 0x03, 0xb700, 0xb720, 0x013b, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x05, 0xc8, 0x03, 0xb700, 0xb726, 0x0143, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb780, 0xb720, 0x013b, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb784, 0xb724, 0x013f, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb785, 0xb725, 0x0140, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb786, 0xb726, 0x0143, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb787, 0xb727, 0x0144, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb788, 0xb728, 0x0145, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb789, 0xb729, 0x0146, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb78a, 0xb72a, 0x0147, 0x0017},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x01, 0xc8, 0x03, 0xb800, 0xb820, 0x016a, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x03, 0xc8, 0x03, 0xb800, 0xb821, 0x016d, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x01, 0xc8, 0x03, 0xb800, 0xb824, 0x0172, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x01, 0xc8, 0x03, 0xb800, 0xb825, 0x0174, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x01, 0xc8, 0x03, 0xb800, 0xb826, 0x017b, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_READ_03H, 0x01, 0xc8, 0x03, 0xb800, 0xb827, 0x022a, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb880, 0xb820, 0x016a, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb881, 0xb821, 0x016d, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb882, 0xb822, 0x016e, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb883, 0xb823, 0x016f, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb884, 0xb824, 0x0172, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb885, 0xb825, 0x0174, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb886, 0xb826, 0x017b, 0x0018},
    {MODBUS_EN, SLAVE_ID, MODBUS_WRITE_06H, 0x01, 0xc8, 0x03, 0xb887, 0xb827, 0x022a, 0x0018},
};
modbosCmd_t modbusCmdNow = {0};
u8 CmdIndex              = 0;

const dataCheckCmd_t dataCheckLib[] = {
    // en      page  data    back   flag
    {MODBUS_EN, 19, 0xb320, 0xb350, 0xb380},   //
    {MODBUS_EN, 19, 0xb322, 0xb352, 0xb382},   //
    {MODBUS_EN, 19, 0xb323, 0xb353, 0xb383},   //
    {MODBUS_EN, 19, 0xb324, 0xb354, 0xb384},   //
    {MODBUS_EN, 19, 0xb325, 0xb355, 0xb385},   //
    {MODBUS_EN, 19, 0xb326, 0xb356, 0xb386},   //
    {MODBUS_EN, 20, 0xb420, 0xb450, 0xb480},   //
    {MODBUS_EN, 20, 0xb421, 0xb451, 0xb481},   //
    {MODBUS_EN, 20, 0xb423, 0xb453, 0xb483},   //
    {MODBUS_EN, 20, 0xb424, 0xb454, 0xb484},   //
    {MODBUS_EN, 20, 0xb425, 0xb455, 0xb485},   //
    {MODBUS_EN, 20, 0xb426, 0xb456, 0xb486},   //
    {MODBUS_EN, 20, 0xb427, 0xb457, 0xb487},   //
    {MODBUS_EN, 20, 0xb428, 0xb458, 0xb488},   //
    {MODBUS_EN, 20, 0xb42a, 0xb45a, 0xb48a},   //
    {MODBUS_EN, 23, 0xb720, 0xb750, 0xb780},   //
    {MODBUS_EN, 23, 0xb724, 0xb754, 0xb784},   //
    {MODBUS_EN, 23, 0xb725, 0xb755, 0xb785},   //
    {MODBUS_EN, 23, 0xb726, 0xb756, 0xb786},   //
    {MODBUS_EN, 23, 0xb727, 0xb757, 0xb787},   //
    {MODBUS_EN, 23, 0xb728, 0xb758, 0xb788},   //
    {MODBUS_EN, 23, 0xb729, 0xb759, 0xb789},   //
    {MODBUS_EN, 23, 0xb72a, 0xb75a, 0xb78a},   //
    {MODBUS_EN, 24, 0xb820, 0xb850, 0xb880},   //
    {MODBUS_EN, 24, 0xb821, 0xb851, 0xb881},   //
    {MODBUS_EN, 24, 0xb822, 0xb852, 0xb882},   //
    {MODBUS_EN, 24, 0xb823, 0xb853, 0xb883},   //
    {MODBUS_EN, 24, 0xb824, 0xb854, 0xb884},   //
    {MODBUS_EN, 24, 0xb825, 0xb855, 0xb885},   //
    {MODBUS_EN, 24, 0xb826, 0xb856, 0xb886},   //
    {MODBUS_EN, 24, 0xb827, 0xb857, 0xb887},   //
    {MODBUS_DIS, 23, 0xb72a, 0xb75a, 0xb78a},  //
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
                case MODBUS_READ_03H:
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
                case MODBUS_WRITE_06H:
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
        if (modbusCmdNow.funCode == MODBUS_READ_03H)
        {
            Modbus_Read_Register(&modbusCmdNow);
            cmdTxFlag = 1;
        }
        else if (modbusCmdNow.funCode == MODBUS_WRITE_06H)
        {
            u16 value;
            ReadDGUS(modbusCmdNow.VPAddr, (u8 *)(&value), 2);
            Modbus_Write_Register06H(&modbusCmdNow, value);
            cmdTxFlag = 1;
        }
    }
    else
    {
        CmdIndex = 0;
    }
}
// modbus 03H 读取寄存器
void Modbus_Read_Register(modbosCmd_t *CmdNow)
{
    u16 crc_data;
    u8 len;
    u8 modbus_tx_buf[20];

    len                  = 0;
    modbus_tx_buf[len++] = CmdNow->slaveID;
    modbus_tx_buf[len++] = MODBUS_READ_03H;                  // command
    modbus_tx_buf[len++] = (CmdNow->slaveAddr >> 8) & 0xFF;  // register
    modbus_tx_buf[len++] = CmdNow->slaveAddr & 0xFF;
    modbus_tx_buf[len++] = (CmdNow->length >> 8) & 0xFF;  // register number
    modbus_tx_buf[len++] = CmdNow->length & 0xFF;
    crc_data             = crc16table(modbus_tx_buf, len);
    modbus_tx_buf[len++] = (crc_data >> 8) & 0xFF;
    modbus_tx_buf[len++] = crc_data & 0xFF;
    Uart2SendStr(modbus_tx_buf, len);
}

// modbus 06H 发送
void Modbus_Write_Register06H(modbosCmd_t *CmdNow, u16 value)
{
    u16 crc_data;
    u8 len;
    u8 modbus_tx_buf[20];

    len                  = 0;
    modbus_tx_buf[len++] = CmdNow->slaveID;
    modbus_tx_buf[len++] = MODBUS_WRITE_06H;                 // command
    modbus_tx_buf[len++] = (CmdNow->slaveAddr >> 8) & 0xFF;  // register
    modbus_tx_buf[len++] = CmdNow->slaveAddr & 0xFF;
    modbus_tx_buf[len++] = (value >> 8) & 0xFF;  // register value
    modbus_tx_buf[len++] = value & 0xFF;
    crc_data             = crc16table(modbus_tx_buf, len);
    modbus_tx_buf[len++] = (crc_data >> 8) & 0xFF;
    modbus_tx_buf[len++] = crc_data & 0xFF;
    Uart2SendStr(modbus_tx_buf, len);
}  // modbus 06H 发送
void Modbus_Write_Register10H(modbosCmd_t *CmdNow, u16 value)
{
    u16 crc_data;
    u8 len;
    u8 modbus_tx_buf[20];

    len                  = 0;
    modbus_tx_buf[len++] = CmdNow->slaveID;
    modbus_tx_buf[len++] = MODBUS_WRITE_06H;                 // command
    modbus_tx_buf[len++] = (CmdNow->slaveAddr >> 8) & 0xFF;  // register
    modbus_tx_buf[len++] = CmdNow->slaveAddr & 0xFF;
    modbus_tx_buf[len++] = (value >> 8) & 0xFF;  // register value
    modbus_tx_buf[len++] = value & 0xFF;
    crc_data             = crc16table(modbus_tx_buf, len);
    modbus_tx_buf[len++] = (crc_data >> 8) & 0xFF;
    modbus_tx_buf[len++] = crc_data & 0xFF;
    Uart2SendStr(modbus_tx_buf, len);
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
        if ((modbusCmdlib[i].modbusEn != MODBUS_EN) || (modbusCmdlib[i].length == 0))
        {
            continue;
        }
        if (modbusCmdlib[i].mode == 0)
        {
            goto getCmdExit;
        }
        else if (modbusCmdlib[i].mode == 1)
        {
            if (picNow == modbusCmdlib[i].modePara)
            {
                goto getCmdExit;
            }
            continue;
        }
        else if (modbusCmdlib[i].mode == 2)
        {
            u16 paraTemp;
            ReadDGUS(modbusCmdlib[i].modePara, (u8 *)(&paraTemp), 2);
            if ((paraTemp & 0xff) == 0x5a)
            {
                if (i < CMD_NUMBER - 1)
                {
                    if ((modbusCmdlib[i + 1].mode == 2) && (modbusCmdlib[i].modePara == modbusCmdlib[i + 1].modePara))
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
        else if (modbusCmdlib[i].mode == 3)
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
                    if ((modbusCmdlib[i + 1].mode == 3) && (modbusCmdlib[i].modePara == modbusCmdlib[i + 1].modePara))
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
