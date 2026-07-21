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
#ifdef FINGER_PRINT
#include "fingerprint.h"
#endif
#ifdef FINGER_VEIN
#include "FingerVein.h"
#endif
#ifdef ESP32
#include "ESP32.h"
#endif
#ifdef NB_73
#include "NB_73.h"
#endif

UserT userToEdit;
static unsigned char menuStatus = RESUME;

static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void showMenu(unsigned char userType) {
      userToEdit.id = getAvailableUserId();
    if (userToEdit.id == 0xff) {
        menuStatus = SHOW_TIPS;
        showTips(USER_DB_FULL);
        return;
    }
    clear_screen();
    playSound(SOUND_INPUT_PASSWORD, 0);
    if (userType == USER_ADMIN) {
        display_GB2312_string(0, 0, "添加管理员用户", 0);
    } else if (userType == USER_GENERAL) {
        display_GB2312_string(0, 0, "添加普通用户", 0);
    } else {
        menuStatus = SHOW_TIPS;
        showTips(INVALID_USER_TYPE);
        return;
    }
    char strBuf[20];
    sprintf(strBuf, "用户ID:%d", userToEdit.id);
    display_GB2312_string(0, 2, (unsigned char *)strBuf, 0);
    display_GB2312_string(0, 4, "密码类型:", 0);
    oledLineInit(40, 6);
    display_GB2312_string(0, 6, "密码:", 0);
}

void onAddUserMenuCreate(unsigned char type) {
    menuStatus = RESUME;
    initNewUser(&userToEdit);
    userToEdit.type = type;
    showMenu(type);
    registerEventCallback(eventCallback);
    restartTimeOutChecking();
}

static void finish(void) {
    unRegisterEventCallback();
    cancelTimeOutChecking();
}

static void onInputStart(void) {
    userToEdit.passWord.type = TYPE_KEY;
    userToEdit.passWord.length = 0;
    memset(userToEdit.passWord.text, 0, PASSWORD_LENGTH);
    OLED_Clear_line(4);
    display_GB2312_string(0, 4, "密码类型:", 0);
    display_GB2312_string(72, 4, passwordTypeMap[userToEdit.passWord.type], 0);
    OLED_Clear_line(6);
    oledLineInit(40, 6);
    display_GB2312_string(0, 6, "密码:", 0);
}

static void onInputChar(unsigned char c) {
    restartTimeOutChecking();
    if (userToEdit.passWord.length > (PASSWORD_LENGTH - 1))return;
    userToEdit.passWord.text[userToEdit.passWord.length] = c;
    userToEdit.passWord.length++;
    oledLineAppendChar(c);
}

static void onInputConfirm(void) {
    restartTimeOutChecking();
    if (menuStatus != RESUME)return;
    if (userToEdit.passWord.length == 0) {
        menuStatus = SHOW_TIPS;
        showTips(PASSWORD_ERROR);
        return;
    }
    saveUser(&userToEdit);
    menuStatus = SHOW_TIPS;
    showTips(ADD_USER_SUCCESS);
    playSound(SOUND_OPERATION_SUCCESS, 0);
}

static void onReadCard(unsigned char *pCode, unsigned char length) {
    restartTimeOutChecking();
    userToEdit.passWord.type = TYPE_CARD;
    userToEdit.passWord.length = length;
    memset(userToEdit.passWord.text, 0, PASSWORD_LENGTH);
    memcpy(userToEdit.passWord.text, pCode, length);

    OLED_Clear_line(4);
    display_GB2312_string(0, 4, "密码类型:", 0);
    display_GB2312_string(72, 4, passwordTypeMap[userToEdit.passWord.type], 0);

    displayPassword(6, userToEdit.passWord.type, userToEdit.passWord.text, userToEdit.passWord.length);
}

#ifdef FINGER_PRINT
static void onFingerPrintEvent(unsigned char *pResult, unsigned char length) {
    restartTimeOutChecking();
    unsigned char buf[20];
    switch (pResult[0]) {
        case FP_TOUCH:
            setFP_PendingCommand(CMD_REGISTER, &userToEdit.id);
            userToEdit.passWord.type = TYPE_FINGER_PRINT;
            userToEdit.passWord.length = 0;
            memset(userToEdit.passWord.text, 0, PASSWORD_LENGTH);
            OLED_Clear_line(4);
            display_GB2312_string(0, 4, "密码类型:", 0);
            display_GB2312_string(72, 4, passwordTypeMap[userToEdit.passWord.type], 0);
            break;

        case FP_REGISTER_NEXT_STEP:
            OLED_Clear_line(6);
            playSound(SOUND_PRESS_FINGER_PRINT_AGAIN, 0);
            sprintf((char *)buf, "第%d/%d次按指纹", *(pResult+1), *(pResult+2));
            display_GB2312_string(0, 6, buf, 0);
            break;

        case FP_OPERATION_FAIL:
            menuStatus = SHOW_TIPS;
            showTips(OPERATION_ERROR);
            playSound(SOUND_OPERATION_FAIL, 0);
            break;

        case FP_OPERATION_SUCCESS:
            saveUser(&userToEdit);
            menuStatus = SHOW_TIPS;
            showTips(ADD_USER_SUCCESS);
            playSound(SOUND_OPERATION_SUCCESS, 0);
            break;

        case FP_OPERATION_TIME_OUT:
            menuStatus = SHOW_TIPS;
            showTips(TIME_OUT);
            playSound(SOUND_OPERATION_FAIL, 0);
            break;

        default:
            break;
    }
}
#endif

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
            
        case READ_CARD_EVENT:
            onReadCard(pData, length);
            break;

#ifdef FINGER_PRINT
        case FINGER_PRINT_EVENT:
            onFingerPrintEvent(pData, length);
            break;
#endif
    }
}
