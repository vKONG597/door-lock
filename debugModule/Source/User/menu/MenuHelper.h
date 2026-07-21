//
// Created by yeliw on 2020/4/14.
//

#ifndef LOCK_MENUHELPER_H
#define LOCK_MENUHELPER_H

typedef enum {
    PASSWORD_ERROR = 0,
    CARD_ERROR,
    USER_DB_FULL,
    INVALID_USER_TYPE,
    ADD_USER_SUCCESS,
    USER_LIST_EMPTY,
    DELETE_USER_SUCCESS,
    DELETE_USER_FAIL,
    EDIT_USER_SUCCESS,
    SET_TIME_SUCCESS,
    OPERATION_ERROR,
    TIME_OUT,
    START_CONFIG_WIFI,
    CONFIG_WIFI_COMPLETE,
    PRY_DOOR_WARNNING
} TipsType;

typedef enum {
    SLEEP = 0,
    RESUME,
    WAIT_ENTER_SETTING_MODE,
    WAIT_PASSWORD_FOR_SETTING,
    INPUT_UNLOCK_PASSWORD,
    SHOW_TIPS
} MenuStatus;



void turnOnTipsLed(void);

void turnOffTipsLed(void);

void showTips(unsigned char code);

void displayPassword(unsigned char y, unsigned char type, unsigned char *password, unsigned char length);

#endif //LOCK_MENUHELPER_H
