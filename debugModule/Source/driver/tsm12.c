#include "stm8l15x.h"
#include "TIM4.h"
#include "tsm12.h"




/* -----------------------------------------------
 * 函数名称：I2C_Delay
 *
 * 函数功能：I2C总线时序延时函数
 *
 * 参数:无
 *
 * 输出: 无
 *
 * 备注:无
 -------------------------------------------------*/
void  I2C_Delay(void)
{   
    u32  i;
    for(i=0; i<10; i++);
}

/* -----------------------------------------------
 * 函数名称：delay
 *
 * 函数功能：一般延时函数
 *
 * 参数:dly:延时时间
 *
 * 输出: 无
 *
 * 备注:无
 -------------------------------------------------*/
void delay(unsigned int dly)
{
	u32 i;
	for(i=0;i<dly;i++)
	{
		//I2C_Delay();
          
	}
}



/* -----------------------------------------------
 * 函数名称：IIC_Init
 *
 * 函数功能：初始化I2C引脚
 *
 * 参数:无
 *
 * 输出: 无
 *
 * 备注:无
 -------------------------------------------------*/
void iicInit(void)
{
   //GPIO_Init(I2C_GPIO, I2C_IN,  GPIO_Mode_In_FL_IT);
   SCL_SET();
   SDA_SET();
   //EXTI_SetPinSensitivity(EXTI_Pin_2, EXTI_Trigger_Falling);
   TSM12_I2C_DISABLE();
}

/* -----------------------------------------------
 * 函数名称：I2C_Start
 *
 * 函数功能：产生串行通信起始信号
 *
 * 参数:无
 *
 * 输出: 无
 *
 * 备注:无
 -------------------------------------------------*/
void I2C_Start(void)
{
	SDA_SET();//SDA线输出高电平
	SCL_SET();//SCL线输出高电平
	I2C_Delay();
	SDA_CLR();//SDA线产生下降沿
	I2C_Delay();
}

/* -----------------------------------------------
 * 函数名称：I2C_Stop
 *
 * 函数功能：产生串行通信停止信号
 *
 * 参数:无
 *
 * 输出: 无
 *
 * 备注:无
 -------------------------------------------------*/
void I2C_Stop(void)
{
	SDA_CLR();//SDA线输出低电平
	SCL_SET();//SCL线输出高电平
	I2C_Delay();
	SDA_SET();//SDA线产生上升沿
	I2C_Delay();
}

/* -----------------------------------------------
 * 函数名称：I2C_SendByte
 *
 * 函数功能：向I2C总线发送一个字节的数据
 *
 * 参数:unsigned char data，要发送给从机的数据
 *
 * 输出: unsigned char ack，接收到的应答信号
 *
 * 备注:无
 -------------------------------------------------*/
unsigned char I2C_SendByte(unsigned char data)
{ 
	signed char i;
	unsigned char ack;
	
	SCL_CLR();
	for(i = 7; i >= 0; i--)
	{
		if( 0 != (data & (1 << i)) )//如果data的第i位不为0
		{
			SDA_SET();//SDA线输出高电平
		}
		else
		{
			SDA_CLR();//否则SDA线输出低电平
		}
		I2C_Delay();
		SCL_SET();
		I2C_Delay();
		SCL_CLR();//SCL为低，SDA的电平才可以变化
		I2C_Delay();
	}
	
	//接收从机应答
	SDA_SET();   //拉高总线    
	I2C_Delay();
	SDA_IN();    //SDA线设置为输入
	SCL_SET();
	I2C_Delay();
	if(SDA_READ())
	{
		ack = 0;//从机应答nack
	}
	else
	{
		ack = 1;//从机应答ack
	}
	SCL_CLR();
	I2C_Delay();
	SDA_OUT(); //SDA线重新设置为输出
        
	return ack;
}

