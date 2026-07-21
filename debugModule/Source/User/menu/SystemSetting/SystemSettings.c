//
// Created by yeliw on 2020/4/14.
//

#include "stm8l15x.h"
#include "oled.h"
#include "EventManager.h"
#include "dateTime.h"
#include "nvc.h"
#include "MainMenu.h"
#include "SettingMenu.h"
#include "VoiceSetting.h"
#include "RecoverSetting.h"
#include "ConfigWifi.h"
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
          playSound(SOUND_VOICE_SETTING, 0);
            break;

        case 1:
            playSound(SOUND_WIFI_CONFIG, 0);
            break;
            
        case 2:
          playSound(SOUND_RECOVER_FACTORY_CONFIG, 0);
          break;
    }
    displayChar('>', 0, selected * 2, 0);
    display_GB2312_string(16, 0, "语音设置", 0);
    display_GB2312_string(16, 2, "wifi配置", 0);
    display_GB2312_string(16, 4, "恢复出厂设置", 0);
}

void onSystemSettingsCreate(void) {
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
        case 0://进入语音设置
            onVoiceSettingsCreate();
            break;


        case 1:
            onConfigWifiCreate();
            break;
        case 2://进入回复出厂设置
            onRecoverSettingsCreate();
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
            onSettingMenuCreate();
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

        case KEY_INPUT_CHARS_EVENT:
            onInputChar(*pData);
            break;

        case KEY_INPUT_CANCEL_EVENT:
            finish();
            onSettingMenuCreate();
            break;

        case KEY_CONFIRM_EVENT:
            onInputConfirm();
            break;
    }
}
