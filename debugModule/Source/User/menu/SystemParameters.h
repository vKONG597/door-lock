//
// Created by xmy on 2020/1/10.
//

#ifndef USER_MANAGER_H_
#define USER_MANAGER_H_
#define EEPROM_STARTADDR 0x1000

void saveToEEPROM(unsigned char *addr, unsigned char *buffer, unsigned int length);
void setFlag(unsigned char index);
void resetFlag(unsigned char index);
unsigned char readFlag(unsigned char index);
void initSystemParameters(void);

#endif