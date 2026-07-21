#include <stdio.h>
#include "string.h"
#include "EventManager.h"
#include "stm8l15x_it.h"
#include "TIM4.h"
#include "fingerprint.h"
#include "usart.h"
#include "dateTime.h"

#define FINGER_POWER_ON()   GPIO_ResetBits(GPIOE,GPIO_Pin_2)
#define FINGER_POWER_OFF()  GPIO_SetBits(GPIOE,GPIO_Pin_2)
#define UART_RECEIVE_BUF_LEN    8
#define UART_SEND_BUF_LEN   15
u32 startTime;
#define MAX_WAIT_TIME 10

typedef enum {
    FP_IDLE,
    FP_WAIT_MODULE_BOOT_UP,
    FP_WAIT_RESULT,
} FingerPrintState;

static unsigned char receiveBuf[UART_RECEIVE_BUF_LEN];
static unsigned char pendingCommand[UART_SEND_BUF_LEN] = {0};
static unsigned char pendingCommandLength = 0;
static unsigned char receiveLength = 0;
static unsigned char interruptFlag = 0;
static unsigned char status = FP_IDLE;
static unsigned char bootUpMessage[] = {0xf5, 0x09, 0x00, 0x96, 0xff, 0x00, 0x60, 0xf5};
static OnEventCallback fingerPrintCallback = 0;

void registerFingerPrintCallback(OnEventCallback callback) {
    fingerPrintCallback = callback;
}

void unRegisterFingerPrintCallback(void) {
    fingerPrintCallback = 0;
}

void onFingerPrintInt(unsigned char event) {
    interruptFlag = event;
}

void onFingerPrintReceiveByte(unsigned char byte) {
    if (receiveLength < UART_RECEIVE_BUF_LEN) {
        receiveBuf[receiveLength] = byte;
        receiveLength++;
    }
}

static uint8_t calcCHK(uint8_t wLen, uint8_t *ptr) {
    uint8_t i, temp = 0;

    for (i = 0; i < wLen; i++) {
        temp ^= *(ptr + i);
    }
    return temp;
}

static void resetFP_Module(void) {
    pendingCommandLength = 0;
    interruptFlag = 0;
    status = FP_IDLE;
    FINGER_POWER_OFF();
}

static void sendPendingCommand(void) {
    if (pendingCommand[1] == NO_COMMAND) {
        resetFP_Module();
        return;
    }
    sendBytes(USART2, pendingCommand, pendingCommandLength);
    receiveLength = 0;
    pendingCommand[1] = NO_COMMAND;
    pendingCommandLength = 0;
    status = FP_WAIT_RESULT;
    startTime = getDateTimeTick();
}

static void oneToNComparison(void) {
    pendingCommandLength = 8;
    pendingCommand[0] = PACKAGE_BOUNDARY;
    pendingCommand[1] = CMD_COMPARISON;
    pendingCommand[2] = 0;
    pendingCommand[3] = 0;
    pendingCommand[4] = 0;
    pendingCommand[5] = 0;
    pendingCommand[6] = calcCHK(5, &pendingCommand[1]);
    pendingCommand[7] = PACKAGE_BOUNDARY;
}

//编辑用户时不需要重启模块(powerUpFlag = 0)，单独删除用户时，需要重启模块(powerUpFlag = 1)
void deleteFPUser(unsigned char id, unsigned char powerUpFlag) {
    if(powerUpFlag){
        FINGER_POWER_OFF();
        status = FP_WAIT_MODULE_BOOT_UP;
        receiveLength = 0;
        FINGER_POWER_ON();
    }

    pendingCommandLength = 8;
    pendingCommand[0] = PACKAGE_BOUNDARY;
    pendingCommand[1] = CMD_DELETE_USER;
    pendingCommand[2] = 0;//不需要录入0xfff个用户，所以高8位恒为0
    pendingCommand[3] = id;
    pendingCommand[4] = 0;
    pendingCommand[5] = 0;
    pendingCommand[6] = calcCHK(5, &pendingCommand[1]);
    pendingCommand[7] = PACKAGE_BOUNDARY;
    startTime = getDateTimeTick();
}

