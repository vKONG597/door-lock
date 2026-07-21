#include "stdio.h"
#include "stm8l15x.h"
#include <string.h>
#include "EventManager.h"
#include "usart.h"
#include "TIM4.h"
#include "dateTime.h"

typedef enum {
    NB_INIT,
    NB_SLEEP_A_WHILE,
    NB_ENABLE_NOTIFY_MESSAGE,
    NB_LOGIN,
    NB_WAIT_LOGIN_RESPONSE,
    NB_WAIT_MESSAGE,
    NB_IDLE,
} NB73_Status;

#define NB_HEART_BEAT_PERIOD 10
#define MAX_NB_WAIT_TIME 10
#define NB_RETRY_WAIT_TIME 6

#define NB_RECEIVE_BUF_LEN 30
static unsigned char receiveBuf[NB_RECEIVE_BUF_LEN];
static unsigned char receiveBufLength = 0;
static unsigned char status = NB_INIT;
static u32 startTime = 0;

#define LOGIN_STR "login"
#define LOGIN_RESPONSE "loginOk"

static OnEventCallback NB_Callback = 0;

void registerNBCallback(OnEventCallback callback) {
    NB_Callback = callback;
}

void unRegisterNBCallback(void) {
    NB_Callback = 0;
}

void onNB73ReceiveByte(unsigned char byte) {
    if (receiveBufLength < NB_RECEIVE_BUF_LEN) {
        receiveBuf[receiveBufLength] = byte;
        receiveBufLength++;
    }
}

static void parseNBData(unsigned char *pNBData, unsigned char *outBuf, unsigned char *outLength){
    unsigned char i=0,j,byte[2];
    *outLength = 0;

    do{
      if(*pNBData < '0' || *pNBData > '9'){
        pNBData++;
        if(++i>NB_RECEIVE_BUF_LEN)return;
        continue;
      }else break;
    }while(1);
    
    i=2;
    while(i>0){
        i--;
        if(*pNBData == ','){
            pNBData++;
            break;
        }
        *outLength = *outLength*10 + *pNBData - '0';
        pNBData++;
    }
    for(i=0;i<*outLength*2;i+=2){
        for(j=0;j<2;j++){
            if(pNBData[i+j]>='0'&&pNBData[i+j]<='9'){
                byte[j] = pNBData[i+j]-'0';
            } else if(pNBData[i+j]>='A'&&pNBData[i+j]<='F'){
                byte[j] = pNBData[i+j]-'A'+10;
            } else if(pNBData[i+j]>='a'&&pNBData[i+j]<='f'){
                byte[j] = pNBData[i+j]-'a'+10;
            }
        }
        outBuf[i/2]=byte[0]<<4 | byte[1];
    }
}

static void bytesToHexString(unsigned char *inBuf, unsigned char inLength, unsigned char *outBuf){
    char i, hexArray[] = "0123456789ABCDEF";
    for(i=0;i<inLength;i++){
        outBuf[i*2] = hexArray[inBuf[i]>>4];
        outBuf[i*2+1] = hexArray[inBuf[i]&0x0f];
    }
    outBuf[i*2]='\0';
}

//示例命令:"AT+NMGS=5,login\r\n"
void sendDataByNB(unsigned char *pData, unsigned char length){
    unsigned char sendBuf[NB_RECEIVE_BUF_LEN]={0};
    sendBytes(USART3, "AT+NMGS=", 8);
    sprintf((char *)sendBuf, "%d,", length);
    sendBytes(USART3, sendBuf, strlen((char const *)sendBuf));
    bytesToHexString(pData, length, sendBuf);
    sprintf((char *)sendBuf, "%s\r\n", sendBuf);
    sendBytes(USART3, sendBuf, strlen((char const *)sendBuf));
}

static unsigned char checkIfReceivePackage(void) {
    if (receiveBufLength > 2
        && receiveBuf[receiveBufLength - 2] == '\r'
        && receiveBuf[receiveBufLength - 1] == '\n') {
        return 1;
    }
    return 0;
}