/* -----------------------------------------------
 * 函数名称：I2C_RecvByte
 *
 * 函数功能：从I2C总线读取一个字节的数据
 *
 * 参数:  unsigned char ack，发送给从机的应答信号
 *
 * 输出: unsigned char data，接收到的数据
 *
 * 备注:无
 -------------------------------------------------*/
unsigned char I2C_RecvByte(unsigned char ack)
{
	signed char i;
	unsigned char data = 0;//data初始化为0
	
	SDA_SET();     //拉高SDA线
	
	
	SCL_CLR();
	SDA_IN();      //SDA线设置为输入
	for(i = 7; i >= 0; i--)
	{
		SCL_SET();
		I2C_Delay();
		if(SDA_READ())//如果第i个时钟时总线为高电平，则把data的第i位置1
		{
			data |= (1 << i);
		}
		SCL_CLR();
		I2C_Delay();
	}
	
	//发送应答信号给从机
	SDA_OUT();    //SDA线重新设置为输出
	if(1 == ack)
	{
		SDA_CLR();//1表示发送ack
	}
	else
	{
		SDA_SET();//0表示发送nack
	}
	I2C_Delay();
	SCL_SET();
	I2C_Delay();
	SCL_CLR();
	I2C_Delay();
	
	return data;
}


/* -----------------------------------------------
 * 函数名称：TSM12_Write_Byte
 *
 * 函数功能：向TSM12写一个字节的数据
 *
 * 参数: REG_Address： TSM12的字节地址
 *		 unsigned char REG_data：要发送的数据
 *
 * 输出: 写入成功：1； 写入失败：0
 *
 * 备注:无
 -------------------------------------------------*/
unsigned char  TSM12_Write_Byte(unsigned char REG_Address, unsigned char REG_data)
{
	I2C_Start();
	if(I2C_SendByte(TSM12_Slave_Adress) == 0)//从机地址+写命令
	{
		return 0;
	}
	if(I2C_SendByte(REG_Address) == 0)     //写字节地址
	{
		return 0;
	}
	if(I2C_SendByte(REG_data) == 0)        //写数据
	{
		return 0;
	}
	I2C_Stop();
	delay(50);
	return 1;
}
/* -----------------------------------------------
 * 函数名称：TSM12_Read_Byte
 *
 * 函数功能：从TSM12中读一个字节的数据
 *
 * 参数:  REG_Address：TSM12的字节地址
 * 
 * 输出: data：读取到的数据
 *
 * 备注:无
 -------------------------------------------------*/
unsigned char TSM12_Read_Byte(unsigned char REG_Address)
{
	unsigned char data;
	I2C_Start();
	I2C_SendByte(TSM12_Slave_Adress);//从机地址+写命令
	I2C_SendByte(REG_Address);     //写字节地址
	I2C_Start();
	I2C_SendByte(TSM12_Slave_Adress|0x01);//从机地址+读命令
	data = I2C_RecvByte(0);        //读一个数据，发送nack应答
	I2C_Stop();
	return data;
}

