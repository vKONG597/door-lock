//
// Created by xmy on 2019/12/10.
//

#include <stdio.h>
#include "stm8l15x.h"
#include "TIM4.h"
#include<string.h>
#include "EventManager.h"
#include "ESP32.h"
#include "usart.h"
#include "dateTime.h"

typedef enum {
    ESP_INIT,
    ESP_WAIT_RESET,
    ESP_WAIT_ATE0_RESPONSE,
    ESP_IDLE,

    ESP_CHECK_CONNECT_STATUS,
    ESP_CONNECT_TO_SERVER,
    ESP_WAIT_WIFI_DATA,
    ESP_INTO_STATION_MODE,
    ESP_ENABLE_AUTO_CONNECT_AP,
    ESP_STOP_SMART_CONFIG,
    ESP_START_SMART_CONFIG,

    ESP_BLE_INIT,
    ESP_BLE_SET_NAME,
    ESP_BLE_SET_ADV_DATA,
    ESP_BLE_GATT_SSRVCRE,
    ESP_BLE_GATT_SSRVSTART,
    ESP_BLE_ADVSTART,
    ESP_BLE_WAIT_CONNECT,
    ESP_WAIT_BLE_DATA,
} NB73_Status;

#define MAX_ESP_WAIT_TIME 20
#define ESP_RECEIVE_BUF_LEN 100
static unsigned char receiveBuf[ESP_RECEIVE_BUF_LEN];
static unsigned char receiveBufLength = 0;
static unsigned char status = ESP_INIT;
static u32 startTime = 0;


static OnEventCallback ESP32_Callback = 0;

void registerESP32Callback(OnEventCallback callback) {
    ESP32_Callback = callback;
}

void unRegisterESP32Callback(void) {
    ESP32_Callback = 0;
}

void onESPReceiveByte(unsigned char byte) {
    if (receiveBufLength < ESP_RECEIVE_BUF_LEN) {
        receiveBuf[receiveBufLength] = byte;
        receiveBufLength++;
    }
}


#ifdef ESP32_WIFI_MODE

#define SERVER_IP   "192.168.3.85"
#define SERVER_PORT   "34561"

char *wifiData = 0;
unsigned char wifiDataLength = 0;

static unsigned char *parseWifiData(unsigned char *pESPData, unsigned char *outLength) {
    *outLength = 0;
    while (*pESPData != ':') {
        *outLength = *outLength * 10 + *pESPData - '0';
        pESPData++;
    }
    return pESPData + 1;
}

static unsigned char checkIfReceivePackage(void) {
    if (receiveBufLength > 2
        && receiveBuf[receiveBufLength - 2] == '\r'
        && receiveBuf[receiveBufLength - 1] == '\n') {
        return 1;
    }

    if(receiveBufLength >= 8 && strstr((char const *)receiveBuf, "+IPD")) {
        wifiData = (char *)parseWifiData(receiveBuf + 7, &wifiDataLength);
        if((wifiDataLength+9) == receiveBufLength){
          return 1;
        }
    }

    return 0;
}

void startSmartConfig(void) {
    status = ESP_INTO_STATION_MODE;
    sendBytes(USART3, "AT+CWMODE=1\r\n", 13);
    receiveBufLength = 0;
    memset(receiveBuf, 0, ESP_RECEIVE_BUF_LEN);
}

void esp32SendWifiData(unsigned char *pData, unsigned char length) {
    sendBytes(USART3, "AT+CIPSEND=", 11);
    char lengthBuf[10];
    sprintf(lengthBuf, "%d\r\n", length);
    sendBytes(USART3, (unsigned char *)lengthBuf, strlen(lengthBuf));
    Delay_ms(500);//为了简化代码，不异步等待接收 '>' 符号
    sendBytes(USART3, pData, length);
}

