//
// Created by yeliw on 2020/4/14.
//
#include "stdio.h"
#include "stm8l15x.h"
#include "oled.h"
#include "EventManager.h"
#include "dateTime.h"
#include "MainMenu.h"
#include "ManageUserMenu.h"
#include "MenuHelper.h"
#include <RTC.h>
#include "SettingMenu.h"
#include "nvc.h"
#include "power.h"

#define SET_YEAR_TENS 0
#define SET_YEAR_UNIT 1
#define SET_MONTH_TENS 2
#define SET_MONTH_UNIT 3
#define SET_DAY_TENS 4
#define SET_DAY_UNIT 5
#define SET_HOUR_TENS 6
#define SET_HOUR_UNIT 7
#define SET_MIN_TENS 8
#define SET_MIN_UNIT 9
#define SET_SEC_TENS 10
#define SET_SEC_UNIT 11
#define CONFIRM_SET_TIME 12

#define ACTION_UPDATE 0
#define ACTION_SET_TIME 1

static DateTime dateTime;
static unsigned char menuStatus = RESUME;
static unsigned char settingStatus = 0;

static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length);

static void onInputChar(unsigned char c);

static void onInputConfirm(void);

static void updateUi(unsigned char c, unsigned char action) {
    switch (settingStatus) {
        case SET_YEAR_TENS:
            if(action == ACTION_SET_TIME)dateTime.RTC_DateStr.RTC_Year = (c-'0')*10 + dateTime.RTC_DateStr.RTC_Year % 10;
            displayChar(dateTime.RTC_DateStr.RTC_Year / 10 + '0', 16, 2, 1);
            break;
        case SET_YEAR_UNIT:
            if(action == ACTION_SET_TIME)dateTime.RTC_DateStr.RTC_Year = c-'0' + dateTime.RTC_DateStr.RTC_Year / 10 * 10;
            displayChar(dateTime.RTC_DateStr.RTC_Year / 10 + '0', 16, 2, 0);
            displayChar(dateTime.RTC_DateStr.RTC_Year % 10 + '0', 24, 2, 1);
            break;
        case SET_MONTH_TENS:
            if(action == ACTION_SET_TIME)dateTime.RTC_DateStr.RTC_Month = (RTC_Month_TypeDef)((c-'0')*10 + dateTime.RTC_DateStr.RTC_Month % 10);
            displayChar(dateTime.RTC_DateStr.RTC_Year % 10 + '0', 24, 2, 0);
            displayChar(dateTime.RTC_DateStr.RTC_Month / 10 + '0', 40, 2, 1);
            break;
        case SET_MONTH_UNIT:
            if(action == ACTION_SET_TIME)dateTime.RTC_DateStr.RTC_Month = (RTC_Month_TypeDef)(c-'0' + dateTime.RTC_DateStr.RTC_Month / 10 * 10);
            displayChar(dateTime.RTC_DateStr.RTC_Month / 10 + '0', 40, 2, 0);
            displayChar(dateTime.RTC_DateStr.RTC_Month % 10 + '0', 48, 2, 1);
            break;
        case SET_DAY_TENS:
            if(action == ACTION_SET_TIME)dateTime.RTC_DateStr.RTC_Date = (c-'0')*10 + dateTime.RTC_DateStr.RTC_Date % 10;
            displayChar(dateTime.RTC_DateStr.RTC_Month % 10 + '0', 48, 2, 0);
            displayChar(dateTime.RTC_DateStr.RTC_Date / 10 + '0', 64, 2, 1);
            break;
        case SET_DAY_UNIT:
            if(action == ACTION_SET_TIME)dateTime.RTC_DateStr.RTC_Date = c-'0' + dateTime.RTC_DateStr.RTC_Date / 10 * 10;
            displayChar(dateTime.RTC_DateStr.RTC_Date / 10 + '0', 64, 2, 0);
            displayChar(dateTime.RTC_DateStr.RTC_Date % 10 + '0', 72, 2, 1);
            break;

        case SET_HOUR_TENS:
            if(action == ACTION_SET_TIME)dateTime.RTC_TimeStr.RTC_Hours = (c-'0')*10 + dateTime.RTC_TimeStr.RTC_Hours % 10;
            displayChar(dateTime.RTC_DateStr.RTC_Date % 10 + '0', 72, 2, 0);
            displayChar(dateTime.RTC_TimeStr.RTC_Hours / 10 + '0', 0, 4, 1);
            break;
        case SET_HOUR_UNIT:
            if(action == ACTION_SET_TIME)dateTime.RTC_TimeStr.RTC_Hours = c-'0' + dateTime.RTC_TimeStr.RTC_Hours / 10 * 10;
            displayChar(dateTime.RTC_TimeStr.RTC_Hours / 10 + '0', 0, 4, 0);
            displayChar(dateTime.RTC_TimeStr.RTC_Hours % 10 + '0', 8, 4, 1);
            break;
        case SET_MIN_TENS:
            if(action == ACTION_SET_TIME)dateTime.RTC_TimeStr.RTC_Minutes = (c-'0')*10 + dateTime.RTC_TimeStr.RTC_Minutes % 10;
            displayChar(dateTime.RTC_TimeStr.RTC_Hours % 10 + '0', 8, 4, 0);
            displayChar(dateTime.RTC_TimeStr.RTC_Minutes / 10 + '0', 24, 4, 1);
            break;
        case SET_MIN_UNIT:
            if(action == ACTION_SET_TIME)dateTime.RTC_TimeStr.RTC_Minutes = c-'0' + dateTime.RTC_TimeStr.RTC_Minutes / 10 * 10;
            displayChar(dateTime.RTC_TimeStr.RTC_Minutes / 10 + '0', 24, 4, 0);
            displayChar(dateTime.RTC_TimeStr.RTC_Minutes % 10 + '0', 32, 4, 1);
            break;
        case SET_SEC_TENS:
            if(action == ACTION_SET_TIME)dateTime.RTC_TimeStr.RTC_Seconds = (c-'0')*10 + dateTime.RTC_TimeStr.RTC_Seconds % 10;
            displayChar(dateTime.RTC_TimeStr.RTC_Minutes % 10 + '0', 32, 4, 0);
            displayChar(dateTime.RTC_TimeStr.RTC_Seconds / 10 + '0', 48, 4, 1);
            break;
        case SET_SEC_UNIT:
            if(action == ACTION_SET_TIME)dateTime.RTC_TimeStr.RTC_Seconds = c-'0' + dateTime.RTC_TimeStr.RTC_Seconds / 10 * 10;
            displayChar(dateTime.RTC_TimeStr.RTC_Seconds / 10 + '0', 48, 4, 0);
            displayChar(dateTime.RTC_TimeStr.RTC_Seconds % 10 + '0', 56, 4, 1);
            break;

        case CONFIRM_SET_TIME:
            displayChar(dateTime.RTC_TimeStr.RTC_Seconds % 10 + '0', 56, 4, 0);
            display_GB2312_string(0, 6, "确定", 1);
            break;
    }
}

