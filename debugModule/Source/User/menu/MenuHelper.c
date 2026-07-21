//
// Created by yeliw on 2020/4/14.
//
#include "stdio.h"
#include "stm8l15x.h"
#include "led.h"
#include "MenuHelper.h"
#include "oled.h"
#include <SystemParameters.h>

void turnOnTipsLed(void) {
    turnOnLedByIndex(2);
    turnOnLedByIndex(4);
    turnOnLedByIndex(6);
    turnOnLedByIndex(8);
}

void turnOffTipsLed(void) {
    turnOffLedByIndex(2);
    turnOffLedByIndex(4);
    turnOffLedByIndex(6);
    turnOffLedByIndex(8);
}

void showTips(unsigned char code) {
    clear_screen();
    switch (code) {
        case PASSWORD_ERROR:
            display_GB2312_string(0, 0, "密码错误", 0);
            break;

        case CARD_ERROR:
            display_GB2312_string(0, 0, "卡号错误", 0);
            break;

        case USER_DB_FULL:
            display_GB2312_string(0, 0, "用户数据库已满", 0);
            break;

        case INVALID_USER_TYPE:
            display_GB2312_string(0, 0, "非法用户类型", 0);
            break;

        case ADD_USER_SUCCESS:
            display_GB2312_string(0, 0, "增加用户成功", 0);
            break;

        case USER_LIST_EMPTY:
            display_GB2312_string(0, 0, "用户列表为空", 0);
            break;

        case DELETE_USER_FAIL:
            display_GB2312_string(0, 0, "删除用户失败", 0);
            break;

        case DELETE_USER_SUCCESS:
            display_GB2312_string(0, 0, "删除用户成功", 0);
            break;

        case EDIT_USER_SUCCESS:
            display_GB2312_string(0, 0, "编辑用户成功", 0);
            break;

        case SET_TIME_SUCCESS:
            display_GB2312_string(0, 0, "设置时间成功", 0);
            break;

        case OPERATION_ERROR:
            display_GB2312_string(0, 0, "操作失败", 0);
            break;

        case TIME_OUT:
            display_GB2312_string(0, 0, "操作超时", 0);
            break;

        case START_CONFIG_WIFI:
            display_GB2312_string(0, 0, "配置wifi中...", 0);
            break;

        case CONFIG_WIFI_COMPLETE:
            display_GB2312_string(0, 0, "配置wifi完成", 0);
            break;
            
        case PRY_DOOR_WARNNING:
            display_GB2312_string(0, 0, "门锁被撬", 0);
            break;
    }
}

void displayPassword(unsigned char y, unsigned char type, unsigned char *password, unsigned char length) {
    unsigned char i;
    char strBuf[5];
    OLED_Clear_line(y);
    display_GB2312_string(0, y, "密码:", 0);
    oledLineInit(40, y);
    switch (type) {
        case TYPE_KEY:
        case TYPE_NB:
        case TYPE_WIFI:
        case TYPE_BLE:
            oledLineAppendStr(password);
            break;

        case TYPE_CARD:
            for (i = 0; i < length; i++) {
                sprintf(strBuf, "%x", password[i]);
                oledLineAppendStr((unsigned char *)strBuf);
            }
            break;
    }
}