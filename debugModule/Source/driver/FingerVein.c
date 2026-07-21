//
// Created by yeliw on 2020/4/17.
//
#include "stm8l15x.h"
#include "EventManager.h"
#include "FingerVein.h"
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "dateTime.h"


/*包数据结构24字节*/
typedef struct {
    unsigned short wPrefix; //包标识xAABB
    unsigned char bAddress; //设备地址，0为广播
    unsigned char bCmd; //命令码
    unsigned char bEncode; //数据编码，默认为0
    unsigned char bDataLen; //有效数据长度（字节）
    unsigned char bData[16]; //包数据
    unsigned short wCheckSum; //包检验，0-22字节和
} XG_PACKET;

typedef enum {
    FV_WAIT_BOOT_UP = 0,
    FV_WAIT_CONNECTED,
    FV_WAIT_RESPONSE,
    FV_IDLE,
} FingerVeinState;

#define FINGER_VEIN_POWER_ON()   GPIO_ResetBits(GPIOE,GPIO_Pin_1)
#define FINGER_VEIN_POWER_OFF()  GPIO_SetBits(GPIOE,GPIO_Pin_1)


#define RECEIVE_BUF_LEN 24
static unsigned char status = FV_WAIT_BOOT_UP;
static unsigned char receiveLength = 0;
static unsigned char interruptFlag = 0;
static unsigned char receiveBuf[RECEIVE_BUF_LEN];
static unsigned int startTime = 0;

static OnEventCallback fingerVeinCallback = 0;

void registerFingerVeinCallback(OnEventCallback callback) {
    fingerVeinCallback = callback;
}

void unRegisterFingerVeinCallback(void) {
    fingerVeinCallback = 0;
}

void fingerVeinInit(void) {
    status = FV_WAIT_BOOT_UP;
    startTime = getDateTimeTick();
    receiveLength = 0;
    interruptFlag = 0;
    FINGER_VEIN_POWER_ON();
}

void onFingerVeinInt(void) {
    interruptFlag = 1;
}

void onFingerVeinReceiveByte(unsigned char byte) {
    if (receiveLength < RECEIVE_BUF_LEN) {
        receiveBuf[receiveLength] = byte;
        receiveLength++;
    }
}

static unsigned short calcCheckSum(unsigned char *pBuf, unsigned int len) {
    unsigned short Sum = 0;
    unsigned int i;

    for (i = 0; i < len; i++) {
        Sum += pBuf[i];
    }
    return Sum;
}

static void FV_ConnectDev(void) {
    XG_PACKET packet = {0};
    packet.wPrefix = XG_PREFIX_CODE;
    packet.bAddress = 0;
    packet.bCmd = XG_CMD_CONNECTION;
    packet.bEncode = 0;
    packet.bDataLen = 8;
    memcpy(packet.bData, "00000000", 8);
    packet.wCheckSum = calcCheckSum((unsigned char *) &packet.wPrefix,
                                    sizeof(XG_PACKET) - sizeof(packet.wCheckSum));

    //转换成小端模式
    unsigned char *p = (unsigned char *) &packet.wCheckSum, temp;
    temp = p[0];
    p[0] = p[1];
    p[1] = temp;

    receiveLength = 0;
    sendBytes(USART2, (unsigned char *) &packet, sizeof(XG_PACKET));
}

static const unsigned char commandVerify[] = {0xBB, 0xAA, 0x00, 0x17, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
                                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                              0x80, 0x01};

void veryfyFingerVein(void) {//1:N识别
    receiveLength = 0;
    status = FV_WAIT_RESPONSE;
    sendBytes(USART2, (unsigned char *) commandVerify, sizeof(XG_PACKET));
}

static const unsigned char commandDeleteAllUser[] = {0xBB, 0xAA, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                     0x00, 0x00, 0x00, 0x77, 0x01};

void FV_DeleteAllUser(void) {
    status = FV_WAIT_RESPONSE;
    receiveLength = 0;
    sendBytes(USART2, (unsigned char *) commandDeleteAllUser, sizeof(XG_PACKET));
}

void FV_DeleteUser(unsigned char id) {
    XG_PACKET packet = {0};
    packet.wPrefix = XG_PREFIX_CODE;
    packet.bAddress = 0;
    packet.bCmd = XG_CMD_CLEAR_ENROLL;
    packet.bEncode = 0;
    packet.bDataLen = 4;
    packet.bData[0] = id+1;//指静脉的id不能用0
    packet.wCheckSum = calcCheckSum((unsigned char *) &packet.wPrefix,
                                    sizeof(XG_PACKET) - sizeof(packet.wCheckSum));

    //转换成小端模式
    unsigned char *p = (unsigned char *) &packet.wCheckSum, temp;
    temp = p[0];
    p[0] = p[1];
    p[1] = temp;

    receiveLength = 0;
    status = FV_WAIT_RESPONSE;
    USART_ClearITPendingBit(USART2,USART_IT_RXNE);            //清除UART2挂起标志
    sendBytes(USART2, (unsigned char *) &packet, sizeof(XG_PACKET));
}

