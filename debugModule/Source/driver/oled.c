#include "stm8l15x.h"
#include "oled.h"

void oledDelay(int n_ms)
{
    int j,k;
    for(j=0;j<n_ms;j++)
        for(k=0;k<1;k++);
}

/*写指令到LCD模块*/
void transfer_command_lcd(int data1)
{
    char i;
    lcd_rs(0);;;
//oledDelay(1);
    for(i=0;i<8;i++)
    {
        lcd_sclk(0);
        if(data1&0x80) {lcd_sid(1);}
        else {lcd_sid(0);}
        //oledDelay(1);
        lcd_sclk(1);
        //oledDelay(1);

        data1<<=1;
    }
    lcd_rs(1);

}

/*写数据到LCD模块*/
void transfer_data_lcd(int data1)
{
    char i;
    lcd_rs(1);;;
//oledDelay(1);
    for(i=0;i<8;i++)
    {
        lcd_sclk(0);  
        if(data1&0x80) {lcd_sid(1);}
        else {lcd_sid(0);}
        //oledDelay(1);
        lcd_sclk(1);
        //oledDelay(1);
        data1<<=1;
    }
    lcd_rs(1);
}

void initial_lcd(void)
{
  lcd_reset(0);
  oledDelay(20);
  lcd_reset(1);
  oledDelay(20);
      transfer_command_lcd(0xae);//--turn off oled panel
      transfer_command_lcd(0x00);//---set low column address
      transfer_command_lcd(0x10);//---set high column address
      transfer_command_lcd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
      transfer_command_lcd(0x81);//--set contrast control register
      transfer_command_lcd(0xcf); // Set SEG Output Current Brightness
      transfer_command_lcd(0xa1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
      transfer_command_lcd(0xc8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
      transfer_command_lcd(0xa6);//--set normal display
      transfer_command_lcd(0xa8);//--set multiplex ratio(1 to 64)
      transfer_command_lcd(0x3f);//--1/64 duty
      transfer_command_lcd(0xd3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
      transfer_command_lcd(0x00);//-not offset
      transfer_command_lcd(0xd5);//--set display clock divide ratio/oscillator frequency
      transfer_command_lcd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
      transfer_command_lcd(0xd9);//--set pre-charge period
      transfer_command_lcd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
      transfer_command_lcd(0xda);//--set com pins hardware configuration
      transfer_command_lcd(0x12);
      transfer_command_lcd(0xdb);//--set vcomh
      transfer_command_lcd(0x00);//Set VCOM Deselect Level
      transfer_command_lcd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
      transfer_command_lcd(0x02);//
      transfer_command_lcd(0x8d);//--set Charge Pump enable/disable
      transfer_command_lcd(0x14);//--set(0x10) disable
      transfer_command_lcd(0xa4);// Disable Entire Display On (0xa4/0xa5)
      transfer_command_lcd(0xa6);// Disable Inverse Display On (0xa6/a7)
      transfer_command_lcd(0xaf);//--turn on oled panel
        
    lcd_address(0,0);
    clear_screen();
}

void lcd_address(u8 page,u8 column)
{

    transfer_command_lcd(0xb0 + column);   /*设置页地址*/
    transfer_command_lcd(((page & 0xf0) >> 4) | 0x10);	/*设置列地址的高4位*/
    transfer_command_lcd((page & 0x0f) | 0x01);	/*设置列地址的低4位*/
}

/*全屏清屏*/

void clear_screen(void)
{
    unsigned char i,j;

    for(i=0;i<8;i++)
    {
        transfer_command_lcd(0xb0+i);
        transfer_command_lcd(0x00);
        transfer_command_lcd(0x10);
        for(j=0;j<128;j++)
        {
            transfer_data_lcd(0x00);
        }
    }
    
}

/*清除第 y 行*/

void OLED_Clear_line(u8 y)
{
	unsigned char x;
	transfer_command_lcd(0xb0+y);
	transfer_command_lcd(0x01);
	transfer_command_lcd(0x10);
	for(x=0;x<128;x++)
		transfer_data_lcd(0);
        transfer_command_lcd(0xb0+y+1);
	transfer_command_lcd(0x01);
	transfer_command_lcd(0x10);
	for(x=0;x<128;x++)
		transfer_data_lcd(0);
}



/*显示16x16点阵图像、汉字、生僻字或16x16点阵的其他图标*/
void display_graphic_16x16(u8 page,u8 column,const u8 *dp,u8 state)
{
    u8 i,j;
    if(!state){ //正显
      for(j=2;j>0;j--)
      {
          lcd_address(column,page);
          for (i=0;i<16;i++)
          {
              transfer_data_lcd(*dp);					/*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
              dp++;
          }
          page++;
      }
    }
    else{ //反显
      for(j=2;j>0;j--)
      {
          lcd_address(column,page);
          for (i=0;i<16;i++)
          {
              transfer_data_lcd(~(*dp));				/*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
              dp++;
          }
          page++;
      }
    }
}

void display_graphic_8x16(u8 page,u8 column,u8 *dp,u8 state)
{
    u8 i,j;
    if(!state){
      for(j=2;j>0;j--)
      {
          lcd_address(column,page);
          for (i=0;i<8;i++)
          {
              transfer_data_lcd(*dp);					/*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
              dp++;
          }
          page++;
      }
    }
    else{
      for(j=2;j>0;j--)
      {
          lcd_address(column,page);
          for (i=0;i<8;i++)
          {
              transfer_data_lcd(~(*dp));			        /*写数据到LCD,每写完一个8位的数据后列地址自动加1*/
              dp++;
          }
          page++;
      }
    }

}





                                         
void send_command_to_ROM( u8 datu )
{
	u8 i;
	for(i=0;i<8;i++ )
	{
		if(datu&0x80)
			{RomSi(1);}
		else
			{RomSi(0);}
			datu = datu<<1;
			RomClk(0);;;
			RomClk(1);;;
	}
}

u8 get_data_from_ROM(void )
{
	
	u8 i;
	u8 ret_data=0;
	RomClk(1);;;
	//GBZK_ROMOUTSET();
	for(i=0;i<8;i++)
	{
		//Rom_OUT(1);;; 
		RomClk(0);;;
		ret_data=ret_data<<1;
		if( ROM_OUT ){
                  ret_data |= 1;
                }
		RomClk(1);
	}
	//GBZK_ROMOUTRESET();
	return(ret_data);
}


void get_n_bytes_data_from_ROM(u8 addrHigh,u8 addrMid,u8 addrLow,u8 *pBuff,u8 DataLen )
{
	u8 i;
	Rom_CS(0);	
	RomClk(0);
	send_command_to_ROM(0x03);
	send_command_to_ROM(addrHigh);
	send_command_to_ROM(addrMid);
	send_command_to_ROM(addrLow);
	for(i = 0; i < DataLen; i++ )
	     *(pBuff+i) =get_data_from_ROM();
	Rom_CS(1);
}
u32  fontaddr=0;

u8 getCharModel(u8* text, u8* buf)
{
	u8 i= 0;
	u8 addrHigh,addrMid,addrLow ;
	u8 size=0;
	
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{						
			fontaddr = (text[i]- 0xb0)*94; 
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (u32)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  
			addrMid = (fontaddr&0xff00)>>8;      
			addrLow = fontaddr&0xff;	     
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,buf,32 );

			
			size=16;
			break;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{
                  
			fontaddr = (text[i]- 0xa1)*94; 
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (u32)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  
			addrMid = (fontaddr&0xff00)>>8;      
			addrLow = fontaddr&0xff;	     
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,buf,32 );
                        
			
			size=16;
			break;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{						
			//unsigned char fontbuf[16];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,buf,16 );
			
                        
			
			size=8;
			break;
		}
		else
			i++;
	}
	return size;
}

void display_GB2312_string(u8 x,u8 y,u8 *text,u8 state)
{
	u8 i= 0;
	u8 addrHigh,addrMid,addrLow ;
	u8 fontbuf[32]={0};			
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{						
			/*国标简体（GB2312）汉字在晶联讯字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+ 846)*32+ BaseAdd;BaseAdd=0*/
			/*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
			fontaddr = (text[i]- 0xb0)*94; 
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (u32)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
			addrLow = fontaddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf,state);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=2;
			x+=16;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{						
			/*国标简体（GB2312）15x16点的字符在晶联讯字库IC中的地址由以下公式来计算：*/
			/*Address = ((MSB - 0xa1) * 94 + (LSB - 0xA1))*32+ BaseAdd;BaseAdd=0*/
			/*由于担心8位单片机有乘法溢出问题，所以分三部取地址*/
			fontaddr = (text[i]- 0xa1)*94; 
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (u32)(fontaddr*32);
			
			addrHigh = (fontaddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			addrMid = (fontaddr&0xff00)>>8;      /*地址的中8位,共24位*/
			addrLow = fontaddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32 );/*取32个字节的数据，存到"fontbuf[32]"*/
			display_graphic_16x16(y,x,fontbuf,state);/*显示汉字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=2;
			x+=16;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))	
		{						
			unsigned char fontbuf[16];			
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);			
			addrHigh = (fontaddr&0xff0000)>>16;
			addrMid = (fontaddr&0xff00)>>8;
			addrLow = fontaddr&0xff;

			get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16 );/*取16个字节的数据，存到"fontbuf[32]"*/
			
			display_graphic_8x16(y,x,fontbuf,state);/*显示8x16的ASCII字到LCD上，y为页地址，x为列地址，fontbuf[]为数据*/
			i+=1;
			x+=8;
		}
		else
			i++;	
	}
	
}

unsigned char lineX,lineY;
//x:0~128
//y:0~4
void oledLineInit(unsigned char x, unsigned char y){
    lineX = x;
    lineY = y;
    //OLED_Clear_line(y);
}

void oledLineAppendChar(unsigned char c){
    unsigned char fontbuf[16];
    u8 addrHigh,addrMid,addrLow ;
    fontaddr = (c- 0x20);
    fontaddr = (unsigned long)(fontaddr*16);
    fontaddr = (unsigned long)(fontaddr+0x3cf80);
    addrHigh = (fontaddr&0xff0000)>>16;
    addrMid = (fontaddr&0xff00)>>8;
    addrLow = fontaddr&0xff;

    get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16 );
    display_graphic_8x16(lineY,lineX,fontbuf,0);
    lineX+=8;
}

void oledLineAppendStr(unsigned char *str){
    while(*str != '\0'){
        oledLineAppendChar(*str);
        str++;
    }
}

void displayChar(unsigned char c, unsigned char x, unsigned char y, unsigned char state){
    unsigned char fontbuf[16];
    u8 addrHigh,addrMid,addrLow ;
    fontaddr = (c- 0x20);
    fontaddr = (unsigned long)(fontaddr*16);
    fontaddr = (unsigned long)(fontaddr+0x3cf80);
    addrHigh = (fontaddr&0xff0000)>>16;
    addrMid = (fontaddr&0xff00)>>8;
    addrLow = fontaddr&0xff;

    get_n_bytes_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16 );
    display_graphic_8x16(y,x,fontbuf,state);
}