static void parseResult(void) {

    switch (status) {
        case ESP_WAIT_ATE0_RESPONSE:
            if (strstr((char const *)receiveBuf, "OK") != 0) {//关闭回显成功
                status = ESP_CHECK_CONNECT_STATUS;
                sendBytes(USART3, "AT+CIPSTATUS\r\n", 14);
                startTime = getDateTimeTick();
            }
            break;

        case ESP_CHECK_CONNECT_STATUS:
            if (strstr((char const *)receiveBuf, "STATUS:3") != 0 //3: The ESP32 Station has created a TCP or UDP transmission
                || strstr((char const *)receiveBuf, "STATUS:1") != 0) {//1: The ESP32 station is idle.
                status = ESP_WAIT_WIFI_DATA;//已连接服务器
            } else if (strstr((char const *)receiveBuf, "STATUS:5") != 0) {
                status = ESP_IDLE;//wifi模块未联网
                startTime = getDateTimeTick();
            } else {
                status = ESP_CONNECT_TO_SERVER;//未连接tcp服务器
                sendBytes(USART3, "AT+CIPSTART=\"TCP\",\"", 19);
                sendBytes(USART3, SERVER_IP, strlen(SERVER_IP));
                sendBytes(USART3, "\",", 2);
                sendBytes(USART3, SERVER_PORT, strlen(SERVER_PORT));
                sendBytes(USART3, "\r\n", 2);
            }
            break;

        case ESP_CONNECT_TO_SERVER:
            if (strstr((char const *)receiveBuf, "CONNECT") != 0) {//连接服务器成功
                status = ESP_WAIT_WIFI_DATA;
            } else if (strstr((char const *)receiveBuf, "ERROR") != 0) {//连接服务器失败
                status = ESP_IDLE;
                startTime = getDateTimeTick();
            }
            break;

        case ESP_WAIT_WIFI_DATA:
            if (wifiData != 0) {//接收到数据
                ESP32_Callback(ESP32_WIFI_EVENT, (unsigned char *)wifiData, wifiDataLength);
            } else if (strstr((char const *)receiveBuf, "CLOSE") != 0) {
                status = ESP_INIT;
            }
            break;

        case ESP_INTO_STATION_MODE:
            if (strstr((char const *)receiveBuf, "OK") != 0) {
                status = ESP_ENABLE_AUTO_CONNECT_AP;
                sendBytes(USART3, "AT+CWAUTOCONN=1\r\n", 17);//使能上电自动连接ap
            } else {
                sendBytes(USART3, "AT+CWMODE=1\r\n", 13);//重复发送进入Station mode命令
            }
            break;

        case ESP_ENABLE_AUTO_CONNECT_AP:
            if (strstr((char const *)receiveBuf, "OK") != 0) {
                status = ESP_STOP_SMART_CONFIG;
                sendBytes(USART3, "AT+CWSTOPSMART\r\n", 16);// 先释放一下ESP32内部快连所占内存
            } else {
                sendBytes(USART3, "AT+CWAUTOCONN=1\r\n", 17);//重复发送使能上电自动连接ap命令
            }
            break;

        case ESP_STOP_SMART_CONFIG:
            if (strstr((char const *)receiveBuf, "OK") != 0) {
                status = ESP_START_SMART_CONFIG;
                sendBytes(USART3, "AT+CWSTARTSMART=3\r\n", 19);// ESP-Touch和Airkiss智能配网
            } else {
                sendBytes(USART3, "AT+CWSTOPSMART\r\n", 16);// 完成配网,释放ESP32内部快连所占内存
            }
            break;

        case ESP_START_SMART_CONFIG:
            if (strstr((char const *)receiveBuf, "WIFI GOT IP") != 0) {//配网成功
                ESP32_Callback(ESP32_WIFI_EVENT, 0, 0);
                status = ESP_INIT;//重新初始化
                sendBytes(USART3, "AT+CWSTOPSMART\r\n", 16);// 完成配网,释放ESP32内部快连所占内存
            }
            break;
    }

    wifiData = 0;
    receiveBufLength = 0;
    memset(receiveBuf, 0, ESP_RECEIVE_BUF_LEN);
}

