//
// Created by yeliw on 2020/4/17.
//

#ifndef LOCK_FINGERVEIN_H
#define LOCK_FINGERVEIN_H

#define XG_PREFIX_CODE                0xBBAA //包标识

/*设备相关指令*/
#define XG_CMD_CONNECTION             0x01 //连接设备,必须带位以上密码，默认密码全为(0x30)
#define XG_CMD_CLOSE_CONNECTION       0x02 //关闭连接
#define XG_CMD_GET_SYSTEM_INFO        0x03 //获取版本号和设置信息
#define XG_CMD_FACTORY_SETTING        0x04 //恢复出厂设置
#define XG_CMD_SET_DEVICE_ID          0x05 //设置设备编号-255
#define XG_CMD_SET_BAUDRATE           0x06 //设置波特率-4
#define XG_CMD_SET_SECURITYLEVEL      0x07 //设置安全等级-4
#define XG_CMD_SET_TIMEOUT            0x08 //设置等待手指放入超时-255秒
#define XG_CMD_SET_DUP_CHECK          0x09 //设置重复登录检查-1
#define XG_CMD_SET_PASSWORD           0x0A //设置通信密码
#define XG_CMD_CHECK_PASSWORD         0x0B //检查密码是否正确
#define XG_CMD_REBOOT                 0x0C //复位重启设备
#define XG_CMD_SET_SAME_FV            0x0D //登记的时候检查是否为同一根手指

/*识别相关指令*/
#define XG_CMD_FINGER_STATUS          0x10 //检测手指放置状态
#define XG_CMD_CLEAR_ENROLL           0x11 //清除指定ID登录数据
#define XG_CMD_CLEAR_ALL_ENROLL       0x12 //清除所有ID登录数据
#define XG_CMD_GET_EMPTY_ID           0x13 //获取空（无登录数据）ID
#define XG_CMD_GET_ENROLL_INFO        0x14 //获取总登录用户数和模板数
#define XG_CMD_GET_ID_INFO            0x15 //获取指定ID登录信息
#define XG_CMD_ENROLL                 0x16 //指定ID登录
#define XG_CMD_VERIFY                 0x17 //1:1认证或:N识别
#define XG_CMD_IDENTIFY_FREE          0x18 //FREE识别模式，自动识别并发送状态
#define XG_CMD_CANCEL                 0x19 //取消FREE识别模式
#define XG_CMD_RAM_MODE               0x1A //内存操作模式

/******************************高级指令****************************************/
/*数据读写相关指令*/
#define XG_CMD_READ_DATA              0x20 //从设备读取数据
#define XG_CMD_WRITE_DATA             0x21 //写入数据到设备
#define XG_CMD_READ_ENROLL            0x22 //读取指定ID登录数据
#define XG_CMD_WRITE_ENROLL           0x23 //写入（覆盖）指定ID登录数据

/******************************错误代码****************************************/
#define	XG_ERR_SUCCESS               0x00 //操作成功
#define	XG_ERR_FAIL                  0x01 //操作失败
#define XG_ERR_COM                    0x02 //通讯错误
#define XG_ERR_DATA                   0x03 //数据校验错误
#define XG_ERR_INVALID_PWD            0x04 //密码错误
#define XG_ERR_INVALID_PARAM          0x05 //参数错误
#define XG_ERR_INVALID_ID             0x06 //ID错误
#define XG_ERR_EMPTY_ID               0x07 //指定ID为空（无登录数据）
#define XG_ERR_NOT_ENOUGH             0x08 //无足够登录空间
#define XG_ERR_NO_SAME_FINGER         0x09 //不是同一根手指
#define XG_ERR_DUPLICATION_ID         0x0A //有相同登录ID
#define XG_ERR_TIME_OUT               0x0B //等待手指输入超时
#define XG_ERR_VERIFY                 0x0C //认证失败
#define XG_ERR_NO_NULL_ID             0x0D //已无空ID
#define XG_ERR_BREAK_OFF              0x0E //操作中断
#define XG_ERR_NO_CONNECT             0x0F //未连接

/******************************状态代码****************************************/
#define XG_INPUT_FINGER               0x20 //请求放入手指
#define XG_RELEASE_FINGER             0x21 //请求拿开手指

typedef enum{
    FV_TOUCH = 0,
    FV_VERIFY_SUCCESS,
    FV_VERIFY_FAIL,
    FV_PUSH_FINGER,
    FV_RELEASE_FINGER,
    FV_ENROLL_SUCCESS,
    FV_ENROLL_FAIL,
    FV_CLEAR_ENROLL_SUCCESS,
    FV_CLEAR_ENROLL_FAIL,
}FV_ResultCode;

void fingerVeinProcess(void);
void fingerVeinInit(void);
void onFingerVeinInt(void);
void onFingerVeinReceiveByte(unsigned char byte);
void veryfyFingerVein(void);
void FV_DeleteAllUser(void);
void FV_DeleteUser(unsigned char id);
void FV_Enroll(unsigned char id);
void registerFingerVeinCallback(OnEventCallback callback);
void unRegisterFingerVeinCallback(void);

#endif //LOCK_FINGERVEIN_H
