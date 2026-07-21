//
// Created by xmy on 2020/1/10.
//

#include "stdio.h"
#include "stm8l15x.h"
#include "SystemParameters.h"
#include "string.h"
#include "EventManager.h"

#define SAVE_FLAG 0x55
#define EEPROM_STARTADDR 0x1000
#define MAX_ROM_SIZE 255
unsigned char ramData[MAX_ROM_SIZE] = {0};
unsigned char *romData = (unsigned char *) EEPROM_STARTADDR;
void saveToEEPROM(unsigned char *addr, unsigned char *buffer, unsigned int length) {

    unsigned int i;
    uint8_t *Ptr_SegAddr; //Segment pointer

    Ptr_SegAddr = (uint8_t * )(addr);  //Initialize  pointer

    FLASH_Unlock(FLASH_MemType_Data);

    while (!FLASH_IAPSR_DUL);//如果DUL没有置位,说明解锁不成功,最好加入超时判断

    asm("sim");//关闭中断

    for (i = 0; i < length; i++) {
        *(Ptr_SegAddr + i) = buffer[i]; //指定地址，写1 字节
    }

    while (!FLASH_IAPSR_EOP);//等待写操作完成,最好加入超时判断

    asm("rim");//打开中断

    FLASH_Lock(FLASH_MemType_Data);
}

void setFlag(unsigned char index) {
    if (index >= MAX_ROM_SIZE) {
        return;
    }
    ramData[index] = SAVE_FLAG;
    saveToEEPROM(romData + index, ramData+index, 1);
}

void resetFlag(unsigned char index) {
    if (index >= MAX_ROM_SIZE) {
        return;
    }
    ramData[index] = 0;
    saveToEEPROM(romData + index, ramData+index, 1);
}

unsigned char readFlag(unsigned char index) {
    if (index >= MAX_ROM_SIZE) {
        return 0;
    }
    if (ramData[index] == SAVE_FLAG) {
        return 1;
    }
    return 0;
}

void initSystemParameters(void){
    memcpy(ramData, romData, MAX_ROM_SIZE);
}