void ESP32Process(void) {
    if (ESP32_Callback == 0)return;
    switch (status) {
        case ESP_INIT:
            sendBytes(USART3, "ATE0\r\n", 6);
            status = ESP_WAIT_ATE0_RESPONSE;
            startTime = getDateTimeTick();
            break;

        case ESP_WAIT_ATE0_RESPONSE:
        case ESP_CHECK_CONNECT_STATUS:
        case ESP_CONNECT_TO_SERVER:
            if (dateTimePassed_S(startTime, MAX_ESP_WAIT_TIME)) {
                status = ESP_INIT;
            }
        case ESP_INTO_STATION_MODE:
        case ESP_ENABLE_AUTO_CONNECT_AP:
        case ESP_STOP_SMART_CONFIG:
        case ESP_START_SMART_CONFIG:
        case ESP_WAIT_WIFI_DATA:
            if (!checkIfReceivePackage()) return;
            parseResult();
            break;

        case ESP_IDLE:
            if (dateTimePassed_S(startTime, MAX_ESP_WAIT_TIME)) {
                status = ESP_INIT;
            }
            break;
    }
}
#endif

#ifdef ESP32_BLE_MODE
#define BLE_NAME "lock"
#define BLE_NAME_HEX "0201060A096C6F636B2074657374" //020106为固定字符串  0A：字段长度 09：设备名称长度  6C6F636B2074657374:HEX表示的"lock test"
static unsigned char checkIfReceivePackage(void) {
    if (receiveBufLength > 2
        && receiveBuf[receiveBufLength - 2] == '\r'
        && receiveBuf[receiveBufLength - 1] == '\n') {
        return 1;
    }
    return 0;
}


//数据示例：AT+BLEGATTSNTFY=0,1,6,1    //0:第0个连接，1:第一个service， 6：第6个char，1:字节数
void esp32SendBleData(unsigned char *pData, unsigned char length) {
    sendBytes(USART3, "AT+BLEGATTSNTFY=0,1,6,", 22);
    char lengthBuf[10];
    sprintf(lengthBuf, "%d\r\n", length);
    sendBytes(USART3, (unsigned char *)lengthBuf, strlen(lengthBuf));
    Delay_ms(500);//为了简化代码，不异步等待接收 '>' 符号
    sendBytes(USART3, pData, length);
}

//数据示例：+WRITE:0,1,3,,2,04   //2：两个字节，04：数据
static unsigned char *parseBleData(unsigned char *inputData, unsigned char *outLength) {
    unsigned char i = 0;
    *outLength = 0;
    unsigned char *out=0;

    while (i < 4) {
        if (*inputData == ',')i++;
        inputData++;
    }
    while (*inputData != ',') {
        *outLength = *outLength * 10 + *inputData - '0';
        inputData++;
    }
    out = inputData + 1;
    while(*inputData != '\r'){
      inputData++;
    }
    *inputData = 0;
    return out;
}

