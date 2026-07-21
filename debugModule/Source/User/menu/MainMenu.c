//
// Created by Administrator on 2020/1/10.
//

#include "stdio.h"
#include "stm8l15x.h"
#include "oled.h"
#include "EventManager.h"
#include "SystemParameters.h"
#include "MainMenu.h"
#include "Board.h"
#include "keys.h"

#define USART2_F1020_EN_PORT GPIOD
#define USART2_F1020_EN_PIN GPIO_Pin_7
#define F1020_DEBUG_EN_PORT GPIOD
#define F1020_DEBUG_EN_PIN GPIO_Pin_6
#define DEBUG_F1020_EN_PORT GPIOD
#define DEBUG_F1020_EN_PIN GPIO_Pin_5
#define F1020_USART2_EN_PORT GPIOD
#define F1020_USART2_EN_PIN GPIO_Pin_4
#define USART2_BIOVO_EN_PORT GPIOF
#define USART2_BIOVO_EN_PIN GPIO_Pin_7
#define BIOVO_DEBUG_EN_PORT GPIOF
#define BIOVO_DEBUG_EN_PIN GPIO_Pin_6
#define DEBUG_BIOVO_EN_PORT GPIOF
#define DEBUG_BIOVO_EN_PIN GPIO_Pin_5
#define BIOVO_USART2_EN_PORT GPIOF
#define BIOVO_USART2_EN_PIN GPIO_Pin_4
#define USART3_ESP32_EN_PORT GPIOF
#define USART3_ESP32_EN_PIN GPIO_Pin_1
#define ESP32_DEBUG_EN_PORT GPIOF
#define ESP32_DEBUG_EN_PIN GPIO_Pin_0
#define DEBUG_ESP32_EN_PORT GPIOB
#define DEBUG_ESP32_EN_PIN GPIO_Pin_7
#define ESP32_USART3_EN_PORT GPIOB
#define ESP32_USART3_EN_PIN GPIO_Pin_6
#define USART3_NB_EN_PORT GPIOB
#define USART3_NB_EN_PIN GPIO_Pin_5
#define NB_DEBUG_EN_PORT GPIOB
#define NB_DEBUG_EN_PIN GPIO_Pin_4
#define DEBUG_NB_EN_PORT GPIOB
#define DEBUG_NB_EN_PIN GPIO_Pin_3
#define NB_USART3_EN_PORT GPIOB
#define NB_USART3_EN_PIN GPIO_Pin_2

typedef struct {
    GPIO_TypeDef *PORT;
    uint8_t Pin;
} CTRL_PIN;

enum {
    USART2_F1020_EN = 0,
    F1020_DEBUG_EN,
    DEBUG_F1020_EN,
    F1020_USART2_EN,

    USART2_BIOVO_EN,
    BIOVO_DEBUG_EN,
    DEBUG_BIOVO_EN,
    BIOVO_USART2_EN,

    USART3_ESP32_EN,
    ESP32_DEBUG_EN,
    DEBUG_ESP32_EN,
    ESP32_USART3_EN,

    USART3_NB_EN,
    NB_DEBUG_EN,
    DEBUG_NB_EN,
    NB_USART3_EN,
} PIN_INDEX;

CTRL_PIN ctrlPins[] = {
        {USART2_F1020_EN_PORT, USART2_F1020_EN_PIN},
        {F1020_DEBUG_EN_PORT,  F1020_DEBUG_EN_PIN},
        {DEBUG_F1020_EN_PORT,  DEBUG_F1020_EN_PIN},
        {F1020_USART2_EN_PORT, F1020_USART2_EN_PIN},
        {USART2_BIOVO_EN_PORT, USART2_BIOVO_EN_PIN},
        {BIOVO_DEBUG_EN_PORT,  BIOVO_DEBUG_EN_PIN},
        {DEBUG_BIOVO_EN_PORT,  DEBUG_BIOVO_EN_PIN},
        {BIOVO_USART2_EN_PORT, BIOVO_USART2_EN_PIN},
        {USART3_ESP32_EN_PORT, USART3_ESP32_EN_PIN},
        {ESP32_DEBUG_EN_PORT,  ESP32_DEBUG_EN_PIN},
        {DEBUG_ESP32_EN_PORT,  DEBUG_ESP32_EN_PIN},
        {ESP32_USART3_EN_PORT, ESP32_USART3_EN_PIN},
        {USART3_NB_EN_PORT,    USART3_NB_EN_PIN},
        {NB_DEBUG_EN_PORT,     NB_DEBUG_EN_PIN},
        {DEBUG_NB_EN_PORT,     DEBUG_NB_EN_PIN},
        {NB_USART3_EN_PORT,    NB_USART3_EN_PIN},
};

#define MENU_LINE 12
#define MENU_X 10
unsigned char userSelect = 0;

enum {
    PC_ESP_CPU=0,
    PC_NB_CPU,
    PC_FP_CPU,
    PC_FV_CPU,

