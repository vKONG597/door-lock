/*TSM12 Register Address*/
#ifndef TSM12_H_
#define TSM12_H_
#define SENS1 0x02
#define SENS2 0x03
#define SENS3 0x04
#define SENS4 0x05
#define SENS5 0x06
#define SENS6 0x07
#define CTRL1 0x08
#define CTRL2 0x09
#define REF_RST1 0x0A
#define REF_RST2 0x0B
#define CH_HOLD1 0x0C
#define CH_HOLD2 0x0D
#define CAL_HOLD1 0x0E
#define CAL_HOLD2 0x0F
#define OUTPUT1 0x10
#define OUTPUT2 0x11
#define OUTPUT3 0x12


void  I2C_Delay(void);
void delay(unsigned int dly);

void iicInit(void);
void I2C_Start(void);
void I2C_Stop(void);

unsigned char I2C_SendByte(unsigned char data);
unsigned char I2C_RecvByte(unsigned char ack);

unsigned char TSM12_Write_Byte(unsigned char REG_Address, unsigned char REG_data);
unsigned char TSM12_Read_Byte(unsigned char REG_Address);
void tsm12Init(void);

void TSM12_Exit_Sleep_Mode(void);
void TSM12_Into_Sleep_Mode(void);

unsigned char TSM12_Read_Reg(unsigned char REG_Address);


/*I2C GPIO…Ë÷√ */
#define I2C_GPIO GPIOD
#define I2C_SCL  GPIO_Pin_7
#define I2C_SDA  GPIO_Pin_6
#define I2C_INT   GPIO_Pin_5
#define I2C_EN   GPIO_Pin_4

#define TSM12_I2C_ENABLE()  GPIO_ResetBits(I2C_GPIO, I2C_EN)
#define TSM12_I2C_DISABLE() GPIO_SetBits(I2C_GPIO, I2C_EN)

#define SCL_SET()   GPIO_SetBits(I2C_GPIO, I2C_SCL)
#define SCL_CLR()   GPIO_ResetBits(I2C_GPIO, I2C_SCL)
#define SDA_SET()   GPIO_SetBits(I2C_GPIO, I2C_SDA)
#define SDA_CLR()   GPIO_ResetBits(I2C_GPIO, I2C_SDA)
#define SDA_READ()  GPIO_ReadInputDataBit(I2C_GPIO, I2C_SDA)

#define SCL_OUT() GPIO_Init(I2C_GPIO, I2C_SCL, GPIO_Mode_Out_PP_High_Fast)
#define SDA_OUT() GPIO_Init(I2C_GPIO, I2C_SDA, GPIO_Mode_Out_PP_High_Fast)
#define SDA_IN()  GPIO_Init(I2C_GPIO, I2C_SDA, GPIO_Mode_In_FL_No_IT)

/*Slave Address*/
#define TSM12_Slave_Adress 0xD0 //write

void onTouchKeyEvent(void);
unsigned char readTsm(void);
#endif