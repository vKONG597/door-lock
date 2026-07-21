//
// Created by Administrator on 2020/1/5.
//
#include "stdio.h"
#include "string.h"
#include "stm8l15x.h"
#include "rc523.h"
#include "TIM4.h"


#define RC523_PORT    GPIOF
#define SCK           GPIO_Pin_6
#define MISO          GPIO_Pin_4
#define MOSI          GPIO_Pin_5
#define NSS           GPIO_Pin_7


#define CLR_NSS GPIO_ResetBits(RC523_PORT,NSS)
#define SET_NSS GPIO_SetBits(RC523_PORT,NSS)
#define CLR_SCK GPIO_ResetBits(RC523_PORT,SCK)
#define SET_SCK GPIO_SetBits(RC523_PORT,SCK)
#define CLR_MOSI GPIO_ResetBits(RC523_PORT,MOSI)
#define SET_MOSI GPIO_SetBits(RC523_PORT,MOSI)
#define GET_MISO GPIO_ReadInputDataBit(RC523_PORT,MISO)

void spiInit(void){

}
void rcDelay(int dly);
void rcDelay(int dly)
{
    int i;
    for(i=0;i<dly;i++)
    {
    }
}

void WriteRawRC(u8 Address, u8 value);
void WriteRawRC(u8 Address, u8 value)
{
    u8 i, ucAddr;
    CLR_SCK;
    CLR_NSS;
    rcDelay(5);
    ucAddr = ((Address<<1)&0x7E);
    for(i=8;i>0;i--){
        if((ucAddr&0x80) == 0x80){
            SET_MOSI;
        }else{
            CLR_MOSI;
        }
        SET_SCK;
        rcDelay(5);
        ucAddr<<=1;
        CLR_SCK;
        rcDelay(5);
    }

    for(i=8;i>0;i--){
        if((value&0x80) == 0x80){
            SET_MOSI;
        }else{
            CLR_MOSI;
        }
        SET_SCK;
        rcDelay(5);
        value<<=1;
        CLR_SCK;
        rcDelay(5);
    }
    SET_NSS;
    SET_SCK;
}
u8 ReadRawRC(u8 Address);

u8 ReadRawRC(u8 Address){
    u8 i,ucAddr;
    u8 ucResult=0;
    CLR_SCK;
    CLR_NSS;
    rcDelay(5);
    ucAddr = ((Address<<1)&0x7E) | 0x80;

    for(i=8;i>0;i--){
        if((ucAddr&0x80) == 0x80){
            SET_MOSI;
        }else{
            CLR_MOSI;
        }
        SET_SCK;
        rcDelay(5);
        ucAddr<<=1;
        CLR_SCK;
        rcDelay(5);
    }


    for(i=8;i>0;i--){

        SET_SCK;
        rcDelay(5);
        ucResult<<=1;
        if(GET_MISO){
            ucResult |= 1;
        }
        CLR_SCK;
        rcDelay(5);
    }
    SET_NSS;
    SET_SCK;
    return ucResult;
}


void PcdReset(void){
    WriteRawRC(ModeReg, 0x3D);
    WriteRawRC(TReloadRegL, 30);
    WriteRawRC(TReloadRegH, 0);
    WriteRawRC(TModeReg, 0x8D);
    WriteRawRC(TPrescalerReg, 0x40);
    WriteRawRC(TxAutoReg, 0x40);
}


void PcdAntennaOn(void){
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}
/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}


#define MAXRLEN 18

/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡
//                0x26 = 寻未进入休眠状态的卡
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
    int status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x07);
    SetBitMask(TxControlReg,0x03);

    ucComMF522Buf[0] = req_code;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x10))
    {
        *pTagType     = ucComMF522Buf[0];
        *(pTagType+1) = ucComMF522Buf[1];
    }
    else
    {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{
    int status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);
    //temp = (unLen/8)-1;
    if (status == MI_OK)
    {
        for (i=0; i<4; i++)
        {
            *(pSnr+i)  = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];

        }
        
        if (snr_check != ucComMF522Buf[i])
        {   status = MI_ERR;    }
    }

    SetBitMask(CollReg,0x80);
   
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    int status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
        ucComMF522Buf[i+2] = *(pSnr+i);
        ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥
