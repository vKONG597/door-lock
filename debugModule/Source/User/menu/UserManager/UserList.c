//
// Created by yeliw on 2020/4/14.
//

#include "stdio.h"
#include "stm8l15x.h"
#include "SystemParameters.h"
#include "oled.h"
#include "EventManager.h"
#include "dateTime.h"
#include "MainMenu.h"
#include "ManageUserMenu.h"
#include "MenuHelper.h"
#include "SystemSettings.h"
#include "nvc.h"
#include "power.h"
static unsigned char userIdList[MAX_USER_COUNT] = {0};
static unsigned char usersCount = 0;
static unsigned char menuStatus = RESUME;
static unsigned char userSelect = 0;

void (*onNextMenuCreate)(unsigned char c);

static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void showMenu(unsigned char id) {
    if (usersCount == 0) {
        menuStatus = SHOW_TIPS;
        showTips(USER_LIST_EMPTY);
        playSound(SOUND_USER_LIST_EMPTY, 0);
        return;
    }

    clear_screen();
    displayChar('>', 0, (id % 4) * 2, 0);
    unsigned char i, start = id / 4 * 4, end = usersCount - 1, strBuf[20];
    if ((end - start) >= 4) {
        end = start + 3;
    }

    for (i = start; i <= end; i++) {
        sprintf((char *)strBuf, "ID:%d %s", userIdList[i], getUserById(userIdList[i])->type == USER_ADMIN ? "管理员":"");
        display_GB2312_string(16, (i - start) * 2, strBuf, 0);
    }
}

void onUserListCreate(void (*nextMenu)(unsigned char c)) {
    onNextMenuCreate = nextMenu;
    menuStatus = RESUME;
    userSelect = 0;
    getUsersIndexList(userIdList, &usersCount);
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
    restartTimeOutChecking();

    if (menuStatus != RESUME || usersCount == 0) {
        return;
    }
    finish();
    onNextMenuCreate(userIdList[userSelect]);
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
            if (usersCount == 0)return;
            if (userSelect < (usersCount - 1))userSelect++;
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
            onManageUserMenuCreate();
            break;

        case KEY_CONFIRM_EVENT:
            onInputConfirm();
            break;
    }
}

