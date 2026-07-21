#ifndef INPUT_DEVICE_MANAGER_H_
#define INPUT_DEVICE_MANAGER_H_

typedef enum {
    KEY_EVENT = 0,
} EventType;

typedef void (*OnEventCallback)(unsigned char event, unsigned char *pData, unsigned char length);

void registerEventCallback(OnEventCallback callback);

void unRegisterEventCallback(void);

#endif