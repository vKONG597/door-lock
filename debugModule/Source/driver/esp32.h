//
// Created by xmy on 2019/12/10.
//

#ifndef LOCK_ESP32_H
#define LOCK_ESP32_H


//wifi或ble二选一
//#define ESP32_WIFI_MODE
#define ESP32_BLE_MODE



void ESP32Process(void);
void onESPReceiveByte(unsigned char byte);

#ifdef ESP32_WIFI_MODE
void esp32SendWifiData(unsigned char *pData, unsigned char length);
void startSmartConfig(void);
#endif
#ifdef ESP32_BLE_MODE
void esp32SendBleData(unsigned char *pData, unsigned char length);
#endif

void registerESP32Callback(OnEventCallback callback);
void unRegisterESP32Callback(void);
#endif //LOCK_ESP32_H