void deleteAllFPUser(void) {
    FINGER_POWER_OFF();
    status = FP_WAIT_MODULE_BOOT_UP;
    receiveLength = 0;
    FINGER_POWER_ON();
    pendingCommandLength = 8;
    pendingCommand[0] = PACKAGE_BOUNDARY;
    pendingCommand[1] = CMD_CLEAR_ALL_USER;
    pendingCommand[2] = 0;
    pendingCommand[3] = 0;
    pendingCommand[4] = 0;
    pendingCommand[5] = 0;
    pendingCommand[6] = calcCHK(5, &pendingCommand[1]);
    pendingCommand[7] = PACKAGE_BOUNDARY;
    startTime = getDateTimeTick();
}

#define MAX_REGISTER_FP_SETP2_COUNT 4
unsigned char registerFingerPrintStep2Count = MAX_REGISTER_FP_SETP2_COUNT;

static void registerFingerPrintStep1(unsigned char id) {
    pendingCommandLength = 8;
    pendingCommand[0] = PACKAGE_BOUNDARY;
    pendingCommand[1] = CMD_REGISTER;//命令一
    pendingCommand[2] = 0;//用户号高8位
    pendingCommand[3] = id;//用户号低8位
    pendingCommand[4] = 1;//用户权限，本应用中不使用该字段
    pendingCommand[5] = 0;
    pendingCommand[6] = calcCHK(5, &pendingCommand[1]);
    pendingCommand[7] = PACKAGE_BOUNDARY;
    registerFingerPrintStep2Count = MAX_REGISTER_FP_SETP2_COUNT;
}

static void registerFingerPrintStep2(void) {
    if (registerFingerPrintStep2Count == 0) return;
    registerFingerPrintStep2Count--;
    pendingCommandLength = 8;
    pendingCommand[1] = CMD_REGISTER_STEP2;//命令二
    pendingCommand[6] = calcCHK(5, &pendingCommand[1]);
}

static void registerFingerPrintStep3(void) {
    pendingCommandLength = 8;
    pendingCommand[1] = CMD_REGISTER_STEP3;//命令三
    pendingCommand[6] = calcCHK(5, &pendingCommand[1]);
}

void setFP_PendingCommand(unsigned char command, unsigned char *data) {
    switch (command) {
        case CMD_COMPARISON:
            oneToNComparison();
            break;

        case CMD_REGISTER:
            registerFingerPrintStep1(*data);
            break;
    }
}

static unsigned char checkCHK(void) {
    unsigned char i, chk = 0;
    for (i = 1; i < 6; i++) {
        chk ^= receiveBuf[i];
    }
    if (chk == receiveBuf[UART_RECEIVE_BUF_LEN - 2])return 1;
    return 0;
}

