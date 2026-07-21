//
// Created by yeliw on 2020/4/18.
//


#include "stm8l15x.h"
#include "oled.h"
#include "EventManager.h"
#include "dateTime.h"
#include "MainMenu.h"
#include "SystemSettings.h"
#include "ESP32.h"
#include "MenuHelper.h"
#include "nvc.h"
#include "power.h"
static unsigned char userSelect = 0;
static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void showMenu(unsigned char selected) {
    clear_screen();
    displayChar('>', 0, selected * 2, 0);
    display_GB2312_string(16, 0, "返回", 0);
    display_GB2312_string(16, 2, "开始配置wifi", 0);
}

void onConfigWifiCreate(void) {
    userSelect = 0;
    showMenu(0);
    registerEventCallback(eventCallback);
    restartTimeOutChecking();
}

static void finish(void) {
    unRegisterEventCallback();
    cancelTimeOutChecking();
}

static void onInputConfirm(void) {
    cancelTimeOutChecking();
    switch (userSelect) {
        case 0://返回
            finish();
            onSystemSettingsCreate();
            break;

        case 1://开始配置wifi
#ifdef ESP32_WIFI_MODE
            playSound(SOUND_START_WIFI_CONFIG, 0);
            startSmartConfig();
            showTips(START_CONFIG_WIFI);
#else
            playSound(SOUND_OPERATION_FAIL, 0);
            showTips(OPERATION_ERROR);
#endif
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
            onSystemSettingsCreate();
            break;

        case '6'://确认
            onInputConfirm();
            break;

        case '8'://向下
            if (userSelect < 1)userSelect++;
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

        case KEY_INPUT_CHARS_EVENT:
            onInputChar(*pData);
            break;

        case KEY_INPUT_CANCEL_EVENT:
            finish();
            onSystemSettingsCreate();
            break;

        case KEY_CONFIRM_EVENT:
            onInputConfirm();
            break;

        case ESP32_WIFI_EVENT:
            if(length == 0){
                showTips(CONFIG_WIFI_COMPLETE);
            }
            break;
    }
}
