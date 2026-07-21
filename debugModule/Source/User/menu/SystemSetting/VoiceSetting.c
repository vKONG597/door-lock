//
// Created by yeliw on 2020/4/14.
//


#include "stm8l15x.h"
#include "oled.h"
#include "EventManager.h"
#include "dateTime.h"
#include "nvc.h"
#include "MainMenu.h"
#include "SystemSettings.h"
#include "VoiceSetting.h"
#include "SystemParameters.h"
#include "nvc.h"
#include "power.h"

static unsigned char userSelect = 0;

static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void showMenu(unsigned char selected) {
    clear_screen();
    displayChar('>', 0, selected * 2, 0);
    display_GB2312_string(16, 0, "开", 0);
    display_GB2312_string(16, 2, "关", 0);
}

void onVoiceSettingsCreate(void) {
    userSelect = 0;
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

static void onInputConfirm(void) {
    cancelTimeOutChecking();
    switch (userSelect) {
        case 0://打开声音
            setVoiceEnable(1);
            clear_screen();
            display_GB2312_string(16, 0, "设置成功", 0);
            playSound(SOUND_OPERATION_SUCCESS, 0);
            break;

        case 1://关闭声音
            setVoiceEnable(0);
            clear_screen();
            display_GB2312_string(16, 0, "设置成功", 0);
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

        case KEY_INPUT_START_EVENT:
            onInputStart();
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
    }
}
