#ifndef FINGER_PRINT_H_
#define FINGER_PRINT_H_

#define INT_CLEARED         0
#define INT_TOUCH           1
#define INT_LEAVE           2
#define PACKAGE_BOUNDARY	0xf5//数据包开始
#define ACK_SUCCESS  		0x00	//操作成功
#define ACK_NOUSER   		0x05	//无此用户
#define ACK_TIMEOUT  		0x08	//采集超时

#define NO_COMMAND 0
#define CMD_REGISTER 0x01
#define CMD_REGISTER_STEP2 0x02
#define CMD_REGISTER_STEP3 0x03
#define CMD_DELETE_USER 0x04//删除指定编号指纹
#define CMD_CLEAR_ALL_USER 0x05//清空所有指纹
//#define COM_GET_UNREGISTER_ID 0x47//取指定范围内首个未注册用户号
#define CMD_COMPARISON 0x0c//1:N比对

typedef enum{
    FP_TOUCH = 0,
    FP_COMPARISON_ERROR,
    FP_COMPARISON_SUCCESS,
    FP_OPERATION_SUCCESS,
    FP_DELETE_USER_SUCCESS,
    FP_DELETE_USER_FAIL,
    FP_OPERATION_FAIL,
    FP_REGISTER_NEXT_STEP,
    FP_OPERATION_TIME_OUT,
}FP_ResultCode;

void onFingerPrintInt(unsigned char event);
void fingerPrintProcess(void);
void registerFingerPrintCallback(OnEventCallback callback);
void unRegisterFingerPrintCallback(void);
void onFingerPrintReceiveByte(unsigned char byte);
void setFP_PendingCommand(unsigned char command, unsigned char *data);
void fingerPrintInit(void);
void deleteFPUser(unsigned char id, unsigned char powerUpFlag);
void deleteAllFPUser(void);

#endif