static unsigned char parseResult(void) {
    if (receiveBuf[0] != PACKAGE_BOUNDARY || receiveBuf[UART_RECEIVE_BUF_LEN - 1] != PACKAGE_BOUNDARY)return 0;
    if (checkCHK() == 0)return 0;
    unsigned char result[3] = {0};
    switch (receiveBuf[1]) {
        case CMD_COMPARISON:
            if (receiveBuf[4] == ACK_NOUSER || receiveBuf[4] == ACK_TIMEOUT) {
                result[0] = FP_COMPARISON_ERROR;
                fingerPrintCallback(FINGER_PRINT_EVENT, result, 1);
            } else {
                result[0] = FP_COMPARISON_SUCCESS;
                result[1] = receiveBuf[3];//用户号低8位
                fingerPrintCallback(FINGER_PRINT_EVENT, result, 2);
            }
            break;

        case CMD_CLEAR_ALL_USER:
        case CMD_DELETE_USER:
            result[0] = FP_DELETE_USER_FAIL;
            if (receiveBuf[4] == ACK_SUCCESS || receiveBuf[4] == ACK_NOUSER) {
                result[0] = FP_DELETE_USER_SUCCESS;
            }
            fingerPrintCallback(FINGER_PRINT_EVENT, result, 1);
            break;

        case CMD_REGISTER:
            if (receiveBuf[4] != ACK_SUCCESS) {
                return 0;
            }
            result[0] = FP_REGISTER_NEXT_STEP;
            result[1] = 2;
            result[2] = MAX_REGISTER_FP_SETP2_COUNT + 2;
            fingerPrintCallback(FINGER_PRINT_EVENT, result, 1);
            registerFingerPrintStep2();
            break;

        case CMD_REGISTER_STEP2:
            if (receiveBuf[4] != ACK_SUCCESS) {
                return 0;
            }

            if (registerFingerPrintStep2Count > 0) {
                result[1] = MAX_REGISTER_FP_SETP2_COUNT - registerFingerPrintStep2Count + 2;
                registerFingerPrintStep2();
            } else {
                result[1] = MAX_REGISTER_FP_SETP2_COUNT + 2;
                registerFingerPrintStep3();
            }
            result[0] = FP_REGISTER_NEXT_STEP;
            result[2] = MAX_REGISTER_FP_SETP2_COUNT + 2;
            fingerPrintCallback(FINGER_PRINT_EVENT, result, 1);
            break;

        case CMD_REGISTER_STEP3:
            if (receiveBuf[4] != ACK_SUCCESS) {
                return 0;
            }
            result[0] = FP_OPERATION_SUCCESS;
            fingerPrintCallback(FINGER_PRINT_EVENT, result, 1);
            break;
    }
    return 1;
}

void fingerPrintProcess(void) {
    if (fingerPrintCallback == 0) return;

    if (interruptFlag == INT_TOUCH) {
        interruptFlag = INT_CLEARED;
        status = FP_WAIT_MODULE_BOOT_UP;
        startTime = getDateTimeTick();
        receiveLength = 0;
        FINGER_POWER_ON();
        fingerPrintCallback(FINGER_PRINT_EVENT, 0, 0);//通知上层应用，手指已经放下
        return;
    } else if (interruptFlag == INT_LEAVE) {
        interruptFlag = INT_CLEARED;
        return;
    }

    switch (status) {
        case FP_IDLE:
            break;

        case FP_WAIT_MODULE_BOOT_UP:
            if (dateTimePassed_S(startTime, MAX_WAIT_TIME)) {
                resetFP_Module();
                unsigned char code = FP_OPERATION_TIME_OUT;
                fingerPrintCallback(FINGER_PRINT_EVENT, &code, 0);
                return;
            }
            if (receiveLength < UART_RECEIVE_BUF_LEN) return;
            if (memcmp(receiveBuf, bootUpMessage, UART_RECEIVE_BUF_LEN) == 0) {
                receiveLength = 0;
                sendPendingCommand();//模块开机后，执行上层应用下发的命令
            }
            break;

        case FP_WAIT_RESULT:
            if (dateTimePassed_S(startTime, MAX_WAIT_TIME)) {
                resetFP_Module();
                unsigned char code = FP_OPERATION_TIME_OUT;
                fingerPrintCallback(FINGER_PRINT_EVENT, &code, 0);
                return;
            }
            if (receiveLength < UART_RECEIVE_BUF_LEN) return;
            if (parseResult() == 0) {
                unsigned char code = FP_OPERATION_FAIL;
                fingerPrintCallback(FINGER_PRINT_EVENT, &code, 0);
                resetFP_Module();
            } else {
                sendPendingCommand();
            }
            break;
    }
}


void fingerPrintInit() {
    FINGER_POWER_OFF();
    status = FP_IDLE;
}