    PC_ESP,
    PC_NB,
    PC_FP,
    PC_FV,

    ESP_CPU,
    NB_CPU,
    FP_CPU,
    FV_CPU,
}MENU_INDEX;

unsigned char menuBuf[MENU_LINE][14] = {
        {"PC<-ESP<->CPU"},
        {"PC<-NB<->CPU"},
        {"PC<-FP<->CPU"},
        {"PC<-FV<->CPU"},

        {"PC<->ESP"},
        {"PC<->NB"},
        {"PC<->FP"},
        {"PC<->FV"},

        {"ESP<->CPU"},
        {"NB<->CPU"},
        {"FP<->CPU"},
        {"FV<->CPU"}
};

void disableAllConnection(void){
    unsigned char i;
    for (int i = 0; i < sizeof(ctrlPins); ++i) {
        GPIO_ResetBits(ctrlPins[i].PORT, ctrlPins[i].Pin);
    }
}

void enableConnections(void){
    if(readFlag(PC_ESP_CPU)){
        GPIO_SetBits(ctrlPins[ESP32_DEBUG_EN].PORT, ctrlPins[ESP32_DEBUG_EN].Pin);
        GPIO_SetBits(ctrlPins[USART3_ESP32_EN].PORT, ctrlPins[USART3_ESP32_EN].Pin);
        GPIO_SetBits(ctrlPins[ESP32_USART3_EN].PORT, ctrlPins[ESP32_USART3_EN].Pin);
    }
    if(readFlag(PC_NB_CPU)){
        GPIO_SetBits(ctrlPins[NB_DEBUG_EN].PORT, ctrlPins[NB_DEBUG_EN].Pin);
        GPIO_SetBits(ctrlPins[NB_USART3_EN].PORT, ctrlPins[NB_USART3_EN].Pin);
        GPIO_SetBits(ctrlPins[USART3_NB_EN].PORT, ctrlPins[USART3_NB_EN].Pin);
    }
    if(readFlag(PC_FP_CPU)){
        GPIO_SetBits(ctrlPins[F1020_DEBUG_EN].PORT, ctrlPins[F1020_DEBUG_EN].Pin);
        GPIO_SetBits(ctrlPins[F1020_USART2_EN].PORT, ctrlPins[F1020_USART2_EN].Pin);
        GPIO_SetBits(ctrlPins[USART2_F1020_EN].PORT, ctrlPins[USART2_F1020_EN].Pin);
    }
    if(readFlag(PC_FV_CPU)){
        GPIO_SetBits(ctrlPins[BIOVO_DEBUG_EN].PORT, ctrlPins[BIOVO_DEBUG_EN].Pin);
        GPIO_SetBits(ctrlPins[BIOVO_USART2_EN].PORT, ctrlPins[BIOVO_USART2_EN].Pin);
        GPIO_SetBits(ctrlPins[USART2_BIOVO_EN].PORT, ctrlPins[USART2_BIOVO_EN].Pin);
    }
    if(readFlag(PC_ESP)){
        GPIO_SetBits(ctrlPins[ESP32_DEBUG_EN].PORT, ctrlPins[ESP32_DEBUG_EN].Pin);
        GPIO_SetBits(ctrlPins[DEBUG_ESP32_EN].PORT, ctrlPins[DEBUG_ESP32_EN].Pin);
    }
    if(readFlag(PC_NB)){
        GPIO_SetBits(ctrlPins[NB_DEBUG_EN].PORT, ctrlPins[NB_DEBUG_EN].Pin);
        GPIO_SetBits(ctrlPins[DEBUG_NB_EN].PORT, ctrlPins[DEBUG_NB_EN].Pin);
    }
    if(readFlag(PC_FP)){
        GPIO_SetBits(ctrlPins[F1020_DEBUG_EN].PORT, ctrlPins[F1020_DEBUG_EN].Pin);
        GPIO_SetBits(ctrlPins[DEBUG_F1020_EN].PORT, ctrlPins[DEBUG_F1020_EN].Pin);
    }
    if(readFlag(PC_FV)){
        GPIO_SetBits(ctrlPins[BIOVO_DEBUG_EN].PORT, ctrlPins[BIOVO_DEBUG_EN].Pin);
        GPIO_SetBits(ctrlPins[DEBUG_BIOVO_EN].PORT, ctrlPins[DEBUG_BIOVO_EN].Pin);
    }
    if(readFlag(ESP_CPU)){
        GPIO_SetBits(ctrlPins[USART3_ESP32_EN].PORT, ctrlPins[USART3_ESP32_EN].Pin);
        GPIO_SetBits(ctrlPins[ESP32_USART3_EN].PORT, ctrlPins[ESP32_USART3_EN].Pin);
    }
    if(readFlag(NB_CPU)){
        GPIO_SetBits(ctrlPins[USART3_NB_EN].PORT, ctrlPins[USART3_NB_EN].Pin);
        GPIO_SetBits(ctrlPins[NB_USART3_EN].PORT, ctrlPins[NB_USART3_EN].Pin);
    }
    if(readFlag(FP_CPU)){
        GPIO_SetBits(ctrlPins[USART2_F1020_EN].PORT, ctrlPins[USART2_F1020_EN].Pin);
        GPIO_SetBits(ctrlPins[F1020_USART2_EN].PORT, ctrlPins[F1020_USART2_EN].Pin);
    }
    if(readFlag(FV_CPU)){
        GPIO_SetBits(ctrlPins[USART2_BIOVO_EN].PORT, ctrlPins[USART2_BIOVO_EN].Pin);
        GPIO_SetBits(ctrlPins[BIOVO_USART2_EN].PORT, ctrlPins[BIOVO_USART2_EN].Pin);
    }
}

