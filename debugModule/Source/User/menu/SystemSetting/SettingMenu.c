//
// Created by yeliw on 2020/4/13.
//

#include "stm8l15x.h"
#include "oled.h"
#include "EventManager.h"
#include "dateTime.h"
#include "nvc.h"
#include "MainMenu.h"
#include "SystemSettings.h"
#include "MenuHelper.h"
#include "ManageUserMenu.h"
#include "DateSetting.h"
#include "power.h"

static unsigned char userSelect = 0;

static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void finish(void);

static void showMenu(unsigned char selected) {
    clear_screen();
    switch (selected) {
        case 0:
            playSound(SOUND_SYSTEM_SETTING, 0);
            break;

        case 1:
            playSound(SOUND_USER_MANAGE, 0);
            break;

        case 2:
            playSound(SOUND_TIME_SETTING, 0);
            break;
    }
    displayChar('>', 0, selected*2, 0);
    display_GB2312_string(16, 0, "系统设置", 0);
    display_GB2312_string(16, 2, "用户管理", 0);
    display_GB2312_string(16, 4, "时间设置", 0);
}

void onSettingMenuCreate(void) {
    userSelect = 0;
    showMenu(0);
    registerEventCallback(eventCallback);
    restartTimeOutChecking();
    turnOnTipsLed();
}


static void finish(void) {
    unRegisterEventCallback();
    cancelTimeOutChecking();
    turnOffTipsLed();
}

static void onInputStart(void) {
    turnOffTipsLed();
}

static void onInputConfirm(void) {
    turnOffTipsLed();
    cancelTimeOutChecking();
    switch (userSelect) {
        case 0://进入系统设置
            onSystemSettingsCreate();
            break;

        case 1://进入用户管理
            onManageUserMenuCreate();
            break;

        case 2://进入时间设置
            onDateSettingCreate();
            break;
    }
}

static void onInputChar(unsigned char c) {
    restartTimeOutChecking();
    switch (c) {
        case '2'://向上
            if (userSelect > 0)userSelect--;
            showMenu(userSelect);
            break;

        case '4'://返回
            finish();
            onMainMenuCreate();
            break;

        case '6'://确认
            onInputConfirm();
            break;

        case '8'://向下
            if (userSelect < 2)userSelect++;
            showMenu(userSelect);
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
            onMainMenuCreate();
            break;

        case KEY_CONFIRM_EVENT:
            onInputConfirm();
            break;
    }
}