static void parseResult() {
    switch (status) {
        case ESP_WAIT_RESET:
            if (strstr((char const *)receiveBuf, "ready") != 0) {
                sendBytes(USART3, "ATE0\r\n", 6);//关闭回显
                status = ESP_WAIT_ATE0_RESPONSE;
                startTime = getDateTimeTick();
            }
            break;

        case ESP_WAIT_ATE0_RESPONSE:
            if (strstr((char const *)receiveBuf, "OK") != 0) {//关闭回显成功
                status = ESP_BLE_INIT;
                sendBytes(USART3, "AT+BLEINIT=2\r\n", 14);//初始化BLE
                startTime = getDateTimeTick();
            }
            break;

        case ESP_BLE_INIT:
            if (strstr((char const *)receiveBuf, "OK") != 0) {//BLE初始化成功
                status = ESP_BLE_GATT_SSRVCRE;
                sendBytes(USART3, "AT+BLEGATTSSRVCRE\r\n", 19);//创建服务
                startTime = getDateTimeTick();
            }

            break;

        case ESP_BLE_GATT_SSRVCRE:
            if (strstr((char const *)receiveBuf, "OK") != 0) {//创建服务成功,启动服务
                status = ESP_BLE_GATT_SSRVSTART;
                sendBytes(USART3, "AT+BLEGATTSSRVSTART\r\n", 21);
                startTime = getDateTimeTick();
            }
            break;
            
        case ESP_BLE_GATT_SSRVSTART:
          if (strstr((char const *)receiveBuf, "OK") != 0) {//启动服务成功，设置蓝牙名字
                status = ESP_BLE_SET_NAME;
                sendBytes(USART3, "AT+BLENAME=\"", 12);//设置ble名字
                sendBytes(USART3, BLE_NAME, strlen(BLE_NAME));
                sendBytes(USART3, "\"\r\n", 3);
                startTime = getDateTimeTick();
            }
            break;
            
        case ESP_BLE_SET_NAME:
            if (strstr((char const *)receiveBuf, "OK") != 0) {//设置ble名字成功
                status = ESP_BLE_SET_ADV_DATA;
                sendBytes(USART3, "AT+BLEADVDATA=\"", 15);//设置广播数据
                sendBytes(USART3, BLE_NAME_HEX, strlen(BLE_NAME_HEX));
                sendBytes(USART3, "\"\r\n", 3);
                startTime = getDateTimeTick();
            }
            break;

        case ESP_BLE_SET_ADV_DATA:
            if (strstr((char const *)receiveBuf, "OK") != 0) {//设置广播数据成功
                status = ESP_BLE_ADVSTART;
                sendBytes(USART3, "AT+BLEADVSTART\r\n", 16);
                startTime = getDateTimeTick();
            }
            break;

        case ESP_BLE_ADVSTART:
            if (strstr((char const *)receiveBuf, "OK") != 0) {//启动广播成功，等待连接
                status = ESP_BLE_WAIT_CONNECT;
            }
            break;

        case ESP_BLE_WAIT_CONNECT:
            if (strstr((char const *)receiveBuf, "+BLECONN:") != 0) {//新连接
                status = ESP_WAIT_BLE_DATA;
            }
            break;

        case ESP_WAIT_BLE_DATA:
            if (strstr((char const *)receiveBuf, "+WRITE:") != 0) {//收到数据
                unsigned char *data, length;
                data = parseBleData(receiveBuf, &length);
                ESP32_Callback(ESP32_BLE_EVENT, data, length);
            } else if (strstr((char const *)receiveBuf, "+BLEDISCONN:") != 0) {//连接断开，重新启动广播
                status = ESP_BLE_ADVSTART;
                sendBytes(USART3, "AT+BLEADVSTART\r\n", 16);
                startTime = getDateTimeTick();
            }
            break;
    }

    receiveBufLength = 0;
    memset(receiveBuf, 0, ESP_RECEIVE_BUF_LEN);
}

void ESP32Process(void) {
    if (ESP32_Callback == 0)return;
    switch (status) {
        case ESP_INIT:
            sendBytes(USART3, "AT+RST\r\n", 8);
            status = ESP_WAIT_RESET;
            startTime = getDateTimeTick();
            break;

        case ESP_BLE_INIT:
        case ESP_BLE_SET_NAME:
        case ESP_BLE_SET_ADV_DATA:
        case ESP_BLE_GATT_SSRVCRE:
        case ESP_BLE_GATT_SSRVSTART:
        case ESP_BLE_ADVSTART:
        case ESP_WAIT_RESET:
        case ESP_WAIT_ATE0_RESPONSE:
            if (dateTimePassed_S(startTime, MAX_ESP_WAIT_TIME)) {
                status = ESP_INIT;
            }
        case ESP_BLE_WAIT_CONNECT:
        case ESP_WAIT_BLE_DATA:
            if (!checkIfReceivePackage()) return;
            parseResult();
            break;
    }
}

#endif