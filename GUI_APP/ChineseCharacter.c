/*******************************************************************
 * @Warning      : Without permission from the author,Not for commercial use
 * @File         : ChineseCharacter.c
 * @Author       : xiaowine
 * @Date         : 2020-08-28 18:14:20
 * @version      : v01.00
 * @ **************************************************************
 * @LastEditTime : 2020-08-31 15:27:55
 * @LastEditors  : xiaowine
 * @ **************************************************************
 * @brief        : 汉字字符集
 * @Description  : 文件编码必须为 GBK
 * @FilePath     : \T5L_C51_PRJ\GUI_APP\ChineseCharacter.c
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/

const char test[] = "测试汉字";

const char alarmMessage[115][32] = {
#include "alarmMessage.h"
    // {"ID00回风高温"},
    // {"ID01回风低温"},
    // {"ID02回风高湿"},
    // {"ID03回风低湿"},
    // {"ID04送风高温"},
    // {"ID05送风低温"},
    // {"ID06送风高湿"},
    // {"ID07送风低湿"},
    // {"ID08保留"},
    // {"ID09内部通信"},
    // {"ID10NTC异常"},
    // {"ID11风机1故障"},
    // {"ID12保留"},
    // {"ID13保留"},
    // {"ID14保留"},
    // {"ID15保留"},
    // {"ID16外风机故障"},
    // {"ID17风机1运行超时"},
    // {"ID18保留"},
    // {"ID19保留"},
    // {"ID20保留"},
    // {"ID21保留"},
    // {"ID22外风机运行超时"},
    // {"ID23高压1告警"},
    // {"ID24高压1锁死"},
    // {"ID25低压1告警"},
    // {"ID26低压1锁死"},
    // {"ID27排气温度1告警"},
    // {"ID28排气温度1锁死"},
    // {"ID29短周期1告警"},
    // {"ID30压机1运行超时"},
    // {"ID31高压2告警"},
    // {"ID32高压2锁死"},
    // {"ID33低压压力告警"},
    // {"ID34低压2锁死"},
    // {"ID35排气温度2告警"},
    // {"ID36排气温度2锁死"},
    // {"ID37短周期2告警"},
    // {"ID38压机2运行超时"},
    // {"ID39加湿器电流过大"},
    // {"ID40加湿器高水位"},
    // {"ID41加湿器缺水"},
    // {"ID42加湿器运行超时"},
    // {"ID43加热器告警"},
    // {"ID44加热器1超时"},
    // {"ID45加热器2超时"},
    // {"ID46断电重启"},
    // {"ID47电源相序错误"},
    // {"ID48电源频率偏高"},
    // {"ID49电源频率偏低"},
    // {"ID50A相过压"},
    // {"ID51B相过压"},
    // {"ID52C相过压"},
    // {"ID53A相欠压"},
    // {"ID54B相欠压"},
    // {"ID55C相欠压"},
    // {"ID56A相缺相"},
    // {"ID57B相缺相"},
    // {"ID58C相缺相"},
    // {"ID59气流丢失告警"},
    // {"ID60过滤网运行超时"},
    // {"ID61过滤网堵告警"},
    // {"ID62远程关机告警"},
    // {"ID63溢水告警"},
    // {"ID64群控通信异常"},
    // {"ID65保留"},
    // {"ID66低过热度1告警"},
    // {"ID67低过热度2告警"},
    // {"ID68烟雾告警"},
    // {"ID69用户自定义告警"},
    // {"ID70电源异常"},
    // {"ID71保留"},
    // {"ID72保留"},
    // {"ID73保留"},
    // {"ID74保留"},
    // {"ID75保留"},
    // {"ID76保留"},
    // {"ID77保留"},
    // {"ID78保留"},
    // {"ID79保留"},
    // {"ID80排水高水位"},
    // {"ID81加湿器异常"},
    // {"ID82风压过低"},
    // {"ID83保留"},
    // {"ID84系统1高压压力"},
    // {"ID85保留"},
    // {"ID86MOP1告警"},
    // {"ID87MOP2告警"},
    // {"ID88LOP1告警"},
    // {"ID89LOP2告警"},
    // {"ID90设备使用超时"},
    // {"ID91保留"},
    // {"ID92保留"},
    // {"ID93保留"},
    // {"ID94保留"},
    // {"ID95保留"},
    // {"ID96保留"},
    // {"ID97保留"},
    // {"ID98保留"},
    // {"ID99保留"},
    // {"ID100保留"},
    // {"ID101保留"},
    // {"ID102保留"},
    // {"ID103保留"},
    // {"ID104保留"},
    // {"ID105保留"},
    // {"ID106保留"},
    // {"ID107保留"},
    // {"ID108保留"},
    // {"ID109保留"},
    // {"ID110保留"},
    // {"ID111保留"},
    // {"ID112保留"},
    // {"ID113保留"},
    // {"ID114保留"},
};