void FV_Enroll(unsigned char id) {
    XG_PACKET packet = {0};
    packet.wPrefix = XG_PREFIX_CODE;
    packet.bAddress = 0;
    packet.bCmd = XG_CMD_ENROLL;
    packet.bEncode = 0;
    packet.bDataLen = 4;
    packet.bData[0] = id+1;//指静脉的id不能用0
    packet.wCheckSum = calcCheckSum((unsigned char *) &packet.wPrefix,
                                    sizeof(XG_PACKET) - sizeof(packet.wCheckSum));

    //转换成小端模式
    unsigned char *p = (unsigned char *) &packet.wCheckSum, temp;
    temp = p[0];
    p[0] = p[1];
    p[1] = temp;

    receiveLength = 0;
    status = FV_WAIT_RESPONSE;
    sendBytes(USART2, (unsigned char *) &packet, sizeof(XG_PACKET));
}

static unsigned char checkPackage(void) {
    if (receiveBuf[0] != 0xBB || receiveBuf[1] != 0xAA)return 0;
    unsigned short checkSum = calcCheckSum(receiveBuf, 22);
    if (((checkSum & 0xff) != receiveBuf[22])
        || (((checkSum >> 8) & 0xff) != receiveBuf[23]))
        return 0;
    return 1;
}

static unsigned char parseResult(void) {
    if (checkPackage() == 0) {
        return 0;
    }
    XG_PACKET *packet = (XG_PACKET *) receiveBuf;
    unsigned char result[2] = {0};
    switch (packet->bCmd) {
        case XG_CMD_CONNECTION:
            status = FV_IDLE;
            if (packet->bData[0] != XG_ERR_SUCCESS) {//连接不成功
                status = FV_WAIT_BOOT_UP;
                return 0;
            }
            break;

        case XG_CMD_VERIFY:
            if (packet->bData[0] == XG_INPUT_FINGER) {//提示放入手指
                //什么都不需要做
            } else if (packet->bData[0] == XG_RELEASE_FINGER) {//提示拿开手指
                //什么都不需要做
            } else if (packet->bData[0] == XG_ERR_SUCCESS) {//识别成功 packet->bData[1]~packet->bData[4]为识别到的用户id
                result[0] = FV_VERIFY_SUCCESS;
                result[1] = packet->bData[1]-1;//id,只需要最低字节//指静脉的id不能用0
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 2);
                status = FV_IDLE;
            } else if (packet->bData[0] == XG_ERR_FAIL && packet->bData[1] == XG_ERR_VERIFY) {//识别失败
                result[0] = FV_VERIFY_FAIL;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            } else if (packet->bData[0] == XG_ERR_FAIL && packet->bData[1] == XG_ERR_NO_CONNECT) {//未连接
                result[0] = FV_VERIFY_FAIL;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            } else {//未知错误
                result[0] = FV_VERIFY_FAIL;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            }
            break;

        case XG_CMD_CLEAR_ENROLL:
            result[0] = FV_CLEAR_ENROLL_FAIL;
            if(packet->bData[0] == XG_ERR_SUCCESS){
                result[0] = FV_CLEAR_ENROLL_SUCCESS;
            }
            fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
            status = FV_IDLE;
            break;

        case XG_CMD_CLEAR_ALL_ENROLL:
            //暂时忽略此回应
            status = FV_IDLE;
            break;

        case XG_CMD_ENROLL:
            if (packet->bData[0] == XG_INPUT_FINGER) {//提示放入手指
                result[0] = FV_PUSH_FINGER;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
            } else if (packet->bData[0] == XG_RELEASE_FINGER) {//提示拿开手指
                result[0] = FV_RELEASE_FINGER;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
            } else if (packet->bData[0] == XG_ERR_SUCCESS) {//录入成功
                result[0] = FV_ENROLL_SUCCESS;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            } else if (packet->bData[0] == XG_ERR_FAIL && packet->bData[1] == XG_ERR_NOT_ENOUGH) {//空间不足
                result[0] = FV_ENROLL_FAIL;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            } else if (packet->bData[0] == XG_ERR_FAIL && packet->bData[1] == XG_ERR_TIME_OUT) {//超时
                result[0] = FV_ENROLL_FAIL;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            } else if (packet->bData[0] == XG_ERR_FAIL && packet->bData[1] == XG_ERR_INVALID_ID) {//id错误
                result[0] = FV_ENROLL_FAIL;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            } else if (packet->bData[0] == XG_ERR_FAIL && packet->bData[1] == XG_ERR_DUPLICATION_ID) {//此用户已登记
                result[0] = FV_ENROLL_FAIL;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            } else {//未知错误
                result[0] = FV_ENROLL_FAIL;
                fingerVeinCallback(FINGER_VEIN_EVENT, result, 1);
                status = FV_IDLE;
            }
            break;
    }
    return 1;
}

void fingerVeinProcess(void) {
    if (fingerVeinCallback == 0)return;

    if (interruptFlag == 1) {
        interruptFlag = 0;
        if (status == FV_IDLE) {
            unsigned char code = FV_TOUCH;
            fingerVeinCallback(FINGER_VEIN_EVENT, &code, 1);
        }
    }

    switch (status) {
        case FV_IDLE:
            break;

        case FV_WAIT_BOOT_UP:
            if (dateTimePassed_S(startTime, 10)) {
                status = FV_WAIT_CONNECTED;
            }
            break;

        case FV_WAIT_CONNECTED:
            FV_ConnectDev();
            status = FV_WAIT_RESPONSE;
            break;

        case FV_WAIT_RESPONSE:
            if (receiveLength < 24)break;
            if (parseResult() == 0) {
                fingerVeinInit();
            }
            receiveLength = 0;
            break;
    }
}

