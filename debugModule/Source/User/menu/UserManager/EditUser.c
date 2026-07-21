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

static UserT userToEdit;
static unsigned char menuStatus = RESUME;

static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void showMenu(void) {
    clear_screen();
    char strBuf[20];
    playSound(SOUND_INPUT_PASSWORD, 0);
    display_GB2312_string(0, 0, "编辑用户", 0);
    sprintf(strBuf, "用户ID:%d", userToEdit.id);
    display_GB2312_string(0, 2, (unsigned char *)strBuf, 0);
    display_GB2312_string(0, 4, "密码类型:", 0);
    display_GB2312_string(72, 4, passwordTypeMap[userToEdit.passWord.type], 0);
    displayPassword(6, userToEdit.passWord.type, userToEdit.passWord.text, userToEdit.passWord.length);
}

void onEditUserCreate(unsigned char id) {
    menuStatus = RESUME;
    memcpy(&userToEdit, getUserById(id), sizeof(UserT));
    showMenu();
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
    display_GB2312_string(0, 6, "密码:", 0);
    oledLineInit(40, 6);
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
    showTips(EDIT_USER_SUCCESS);
    playSound(SOUND_OPERATION_SUCCESS, 0);
}

static void onReadCard(unsigned char *pCode, unsigned char length) {
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
static void startRegisterFPTypePassword(void) {
    setFP_PendingCommand(CMD_REGISTER, &userToEdit.id);
    userToEdit.passWord.type = TYPE_FINGER_PRINT;
    userToEdit.passWord.length = 0;
    memset(userToEdit.passWord.text, 0, PASSWORD_LENGTH);
    OLED_Clear_line(4);
    display_GB2312_string(0, 4, "密码类型:", 0);
    display_GB2312_string(72, 4, passwordTypeMap[userToEdit.passWord.type], 0);
}

static void onFingerPrintEvent(unsigned char *pResult, unsigned char length) {
    restartTimeOutChecking();
    switch (pResult[0]) {
        case FP_TOUCH:
            if (userToEdit.passWord.type == TYPE_FINGER_PRINT) {
                deleteFPUser(userToEdit.id, 0);//如果原来的密码类型是指纹，则需要先把原来的指纹删除掉
                return;
            }
            startRegisterFPTypePassword();
            break;

        case FP_DELETE_USER_FAIL:
            deleteUserById(userToEdit.id);
            menuStatus = SHOW_TIPS;
            showTips(OPERATION_ERROR);
            playSound(SOUND_OPERATION_FAIL, 0);
            break;

        case FP_DELETE_USER_SUCCESS:
            startRegisterFPTypePassword();//删除指纹成功，启动指纹录入
            break;

        case FP_REGISTER_NEXT_STEP:
            OLED_Clear_line(6);
            unsigned char buf[20];
            playSound(SOUND_PRESS_FINGER_PRINT_AGAIN, 0);
            sprintf((char *)buf, "第%d/%d次按指纹", *(pResult+1), *(pResult+2));
            display_GB2312_string(0, 6, buf, 0);
            break;

        case FP_OPERATION_FAIL:
            deleteUserById(userToEdit.id);
            menuStatus = SHOW_TIPS;
            showTips(OPERATION_ERROR);
            playSound(SOUND_OPERATION_FAIL, 0);
            break;

        case FP_OPERATION_SUCCESS:
            saveUser(&userToEdit);
            menuStatus = SHOW_TIPS;
            showTips(EDIT_USER_SUCCESS);
            playSound(SOUND_OPERATION_SUCCESS, 0);
            break;

        case FP_OPERATION_TIME_OUT:
            deleteUserById(userToEdit.id);
            menuStatus = SHOW_TIPS;
            showTips(TIME_OUT);
            playSound(SOUND_OPERATION_SUCCESS, 0);
            break;

        default:
            //resetFP_Module();
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
