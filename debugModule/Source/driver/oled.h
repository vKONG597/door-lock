#ifndef OLED_H_
#define OLED_H_

#define lcd_reset(a)	if (a)	\
					GPIO_SetBits(GPIOE,GPIO_Pin_5);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_5)
                                          
#define lcd_rs(a)	if (a)	\
					GPIO_SetBits(GPIOE,GPIO_Pin_4);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_4)


#define lcd_sid(a)	if (a)	\
                                                    GPIO_SetBits(GPIOE,GPIO_Pin_3);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_3)

#define lcd_sclk(a)	if (a)	\
					GPIO_SetBits(GPIOE,GPIO_Pin_2);\
					else		\
					GPIO_ResetBits(GPIOE,GPIO_Pin_2)


#define RomSi(a)	if (a)	\
					GPIO_SetBits(GPIOG,GPIO_Pin_2);\
					else		\
					GPIO_ResetBits(GPIOG,GPIO_Pin_2)

#define RomClk(a)	if (a)	\
					GPIO_SetBits(GPIOG,GPIO_Pin_0);\
					else		\
					GPIO_ResetBits(GPIOG,GPIO_Pin_0)
#define Rom_CS(a)	if (a)	\
					GPIO_SetBits(GPIOG,GPIO_Pin_1);\
					else		\
					GPIO_ResetBits(GPIOG,GPIO_Pin_1)


                                          
#define ROM_OUT    GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3)
void OLED_Clear_line(u8 y);
void transfer_command_lcd(int data1);
void transfer_data_lcd(int data1);
void oledDelay(int n_ms);
void initial_lcd(void);
void lcd_address(u8 page,u8 column);
void clear_screen(void);
void display_graphic_16x16(u8 page,u8 column,const u8 *dp,u8 state);
void display_graphic_8x16(u8 page,u8 column,u8 *dp,u8 state);
void send_command_to_ROM( u8 datu );             
u8 get_data_from_ROM( void);
void get_n_bytes_data_from_ROM(u8 addrHigh,u8 addrMid,u8 addrLow,u8 *pBuff,u8 DataLen );
u8 getCharModel(u8* text, u8* buf);
void display_GB2312_string(u8 x,u8 y,u8 *text,u8 state);

void oledLineInit(unsigned char x, unsigned char y);
void oledLineAppendChar(unsigned char c);
void oledLineAppendStr(unsigned char *str);
void displayChar(unsigned char c, unsigned char x, unsigned char y, unsigned char state);
#endif


