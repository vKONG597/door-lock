#ifndef LED_H_
#define LED_H_

#define NUMBER_ZERO_LED_INDEX 0
#define NUMBER_ONE_LED_INDEX 1
#define NUMBER_TOW_LED_INDEX 2
#define NUMBER_THREE_LED_INDEX 3
#define NUMBER_FOR_LED_INDEX 4
#define NUMBER_FIVE_LED_INDEX 5
#define NUMBER_SIX_LED_INDEX 6
#define NUMBER_SEVEN_LED_INDEX 7
#define NUMBER_EIGHT_LED_INDEX 8
#define NUMBER_NINE_LED_INDEX 9
#define START_LED_INDEX 10
#define END_LED_INDEX 11
#define GREEN_LED_INDEX 12
#define RED_LED_INDEX 13

typedef struct{
	u8 state;//当前状态，0：不变，1：正在变亮，2：正在变暗
	u8 brightness;//亮度
        GPIO_TypeDef* GPIO;
        u8 Pin;
}Led;

void ledInit(void);
void turnOnLed(Led led);
void turnOffLedByIndex(unsigned char index);
void turnOffLed(Led led);
void flashLed(Led led);
void ledProgress(void );
void turnOnLedByIndex(unsigned char index);
void turnOffAllLed(void);
unsigned char isLedFlashing(unsigned char index);
void flashLedByIndex(unsigned char index);
#endif