//
// Created by yeliw on 2020/4/14.
//
#include "stdio.h"
#include "string.h"
#include "stm8l15x.h"
#include "SystemParameters.h"
#include "oled.h"
#include "EventManager.h"
#include "dateTime.h"
#include "MainMenu.h"
#include "ManageUserMenu.h"
#include "MenuHelper.h"
#include "Board.h"
#include "nvc.h"
#include "power.h"
#ifdef FINGER_VEIN
#include "FingerVein.h"
#endif

#ifdef FINGER_PRINT
#include "fingerprint.h"
#endif

static unsigned char userId = 0;
static unsigned char userSelect = 0;//0:取消；1：删除
static unsigned char menuStatus = RESUME;
static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void showMenu(unsigned char select) {
    clear_screen();
    display_GB2312_string(0, 0, "删除用户", 0);
    unsigned char strBuf[10];
    sprintf((char *)strBuf, "用户ID：%d", userId);
    display_GB2312_string(0, 2, strBuf, 0);

    display_GB2312_string(0, 6, "取消", !select);
    display_GB2312_string(95, 6, "删除", select);
}

void onDeleteUserCreate(unsigned char id) {
    menuStatus = RESUME;
    userId = id;
    showMenu(0);
    registerEventCallback(eventCallback);
    restartTimeOutChecking();
}

static void finish(void) {
    unRegisterEventCallback();
    cancelTimeOutChecking();
}

static void onInputStart(void) {

}

void onDeleteSuccess(void){
    menuStatus = SHOW_TIPS;
    showTips(DELETE_USER_SUCCESS);
}

static void onInputConfirm(void) {
    restartTimeOutChecking();
    if(menuStatus != RESUME)return;

    if(userSelect == 0){
        finish();
        onManageUserMenuCreate();
        return;
    } else if(deleteUserById(userId) == 0){
        onDeleteSuccess();
        playSound(SOUND_DELETE_USER_SUCCESS, 0);
    }
}

static void onInputChar(unsigned char c) {
    restartTimeOutChecking();
    switch (c) {
        case '2'://向上
            break;

        case '4'://向左
            userSelect = 0;
            showMenu(userSelect);
            break;

        case '6'://向右
            userSelect = 1;
            showMenu(userSelect);
            break;

        case '8'://向下
            break;
    }
}


static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length) {
    switch (event) {
        case TIME_OUT_EVENT:
            finish();
            intoSleep();
            break;

        case KEY_INPUT_START_EVENT:
            onInputStart();
            break;

        case KEY_INPUT_CHARS_EVENT:
            onInputChar(*pData);
            break;

        case KEY_INPUT_CANCEL_EVENT:
            finish();
            onManageUserMenuCreate();
            break;

        case KEY_CONFIRM_EVENT:
            onInputConfirm();
            break;

#ifdef FINGER_PRINT
        case FINGER_PRINT_EVENT:
            onDeleteSuccess();
            break;
#endif
    }
}
