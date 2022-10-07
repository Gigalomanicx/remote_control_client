#include "thread.h"
#include "ControlCAN.h"

CANThread::CANThread(QObject *parent) : QThread(parent)
{
    isStop = false;
}

void CANThread::CAN_Para_Init()
{
    /**CAN 相关参数初始化**/
    m_devtype = VCI_USBCAN_2E_U;
    m_devind = 0;
    m_cannum = 0;
}

//设置接收停止标志位
void CANThread::setFlag(bool flag)
{
    isStop = flag;
}

//接收进程
void CANThread::run()
{
    VCI_ERR_INFO errinfo;
    uint len = 1;
    uint length;

    while(isStop == false)
    {
        msleep(100);
        //此函数用以获取指定 CAN 通道的接收缓冲区中，接收到但返回尚未被读取的帧数量。
        length = VCI_GetReceiveNum(static_cast<DWORD>(m_devtype),m_devind,static_cast<DWORD>(m_cannum));
        if(length <= 0)
        {
            continue;
        }
        //VCI_CAN_OBJ 结构体是 CAN 帧结构体，即 1 个结构体表示一个帧的数据结构。
        //PVCI_CAN_OBJ是VCI_CAN_OBJ的指针形式。
        PVCI_CAN_OBJ frameinfo = new VCI_CAN_OBJ[length];
        //此函数从指定的设备 CAN 通道的接收缓冲区中读取数据。
        len=VCI_Receive(static_cast<DWORD>(m_devtype),m_devind,static_cast<DWORD>(m_cannum),frameinfo,length,200);
        if(len<=0)
        {
            //注意：如果没有读到数据则必须调用此函数来读取出当前的错误码，
            //此函数用以获取 CAN 卡发生的最近一次错误信息。
            VCI_ReadErrInfo(static_cast<DWORD>(m_devtype),m_devind,static_cast<DWORD>(m_cannum),&errinfo);
            qDebug()<<"ErrorCode:"<<QString::number(errinfo.ErrCode,16);
        }
        else
        {
            emit DataReceiveDone(frameinfo,length);
        }
        delete [] frameinfo;

        if(true == isStop)
        {
            break;
        }
    }
}
