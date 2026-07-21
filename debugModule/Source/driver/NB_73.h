#ifndef NB_73_H
#define NB_73_H

void registerNBCallback(OnEventCallback callback);
void unRegisterNBCallback(void);
void sendDataByNB(unsigned char *pData, unsigned char length);
void onNB73ReceiveByte(unsigned char byte);
void sendDataByNB(unsigned char *pData, unsigned char length);
void NB73Process(void);
void heartBeatTimeTick(void);

#endif
