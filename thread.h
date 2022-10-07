#ifndef CANTHREAD_H
#define CANTHREAD_H

#include <QThread>
#include <QDebug>
#include "ControlCAN.h"

class CANThread : public QThread
{
    Q_OBJECT
public:
    explicit CANThread(QObject *parent = nullptr);

protected:
    //QThread的虚函数
    //run()函数通过调用exec()函数来启动事件循环机制，并且在线程内部处理Qt的事件。
    void run();

private:
    bool isStop;

public:
    /**CAN 配置相关参数**/
    int m_cannum;//can设备索引
    int m_devtype;//can卡类型号
    DWORD m_devind;//can第几路通道

public:
    /**CAN参数初始化函数**/
    void CAN_Para_Init();

signals:
    void DataReceiveDone(PVCI_CAN_OBJ objs,uint length);

public:
    void setFlag(bool flag = true);
};

#endif // CANTHREAD_H