//                 0x61 = 验证B密钥
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    int status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<6; i++)
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
    //   memcpy(&ucComMF522Buf[2], pKey, 6);
    //   memcpy(&ucComMF522Buf[8], pSnr, 4);

    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRead(unsigned char addr,unsigned char *pData)
{
    int status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
        //   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    int status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pData, 16);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款
//               0xC1 = 充值
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    int status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    if (status == MI_OK)
    {
        // memcpy(ucComMF522Buf, pValue, 4);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    int status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return status;
}

/////////////////////////////////////////////////////////////////////
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int  *pOutLenBit)
{
    int status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
        case PCD_AUTHENT:
            irqEn   = 0x12;
            waitFor = 0x10;
            break;
        case PCD_TRANSCEIVE:
            irqEn   = 0x77;
            waitFor = 0x30;
            break;
        default:
            break;
    }

    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);

    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }
    WriteRawRC(CommandReg, Command);


    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }

    i = 1000;//根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   status = MI_NOTAGERR;   }
            if (Command == PCD_TRANSCEIVE)
            {
                n = ReadRawRC(FIFOLevelReg);
                lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg);    }
            }
        }
        else
        {   status = MI_ERR;   }

    }


    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE);
    return status;
}
/*******************************************************************************
 * 函数名称：unsigned char* readCard(void)
 * 函数说明：读取卡号
 * 参数：none
 * 返回值：指向卡id的指针
 ******************************************************************************/
unsigned char* readCard(unsigned char *length){
    unsigned char status,card_leave;
    static unsigned char strp[20];//当前卡号
    static unsigned char strb[20];//上一次卡号
    static unsigned char cardidbuf[20];//卡号
    *length = 0;
    unsigned char *p = NULL;
//   for(int i=0;i<4;i++){
//       sprintf(strb+i*2,"%x",g_ucTempbuf[i]);
//   }
    card_leave = cardLeave();
    if(1==card_leave) memset(cardidbuf,0,20);//如果卡离开，则上次的卡号清零
    sprintf((char *)strb,"%x %x %x %x",cardidbuf[0],cardidbuf[1],cardidbuf[2],cardidbuf[3]);
    status = PcdRequest(PICC_REQALL, cardidbuf);//寻卡
    if (status != MI_OK){
        return p;
    }

    status = PcdAnticoll(cardidbuf);//防冲撞
    if (status != MI_OK){
        return p;
    }
    PcdHalt();
//    for(int i=0;i<4;i++){
//        sprintf(strp+i*2,"%x",g_ucTempbuf[i]);
//    }
    sprintf((char *)strp,"%x %x %x %x",cardidbuf[0],cardidbuf[1],cardidbuf[2],cardidbuf[3]);
    *length = 4;
    if(strcmp((char const *)strb, (char const *)strp)==0) return p;//判断此次与上一次的卡号是否一致，如果两次卡号相同，则说明卡没有离开，返回NUILL
    else return strp;
}
/*******************************************************************************
 * 函数名称：unsigned char cardLeave(void)
 * 函数说明：判断卡是否离开
 * 参数：none
 * 返回值：放卡没有离开：0        卡离开：1
 ******************************************************************************/
unsigned char cardLeave(void){
    unsigned char card_status = 0;
    unsigned char status;
    static unsigned char put_card_flag = 0;
    static unsigned char take_card_flag = 0;
    unsigned char buf[20];
    status = PcdRequest(PICC_REQALL, buf);//寻卡
    if (status != MI_OK){
            card_status = 2;
    }
    status = PcdAnticoll(buf);//防冲撞
    if (status != MI_OK){
            card_status = 2;
    }
    else card_status = 1;
    PcdHalt();
    switch (card_status){
	case 0://无卡操作
            break;
        case 1:
            put_card_flag = 1;//放卡标志
            return 0;
        case 2:
            take_card_flag = 1;//卡离开标志
            if((1==put_card_flag)&&(1==take_card_flag)) {//完成卡离开的动作
                put_card_flag = 0;//标志清零
                take_card_flag = 0;//标志清零
                return 1;
            }
            break;
    }
    
    return 1;
}
/*******************************************************************************
 * 函数名称：void rc523Init(void)
 * 函数说明：rc523初始化
 * 参数：none
 * 返回值：none
 ******************************************************************************/
void rc523Init(void){
    spiInit();
    PcdReset();
    Delay_ms(250);
    PcdAntennaOff();
    PcdAntennaOn();
}
/*******************************************************************************
 * 函数名称：void readCardProccess(void)
 * 函数说明：读卡进程
 * 参数：none
 * 返回值：none
 ******************************************************************************/
void readCardProccess(void){
    unsigned char* cardnum;
    unsigned char length;
    cardnum = readCard(&length);
    if(cardnum != NULL)
    printf("%s",cardnum);
}