void saveUserSelect(void) {
    disableAllConnection();
    switch (userSelect) {
        case PC_ESP_CPU:
            setFlag(userSelect);
            resetFlag(PC_NB_CPU);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_ESP);
            resetFlag(PC_NB);
            resetFlag(PC_FP);
            resetFlag(PC_FV);

            resetFlag(ESP_CPU);
            resetFlag(NB_CPU);
            break;

        case PC_NB_CPU:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_ESP);
            resetFlag(PC_NB);
            resetFlag(PC_FP);
            resetFlag(PC_FV);

            resetFlag(ESP_CPU);
            resetFlag(NB_CPU);
            break;

        case PC_FP_CPU:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_ESP);
            resetFlag(PC_NB);
            resetFlag(PC_FP);
            resetFlag(PC_FV);

            resetFlag(FP_CPU);
            resetFlag(FV_CPU);
            break;

        case PC_FV_CPU:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);
            resetFlag(PC_FP_CPU);

            resetFlag(PC_ESP);
            resetFlag(PC_NB);
            resetFlag(PC_FP);
            resetFlag(PC_FV);

            resetFlag(FP_CPU);
            resetFlag(FV_CPU);
            break;

        case PC_ESP:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_NB);
            resetFlag(PC_FP);
            resetFlag(PC_FV);

            resetFlag(ESP_CPU);
            break;

        case PC_NB:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_ESP);
            resetFlag(PC_FP);
            resetFlag(PC_FV);

            resetFlag(NB_CPU);
            break;

        case PC_FP:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_ESP);
            resetFlag(PC_NB);
            resetFlag(PC_FV);

            resetFlag(FP_CPU);
            break;

        case PC_FV:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_ESP);
            resetFlag(PC_NB);
            resetFlag(PC_FP);

            resetFlag(FV_CPU);
            break;

        case ESP_CPU:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);

            resetFlag(PC_ESP);

            resetFlag(NB_CPU);
            break;

        case NB_CPU:
            setFlag(userSelect);
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);

            resetFlag(PC_NB);

            resetFlag(ESP_CPU);
            break;

        case FP_CPU:
            setFlag(userSelect);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_FP);

            resetFlag(FV_CPU);
            break;

        case FV_CPU:
            setFlag(userSelect);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);

            resetFlag(PC_FV);

            resetFlag(FP_CPU);
            break;

        default:
            resetFlag(PC_ESP_CPU);
            resetFlag(PC_NB_CPU);
            resetFlag(PC_FP_CPU);
            resetFlag(PC_FV_CPU);
            resetFlag(PC_ESP);
            resetFlag(PC_NB);
            resetFlag(PC_FP);
            resetFlag(PC_FV);
            resetFlag(ESP_CPU);
            resetFlag(NB_CPU);
            resetFlag(FP_CPU);
            resetFlag(FV_CPU);
            break;
    }

    enableConnections();
}

void showMainMenu(void) {
    clear_screen();
    unsigned char showIndex = userSelect / 4 * 4;
    unsigned char i;
    for (i = 0; i < 4; i++) {
        display_GB2312_string(0, (userSelect % 4) * 2, ">", 0);
        display_GB2312_string(MENU_X, i * 2, menuBuf[showIndex], readFlag(showIndex));
        if (++showIndex >= MENU_LINE) {
            break;
        }
    }
}

static void onKeyEvent(unsigned char keyValue) {
    switch (keyValue) {
        case KEY_UP:
            if (userSelect > 0) {
                userSelect--;
            }
            showMainMenu();
            break;

        case KEY_CONFIRM:
            saveUserSelect();
            showMainMenu();
            break;

        case KEY_DOWN:
            if (userSelect < (MENU_LINE - 1)) {
                userSelect++;
            }
            showMainMenu();
            break;
    }
}

static void eventCallback(unsigned char event, unsigned char *pData, unsigned char length) {
    switch (event) {
        case KEY_EVENT:
            onKeyEvent(*pData);
            break;
    }
}

void onMainMenuCreate(void) {
    showMainMenu();
    disableAllConnection();
    enableConnections();
    registerEventCallback(eventCallback);
}