void tsm12Init(void)
{
    iicInit();
    /* Power ON */ 
    TSM12_I2C_ENABLE(); 
    Delay_ms(400); 
    
    TSM12_Write_Byte(CTRL2, 0x0F);
    TSM12_Write_Byte(CTRL2, 0x07);//enable sleep mode 

    TSM12_Write_Byte(SENS1, 0x77);// 9, 6 
    TSM12_Write_Byte(SENS2, 0x77);// 3, # 
    TSM12_Write_Byte(SENS3, 0x77);// 8, 5 
    TSM12_Write_Byte(SENS4, 0x77);// 2, 0 
    TSM12_Write_Byte(SENS5, 0x77);// *, 1 
    TSM12_Write_Byte(SENS6, 0x77);// 4, 7 
    
    // General Control Register1 setting
    TSM12_Write_Byte(CTRL1, 0x8B); 
    
    TSM12_Write_Byte(REF_RST1, 0x00);//renew value of everyone channel 
    TSM12_Write_Byte(REF_RST2, 0x00);
    
    TSM12_Write_Byte(CH_HOLD1, 0x00);//enable all channels 
    TSM12_Write_Byte(CH_HOLD2, 0x00);

    
    TSM12_I2C_DISABLE();//TSM12 I2C disable 
}
/******************************************************************************* 
* 名 称: TSM12_Exit_Sleep_Mode() 
* 功 能: TSM12M 退出低功耗模式
* 入口参数 : 无
* 出口参数 : 无
* 说 明: 
*******************************************************************************/ 
void TSM12_Exit_Sleep_Mode(void)
{
  TSM12_I2C_ENABLE();
  delay(100);
  TSM12_Write_Byte(CTRL2, 0x03);//disable sleep
 // TSM12_I2C_DISABLE();
} 
/******************************************************************************* 
* 名 称: TSM12_Into_Sleep_Mode() 
* 功 能: TSM12M 进入低功耗模式
* 入口参数 : 无
* 出口参数 : 无
* 说 明: 
*******************************************************************************/ 
void TSM12_Into_Sleep_Mode(void)
{
//  TSM12_I2C_DISABLE();
//  delay(100);
  TSM12_I2C_ENABLE();
  delay(100);
  TSM12_Write_Byte(CTRL2, 0x07);//enable sleep
 //TSM12_I2C_DISABLE();
}
/******************************************************************************* 
* 名 称: TSM12_Read_Reg(REG_Address) 
* 功 能: 读取TSM12寄存器的值
* 入口参数 : REG_Address
* 出口参数 : temp
* 说 明: 
*******************************************************************************/ 
unsigned char TSM12_Read_Reg(unsigned char REG_Address)
{
  unsigned char temp;
  TSM12_I2C_ENABLE();
  delay(30);
  temp = TSM12_Read_Byte(REG_Address);//读取该寄存器的值
  TSM12_I2C_DISABLE();
  return temp;//返回寄存器的值
}

uint8_t touchFlag = 0;
void onTouchKeyEvent(void){
  touchFlag = 1;
}

unsigned char readTsm(void)
{
    uint8_t temp1,temp2,temp3;
    uint8_t output_value = 0xff;
 
    if(touchFlag == 0)return output_value;
    touchFlag = 0;
    
    temp1 = TSM12_Read_Reg(OUTPUT1);
    temp2 = TSM12_Read_Reg(OUTPUT2);
    temp3 = TSM12_Read_Reg(OUTPUT3);//读寄存器的值

    if(temp1)
    {
        if((temp1 == 0x01)||(temp1 == 0x02)||(temp1 == 0x03)) output_value = 3;
        if((temp1 == 0x04)||(temp1 == 0x08)||(temp1 == 0x0C)) output_value = 6;
        if((temp1 == 0x10)||(temp1 == 0x20)||(temp1 == 0x30)) output_value = 9;
        if((temp1 == 0x40)||(temp1 == 0x80)||(temp1 == 0xC0)) output_value = 11;
        
    }
    if(temp2)
    {
        if((temp2 == 0x01)||(temp2 == 0x02)||(temp2 == 0x03)) output_value = 2;
        if((temp2 == 0x04)||(temp2 == 0x08)||(temp2 == 0x0C)) output_value = 5;
        if((temp2 == 0x10)||(temp2 == 0x20)||(temp2 == 0x30)) output_value = 8;
        if((temp2 == 0x40)||(temp2 == 0x80)||(temp2 == 0xC0)) output_value = 0;
    }
    if(temp3)
    {
        if((temp3 == 0x01)||(temp3 == 0x02)||(temp3 == 0x03)) output_value = 10;
        if((temp3 == 0x04)||(temp3 == 0x08)||(temp3 == 0x0C)) output_value = 7;
        if((temp3 == 0x10)||(temp3 == 0x20)||(temp3 == 0x30)) output_value = 4;
        if((temp3 == 0x40)||(temp3 == 0x80)||(temp3 == 0xC0)) output_value = 1;
    }

    return output_value;
}