static void parseResult(void) {
    if (strstr((char const *)receiveBuf, "[WH-NB73]") != 0) {//NB模块开机通知
        status = NB_INIT;
        receiveBufLength = 0;
        memset(receiveBuf, 0, NB_RECEIVE_BUF_LEN);
        return;
    }
    char *comma;
    switch (status) {
        case NB_ENABLE_NOTIFY_MESSAGE:
            if (strstr((char const *)receiveBuf, "OK") != 0) {//开始登陆
                sendDataByNB(LOGIN_STR, strlen(LOGIN_STR));
                status = NB_LOGIN;
                startTime = getDateTimeTick();
            }
            break;

        case NB_LOGIN:
            if (strstr((char const *)receiveBuf, "+NNMI") != 0) {//收到服务器消息
                sendBytes(USART3, "AT+NMGR\r\n", 9);//取消息
                status = NB_WAIT_LOGIN_RESPONSE;
                startTime = getDateTimeTick();
            }
            break;

        case NB_WAIT_LOGIN_RESPONSE:
            if (strstr((char const *)receiveBuf, ",") != 0) {
                unsigned char buf[NB_RECEIVE_BUF_LEN]={0},length;
                parseNBData(receiveBuf, buf, &length);
                if (strstr((char const *)buf, LOGIN_RESPONSE) != 0) {//判断是否收到 'login'
                    status = NB_IDLE;
                    break;
                }
            }
            sendBytes(USART3, "AT+NMGR\r\n", 9);//重复读取数据
            break;

        case NB_IDLE:
            if (strstr((char const *)receiveBuf, "+NNMI") != 0) {//如果有新消息
                status = NB_WAIT_MESSAGE;
                startTime = getDateTimeTick();
                sendBytes(USART3, "AT+NMGR\r\n", 9);//读取数据
            }
            break;

        case NB_WAIT_MESSAGE:
            comma = strstr((char const *)receiveBuf, ",");
            if (comma != 0 && *(comma-1) != '1') {//判断是否收到消息内容，过滤掉"1,00"
              unsigned char buf[NB_RECEIVE_BUF_LEN]={0},length;
                parseNBData(receiveBuf, buf, &length);
                NB_Callback(NB_EVENT, buf, length);
                startTime = getDateTimeTick();
                status = NB_IDLE;
            } else if (strstr((char const *)receiveBuf, "OK") != 0) {
                status = NB_IDLE;
                startTime = getDateTimeTick();
            } else {
                sendBytes(USART3, "AT+NMGR\r\n", 9);//重复读取数据
            }
            break;
    }
    receiveBufLength = 0;
    memset(receiveBuf, 0, NB_RECEIVE_BUF_LEN);
}

void NB73Process(void) {
    if(NB_Callback == 0) return;
    switch (status) {
        case NB_INIT:
            status = NB_ENABLE_NOTIFY_MESSAGE;
            startTime = getDateTimeTick();
            receiveBufLength = 0;
            sendBytes(USART3, "AT+NNMI=2\r\n", 11);//使能消息接收通知
            break;

        case NB_SLEEP_A_WHILE://重复次数太多，暂停一下
            if (dateTimePassed_S(startTime, NB_RETRY_WAIT_TIME)) {
                status = NB_INIT;
            }
            break;

        case NB_ENABLE_NOTIFY_MESSAGE:
        case NB_LOGIN:
        case NB_WAIT_LOGIN_RESPONSE:
        case NB_WAIT_MESSAGE:
            if (dateTimePassed_S(startTime, MAX_NB_WAIT_TIME)) {
                status = NB_SLEEP_A_WHILE;
                startTime = getDateTimeTick();
                return;
            }
        case NB_IDLE:
            if(status == NB_IDLE && dateTimePassed_S(startTime, NB_HEART_BEAT_PERIOD)) {
                sendDataByNB("beat", 4);//发送心跳
                status = NB_WAIT_MESSAGE;
                startTime = getDateTimeTick();
            }
            if (!checkIfReceivePackage()) return;
            parseResult();
            break;
    }
}
