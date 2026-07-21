//
// Created by yeliw on 2020/4/14.
//

#include "stm8l15x.h"
#include "oled.h"
#include "EventManager.h"
#include "dateTime.h"
#include "MainMenu.h"
#include "SettingMenu.h"
#include "AddUser.h"
#include "SystemParameters.h"
#include "UserList.h"
#include "DeleteUser.h"
#include "EditUser.h"
#include "nvc.h"
#include "power.h"
static unsigned char userSelect = 0;

static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void showMenu(unsigned char selected) {
    clear_screen();
    switch (selected) {
        case 0:
            playSound(SOUND_ADD_GENERAL_USER, 0);
            break;

        case 1:
            playSound(SOUND_ADD_ADMIN_USER, 0);
            break;

        case 2:
            playSound(SOUND_EDIT_USER, 0);
            break;

        case 3:
            playSound(SOUND_DELETE_USER, 0);
            break;
    }
    displayChar('>', 0, selected * 2, 0);
    display_GB2312_string(16, 0, "添加普通用户", 0);
    display_GB2312_string(16, 2, "添加管理员", 0);
    display_GB2312_string(16, 4, "编辑用户", 0);
    display_GB2312_string(16, 6, "删除用户", 0);
}

void onManageUserMenuCreate(void) {
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
        case 0://添加普通用户
            onAddUserMenuCreate(USER_GENERAL);
            break;

        case 1://添加管理员用户
            onAddUserMenuCreate(USER_ADMIN);
            break;

        case 2://编辑用户
            onUserListCreate(onEditUserCreate);
            break;

        case 3://删除用户
            onUserListCreate(onDeleteUserCreate);
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
            if (userSelect < 3)userSelect++;
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
            onSettingMenuCreate();
            break;

        case KEY_CONFIRM_EVENT:
            onInputConfirm();
            break;
    }
}