static void showMenu(void) {
    clear_screen();
    display_GB2312_string(0, 0, "时间设置", 0);
    char strBuf[20];
    sprintf(strBuf, "20%d%d-%d%d-%d%d", dateTime.RTC_DateStr.RTC_Year / 10, dateTime.RTC_DateStr.RTC_Year % 10,
            dateTime.RTC_DateStr.RTC_Month / 10, dateTime.RTC_DateStr.RTC_Month % 10,
            dateTime.RTC_DateStr.RTC_Date / 10, dateTime.RTC_DateStr.RTC_Date % 10);
    display_GB2312_string(0, 2, (u8 *)strBuf, 0);

    sprintf(strBuf, "%d%d:%d%d:%d%d", dateTime.RTC_TimeStr.RTC_Hours / 10, dateTime.RTC_TimeStr.RTC_Hours % 10,
            dateTime.RTC_TimeStr.RTC_Minutes / 10, dateTime.RTC_TimeStr.RTC_Minutes % 10,
            dateTime.RTC_TimeStr.RTC_Seconds / 10, dateTime.RTC_TimeStr.RTC_Seconds % 10);

    display_GB2312_string(0, 4, (u8 *)strBuf, 0);

    display_GB2312_string(0, 6, "确定", 0);
    updateUi(0, ACTION_UPDATE);
}

void onDateSettingCreate(void) {
    menuStatus = RESUME;
    settingStatus = SET_YEAR_TENS;
    dateTime = getTime();
    showMenu();
    registerEventCallback(eventCallback);
    restartTimeOutChecking();
}

static void finish(void) {
    unRegisterEventCallback();
    cancelTimeOutChecking();
}

static void onInputStart(void) {
}

static void onInputChar(unsigned char c) {
    restartTimeOutChecking();
    updateUi(c, ACTION_SET_TIME);
}

static void onInputConfirm(void) {
    restartTimeOutChecking();
    if(menuStatus != RESUME)return;
    if(settingStatus <= CONFIRM_SET_TIME)settingStatus++;
    if(settingStatus > CONFIRM_SET_TIME){
        RTCSetTime(dateTime);
        menuStatus = SHOW_TIPS;
        showTips(SET_TIME_SUCCESS);
        playSound(SOUND_OPERATION_SUCCESS, 0);
        return;
    }
    updateUi(0, ACTION_UPDATE);
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
