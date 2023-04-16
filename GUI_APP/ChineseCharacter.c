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
 * @brief        : �����ַ���
 * @Description  : �ļ��������Ϊ GBK
 * @FilePath     : \T5L_C51_PRJ\GUI_APP\ChineseCharacter.c
 * @ **************************************************************
 * @attention    :
 * @Powered By xiaowine
 * @<h2><center>&copy;  Copyright(C) cee0.com 2020</center></h2>
 * @All rights reserved
 ******************************************************************/

const char test[] = "���Ժ���";

const char alarmMessage[115][32] = {
    // #include "alarmMessage.h"
    {"ID00 High Return Temperature"},
    {"ID01 Low return temperature"},
    {"ID02 High return humidity"},
    {"ID03 Low return humidity"},
    {"ID04 High supply temperature"},
    {"ID05 Low supply temperature"},
    {"ID06 High supply humidity"},
    {"ID07 Low supply humidity"},
    {"ID08 Reserve"},
    {"ID09 Internal communicat error"},
    {"ID10 NTC error"},
    {"ID11 Fan1 error"},
    {"ID12 Reserve"},
    {"ID13 Reserve"},
    {"ID14 Reserve"},
    {"ID15 Reserve"},
    {"ID16 External fan error"},
    {"ID17 Fan 1 run timeout"},
    {"ID18 Reserve"},
    {"ID19 Reserve"},
    {"ID20 Power board com error"},
    {"ID21 Reserve"},
    {"ID22 External fan timeout"},
    {"ID23 High pressure 1 alarm"},
    {"ID24 High pressure 1 lock"},
    {"ID25 Low pressure 1 alarm"},
    {"ID26 Low Pressure 1 lock"},
    {"ID27 exhaust temp 1 alarm"},
    {"ID28 exhaust temp 1 locked"},
    {"ID29 Compressor 1 short period"},
    {"ID30 Compressor 1 run timeout"},
    {"ID31 High pressure 2 alarm"},
    {"ID32 High pressure 2 lock"},
    {"ID33 Low pressure alarm"},
    {"ID34 Low Pressure 2 lock"},
    {"ID35 exhaust temp 2 alarm"},
    {"ID36 exhaust temp 2 locked"},
    {"ID37 short period 2 alarm"},
    {"ID38 Compressor 2 run timeout"},
    {"ID39 Humidifier's current large"},
    {"ID40 Humidifier high water level"},
    {"ID41 Humidifier no water"},
    {"ID42 Humidifier run timeout"},
    {"ID43 Heater alarm"},
    {"ID44 Heater 1 timeout"},
    {"ID45 Heater 2 timeout"},
    {"ID46 Power off restart"},
    {"ID47 Power phase sequence error"},
    {"ID48 Power freq high"},
    {"ID49 Power freq low"},
    {"ID50 Phase A overvoltage"},
    {"ID51 Phase B overvoltage"},
    {"ID52 Phase C overvoltage"},
    {"ID53 Phase A lower-voltage"},
    {"ID54 Phase B lower-voltage"},
    {"ID55 Phase C lower-voltage"},
    {"ID56 Phase A loss"},
    {"ID57 Phase B loss"},
    {"ID58 Phase C loss"},
    {"ID59 Loss of air flow"},
    {"ID60 Fliter run timeout"},
    {"ID61 Filter clogged "},
    {"ID62 Remote shutdown"},
    {"ID63 Water under floor"},
    {"ID64 Teamwork communiction error"},
    {"ID65 Reserve"},
    {"ID66 Low superheat 1"},
    {"ID67 Low superheat 2"},
    {"ID68 Smoke alarm"},
    {"ID69 User alarm"},
    {"ID70 Power error"},
    {"ID71 Reserve"},
    {"ID72 Reserve"},
    {"ID73 Reserve"},
    {"ID74 Reserve"},
    {"ID75 Reserve"},
    {"ID76 Low pressure"},
    {"ID77 Reserve"},
    {"ID78 Low pressure sensor error"},
    {"ID79 High pressure sensor error"},
    {"ID80 High level condensate water"},
    {"ID81 Humidifer error"},
    {"ID82 Wind pressure low"},
    {"ID83 Reserve"},
    {"ID84 High pressure 1"},
    {"ID85 Reserve"},
    {"ID86 MOP1 alarm"},
    {"ID87 MOP2 alarm"},
    {"ID88 LOP1 alarm"},
    {"ID89 LOP2 alarm"},
    {"ID90 Equipment period of time"},
    {"ID91 Reserve"},
    {"ID92 Reserve"},
    {"ID93 Reserve"},
    {"Ҽ��������½��ƾ�ʰҼ��������½"},
    {"0123456789abcdef0123456789abcdef"},
    {"ID96 Reserve"},
    {"ID97 Reserve"},
    {"ID98 Reserve"},
    {"ID99 Reserve"},
    {"ID100 Reserve"},
    {"ID101 Reserve"},
    {"ID102 Reserve"},
    {"ID103 Reserve"},
    {"ID104 Reserve"},
    {"ID105 Reserve"},
    {"ID106 Reserve"},
    {"ID107 Reserve"},
    {"ID108 Reserve"},
    {"ID109 Reserve"},
    {"ID110 Reserve"},
    {"ID111 Reserve"},
    {"ID112 Reserve"},
    {"ID113 Reserve"},
    {"Ҽ��������½��ƾ�ʰҼ��������½"},
};